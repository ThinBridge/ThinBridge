param(
    [Parameter(Mandatory)]
    [string]$Executable,
    [ValidateSet('Registry32', 'Registry64')]
    [string]$BinaryConfigResponseRegistryView
)

$ErrorActionPreference = 'Stop'
$Executable = (Resolve-Path -LiteralPath $Executable).ProviderPath

function New-Request([string]$Body) {
    $bytes = [Text.Encoding]::UTF8.GetBytes($Body)
    [byte[]]([BitConverter]::GetBytes($bytes.Length) + $bytes)
}

function Test-Exchange(
    [string]$Name,
    [byte[]]$Request,
    [int]$ExitCode,
    [string]$Diagnostic = '',
    [string]$Response = ''
) {
    $start = [Diagnostics.ProcessStartInfo]::new($Executable)
    $start.UseShellExecute = $false
    $start.CreateNoWindow = $true
    $start.RedirectStandardInput = $true
    $start.RedirectStandardOutput = $true
    $start.RedirectStandardError = $true
    $process = [Diagnostics.Process]::Start($start)
    $output = [IO.MemoryStream]::new()
    try {
        $readOutput = $process.StandardOutput.BaseStream.CopyToAsync($output)
        $readError = $process.StandardError.ReadToEndAsync()
        $process.StandardInput.BaseStream.Write($Request, 0, $Request.Length)
        $process.StandardInput.Close()
        if (-not $process.WaitForExit(10000)) {
            $process.Kill()
            throw "$Name timed out"
        }
        [void]$readOutput.GetAwaiter().GetResult()
        $stderr = $readError.GetAwaiter().GetResult()
        if ($process.ExitCode -ne $ExitCode -or $stderr -cne $Diagnostic) {
            throw "$Name returned exit code $($process.ExitCode) and stderr '$stderr'"
        }

        $actual = $output.ToArray()
        $expected = [byte[]]::new(0)
        if ($Response.Length) {
            $expected = [byte[]](New-Request $Response)
        }
        if ([Convert]::ToHexString($actual) -cne [Convert]::ToHexString($expected)) {
            throw "$Name returned unexpected stdout bytes: $([Convert]::ToHexString($actual))"
        }
        Write-Host "PASS $Name"
    }
    finally {
        $process.Dispose()
        $output.Dispose()
    }
}

Test-Exchange 'Incomplete length prefix' ([byte[]]@(4, 0)) -1 'cannot read 4 bytes'
Test-Exchange 'Incomplete request body' ([byte[]]@(8, 0, 0, 0, 34, 81)) -1 'cannot read 8 bytes'
Test-Exchange 'Unknown command' (New-Request '"X edge"') 0 "unknown command 'X edge'"
Test-Exchange 'Query missing URL separator' (New-Request '"Q edge"') 0 "invalid query request 'Q edge'"
Test-Exchange 'Empty input' ([byte[]]::new(0)) -1 'cannot read 4 bytes'
Test-Exchange 'Empty JSON string is shorter than the minimum' (New-Request '""') -1 'command too short (2 bytes)'
Test-Exchange 'Negative length is rejected' ([BitConverter]::GetBytes([int]-1)) -1 'command too short (-1 bytes)'
Test-Exchange 'Single-character query passes framing but fails dispatch' (New-Request '"Q"') 0 "command too short 'Q'"
Test-Exchange 'Query with only a prefix and space' (New-Request '"Q "') 0 "command too short 'Q '"
Test-Exchange 'Resource request missing tab count separator' (New-Request '"R edge"') 0 "invalid query request 'R edge'"

$firstFrame = New-Request '"X first"'
$secondFrame = New-Request '"X second"'
Test-Exchange 'Only the first frame is dispatched' ($firstFrame + $secondFrame) 0 "unknown command 'X first'"

# 760bf02 made stdin binary. In text mode, 0x1A in the length prefix acts as EOF,
# and the 0x0D 0x0A pair in a 2573-byte length prefix collapses to 0x0A.
foreach ($length in @(0x1A, 0x0A0D)) {
    $command = 'X ' + ('a' * ($length - 4))
    Test-Exchange "Binary stdin length $length" (New-Request ('"' + $command + '"')) 0 "unknown command '$command'"
}

# Check both registry views so the same script works with either executable.
$hasRulefile = $false
$hasExtensionExecfile = $false
foreach ($view in @('Registry32', 'Registry64')) {
    $machine = [Microsoft.Win32.RegistryKey]::OpenBaseKey('LocalMachine', $view)
    try {
        $key = $machine.OpenSubKey('SOFTWARE\ThinBridge')
        if ($null -ne $key) {
            try {
                $names = $key.GetValueNames()
                $hasRulefile = $hasRulefile -or ($names -contains 'Rulefile')
                $hasExtensionExecfile = $hasExtensionExecfile -or ($names -contains 'ExtensionExecfile')
            }
            finally {
                $key.Dispose()
            }
        }
    }
    finally {
        $machine.Dispose()
    }
}
if ($hasRulefile) {
    Write-Host 'SKIP Missing Rulefile because a ThinBridge rule file is configured'
} else {
    Test-Exchange 'Config request with missing Rulefile' (New-Request '"C edge"') 0 'cannot read SOFTWARE\ThinBridge (2)'
}

# BrowserSelector e7c170c records a response length of 8B 0A 00 00.
# Use a temporary Rulefile on a clean test machine to produce that same header.
if ($BinaryConfigResponseRegistryView) {
    $machine = [Microsoft.Win32.RegistryKey]::OpenBaseKey('LocalMachine', $BinaryConfigResponseRegistryView)
    $key = $null
    $registered = $false
    $rulefile = New-TemporaryFile
    try {
        $key = $machine.CreateSubKey('SOFTWARE\ThinBridge')
        if ($key.GetValueNames() -contains 'Rulefile') {
            throw 'The binary response fixture requires an unused ThinBridge Rulefile value'
        }

        $prefix = '{"status":"OK","config":{"CloseEmptyTab":1,"OnlyMainFrame":0,"IgnoreQueryString":0,"DefaultBrowser":"","Sections":['
        $prefix += '{"Name":"firefox","Path":"","ExcludeGroups":[],"Patterns":["'
        $suffix = '"],"Excludes":[]}]}}'
        $url = 'https://example.com/'
        $url += 'a' * (0x0A8B - ($prefix + $url + $suffix).Length)
        [IO.File]::WriteAllText($rulefile.FullName, "[Firefox]`r`n$url`r`n", [Text.Encoding]::ASCII)
        $key.SetValue('Rulefile', $rulefile.FullName, 'String')
        $registered = $true
        Test-Exchange 'Binary stdout length 2699' (New-Request '"C edge"') 0 -Response ($prefix + $url + $suffix)

        $rules = @'
[GLOBAL]
@TOP_PAGE_ONLY
@UNTRUSTED_ZONE
[Default]
@BROWSER_PATH:edge
[Firefox]
@BROWSER_PATH:C:\Browsers\Firefox.exe
@EXCLUDE_GROUP:local
https://example.com/first*
https://example.com/second*
-https://example.com/internal/*
-#https://example.com/commented/*
[Chrome]
@DISABLED
https://disabled.example/*
[Edge]
'@
        [IO.File]::WriteAllText($rulefile.FullName, $rules, [Text.Encoding]::ASCII)
        $config = '{"status":"OK","config":{"CloseEmptyTab":1,"OnlyMainFrame":1,"IgnoreQueryString":1,"DefaultBrowser":"edge","Sections":['
        $config += '{"Name":"firefox","Path":"C:\\Browsers\\Firefox.exe","ExcludeGroups":["local"],'
        $config += '"Patterns":["https://example.com/first*","https://example.com/second*"],"Excludes":["https://example.com/internal/*"]}]}}'
        Test-Exchange 'Config response preserves rules and filters disabled sections' (New-Request '"C edge"') 0 -Response $config

        [IO.File]::WriteAllText($rulefile.FullName, '')
        Test-Exchange 'Config request with an empty rule file' (New-Request '"C edge"') 0 "cannot read $($rulefile.FullName)"
    }
    finally {
        if ($registered) {
            $key.DeleteValue('Rulefile')
        }
        if ($null -ne $key) {
            $key.Dispose()
        }
        $machine.Dispose()
        Remove-Item -LiteralPath $rulefile.FullName
    }
}
if ($hasExtensionExecfile) {
    Write-Host 'SKIP Missing ExtensionExecfile because a redirect executable is configured'
} else {
    Test-Exchange 'Query request with missing ExtensionExecfile' (New-Request '"Q edge https://example.com/"') 0 'cannot read SOFTWARE\ThinBridge (2)'
}

# A local tab policy may display a warning or change the response.
$policy = Get-ItemProperty -LiteralPath 'HKCU:\SOFTWARE\Policies\ThinBridge\RCAP' -ErrorAction SilentlyContinue
if ($policy.EnableTabLimit) {
    Write-Host 'SKIP Resource response because the current user has tab limits enabled'
} else {
    Test-Exchange 'Framed resource response' (New-Request '"R edge 0"') 0 -Response '{"status":"OK","closeTab":0}'
}

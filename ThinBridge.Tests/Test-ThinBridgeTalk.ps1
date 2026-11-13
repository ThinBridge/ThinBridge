param(
    [Parameter(Mandatory)]
    [string]$Executable,
    [switch]$Hooks
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
    [string]$Response = '',
    [switch]$FailLaunch,
    [hashtable]$Fixture = @{}
) {
    $start = [Diagnostics.ProcessStartInfo]::new($Executable)
    $start.UseShellExecute = $false
    $start.CreateNoWindow = $true
    $start.RedirectStandardInput = $true
    $start.RedirectStandardOutput = $true
    $start.RedirectStandardError = $true
    if ($Hooks) {
        $start.Environment['THINBRIDGE_TEST_FAIL_LAUNCH'] = if ($FailLaunch) { '1' } else { '0' }
        foreach ($fixtureName in @('RULEFILE', 'POLICY', 'WARNING_BUSY', 'NO_EXECFILE')) {
            [void]$start.Environment.Remove("THINBRIDGE_TEST_$fixtureName")
        }
        foreach ($fixtureName in $Fixture.Keys) {
            $start.Environment["THINBRIDGE_TEST_$fixtureName"] = $Fixture[$fixtureName]
        }
    }
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

function Test-ConfigResponses([string]$Rulefile) {
    # BrowserSelector e7c170c records the 8B 0A 00 00 response header.
    $prefix = '{"status":"OK","config":{"CloseEmptyTab":1,"OnlyMainFrame":0,"IgnoreQueryString":0,"DefaultBrowser":"","Sections":['
    $prefix += '{"Name":"firefox","Path":"","ExcludeGroups":[],"Patterns":["'
    $suffix = '"],"Excludes":[]}]}}'
    $url = 'https://example.com/'
    $url += 'a' * (0x0A8B - ($prefix + $url + $suffix).Length)
    [IO.File]::WriteAllText($Rulefile, "[Firefox]`r`n$url`r`n", [Text.Encoding]::ASCII)
    Test-Exchange 'Binary stdout length 2699' (New-Request '"C edge"') 0 -Response ($prefix + $url + $suffix) -Fixture @{ RULEFILE = $Rulefile }

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
    [IO.File]::WriteAllText($Rulefile, $rules, [Text.Encoding]::ASCII)
    $config = '{"status":"OK","config":{"CloseEmptyTab":1,"OnlyMainFrame":1,"IgnoreQueryString":1,"DefaultBrowser":"edge","Sections":['
    $config += '{"Name":"firefox","Path":"C:\\Browsers\\Firefox.exe","ExcludeGroups":["local"],'
    $config += '"Patterns":["https://example.com/first*","https://example.com/second*"],"Excludes":["https://example.com/internal/*"]}]}}'
    Test-Exchange 'Config response preserves rules and filters disabled sections' (New-Request '"C edge"') 0 -Response $config -Fixture @{ RULEFILE = $Rulefile }

    [IO.File]::WriteAllText($Rulefile, '')
    Test-Exchange 'Config request with an empty rule file' (New-Request '"C edge"') 0 "cannot read $Rulefile" -Fixture @{ RULEFILE = $Rulefile }
}

if ($Hooks) {
    $path = 'C:\ThinBridge Test\ThinBridge.exe'
    $url = 'https://example.com/a b?x=1&y=2'
    $command = ' "' + $url + '"'
    $diagnostic = "path=$path`r`ncommand=$command`r`n"
    $edgeLaunch = $diagnostic + "flags=512`r`ninherit=0`r`n"
    $firefoxLaunch = $diagnostic + "flags=16777216`r`ninherit=0`r`n"
    Test-Exchange 'Q launches Edge request in a new process group' (New-Request ('"Q edge ' + $url + '"')) 0 $edgeLaunch '{"status":"OK"}'
    Test-Exchange 'Q launches Firefox request outside the job' (New-Request ('"Q firefox ' + $url + '"')) 0 $firefoxLaunch '{"status":"OK"}'
    Test-Exchange 'Q launch failure emits no response' (New-Request ('"Q edge ' + $url + '"')) 0 ($edgeLaunch + "cannot exec '$path $command' (2)") -FailLaunch
    Test-Exchange 'Q missing executable emits no response' (New-Request ('"Q edge ' + $url + '"')) 0 'cannot read SOFTWARE\ThinBridge (2)' -Fixture @{ NO_EXECFILE = '1' }
    $emptyCommand = ' ""'
    $emptyLaunch = "path=$path`r`ncommand=$emptyCommand`r`nflags=512`r`ninherit=0`r`n"
    Test-Exchange 'Q accepts an empty URL' (New-Request '"Q edge "') 0 $emptyLaunch '{"status":"OK"}'

    Test-Exchange 'C missing rulefile' (New-Request '"C edge"') 0 'cannot read SOFTWARE\ThinBridge (2)'
    $rulefile = New-TemporaryFile
    try {
        Test-ConfigResponses $rulefile.FullName
        Remove-Item -LiteralPath $rulefile.FullName
        Test-Exchange 'C configured file does not exist' (New-Request '"C edge"') 0 "cannot read $($rulefile.FullName)" -Fixture @{ RULEFILE = $rulefile.FullName }
    }
    finally {
        if (Test-Path -LiteralPath $rulefile.FullName) {
            Remove-Item -LiteralPath $rulefile.FullName
        }
    }

    $keepTab = '{"status":"OK","closeTab":0}'
    $closeTab = '{"status":"OK","closeTab":1}'
    $policy = @{ POLICY = '1' }
    $warning = "warning=Warning`r`nTab count`r`ncaption=ThinBridge`r`nflags=4144`r`nseconds=3`r`n"
    $limit = "warning=Limit reached`r`ncaption=ThinBridge`r`nflags=4144`r`nseconds=7`r`n"
    Test-Exchange 'R without a tab policy' (New-Request '"R edge 11"') 0 -Response $keepTab
    Test-Exchange 'R with tab policy explicitly disabled' (New-Request '"R edge 11"') 0 -Response $keepTab -Fixture @{ POLICY = '0' }
    Test-Exchange 'R below warning threshold' (New-Request '"R edge 7"') 0 -Response $keepTab -Fixture $policy
    Test-Exchange 'R at warning threshold' (New-Request '"R edge 8"') 0 $warning $keepTab -Fixture $policy
    Test-Exchange 'R at maximum still warns without closing' (New-Request '"R edge 10"') 0 $warning $keepTab -Fixture $policy
    Test-Exchange 'R above maximum requests tab closure' (New-Request '"R edge 11"') 0 $limit $closeTab -Fixture $policy
    Test-Exchange 'R suppresses busy warning but still requests closure' (New-Request '"R edge 11"') 0 -Response $closeTab -Fixture @{ POLICY = '1'; WARNING_BUSY = '1' }
    return
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

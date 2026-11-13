param(
    [Parameter(Mandatory)]
    [string]$Executable
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

# A local tab policy may display a warning or change the response.
$policy = Get-ItemProperty -LiteralPath 'HKCU:\SOFTWARE\Policies\ThinBridge\RCAP' -ErrorAction SilentlyContinue
if ($policy.EnableTabLimit) {
    Write-Host 'SKIP Resource response because the current user has tab limits enabled'
} else {
    Test-Exchange 'Framed resource response' (New-Request '"R edge 0"') 0 -Response '{"status":"OK","closeTab":0}'
}

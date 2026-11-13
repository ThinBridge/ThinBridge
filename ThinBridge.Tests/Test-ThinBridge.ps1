param(
    [Parameter(Mandatory)]
    [string]$Executable
)

$ErrorActionPreference = 'Stop'
$Executable = (Resolve-Path -LiteralPath $Executable).ProviderPath

function Test-App([string]$Name, [string[]]$Arguments, [string[]]$Expected,
    [string]$LoopLog, [switch]$SignalEvent) {
    $start = [Diagnostics.ProcessStartInfo]::new($Executable)
    $start.UseShellExecute = $false
    $start.RedirectStandardOutput = $true
    $start.StandardOutputEncoding = [Text.Encoding]::UTF8
    foreach ($argument in $Arguments) {
        $start.ArgumentList.Add($argument)
    }
    [void]$start.Environment.Remove('THINBRIDGE_TEST_LOOP_LOG')
    [void]$start.Environment.Remove('THINBRIDGE_TEST_LOOP_EVENT')
    if ($LoopLog) { $start.Environment['THINBRIDGE_TEST_LOOP_LOG'] = $LoopLog }
    if ($SignalEvent) { $start.Environment['THINBRIDGE_TEST_LOOP_EVENT'] = '1' }

    $process = [Diagnostics.Process]::Start($start)
    try {
        $output = $process.StandardOutput.ReadToEndAsync()
        if (-not $process.WaitForExit(10000)) {
            $process.Kill()
            throw "$Name timed out"
        }
        $actual = $output.GetAwaiter().GetResult()
        $expectedOutput = ($Expected -join "`r`n") + "`r`n"
        if ($process.ExitCode -ne 0 -or $actual -cne $expectedOutput) {
            throw "$Name returned exit code $($process.ExitCode) and output '$actual'"
        }
        Write-Host "PASS $Name"
    }
    finally {
        $process.Dispose()
    }
}

Test-App 'Empty arguments' @() @('', '', '')
Test-App 'Quoted drive path with spaces' @('  "C:\Program Files\page.html"  ') @('C:\Program Files\page.html', '', '')
Test-App 'Single switch' @('/settings') @('', '/settings', '')
Test-App 'Quoted URL preserves internal spaces and query' @('  "https://example.com/a b?q=hello world#part"  ') @('https://example.com/a b?q=hello world#part', '', '')
Test-App 'Unrecognized single argument' @('relative.html') @('', '', '')
Test-App 'Browser option preserves case and Unicode URL' @('eDgE', '  HTTPS://example.com/日本語?q=1#part  ') @('HTTPS://example.com/日本語?q=1#part', 'eDgE', '')
Test-App 'Drive path in second position replaces option' @('Edge', 'C:\page.html') @('', 'C:\page.html', '')
Test-App 'Third URL replaces earlier URLs' @('https://one.example', 'https://two.example', 'https://three.example') @('https://three.example', '', '')
Test-App 'Custom path and ignored fourth argument' @('https://example.com', 'Custom', 'C:\Browser App\browser.exe', 'https://ignored.example') @('https://example.com', 'Custom', 'C:\Browser App\browser.exe')

$log = New-TemporaryFile
try {
    $url = 'https://example.com/'
    Test-App 'Empty loop history permits redirect' @($url) @('0') -LoopLog $log.FullName
    Test-App 'One previous redirect permits redirect' @($url) @('0') -LoopLog $log.FullName
    Test-App 'Two previous redirects still permit redirect' @($url) @('0') -LoopLog $log.FullName
    Test-App 'Three previous redirects block redirect' @($url) @('1') -LoopLog $log.FullName
    Test-App 'Recent block suppresses duplicate warning' @($url) @('2') -LoopLog $log.FullName

    [IO.File]::WriteAllText($log.FullName, "$url`t56500`tBLOCK`r`n")
    Test-App 'Duplicate warning boundary includes 3500 ms' @($url) @('2') -LoopLog $log.FullName
    [IO.File]::WriteAllText($log.FullName, "$url`t56499`tBLOCK`r`n")
    Test-App 'Block older than 3500 ms does not suppress redirect' @($url) @('0') -LoopLog $log.FullName

    [IO.File]::WriteAllText($log.FullName, ("$url`t45001`tOK`r`n" * 3))
    Test-App 'Matching history just inside 15 seconds blocks' @($url) @('1') -LoopLog $log.FullName
    [IO.File]::WriteAllText($log.FullName, ("$url`t45000`tOK`r`n" * 3))
    Test-App 'Matching history at 15 seconds expires' @($url) @('0') -LoopLog $log.FullName
    [IO.File]::WriteAllText($log.FullName, ("https://other.example/`t60000`tOK`r`n" * 3))
    Test-App 'Recent history for a different URL does not block' @($url) @('0') -LoopLog $log.FullName

    $history = "$url`t60000`tBLOCK`r`n"
    [IO.File]::WriteAllText($log.FullName, $history)
    Test-App 'Signaled event bypasses loop checks' @($url) @('0') -LoopLog $log.FullName -SignalEvent
    if ([IO.File]::ReadAllText($log.FullName) -cne $history) {
        throw 'Signaled event unexpectedly changed the loop history'
    }
}
finally {
    Remove-Item -LiteralPath $log.FullName
}

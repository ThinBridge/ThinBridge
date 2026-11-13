param(
    [Parameter(Mandatory)]
    [string]$Executable
)

$ErrorActionPreference = 'Stop'
$Executable = (Resolve-Path -LiteralPath $Executable).ProviderPath

function Test-App([string]$Name, [string[]]$Arguments, [string[]]$Expected,
    [string]$LoopLog, [switch]$SignalEvent, [switch]$Route) {
    $start = [Diagnostics.ProcessStartInfo]::new($Executable)
    $start.UseShellExecute = $false
    $start.RedirectStandardOutput = $true
    $start.StandardOutputEncoding = [Text.Encoding]::UTF8
    foreach ($argument in $Arguments) {
        $start.ArgumentList.Add($argument)
    }
    [void]$start.Environment.Remove('THINBRIDGE_TEST_LOOP_LOG')
    [void]$start.Environment.Remove('THINBRIDGE_TEST_LOOP_EVENT')
    [void]$start.Environment.Remove('THINBRIDGE_TEST_ROUTE')
    if ($LoopLog) { $start.Environment['THINBRIDGE_TEST_LOOP_LOG'] = $LoopLog }
    if ($SignalEvent) { $start.Environment['THINBRIDGE_TEST_LOOP_EVENT'] = '1' }
    if ($Route) { $start.Environment['THINBRIDGE_TEST_ROUTE'] = '1' }

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

$url = 'https://example.com/a b?q=1#part'
$quotedUrl = '"' + $url + '"'
$chrome = @(
    "ShellExecute|open|chrome.exe|$quotedUrl"
    "ShellExecute||chrome.exe $quotedUrl|"
    "ShellExecute||`"C:\Browser Test\chrome.exe`" $quotedUrl|"
    "ShellExecute||`"C:\Program Files\Google\Chrome\Application\chrome.exe`" $quotedUrl|"
    "ShellExecute||`"C:\Program Files (x86)\Google\Chrome\Application\chrome.exe`" $quotedUrl|"
)
Test-App 'Chrome success stops at the first launch' @('--test-launch', 'chrome', $url, '1') @($chrome[0])
Test-App 'Chrome fallback stops at the configured path on success' @('--test-launch', 'chrome', $url, '3') $chrome[0..2]
Test-App 'Chrome falls back through both Program Files paths' @('--test-launch', 'chrome', $url, '0') $chrome
Test-App 'Chrome launches without URL arguments when empty' @('--test-launch', 'chrome', '', '1') @('ShellExecute|open|chrome.exe|')

$firefox = @(
    "ShellExecute|open|firefox.exe|$quotedUrl"
    "ShellExecute||firefox.exe $url|"
    "ShellExecute||`"C:\Browser Test\firefox.exe`" $quotedUrl|"
    "ShellExecute||`"C:\Program Files (x86)\Mozilla Firefox\firefox.exe`" $quotedUrl|"
    "ShellExecute||`"C:\Program Files\Mozilla Firefox\firefox.exe`" $quotedUrl|"
)
Test-App 'Firefox retries with the existing fallback argument format' @('--test-launch', 'firefox', $url, '0') $firefox
Test-App 'Edge uses the microsoft-edge protocol' @('--test-launch', 'edge', $url, '0') @("ShellExecute||microsoft-edge:$url|")
Test-App 'IE without DDE first uses its executable name' @('--test-launch', 'ie', $url, '1') @("ShellExecute|open|iexplore.exe|$quotedUrl")
Test-App 'Default browser fallback tries Explorer and IE' @('--test-launch', 'default', $url, '0') @(
    "ShellExecute||$quotedUrl|"
    "ShellExecute|open|$quotedUrl|"
    "ShellExecute||explorer.exe|$quotedUrl"
    "ShellExecute|open|iexplore.exe|$quotedUrl"
)
$customPath = '"C:\Browser Test\custom.exe"'
$custom = @(
    "CreateProcess|||$customPath $quotedUrl"
    "CreateProcess||$customPath|$quotedUrl"
    "ShellExecute|open|$customPath|$quotedUrl"
    "ShellExecute||$customPath $quotedUrl|"
)
Test-App 'Custom browser stops after a successful process retry' @('--test-launch', 'custom', $url, '2') $custom[0..1]
Test-App 'Custom browser falls back from CreateProcess to ShellExecute' @('--test-launch', 'custom', $url, '0') $custom

$rdpPath = 'C:\Remote Session\session.rdp'
Test-App 'RDP quotes the file path in the first MSTSC command' @('--test-launch', 'rdp', $rdpPath, '1') @(
    "CreateProcess|||`"mstsc.exe`" `"$rdpPath`""
)
Test-App 'VMware percent-encodes the app name and complete URL' @('--test-launch', 'vmware', $url, '1') @(
    'ShellExecute||vmware-view://horizon.example/Remote%20App?url=https%3A%2F%2Fexample.com%2Fa%20b%3Fq%3D1%23part|'
)
Test-App 'VMware omits the URL parameter when empty' @('--test-launch', 'vmware', '', '1') @(
    'ShellExecute||vmware-view://horizon.example/Remote%20App|'
)
$citrix = '"c:\program files\Citrix\ICA Client\SelfServicePlugin\SelfService.exe"'
Test-App 'Citrix quotes the published app name and URL' @('--test-launch', 'citrix', $url, '1') @(
    "CreateProcess|||$citrix -qlaunch `"Published App`" $quotedUrl"
)
Test-App 'Citrix omits the URL argument when empty' @('--test-launch', 'citrix', '', '1') @(
    "CreateProcess|||$citrix -qlaunch `"Published App`""
)

$rdpFile = New-TemporaryFile
try {
    foreach ($argument in @($url, '')) {
        Test-App 'Generate RemoteApp file' @('--test-rdp-file', $rdpFile.FullName, $argument) @('RDP file generated')
        $command = if ($argument) { "/open $argument" } else { '/open' }
        $expected = @(
            'full address:s:rdp.example'
            'remoteapplicationmode:i:1'
            'remoteapplicationname:s:Remote App'
            'remoteapplicationprogram:s:C:\Remote App\app.exe'
            'alternate shell:s:C:\Remote App\app.exe'
            "remoteapplicationcmdline:s:$command"
            'redirectclipboard:i:1'
            'redirectprinters:i:0'
            'RedirectDrives:i:1'
            'screen mode id:i:2'
            'prompt for credentials:i:1'
        )
        if ([IO.File]::ReadAllText($rdpFile.FullName) -cne (($expected -join "`r`n") + "`r`n")) {
            throw 'Unexpected RemoteApp file contents'
        }
    }
}
finally {
    Remove-Item -LiteralPath $rdpFile.FullName
}

$routes = [ordered]@{ RDP = '0'; VMware = '1'; Citrix = '2'; Default = '3'; IE = '4'; Firefox = '5'; Chrome = '6'; eDgE = '7'; Custom = '8' }
foreach ($option in $routes.Keys) {
    Test-App "Startup selects $option" @($option, $url) @($routes[$option], '') -Route
}
Test-App 'Startup keeps configured route without an explicit option' @($url) @('0', '') -Route
Test-App 'Startup resolves a bare custom executable relative to ThinBridge' @($url, 'ThinBridgeTestBrowser.exe') @('8', 'C:\ThinBridge Test\ThinBridgeTestBrowser.exe') -Route

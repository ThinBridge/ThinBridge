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
Test-App 'Empty quoted argument is ignored' @(' "" ') @('', '', '')
Test-App 'Single dash option retains its spelling' @('-SETTINGS') @('', '-SETTINGS', '')
Test-App 'Quoted URL preserves internal spaces and query' @('  "https://example.com/a b?q=hello world#part"  ') @('https://example.com/a b?q=hello world#part', '', '')
Test-App 'Unrecognized single argument' @('relative.html') @('', '', '')
Test-App 'Browser option preserves case and Unicode URL' @('eDgE', '  HTTPS://example.com/日本語?q=1#part  ') @('HTTPS://example.com/日本語?q=1#part', 'eDgE', '')
Test-App 'Drive path in second position replaces option' @('Edge', 'C:\page.html') @('', 'C:\page.html', '')
Test-App 'Third URL replaces earlier URLs' @('https://one.example', 'https://two.example', 'https://three.example') @('https://three.example', '', '')
Test-App 'An empty second argument preserves the first URL' @('https://one.example', '') @('https://one.example', '', '')
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
    [IO.File]::WriteAllText($log.FullName, ("incomplete record`r`n$url`t60000`tOK`r`n" * 2))
    Test-App 'Incomplete history records do not count toward the loop threshold' @($url) @('0') -LoopLog $log.FullName

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
$longUrl = 'https://example.com/日本語/😀?q=%22#' + ('a' * 4096)
Test-App 'Startup preserves a long URL with supplementary Unicode' @($longUrl) @($longUrl, '', '')
Test-App 'Chrome passes the complete long Unicode URL as one quoted argument' @('--test-launch', 'chrome', $longUrl, '1') @(
    "ShellExecute|open|chrome.exe|`"$longUrl`""
)
Test-App 'IE without DDE first uses its executable name' @('--test-launch', 'ie', $url, '1') @("ShellExecute|open|iexplore.exe|$quotedUrl")
$ddeUrl = 'https://example.com/a,b 日本語?q=x,y#part'
Test-App 'IE DDE preserves commas and Unicode in the quoted URL' @('--test-launch', 'ie-dde', $ddeUrl, '0') @(
    "DdeClientTransaction|XTYP_EXECUTE||`"$ddeUrl`""
)
Test-App 'IE falls back to ShellExecute when DDE cannot connect' @('--test-launch', 'ie-dde-unavailable', $url, '1') @(
    "ShellExecute|open|iexplore.exe|$quotedUrl"
)
Test-App 'IE exhausts its executable fallbacks after DDE fails' @('--test-launch', 'ie-dde-unavailable', $url, '0') @(
    "ShellExecute|open|iexplore.exe|$quotedUrl"
    "ShellExecute||iexplorer.exe $quotedUrl|"
    "ShellExecute||`"C:\Browser Test\iexplore.exe`" $quotedUrl|"
    "ShellExecute||`"C:\Program Files (x86)\Internet Explorer\iexplore.exe`" $quotedUrl|"
    "ShellExecute||`"C:\Program Files\Internet Explorer\iexplore.exe`" $quotedUrl|"
)
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
Test-App 'RDP starts MSTSC without a file argument when the path is empty' @('--test-launch', 'rdp', '', '1') @(
    'CreateProcess|||"mstsc.exe"'
)
Test-App 'RDP falls back from CreateProcess to ShellExecute and file association' @('--test-launch', 'rdp', $rdpPath, '0') @(
    "CreateProcess|||`"mstsc.exe`" `"$rdpPath`""
    "CreateProcess||mstsc.exe|$rdpPath"
    "ShellExecute|open|mstsc.exe|$rdpPath"
    "ShellExecute||$rdpPath|"
)
Test-App 'VMware percent-encodes the app name and complete URL' @('--test-launch', 'vmware', $url, '1') @(
    'ShellExecute||vmware-view://horizon.example/Remote%20App?url=https%3A%2F%2Fexample.com%2Fa%20b%3Fq%3D1%23part|'
)
Test-App 'VMware omits the URL parameter when empty' @('--test-launch', 'vmware', '', '1') @(
    'ShellExecute||vmware-view://horizon.example/Remote%20App|'
)
Test-App 'VMware percent-encodes UTF-8 and existing percent escapes' @('--test-launch', 'vmware', 'https://example.com/日本語/😀?q=%22', '1') @(
    'ShellExecute||vmware-view://horizon.example/Remote%20App?url=https%3A%2F%2Fexample.com%2F%E6%97%A5%E6%9C%AC%E8%AA%9E%2F%F0%9F%98%80%3Fq%3D%2522|'
)
$vmwareUri = 'vmware-view://horizon.example/Remote%20App?url=https%3A%2F%2Fexample.com%2Fa%20b%3Fq%3D1%23part'
Test-App 'VMware registered protocol stops after successful open' @('--test-launch', 'vmware-registered', $url, '1') @(
    "ShellExecute|open|$vmwareUri|"
)
Test-App 'VMware retries without the open verb after failure' @('--test-launch', 'vmware-registered', $url, '0') @(
    "ShellExecute|open|$vmwareUri|"
    "ShellExecute||$vmwareUri|"
)
$citrix = '"c:\program files\Citrix\ICA Client\SelfServicePlugin\SelfService.exe"'
Test-App 'Citrix quotes the published app name and URL' @('--test-launch', 'citrix', $url, '1') @(
    "CreateProcess|||$citrix -qlaunch `"Published App`" $quotedUrl"
)
Test-App 'Citrix omits the URL argument when empty' @('--test-launch', 'citrix', '', '1') @(
    "CreateProcess|||$citrix -qlaunch `"Published App`""
)
$registeredCitrix = '"C:\Remote Test\SelfService.exe"'
$citrixArguments = "-qlaunch `"Published App`" $quotedUrl"
Test-App 'Citrix retries the registered launcher before using ShellExecute' @('--test-launch', 'citrix-registered', $url, '0') @(
    "CreateProcess|||$registeredCitrix $citrixArguments"
    "CreateProcess||$registeredCitrix|$citrixArguments"
    "ShellExecute|open|$registeredCitrix|$citrixArguments"
    "ShellExecute||$registeredCitrix $citrixArguments|"
)

$rdpFile = New-TemporaryFile
try {
    $rdpCases = @(
        @{ Name = 'RemoteApp combines the configured command and URL'; Mode = 'remote-app'; Argument = $url; Command = "/open $url" }
        @{ Name = 'RemoteApp uses the configured command alone'; Mode = 'remote-app'; Argument = ''; Command = '/open' }
        @{ Name = 'RemoteApp uses the URL alone'; Mode = 'no-default-command'; Argument = $url; Command = $url }
        @{ Name = 'RemoteApp omits an empty command'; Mode = 'no-default-command'; Argument = '' }
        @{ Name = 'Desktop mode omits all RemoteApp settings'; Mode = 'desktop'; Argument = $url }
    )
    foreach ($case in $rdpCases) {
        Test-App $case.Name @('--test-rdp-file', $rdpFile.FullName, $case.Argument, $case.Mode) @('RDP file generated')
        $remoteApp = $case.Mode -ne 'desktop'
        $expected = @(
            'full address:s:rdp.example'
            "remoteapplicationmode:i:$([int]$remoteApp)"
            if ($remoteApp) {
                'remoteapplicationname:s:Remote App'
                'remoteapplicationprogram:s:C:\Remote App\app.exe'
                'alternate shell:s:C:\Remote App\app.exe'
                if ($case.Command) {
                    "remoteapplicationcmdline:s:$($case.Command)"
                }
            }
            'redirectclipboard:i:1'
            'redirectprinters:i:0'
            'RedirectDrives:i:1'
            'screen mode id:i:2'
            'prompt for credentials:i:1'
        )
        if ([IO.File]::ReadAllText($rdpFile.FullName) -cne (($expected -join "`r`n") + "`r`n")) {
            throw "$($case.Name) produced unexpected RDP file contents"
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
Test-App 'Startup keeps the configured route when only a switch is present' @('/settings') @('0', '') -Route
Test-App 'Missing DisableSetting policy leaves settings enabled' @('--test-setting-policy', 'missing') @('0')
Test-App 'Zero DisableSetting policy leaves settings enabled' @('--test-setting-policy', '0') @('0')
Test-App 'Nonzero DisableSetting policy disables settings' @('--test-setting-policy', '2') @('1')
Test-App 'Startup clears an unresolved traversal-style custom path' @($url, '..\ThinBridgeTestBrowser.exe') @('8', '') -Route

$customFile = New-TemporaryFile
try {
    Test-App 'Startup retains an existing absolute custom path' @($url, $customFile.FullName) @('8', $customFile.FullName) -Route
}
finally {
    Remove-Item -LiteralPath $customFile.FullName
}

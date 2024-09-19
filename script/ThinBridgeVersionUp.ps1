<#
	.SYNOPSIS
		Replace the current version number and GUID of ThinBridge with new ones.
	.PARAMETER new_version
		ThinBridgeVersionUp [new_version]
	.EXAMPLE
		ThinBridgeVersionUp
		with no parameter to get the current version
	.EXAMPLE
		ThinBridgeVersionUp x.y.z.b
		update the version number to x.y.z.b
#>
# Define parameter
Param (
	[ValidatePattern('^\d{1,2}\.\d{1,2}\.\d{1,2}\.\d{1,2}$')]
	[string]$new_version = 'no_value'
)

$THIN_BRIDGE_ROOT_PATH = if ([string]::IsNullOrEmpty($Env:ThinBridgeVersionUpRootPath)) { ".." } else { $Env:ThinBridgeVersionUpRootPath }

$TARGET_PATH_ARRAY = ($THIN_BRIDGE_ROOT_PATH + "/ThinBridge/ThinBridge.rc"),
	($THIN_BRIDGE_ROOT_PATH + "/BHORedirector/BHORedirector.rc"),
	($THIN_BRIDGE_ROOT_PATH + "/TBo365URLSync/TBo365URLSync.rc"),
	($THIN_BRIDGE_ROOT_PATH + "/TBRedirector/TBRedirector.rc"),
	($THIN_BRIDGE_ROOT_PATH + "/ThinBridgeChecker/ThinBridgeChecker.rc"),
	($THIN_BRIDGE_ROOT_PATH + "/ThinBridgeRuleUpdater/ThinBridgeRuleUpdater.rc"),
	($THIN_BRIDGE_ROOT_PATH + "/ThinBridgeX64.iss"),
	($THIN_BRIDGE_ROOT_PATH + "/ThinBridgeX86.iss")

Set-Variable -name VERSION_PATTERN -value '[" ]\d{1,2}[,\.]\d{1,2}[,\.]\d{1,2}[,\.]\d{1,2}' -Option Constant
Set-Variable -name VERSION_PERIOD -value '\d{1,2}\.\d{1,2}\.\d{1,2}\.\d{1,2}' -Option Constant
Set-Variable -name VERSION_COMMA -value '\d{1,2},\d{1,2},\d{1,2},\d{1,2}' -Option Constant

# Define Variables
$new_version_comma = $new_version -Replace '\.',','
$new_version_plus_quote = $new_version + '"'
$new_version_plus_period = $new_version + '.'

# 1. function to extract the lines including the version number
function Extract-current_version() {
	ForEach ($item in $TARGET_PATH_ARRAY) {
		Get-Content -Path $item | Select-String -Pattern $VERSION_PATTERN
	}
}

# 2. function to update the version number and GUID
function Update-version_number($new_version) {
	ForEach ($item in $TARGET_PATH_ARRAY) {
		$encoding = "Unicode"
		if (($item -like "*.iss")) {
			$encoding = "UTF8"
		}
		$replaced_content = (Get-Content -Path $item -Encoding $encoding) |
			ForEach-Object {
				if ($_ -like "*version*") {
					$_ -Replace $VERSION_PERIOD, $new_version
				} else {
					$_
				}
			} |
			ForEach-Object {
				if (($_ -like "*version*") -and ($_ -notlike "*IDC_STATIC_VER*")) {
					$_ -Replace $VERSION_COMMA, $new_version_comma
				} else {
					$_
				}
			}
		Set-Content -Path $item -Value $replaced_content -Encoding $encoding
		# Show the result of replacement on the screen
		Select-String -Path $item -Pattern $VERSION_PATTERN
		Write-Host `n
	}
}

# Switch what to do depending on the parameter value
switch ($new_version) {
	'no_value' {Extract-current_version}
	Default {Update-version_number $new_version}
}

@echo off
setlocal
cd %~dp0

REG ADD HKLM\Software\ThinBridge /f /t REG_SZ /v RuleFile /d "%~dp0\ThinBridgeBHO.ini" /reg:32
REG ADD HKLM\Software\ThinBridge /f /t REG_SZ /v ExtensionExecfile /d "%PROGRAMFILES%\Internet Explorer\iexplore.exe" /reg:32
REG ADD HKLM\Software\Microsoft\Edge\NativeMessagingHosts\com.clear_code.thinbridge /f /t REG_SZ /ve /d "%~dp0\edge.json" /reg:32
REG ADD HKLM\Software\Google\Chrome\NativeMessagingHosts\com.clear_code.thinbridge /f /t REG_SZ /ve /d "%~dp0\chrome.json" /reg:32
REG ADD HKLM\Software\Mozilla\NativeMessagingHosts\com.clear_code.thinbridge /f /t REG_SZ /ve /d "%~dp0\firefox.json" /reg:32

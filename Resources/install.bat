@echo off
setlocal
cd %~dp0

REG ADD HKLM\Software\ThinBridge /f /t REG_SZ /v RuleFile /d "%~dp0\ThinBridgeBHO.ini" /reg:32
REG ADD HKLM\Software\ThinBridge /f /t REG_SZ /v ExtensionExecfile /d "%~dp0\ThinBridge.exe" /reg:32
REG ADD HKLM\Software\Microsoft\Edge\NativeMessagingHosts\com.clear_code.thinbridge /f /t REG_SZ /ve /d "%~dp0\edge.json" /reg:32

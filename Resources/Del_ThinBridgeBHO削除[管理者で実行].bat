@echo off
setlocal
cd %~dp0

:checkMandatoryLevel
for /f "tokens=1 delims=," %%i in ('whoami /groups /FO CSV /NH') do (
    if "%%~i"=="BUILTIN\Administrators" set ADMIN=yes
    if "%%~i"=="Mandatory Label\High Mandatory Level" set ELEVATED=yes
)

if "%ADMIN%" neq "yes" (
   echo このファイルは管理者権限での実行が必要です[Administratorsグループに未所属]
   if "%1" neq "/R" goto runas
   goto exit1
)
if "%ELEVATED%" neq "yes" (
   echo このファイルは管理者権限での実行が必要です[プロセスを昇格が必要]
   if "%1" neq "/R" goto runas
   goto exit1
)

:admins
  echo ThinBridgeBHOの設定を行います。
  regsvr32 ThinBridgeBHO.dll /u
  if "%PROCESSOR_ARCHITECTURE%" NEQ "x86" (
    regsvr32 ThinBridgeBHO64.dll /u
  )
  echo ThinBridgeBHOの設定が完了しました。
  goto exit1

:runas
    powershell -Command Start-Process -Verb runas """%0""" -ArgumentList "/R"
:exit1


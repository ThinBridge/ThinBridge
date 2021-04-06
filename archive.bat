@echo off
setlocal
cd %~dp0

REM A helper script to create a release archive ("ThinBridge.zip").
REM
REM USAGE:
REM
REM   % msbuild /p:configuration=Release
REM   % .\archive.bat

DEL ThinBridge.zip
RD /s /q dist
MKDIR dist

COPY Release\CustomLauncher.exe    dist
COPY Release\HorizonLauncher.exe   dist
COPY Release\ThinBridgeSetting.exe dist
COPY Release\ThinBridge.exe        dist
COPY Release\ThinBridgeBHO.dll     dist
COPY Release\ThinBridgeTalk.exe    dist
COPY Resources\THinBridge.ini      dist
COPY Resources\THinBridgeBHO.ini   dist
COPY Resources\edge.json           dist
COPY Resources\firefox.json        dist
COPY Resources\setting.conf        dist
COPY Resources\install.bat         dist

powershell -Command "Compress-Archive -Path dist\*.* -DestinationPath ThinBridge.zip"

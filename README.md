ThinBridge
==========

ThinBridge is an enterprise browser launcher for Windows. It has the following features:

 * Switch between browsers based on a customizable URL list.
 * Support IE/Edge/Firefox/Chrome
 * Support Remote Desktop, VMWare Horizon and Citrix.

How to Install
--------------

Download the latest release archive from the release page:

https://gitlab.com/clear-code/thinbridge/-/releases

Unzip the archive:

    % powershell -Command "Expand-Archive ThinBridge.zip -Dest C:\ThinBridge"

Exec `install.bat` for system registration:

    % C:\ThinBridge\install.bat

Development
-----------

1. Install Visual Studio 2010 (make sure to select MFC and ATL)

2. Execute the following:

   % msbuild /p:configuration=Release

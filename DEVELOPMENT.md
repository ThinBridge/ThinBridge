# Development Guidelines

* Branch guidelines
* Issue guidelines
* Release guidelines

## Branch guidelines

* Use master branch as stable branch
* Use topic branch to implement new features
* Don't directly commit to master branch

## Issue and Pull request guidelines

* Create new issue before implement new features
  * Discuss and review specification beforehand
* Create new pull request corresponding to existing issue
* Each pull request must be reviewed
* Assign bug, enhancement or breaking-change label to pull request

## Release guidelines

See https://github.com/ThinBridge/ThinBridge#thinbridge%E3%81%AE%E3%83%AA%E3%83%AA%E3%83%BC%E3%82%B9%E6%89%8B%E9%A0%86

* Create a new tag vN.N.N.N (such as v4.2.0.0)
* Create new issue for release
* Create new release via https://github.com/ThinBridge/ThinBridge/releases/new
* Test assets for release
* Publish release note

## How to try extensions for development

ThinBridge addon depends on the native messaging host, which is a part of ThinBridge.
You need to install ThinBridgeSetup_x64.exe downloaded from the release page, like: https://github.com/ThinBridge/ThinBridge/releases/tag/v4.0.2.4

ThinBridge addon works on Chrome and Edge as expected only if the addon is installed via group policy.
(Public addons cannot have `webRequestBlocking` permission, so the addon cannot redirect loading URL to other browsers.)
You need to load development version of addons with following steps, If you are a developer of this addon.

1. Copy the `webextensions` folder of this repository to the client environment.
   Assume that it is placed at `C:\Users\Public\webextensions` .
2. Create a development package for Chrome.
   1. Launch Chrome.
   2. Go to addons manager (`chrome:extensions`).
   3. Activate `Developer mode`.
   4. Choose the folder `webextensions¥chrome` to pack the addon with `Pack extension`. (Then you'll see `chrome.crx` and `chrome.pem` at the upper folder.)
   5. Drag `chrome.crx` and drop onto Chrome's window to install it, and memorise its generated ID. For example: `egoppdngdcoikeknmbgiakconmchahhf`
   6. Uninstall the addon.
   7. Add a URL entry with the generated ID, to the list of `"allowed_origins"` in `C:\Program Files (x86)\ThinBridge\chrome.json`. For example: `"chrome-extension://egoppdngdcoikeknmbgiakconmchahhf/"`
3. Create a development package for Edge.
   1. Launch Edge.
   2. Go to addons manager (`edge:extensions`).
   3. Activate `Developer mode`.
   4. Choose the folder `webextensions¥edge` to pack the addon with `Pack extension`. (Then you'll see `edge.crx` and `edge.pem` at the upper folder.)
   5. Drag `edge.crx` and drop onto Edge's window to install it, and memorise its generated ID. For example: `oapdkmbdgdcjpacbjpcdfhncifimimcj`
   6. Uninstall the addon.
   7. Add a URL entry with the generated ID, to the list of `"allowed_origins"` in `C:\Program Files (x86)\ThinBridge\edge.json`. For example: `"chrome-extension://oapdkmbdgdcjpacbjpcdfhncifimimcj/"`
4. Create the manifest file for installation.
   Put an XML file into the folder `chrome.crx` and `edge.crx` exist in, with following contents including IDs of Chrome addon and Edge addon.
   Assume that it is placed at `C:\Users\Public\webextensions¥manifest.xml`.
   ```xml
   <?xml version='1.0' encoding='UTF-8'?>
   <gupdate xmlns='http://www.google.com/update2/response' protocol='2.0'>
     <app appid='egoppdngdcoikeknmbgiakconmchahhf'><!-- The generated ID of Chrome addon -->
       <updatecheck codebase='file:///C:/Users/Public/webextensions/chrome.crx' version='1.0.0' /><!-- Put real File URL of the `chrome.crx`. -->
     </app>
     <app appid='oapdkmbdgdcjpacbjpcdfhncifimimcj'><!-- The generated ID of Edge addon -->
       <updatecheck codebase='file:///C:/Users/Public/webextensions/edge.crx' version='1.0.0' /><!-- Put real File URL of the `edge.crx`. -->
     </app>
   </gupdate>
   ```
   Please note that you need to synchronize the value of `version` with the one in the `manifest.json`.
4. Install group policy template for Chrome and configure Chrome to load the addon.
   1. Download "Google Chrome Bundle" via https://support.google.com/chrome/a/answer/187202?hl=en#zippy=%2Cwindows and extract contents of the saved zip file.
   2. Copy `Configuration\admx\*.admx`, `Configuration\admx\en-US` and `Configuration\admx\ja` to `C:\Windows\PolicyDefinitions`.
   3. Launch `gpedit.msc` and open `Local Computer Policy` => `Computer Configuration` => `Administrative Templates` => `Google` => `Google Chrome` => `Extensions` => `Configure the list of force-installed apps and extensions`.
   4. Switch to `Enabled`.
   5. Click `Show...`.
   6. Add `<Generated ID of Chrome addon>;<File URL of the manifest XML file>` to the list.
      Assume that it is `egoppdngdcoikeknmbgiakconmchahhf;file:///C:/Users/Public/webextensions/manifest.xml`.
6. Install group policy template for Edge and configure Edge to load the addon.
   1. Download group policy template via the `Get policy files` link in https://www.microsoft.com/en-us/edge/business/download , and extract contents of the saved zip file.
   2. Copy `windows\admx\*.admx`, `windows\admx\en-US` and `windows\admx\ja` to `C:\Windows\PolicyDefinitions`.
   3. Launch `gpedit.msc` and open `Local Computer Policy` => `Computer Configuration` => `Administrative Templates` => `Microsoft Edge` => `Extensions` => `Configure the list of force-installed apps and extensions`.
   4. Switch to `Enabled`.
   5. Click `Show...`.
   6. Add `<Generated ID of Edge addon>;<File URL of the manifest XML file>` to the list.
      Assume that it is `oapdkmbdgdcjpacbjpcdfhncifimimcj;file:///C:/Users/Public/webextensions/manifest.xml`.
7. Set up the client as a domain member if it is not joined to any Active Directory domain. For example, lauch `cmd.exe` as the system administrator and run following commands.
   (Ref: https://hitco.at/blog/apply-edge-policies-for-non-domain-joined-devices/ )
   ```
   reg add HKLM\SOFTWARE\Microsoft\Enrollments\FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF /v EnrollmentState /t reg_dword /d 1 /f
   reg add HKLM\SOFTWARE\Microsoft\Enrollments\FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF /v EnrollmentType /t reg_dword /d 0 /f
   reg add HKLM\SOFTWARE\Microsoft\Enrollments\FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF /v IsFederated /t reg_dword /d 0 /f
   reg add HKLM\SOFTWARE\Microsoft\Provisioning\OMADM\Accounts\FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF /v Flags /t reg_dword /d 0xd6fb7f /f
   reg add HKLM\SOFTWARE\Microsoft\Provisioning\OMADM\Accounts\FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF /v AcctUId /t reg_sz /d "0x000000000000000000000000000000000000000000000000000000000000000000000000" /f
   reg add HKLM\SOFTWARE\Microsoft\Provisioning\OMADM\Accounts\FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF /v RoamingCount /t reg_dword /d 0 /f
   reg add HKLM\SOFTWARE\Microsoft\Provisioning\OMADM\Accounts\FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF /v SslClientCertReference /t reg_sz /d "MY;User;0000000000000000000000000000000000000000" /f
   reg add HKLM\SOFTWARE\Microsoft\Provisioning\OMADM\Accounts\FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF /v ProtoVer /t reg_sz /d "1.2" /f
   ```
8. Launch Chrome and confirm the Chrome addon is automatically installed.
   (You may need to inspect [debug log](#chrome-debug-log).)
9. Launch Edge and confirm the Edge addon is automatically installed.
   (You may need to inspect [debug log](#edge-debug-log).)

You need to update crx packages if you modify codes of addons. Steps:

1. Update new package of Chrome addon.
   1. Launch Chrome.
   2. Go to addons manager `chrome:extensions`.
   3. Choose the folder `webextensions¥chrome` to pack the addon with `Pack extension`. (Then you'll need to specify `chrome.pem` as the private key.)
2. Update new package of Edge addon.
   1. Launch Edge.
   2. Go to addons manager `edge:extensions`.
   3. Choose the folder `webextensions¥edge` to pack the addon with `Pack extension`. (Then you'll need to specify `edge.pem` as the private key.)
3. Edit group policy of Chrome and Edge to deactivate force-installed addons. For example: changing `<Generated ID of the addon>;<File URL of the manifest XML file>` to `#<Generated ID of the addon>;<File URL of the manifest XML file>`.
4. Launch Chrome and Edge to confirm that addons are successfully uninstalled.
5. Exit Chrome and Edge.
6. Edit group policy of Chrome and Edge to re-activate force-installed addons. For example: changing `#<Generated ID of the addon>;<File URL of the manifest XML file>` to `<Generated ID of the addon>;<File URL of the manifest XML file>`.
7. Launch Chrome and Edge to confirm that addons are successfully installed.

If you configured the client as a fake domain member, you should re-configure the client as a non-domain member. Steps:

1. Launch `cmd.exe` as the administrator and run following commands.
   (Ref: https://hitco.at/blog/apply-edge-policies-for-non-domain-joined-devices/ )
   ```
   reg delete HKLM\SOFTWARE\Microsoft\Enrollments\FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF /f
   reg delete HKLM\SOFTWARE\Microsoft\Provisioning\OMADM\Accounts\FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF /f
   ```


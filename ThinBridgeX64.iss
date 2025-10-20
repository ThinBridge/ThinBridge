;ThinBridge Setup--

[Setup]
AppName=ThinBridge
AppVerName=ThinBridge
VersionInfoVersion=4.2.2.0
AppVersion=4.2.2.0
AppMutex=ThinBridgeSetup
;DefaultDirName=C:\ThinBridge
DefaultDirName={code:GetProgramFiles}\ThinBridge
Compression=lzma2
SolidCompression=yes
OutputDir=SetupOutput
OutputBaseFilename=ThinBridgeSetup_x64
AppPublisher=ThinBridge
WizardImageStretch=no
VersionInfoDescription=ThinBridgeSetup
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
DefaultGroupName=ThinBridge
UninstallDisplayIcon={app}\ThinBridge.exe

[Registry]
Root: HKLM; Subkey: "Software\ThinBridge"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\ThinBridge"; ValueType: string; ValueName: "Path"; ValueData: "{app}\"
Root: HKLM; Subkey: "Software\ThinBridge"; ValueType: string; ValueName: "ClientType"; ValueData: ""
Root: HKLM; Subkey: "Software\ThinBridge"; ValueType: string; ValueName: "Version"; ValueData: "4.2.2.0"
Root: HKLM; Subkey: "Software\ThinBridge"; ValueType: string; ValueName: "Rulefile"; ValueData: "{app}\ThinBridgeBHO.ini"
Root: HKLM; Subkey: "Software\ThinBridge"; ValueType: string; ValueName: "RCAPfile"; ValueData: "{app}\ResourceCap.ini"
Root: HKLM; Subkey: "Software\ThinBridge"; ValueType: string; ValueName: "ExtensionExecfile"; ValueData: "{app}\TBRedirector.exe"

Root: HKLM; Subkey: "Software\WOW6432Node\ThinBridge"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\WOW6432Node\ThinBridge"; ValueType: string; ValueName: "Path"; ValueData: "{app}\"
Root: HKLM; Subkey: "Software\WOW6432Node\ThinBridge"; ValueType: string; ValueName: "ClientType"; ValueData: ""
Root: HKLM; Subkey: "Software\WOW6432Node\ThinBridge"; ValueType: string; ValueName: "Version"; ValueData: "4.2.2.0"
Root: HKLM; Subkey: "Software\WOW6432Node\ThinBridge"; ValueType: string; ValueName: "Rulefile"; ValueData: "{app}\ThinBridgeBHO.ini"
Root: HKLM; Subkey: "Software\WOW6432Node\ThinBridge"; ValueType: string; ValueName: "RCAPfile"; ValueData: "{app}\ResourceCap.ini"
Root: HKLM; Subkey: "Software\WOW6432Node\ThinBridge"; ValueType: string; ValueName: "ExtensionExecfile"; ValueData: "{app}\TBRedirector.exe"


;IE Addin
;アドオンの有効化
;Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Policies\Ext"; ValueType: dword; ValueName: "ListBox_Support_CLSID"; ValueData: "1"
;Root: HKLM; Subkey: "Software\Microsoft\Windows\CurrentVersion\Policies\Ext\CLSID"; ValueType: string; ValueName: "{{3A56619B-37AC-40DA-833E-410F3BEDCBDC}"; ValueData: "1";Flags: uninsdeletevalue

;Edge
;Root: HKLM; Subkey: "SOFTWARE\Policies\Microsoft\Edge\ExtensionInstallForcelist"; ValueType: string; ValueName:{code:GetEdgeExtensionIndex}; ValueData:"famoofbkcpjdkihdngnhgbdfkfenhcnf";Flags:uninsdeletevalue
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Edge\NativeMessagingHosts\com.clear_code.thinbridge"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Edge\NativeMessagingHosts\com.clear_code.thinbridge"; ValueType: string; ValueData: "{app}\ThinBridgeHost\edge.json";

;Firefox
Root: HKLM; Subkey: "SOFTWARE\Mozilla\NativeMessagingHosts\com.clear_code.thinbridge"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Mozilla\NativeMessagingHosts\com.clear_code.thinbridge"; ValueType: string; ValueData: "{app}\ThinBridgeHost\firefox.json";

;Chrome
Root: HKLM; Subkey: "SOFTWARE\Google\Chrome\NativeMessagingHosts\com.clear_code.thinbridge"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Google\Chrome\NativeMessagingHosts\com.clear_code.thinbridge"; ValueType: string; ValueData: "{app}\ThinBridgeHost\chrome.json";

;Default Browser
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge"; ValueType: string; ValueData: "ThinBridge";
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\Capabilities"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\Capabilities"; ValueType: string; ValueName: "ApplicationDescription"; ValueData: "ThinBridge Browser Redirector"
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\Capabilities"; ValueType: string; ValueName: "ApplicationIcon"; ValueData: "{app}\ThinBridge.exe"
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\Capabilities"; ValueType: string; ValueName: "ApplicationName"; ValueData: "ThinBridge"
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\Capabilities\Startmenu"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\Capabilities\Startmenu"; ValueType: string; ValueName: "StartMenuInternet"; ValueData: "ThinBridge"
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\Capabilities\URLAssociations"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\Capabilities\URLAssociations"; ValueType: string; ValueName: "http"; ValueData: "ThinBridgeURL"
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\Capabilities\URLAssociations"; ValueType: string; ValueName: "https"; ValueData: "ThinBridgeURL"

Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\DefaultIcon"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\DefaultIcon"; ValueType: string; ValueData: "{app}\ThinBridge.exe"

Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\InstallInfo"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\InstallInfo"; ValueType: string; ValueName: "HideIconsCommand"; ValueData: """{app}\ThinBridge.exe"" --hide-icons"
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\InstallInfo"; ValueType: string; ValueName: "ReinstallCommand"; ValueData: """{app}\ThinBridge.exe"" --make-default-browser"
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\InstallInfo"; ValueType: string; ValueName: "ShowIconsCommand"; ValueData: """{app}\ThinBridge.exe"" --show-icons"
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\InstallInfo"; ValueType: dword; ValueName: "IconsVisible"; ValueData: "1"

Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\shell"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\shell\open"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\shell\open\command"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Clients\StartMenuInternet\ThinBridge\shell\open\command"; ValueType: string; ValueData: """{app}\TBRedirector.exe"" ""%1"""

Root: HKLM; Subkey: "SOFTWARE\RegisteredApplications"; ValueType: string; ValueName: "ThinBridge"; ValueData: "Software\Clients\StartMenuInternet\ThinBridge\Capabilities"



Root: HKLM; Subkey: "SOFTWARE\Classes\ThinBridgeURL"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\ThinBridgeURL"; ValueType: string; ValueData: "ThinBridge"
Root: HKLM; Subkey: "SOFTWARE\Classes\ThinBridgeURL"; ValueType: string; ValueName: "AppUserModelId"; ValueData: "ThinBridge"

Root: HKLM; Subkey: "SOFTWARE\Classes\ThinBridgeURL\Application"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\ThinBridgeURL\Application"; ValueType: string; ValueName: "ApplicationCompany"; ValueData: "ThinBridge"
Root: HKLM; Subkey: "SOFTWARE\Classes\ThinBridgeURL\Application"; ValueType: string; ValueName: "ApplicationDescription"; ValueData: "ThinBridge Browser Redirector"
Root: HKLM; Subkey: "SOFTWARE\Classes\ThinBridgeURL\Application"; ValueType: string; ValueName: "ApplicationIcon"; ValueData: "{app}\ThinBridge.exe"
Root: HKLM; Subkey: "SOFTWARE\Classes\ThinBridgeURL\Application"; ValueType: string; ValueName: "ApplicationName"; ValueData: "ThinBridge"
Root: HKLM; Subkey: "SOFTWARE\Classes\ThinBridgeURL\Application"; ValueType: string; ValueName: "AppUserModelId"; ValueData: "ThinBridge"

Root: HKLM; Subkey: "SOFTWARE\Classes\ThinBridgeURL\DefaultIcon"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\ThinBridgeURL\DefaultIcon"; ValueType: string; ValueData: "{app}\ThinBridge.exe,0"
Root: HKLM; Subkey: "SOFTWARE\Classes\ThinBridgeURL\shell"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\ThinBridgeURL\shell\open"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\ThinBridgeURL\shell\open\command"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\ThinBridgeURL\shell\open\command"; ValueType: string; ValueData: """{app}\TBRedirector.exe"" ""%1"""


;///////////////////
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge"; ValueType: string; ValueData: "ThinBridge";
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\Capabilities"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\Capabilities"; ValueType: string; ValueName: "ApplicationDescription"; ValueData: "ThinBridge Browser Redirector"
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\Capabilities"; ValueType: string; ValueName: "ApplicationIcon"; ValueData: "{app}\ThinBridge.exe,0"
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\Capabilities"; ValueType: string; ValueName: "ApplicationName"; ValueData: "ThinBridge"
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\Capabilities\Startmenu"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\Capabilities\Startmenu"; ValueType: string; ValueName: "StartMenuInternet"; ValueData: "ThinBridge"
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\Capabilities\URLAssociations"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\Capabilities\URLAssociations"; ValueType: string; ValueName: "http"; ValueData: "ThinBridgeURL"
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\Capabilities\URLAssociations"; ValueType: string; ValueName: "https"; ValueData: "ThinBridgeURL"

Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\DefaultIcon"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\DefaultIcon"; ValueType: string; ValueData: "{app}\ThinBridge.exe"

Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\shell"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\shell\open"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\shell\open\command"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\shell\open\command"; ValueType: string; ValueData: """{app}\TBRedirector.exe"" ""%1"""

Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\InstallInfo"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\InstallInfo"; ValueType: string; ValueName: "HideIconsCommand"; ValueData: """{app}\ThinBridge.exe"" --hide-icons"
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\InstallInfo"; ValueType: string; ValueName: "ReinstallCommand"; ValueData: """{app}\ThinBridge.exe"" --make-default-browser"
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\InstallInfo"; ValueType: string; ValueName: "ShowIconsCommand"; ValueData: """{app}\ThinBridge.exe"" --show-icons"
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\InstallInfo"; ValueType: dword; ValueName: "IconsVisible"; ValueData: "1"

Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\shell"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\shell\open"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\shell\open\command"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\Clients\StartMenuInternet\ThinBridge\shell\open\command"; ValueType: string; ValueData: """{app}\TBRedirector.exe"" ""%1"""

Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\RegisteredApplications"; ValueType: string; ValueName: "ThinBridge"; ValueData: "Software\Clients\StartMenuInternet\ThinBridge\Capabilities"


[Languages]
Name: jp; MessagesFile: "compiler:Languages\Japanese.isl"


[Files]
;exe
Source: "Release\TBRedirector.exe"; DestDir: "{app}\";Flags: ignoreversion;permissions:users-readexec admins-full system-full
Source: "Release\ThinBridge.exe"; DestDir: "{app}\";Flags: ignoreversion;permissions:users-readexec admins-full system-full
Source: "Release\ThinBridgeChecker.exe"; DestDir: "{app}\";Flags: ignoreversion;permissions:users-readexec admins-full system-full
Source: "Release\ThinBridgeRuleUpdater.exe"; DestDir: "{app}\";Flags: ignoreversion;permissions:users-readexec admins-full system-full
Source: "Release\ThinBridgeRuleUpdaterSetting.exe"; DestDir: "{app}\";Flags: ignoreversion;permissions:users-readexec admins-full system-full
Source: "Release\ThinBridgeSetting.exe"; DestDir: "{app}\";Flags: ignoreversion;permissions:users-readexec admins-full system-full
Source: "Release\TBo365URLSyncSetting.exe"; DestDir: "{app}\";Flags: ignoreversion;permissions:users-readexec admins-full system-full

;dll
Source: "Release\ThinBridgeBHO.dll"; DestDir: "{app}\";Flags: ignoreversion regserver 32bit;permissions:users-readexec admins-full system-full
Source: "Release\ThinBridgeBHO64.dll"; DestDir: "{app}\";Flags: ignoreversion regserver 64bit; Check: IsWin64;permissions:users-readexec admins-full system-full

;task
Source: "ThinBridgeRuleUpdater\Task\ThinBridgeRuleUpdateTask.xml"; DestDir: "{tmp}";Flags: ignoreversion

;host
Source: "Release\ThinBridgeTalk.exe"; DestDir: "{app}\ThinBridgeHost";Flags: ignoreversion;permissions:users-readexec admins-full system-full

;edge
Source: "Resources\edge.json"; DestDir: "{app}\ThinBridgeHost";Flags: ignoreversion;permissions:users-readexec admins-full system-full
;firefox
Source: "Resources\firefox.json"; DestDir: "{app}\ThinBridgeHost";Flags: ignoreversion;permissions:users-readexec admins-full system-full
;Chrome
Source: "Resources\chrome.json"; DestDir: "{app}\ThinBridgeHost";Flags: ignoreversion;permissions:users-readexec admins-full system-full

[Icons]
Name: "{group}\リダイレクト定義設定"; Filename: "{app}\ThinBridgeSetting.exe"; WorkingDir: "{app}"
Name: "{group}\環境チェッカー"; Filename: "{app}\ThinBridgeChecker.exe"; WorkingDir: "{app}"
Name: "{group}\リダイレクト定義自動更新設定"; Filename: "{app}\ThinBridgeRuleUpdaterSetting.exe"; WorkingDir: "{app}"
Name: "{commonstartup}\ThinBridgeRuleUpdater"; Filename: "{app}\ThinBridgeRuleUpdater.exe"; WorkingDir: "{app}"

[Dirs]
Name: "{app}";Permissions: users-modify
;log関連はアンインストールで消さない
Name: "{app}\TBUpdateLog";Permissions: users-modify;Flags: uninsneveruninstall

[Run] 
Filename: "{app}\ThinBridgeChecker.exe";Parameters: "/log"; Flags: runhidden 

Filename: "schtasks.exe";Parameters: "/Create /F /TN ""ThinBridgeRuleUpdateTask"" /xml ""{tmp}\ThinBridgeRuleUpdateTask.xml"""; Flags: runhidden; Check: ShouldRegisterTaskScheduler;
;Filename: "schtasks.exe";Parameters: "/Change /TN ""ThinBridgeRuleUpdateTask"" /TR ""'{app}\ThinBridgeRuleUpdater.exe'"""; Flags: runhidden 
Filename: "schtasks.exe";Parameters: "/Change /F /SC HOURLY /TN ""ThinBridgeRuleUpdateTask"" /RU SYSTEM /RL HIGHEST /TR ""'{app}\ThinBridgeRuleUpdater.exe'"""; Flags: runhidden; Check: ShouldRegisterTaskScheduler;

Filename: "{sys}\icacls.exe";Parameters: """{app}\TBo365URLSyncSetting.exe"" /inheritance:r"; Flags: runhidden shellexec
Filename: "{sys}\icacls.exe";Parameters: """{app}\TBRedirector.exe"" /inheritance:r"; Flags: runhidden shellexec
Filename: "{sys}\icacls.exe";Parameters: """{app}\ThinBridge.exe"" /inheritance:r"; Flags: runhidden shellexec
Filename: "{sys}\icacls.exe";Parameters: """{app}\ThinBridgeSetting.exe"" /inheritance:r"; Flags: runhidden shellexec
Filename: "{sys}\icacls.exe";Parameters: """{app}\ThinBridgeChecker.exe"" /inheritance:r"; Flags: runhidden shellexec
Filename: "{sys}\icacls.exe";Parameters: """{app}\ThinBridgeRuleUpdater.exe"" /inheritance:r"; Flags: runhidden shellexec
Filename: "{sys}\icacls.exe";Parameters: """{app}\ThinBridgeRuleUpdaterSetting.exe"" /inheritance:r"; Flags: runhidden shellexec
Filename: "{sys}\icacls.exe";Parameters: """{app}\ThinBridgeBHO.dll"" /inheritance:r"; Flags: runhidden shellexec
Filename: "{sys}\icacls.exe";Parameters: """{app}\ThinBridgeBHO64.dll"" /inheritance:r"; Flags: runhidden shellexec
Filename: "{sys}\icacls.exe";Parameters: """{app}\ThinBridgeHost\ThinBridgeTalk.exe"" /inheritance:r"; Flags: runhidden shellexec
Filename: "{sys}\icacls.exe";Parameters: """{app}\ThinBridgeHost\edge.json"" /inheritance:r"; Flags: runhidden shellexec
Filename: "{sys}\icacls.exe";Parameters: """{app}\ThinBridgeHost\firefox.json"" /inheritance:r"; Flags: runhidden shellexec
Filename: "{sys}\icacls.exe";Parameters: """{app}\ThinBridgeHost\chrome.json"" /inheritance:r"; Flags: runhidden shellexec

[UninstallRun]

[Code]
var
  SetTaskSchedulerPage: TInputQueryWizardPage;
  SetTaskSchedulerCheckBox: TNewCheckBox;
  IsSetTaskScheculer: Boolean;

function ShouldRegisterTaskScheduler(): Boolean;
begin
  Result := SetTaskSchedulerCheckBox.Checked;
end;

function GetProgramFiles(Param: string): string;
  begin
    if IsWin64 then Result := ExpandConstant('{pf64}')
    else Result := ExpandConstant('{pf32}')
  end;

procedure TaskKill(FileName: String);
var
  ResultCode: Integer;
begin
    Exec(ExpandConstant('taskkill.exe'), '/f /im ' + '"' + FileName + '"', '', SW_HIDE,ewWaitUntilTerminated, ResultCode);
end;
function InitializeSetup():Boolean;
begin 
	TaskKill('iexplore.exe');
	TaskKill('msedge.exe');
	TaskKill('TBRedirector.exe');
	TaskKill('ThinBridge.exe');
	TaskKill('ThinBridgeSetting.exe');
	TaskKill('ThinBridgeChecker.exe');
	TaskKill('ThinBridgeRuleUpdater.exe');
	TaskKill('ThinBridgeRuleUpdaterSetting.exe');
	Result := True; 
end; 

procedure InitializeWizard;
var CmdParamSetTaskScheduler: string;
begin
  CmdParamSetTaskScheduler := ExpandConstant('{param:SetTaskScheduler|no}');
  SetTaskSchedulerPage := CreateInputQueryPage(wpWelcome, CustomMessage('SetTaskSchedulerPage'), CustomMessage('SetTaskSchedulerPageDescription'), '');
  SetTaskSchedulerCheckBox := TNewCheckBox.Create(WizardForm);
  SetTaskSchedulerCheckBox.Parent := SetTaskSchedulerPage.Surface;
  SetTaskSchedulerCheckBox.Width := SetTaskSchedulerPage.SurfaceWidth;
  SetTaskSchedulerCheckBox.Left := 0;
  SetTaskSchedulerCheckBox.Top := 0;
  SetTaskSchedulerCheckBox.Caption := CustomMessage('SetTaskSchedulerCheckboxCaption');
  SetTaskSchedulerCheckBox.Checked := CmdParamSetTaskScheduler = 'yes';
end;

function GetEdgeExtensionIndex(Value: string):string;
var
  Names: TArrayOfString;
  I: Integer;
  sIndex: String;
	sValue:String;
	iMax:Integer;
	iTemp:Integer;
begin
	iMax :=168;
  if RegGetValueNames(HKLM, 'SOFTWARE\Policies\Microsoft\Edge\ExtensionInstallForcelist', Names) then
  begin
    sIndex := '';
		iMax :=iMax-1;
		iTemp :=0;
    for I := 0 to GetArrayLength(Names)-1 do
		begin
      sIndex := Names[I];
			if(sIndex<>'') then
			begin
				RegQueryStringValue(HKLM,'SOFTWARE\Policies\Microsoft\Edge\ExtensionInstallForcelist',sIndex,sValue);
        if(sValue='famoofbkcpjdkihdngnhgbdfkfenhcnf')then
				begin
				  Result:=sIndex;
				  exit;
				end	else
				begin
					iTemp := StrToInt(sIndex);
					if(iTemp > iMax) then
						iMax := iTemp;
				end;
			end;
		end;
		iMax := iMax +1;
    //MsgBox('List of subkeys:'#13#10#13#10 + IntToStr(iMax), mbInformation, MB_OK);
  end;
	Result:=IntToStr(iMax);
end;

[CustomMessages]
jp.SetTaskSchedulerPage=タスク スケジューラ設定
jp.SetTaskSchedulerPageDescription=リダイレクト定義自動更新プログラムをタスク スケジューラに登録するかどうかを指定してください。
jp.SetTaskSchedulerCheckboxCaption=リダイレクト定義自動更新プログラムをタスク スケジューラに登録する

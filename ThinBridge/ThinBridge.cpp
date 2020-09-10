
#include "stdafx.h"
#include "ThinBridge.h"
#include "RedirectDlg.h"
#include "SettingsDialog.h"
#include "DlgRemoteSetting.h"
#include "DlgSetRDP.h"
#include "DlgSetVMW.h"
#include "DlgSetCX.h"
#include "DlgRD.h"
#include "DlgETC.h"
#include "DlgEco.h"
#include "DlgEtcCAP.h"
#include <DDEML.H>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CRedirectApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CRedirectApp::CRedirectApp()
{
	m_pReExecThread=NULL;
	m_pDlgMsg=NULL;

//	m_iAdvancedLogLevel=DEBUG_LOG_LEVEL_OUTPUT_ALL;
	m_iAdvancedLogLevel=DEBUG_LOG_LEVEL_OUTPUT_URL;

	m_strThisAppName=_T("ThinBridge");
	SetAppID(m_strThisAppName);
	m_iFeatureType = FE_PLUS;
}


CRedirectApp theApp;


BOOL CRedirectApp::InitBaseFunction()
{
	BOOL bRet=TRUE;
	TCHAR szPath[MAX_PATH+1]={0};
	TCHAR szDrive[_MAX_DRIVE]={0};
	TCHAR szDir[_MAX_DIR]={0};
	TCHAR szFileName[_MAX_FNAME]={0};
	TCHAR szExt[_MAX_EXT]={0};
	GetModuleFileName(AfxGetInstanceHandle(),szPath,MAX_PATH);
	_tsplitpath_s(szPath, szDrive,_MAX_DRIVE, szDir,_MAX_DIR, szFileName,_MAX_FNAME, szExt,_MAX_EXT); 
	m_strExeFullPath=szPath;
	m_strExeFileName = szFileName;
	m_strExeFileName += szExt;

	m_strO365ToolFullPath = szDrive;
	m_strO365ToolFullPath += szDir;
	m_strO365ToolFullPath += _T("TBo365URLSyncSetting.exe");

	m_strChromeSwitcherFullPath = szDrive;
	m_strChromeSwitcherFullPath += szDir;
	m_strChromeSwitcherFullPath += _T("TBChromeSwitcher.exe");


	m_strLogFileFullPath = szDrive;
	m_strLogFileFullPath += szDir;

	CTime time = CTime::GetCurrentTime();
	CString strLogFileNow;
	strLogFileNow.Format(_T("%s%s.log"),theApp.m_strThisAppName,time.Format(_T("%Y-%m")));
	m_strLogFileFullPath += strLogFileNow;

	m_strLogFileDir = szDrive;
	m_strLogFileDir += szDir;
	this->LogRotate(m_strLogFileDir);


	TCHAR szTemp[MAX_PATH+1]={0};
	::GetTempPath(MAX_PATH,szTemp);
	CString strTempPath;

	strTempPath = szTemp;
	strTempPath += theApp.m_strThisAppName;
	::CreateDirectory(strTempPath, NULL);
	SetLastError(NO_ERROR);


	//設定データのロード
	m_strSetting_FileFullPath = szDrive;
	m_strSetting_FileFullPath += szDir;
	m_strSetting_FileFullPath += _T("setting.conf");
	this->ReadSetting();

	m_strResourceCAPFullPath = szDrive;
	m_strResourceCAPFullPath += szDir;
	m_strResourceCAPFullPath += _T("ResourceCap.ini");


	//templateの読込
	m_strTemplate_FileFullPath = szDrive;
	m_strTemplate_FileFullPath += szDir;
	m_strTemplate_FileFullPath += _T("rdp_template.conf");

	m_strTestConnect_FileFullPath = strTempPath;
	m_strTestConnect_FileFullPath += _T("\\Test_Connect.rdp");

	//ReadOnceのパスセット
	m_strRdp_ReadOnce_FileFullPath = szDrive;
	m_strRdp_ReadOnce_FileFullPath += szDir;
	m_strRdp_ReadOnce_FileFullPath += _T("rdp_readonce.conf");

	//RDPファイルのパス
	m_strRDP_FileFullPath = strTempPath;
	m_strRDP_FileFullPath += _T("\\");
	m_strRDP_FileFullPath += theApp.m_strThisAppName;

	//ループ防止
	m_strLoopBlockFilePath=strTempPath;
	m_strLoopBlockFilePath+= _T("\\STP.dat");
	m_LoopBlock.SetFilePath(m_strLoopBlockFilePath);

	//PIDを付加する。大量に同時起動されると、コンフリクトする可能性があるため。
	//タイミングにより削除できないケースも考えられるため、下1桁の数字0-9を利用する事にする。
	//最大で16個のRDPファイルをローテーション。
	DWORD dPID = 0;
	dPID = ::GetCurrentProcessId();
	CString strPID;
	strPID.Format(_T("%04d"),dPID);
	//m_strRDP_FileFullPath +=strPID;
	m_strRDP_FileFullPath +=strPID.Right(1);
	m_strRDP_FileFullPath +=_T(".RDP");
	//ファイルが既に存在するか確認
	if(PathFileExists(m_strRDP_FileFullPath))
	{
		//既に、そのファイルが存在している場合。
		//PIDが再利用された場合に、重複する可能性が考えられる(無いとは思うが、念には念を)

		//RDPファイルを格納するフォルダーパス
		CString strRDP_FileTemp;
		strRDP_FileTemp = strTempPath;

		//ファイル名(一時ファイル)
		CString strRDPFileNameTemp;
		strRDPFileNameTemp = theApp.m_strThisAppName;
		strRDPFileNameTemp += strPID;

		//存在チェックするフルパス格納
		CString strRDP_FileTempFullPath;

		for(int iDup=0;iDup<1000;iDup++)
		{
			//<PID>-00～9999.RDPの範囲を確認
			strRDP_FileTempFullPath.Format(_T("%s\\%s-%02d.RDP"),strRDP_FileTemp,strRDPFileNameTemp,iDup);
			//ファイルの存在チェック
			if(!PathFileExists(strRDP_FileTempFullPath))
			{
				//そのファイルが見つからない場合
				//見つからないので、このファイル名を利用し抜ける。(始めに見つけた番号を利用)
				m_strRDP_FileFullPath=strRDP_FileTempFullPath;
				break;
			}
		}
	}

	//機能設定ファイルのフルパス格納
	m_strProfileFullPath = szDrive;
	m_strProfileFullPath += szDir;
	m_strProfileFullPath += theApp.m_strThisAppName;
	m_strProfileFullPath += _T(".ini");
	m_pszProfileName = _tcsdup(m_strProfileFullPath);

	//ThinBridgePlus ini
	m_strRedirectFilePath = szDrive;
	m_strRedirectFilePath += szDir;
	m_strRedirectFilePath +=_T("ThinBridgeBHO.ini");

	m_strRedirect_Script_FilePath = szDrive;
	m_strRedirect_Script_FilePath += szDir;
	m_strRedirect_Script_FilePath += _T("ThinBridgeBHO.tbs");

	return bRet;
}

void CRedirectApp::InitCommandParamOptionParam()
{
	CString Command1;
	CString Command2;
	CString Command3;

	//コマンドラインあり。
	if(m_lpCmdLine[0] != '\0')
	{
		//パラメータが1つだけ
		if(__argc ==2)
		{
			Command1 = CString(__wargv[1]);
			Command1.Replace(_T("\""),_T(""));
			Command1.TrimLeft();
			Command1.TrimRight();

			if(!Command1.IsEmpty())
			{
				//URLかFilePathの場合は、強制的にm_CommandParamとする。
				if(SBUtil::IsURL(Command1))
				{
					m_CommandParam=Command1;
				}
				//filepath
				else if(Command1.Find(_T(":"))==1)
				{
					m_CommandParam=Command1;
				}
				//-は、オプション
				else if(Command1.Find(_T("-"))==0)
				{
					m_OptionParam=Command1;
				}
				// /は、オプション
				else if(Command1.Find(_T("/"))==0)
				{
					m_OptionParam=Command1;
				}
			}
		}
		//コマンドラインが3つ以上、0番は、EXEパス
		else if(__argc==3)
		{
			Command1 = CString(__wargv[1]);
			Command2 = CString(__wargv[2]);
			Command1.Replace(_T("\""),_T(""));
			Command1.TrimLeft();
			Command1.TrimRight();

			Command2.Replace(_T("\""),_T(""));
			Command2.TrimLeft();
			Command2.TrimRight();

			if(!Command1.IsEmpty())
			{
				//URLかFilePathの場合は、強制的にm_CommandParamとする。
				if(SBUtil::IsURL(Command1))
				{
					m_CommandParam=Command1;
				}
				//filepath
				else if(Command1.Find(_T(":"))==1)
				{
					m_CommandParam=Command1;
				}
				else if(Command1.CompareNoCase(_T("Firefox"))==0)
				{
					m_OptionParam=Command1;
				}
				else if(Command1.CompareNoCase(_T("Chrome"))==0)
				{
					m_OptionParam=Command1;
				}
				else if(Command1.CompareNoCase(_T("Edge"))==0)
				{
					m_OptionParam=Command1;
				}
				else if(Command1.CompareNoCase(_T("Custom"))==0)
				{
					m_OptionParam=Command1;
				}
				else if(Command1.CompareNoCase(_T("IE"))==0)
				{
					m_OptionParam=Command1;
				}
				else if(Command1.CompareNoCase(_T("Default"))==0)
				{
					m_OptionParam=Command1;
				}
				else if(Command1.CompareNoCase(_T("RDP"))==0)
				{
					m_OptionParam=Command1;
				}
				else if(Command1.CompareNoCase(_T("VMWARE"))==0)
				{
					m_OptionParam=Command1;
				}
				else if(Command1.CompareNoCase(_T("CITRIX"))==0)
				{
					m_OptionParam=Command1;
				}
			}
			if(!Command2.IsEmpty())
			{
				//URLかFilePathの場合は、強制的にm_CommandParamとする。
				if(SBUtil::IsURL(Command2))
				{
					m_CommandParam=Command2;
				}
				else
				{
					m_OptionParam=Command2;
				}
			}
		}
		//コマンドラインが3つ以上、0番は、EXEパス
		else if(__argc>3)
		{
			Command1 = CString(__wargv[1]);
			Command2 = CString(__wargv[2]);
			Command3 = CString(__wargv[3]);
			Command1.Replace(_T("\""),_T(""));
			Command1.TrimLeft();
			Command1.TrimRight();

			Command2.Replace(_T("\""),_T(""));
			Command2.TrimLeft();
			Command2.TrimRight();

			Command3.Replace(_T("\""),_T(""));
			Command3.TrimLeft();
			Command3.TrimRight();

			if(!Command1.IsEmpty())
			{
				//URLかFilePathの場合は、強制的にm_CommandParamとする。
				if(SBUtil::IsURL(Command1))
				{
					m_CommandParam=Command1;
				}
				//filepath
				else if(Command1.Find(_T(":"))==1)
				{
					m_CommandParam=Command1;
				}
			}
			if(!Command2.IsEmpty())
			{
				//URLかFilePathの場合は、強制的にm_CommandParamとする。
				if(SBUtil::IsURL(Command2))
				{
					m_CommandParam=Command2;
				}
				else
				{
					m_OptionParam=Command2;
				}
			}
			if(!Command3.IsEmpty())
			{
				//URLかFilePathの場合は、強制的にm_CommandParamとする。
				if(SBUtil::IsURL(Command3))
				{
					m_CommandParam=Command3;
				}
				else
				{
					m_EtcParam=Command3;
				}
			}
		}
	}
	CString logmsg;
	logmsg.Format(_T("GetCommand\t%s\tOptionParam=%s"),m_CommandParam,m_OptionParam);
	theApp.WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_GE);
}
void CRedirectApp::InitExecCommandParam()
{
	CString strMsg;
	if(this->SettingConf.m_iSolutionType==PROC_RDP)
	{
		SetLastError(NO_ERROR);
		this->ReadTemplate();
		this->ReadRDP_ReadOnceFile();	
		//RDPファイルをTEMPに出力しMSTSC.exeを実行する。		
		CreateRDPFile(m_CommandParam);
	}
	else if(this->SettingConf.m_iSolutionType==PROC_VMWARE)
	{
		//horizon clientがインストールされていない。
		if(!IsHorizonInstalled())
		{
			strMsg=_T("VMware Horizon Clientがインストールされていないか、設定されていないため起動できません。");
			ShowTimeoutMessageBox(strMsg,m_strThisAppName,MB_OK|MB_ICONERROR,6000);
			return;
		}

		Exec_VMwareHorizon_Start(m_CommandParam);
		if(m_pDlgMsg)
		{
			delete m_pDlgMsg;
			m_pDlgMsg=NULL;
		}
		if(m_pReExecThread)
		{
			delete m_pReExecThread;
			m_pReExecThread = NULL;
		}
	}
	else if(this->SettingConf.m_iSolutionType==PROC_CITRIX)
	{
		//citrix clientがインストールされていない。
		if(!IsCitrixInstalled())
		{
			strMsg=_T("Citrix Receiverがインストールされていないか、設定されていないため起動できません。");
			ShowTimeoutMessageBox(strMsg,m_strThisAppName,MB_OK|MB_ICONERROR,6000);
			return;
		}
		Exec_CitixXenApp_Start(m_CommandParam);
		if(m_pDlgMsg)
		{
			delete m_pDlgMsg;
			m_pDlgMsg=NULL;
		}
		if(m_pReExecThread)
		{
			delete m_pReExecThread;
			m_pReExecThread = NULL;
		}
	}
	else if(this->SettingConf.m_iSolutionType==PROC_LDefaultBrowser)
	{
		Exec_LocalBrowser(m_CommandParam);
	}
	//Local IE
	else if(this->SettingConf.m_iSolutionType==PROC_LIE)
	{
		SetIE_FullPath();
		//Resource CAP
		if(IsResourceCAP_IE())
			return;
		Exec_LocalBrowser(m_CommandParam);
	}
	else if(this->SettingConf.m_iSolutionType==PROC_LFirefox)
	{
		//firefoxがインストールされていない。
		if(!IsFirefoxInstalled())
		{
			strMsg=_T("Mozilla Firefoxがインストールされていないか、設定されていないため起動できません。");
			ShowTimeoutMessageBox(strMsg,m_strThisAppName,MB_OK|MB_ICONERROR,6000);
			return;
		}
		Exec_LocalBrowser(m_CommandParam);
	}
	else if(this->SettingConf.m_iSolutionType==PROC_LChrome)
	{
		//Chromeがインストールされていない。
		if(!IsChromeInstalled())
		{
			strMsg=_T("Google Chromeがインストールされていないか、設定されていないため起動できません。");
			ShowTimeoutMessageBox(strMsg,m_strThisAppName,MB_OK|MB_ICONERROR,6000);
			return;
		}
		Exec_LocalBrowser(m_CommandParam);
	}
	else if(this->SettingConf.m_iSolutionType==PROC_LEdge)
	{
		Exec_LocalBrowser(m_CommandParam);
	}
	else if(this->SettingConf.m_iSolutionType==PROC_LCustom)
	{
		if(!IsCustomInstalled(this->SettingConf.m_strCustomBrowserPath))
		{
			strMsg.Format(_T("指定されたブラウザーが見つからないため起動できません。\n%s"),SettingConf.m_strCustomBrowserPath);
			ShowTimeoutMessageBox(strMsg,m_strThisAppName,MB_OK|MB_ICONERROR,6000);
			return;
		}
		Exec_LocalBrowser(m_CommandParam);
	}
}
void CRedirectApp::InitShowSettingDlg()
{
	CString strMsg;
	SetLastError(NO_ERROR);
	this->ReadTemplate();
	this->ReadRDP_ReadOnceFile();	
	INT_PTR nResponse = 0;
	this->SetIE_FullPath();
	this->SetFirefox_FullPath();
	this->SetChrome_FullPath();

	if(m_iFeatureType == FE_PLUS)
	{
		//表示制限されている場合
		if(SettingConf.m_iKeyCombination)
		{
			strMsg.Format(_T("%s\n%s"),_T("この機能へのアクセスは、システム管理者により制限されています。 "),_T("This Feature has been disabled by your administrator."));
			::MessageBox(NULL,strMsg,theApp.m_strThisAppName,MB_ICONERROR);
			//Keyキーコンビネーションが一致した場合は、設定画面を表示する。
			if(theApp.bValidKeyCombi())
			{
				ShowPlusSettingDlgEx();
			}
		}
		else
		{
			ShowPlusSettingDlgEx();
		}
		return;
	}
	else if(m_iFeatureType == CAP_PLUS)
	{
		//表示制限されている場合
		if(SettingConf.m_iKeyCombination)
		{
			strMsg.Format(_T("%s\n%s"),_T("この機能へのアクセスは、システム管理者により制限されています。 "),_T("This Feature has been disabled by your administrator."));
			::MessageBox(NULL,strMsg,theApp.m_strThisAppName,MB_ICONERROR);
			//Keyキーコンビネーションが一致した場合は、設定画面を表示する。
			if(theApp.bValidKeyCombi())
			{
				ShowPlusSettingDlgCAP();
			}
		}
		else
		{
			ShowPlusSettingDlgCAP();
		}
		return;
	}

}

BOOL CRedirectApp::InitInstance()
{
	_wsetlocale(LC_ALL, _T("jpn")); 
	INITCOMMONCONTROLSEX InitCtrls={0};
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	//ESCが押されている場合は、リダイレクトをしない。
	if(::GetKeyState(VK_ESCAPE)<0)
		return FALSE;

	//各種ファイル等を読込
	if(!this->InitBaseFunction())
		return FALSE;

	//ThinBridgeの動作モードを決定。設定ファイルから。
	InitFeatureSetting();

//	DWORD pidResult=0;
//	CString strParentExePath;
//	if(GetParentPID(pidResult))
//	{
//		strParentExePath=GetExeFilePath2PID(pidResult);
//	}
//	AfxMessageBox(strParentExePath);

	//コマンドラインのセット
	this->InitCommandParamOptionParam();

	//両方セットされている場合 ThinBridgeBHOからのCall
	//ここで、this->SettingConf.m_iSolutionTypeを強制的に上書きする。
	if(!m_CommandParam.IsEmpty() && !m_OptionParam.IsEmpty())
	{
		if(m_OptionParam.CompareNoCase(_T("Firefox"))==0)
		{
			this->SettingConf.m_iSolutionType=PROC_LFirefox;
		}
		else if(m_OptionParam.CompareNoCase(_T("Chrome"))==0)
		{
			this->SettingConf.m_iSolutionType=PROC_LChrome;
		}
		else if(m_OptionParam.CompareNoCase(_T("Edge"))==0)
		{
			this->SettingConf.m_iSolutionType=PROC_LEdge;
		}
		else if(m_OptionParam.CompareNoCase(_T("Custom"))==0)
		{
			this->SettingConf.m_iSolutionType=PROC_LCustom;
		}
		else if(m_OptionParam.CompareNoCase(_T("IE"))==0)
		{
			this->SettingConf.m_iSolutionType=PROC_LIE;
		}
		else if(m_OptionParam.CompareNoCase(_T("Default"))==0)
		{
			this->SettingConf.m_iSolutionType=PROC_LDefaultBrowser;
		}
		else if(m_OptionParam.CompareNoCase(_T("RDP"))==0)
		{
			this->SettingConf.m_iSolutionType=PROC_RDP;
		}
		else if(m_OptionParam.CompareNoCase(_T("VMWARE"))==0)
		{
			this->SettingConf.m_iSolutionType=PROC_VMWARE;
		}
		else if(m_OptionParam.CompareNoCase(_T("CITRIX"))==0)
		{
			this->SettingConf.m_iSolutionType=PROC_CITRIX;
		}
		else
		{
			this->SettingConf.m_iSolutionType=PROC_LCustom;
			this->SettingConf.m_strCustomBrowserPath=m_OptionParam;
		}
	}

	//CommandLineがある場合。	
	if(!m_CommandParam.IsEmpty())
	{
		CString strCmd;
		strCmd.Format(_T("%s %s"), m_CommandParam,m_OptionParam);
		UINT uRet=0;
		CString strLogMsg;
		uRet = m_LoopBlock.CheckLoop(strCmd);
		if(uRet == LB_OK)
			this->InitExecCommandParam();
		else if(uRet == LB_NG)
		{
			strLogMsg.Format(_T("%s\t%s\tLoopBlock"), m_OptionParam, m_CommandParam);
			theApp.WriteDebugTraceDateTime(strLogMsg, DEBUG_LOG_LEVEL_OUTPUT_URL);
			CString strSafeGuardMsg;
			strSafeGuardMsg.Format(_T("%s\n%s"), _T("短時間に連続したブラウザー リダイレクトを検出しました。\n\nブラウザー リダイレクト処理を中断します。\n少し時間をおいてから再試行してください。"), m_CommandParam);
			ShowTimeoutMessageBox(strSafeGuardMsg, m_strThisAppName, MB_OK | MB_ICONWARNING| MB_SYSTEMMODAL,10000);
			return FALSE;
		}
		else if (uRet == LB_NG_NO_MSG)
		{
			strLogMsg.Format(_T("%s\t%s\tLoopBlock"), m_OptionParam, m_CommandParam);
			theApp.WriteDebugTraceDateTime(strLogMsg, DEBUG_LOG_LEVEL_OUTPUT_URL);
			return FALSE;
		}
		else
		{
			strLogMsg.Format(_T("%s\t%s\UnExpected"), m_OptionParam, m_CommandParam);
			theApp.WriteDebugTraceDateTime(strLogMsg, DEBUG_LOG_LEVEL_OUTPUT_URL);
			return FALSE;
		}
	}
	//Command Lineなし
	else
	{
		//コマンドラインが無い場合で、EXEの名称がThinBridgeSetting.exeの場合は
		//設定画面を表示する。
		if(m_strExeFileName.CompareNoCase(_T("ThinBridgeSetting.exe"))==0
		|| m_OptionParam.CompareNoCase(_T("/Config")) == 0)
		{
			this->InitShowSettingDlg();
		}
		//純粋にThinBridge.exeを実行した場合は、IEのスタートページを開く。
		else
		{
			this->InitExecLocalBrowser();
		}
	}

	//AfxPostQuitMessage(0);
	//return TRUE;
	return FALSE;
}
void CRedirectApp::InitExecLocalBrowser()
{
	CString strMsg;

	//リモート系が設定されている場合は、IEを強制
	//初期値がRDPなので、setting.confがない場合に、RDPで接続しようとしてしまうため。
	if(this->SettingConf.m_iSolutionType==PROC_RDP
	||this->SettingConf.m_iSolutionType==PROC_VMWARE
	||this->SettingConf.m_iSolutionType==PROC_CITRIX
	)
	{
		this->SettingConf.m_iSolutionType=PROC_LIE;
	}

	//Local IE
	if(this->SettingConf.m_iSolutionType==PROC_LIE)
	{
		SetIE_FullPath();
		//Resource CAP
		if(IsResourceCAP_IE())
			return;
		//IEを設定した場合は、必ず新しいIE Windowを表示する。
		if(::ShellExecute(NULL,_T("open"),_T("iexplore.exe"),NULL,NULL, SW_SHOW) <= HINSTANCE(32))
		{
			CString IEcmd;
			IEcmd=_T("iexplorer.exe");
			if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
			{
				IEcmd=_T("");
				IEcmd.Format(_T("\"%s\""),theApp.m_strIE_FullPath);
				if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
				{
					IEcmd=_T("");
					IEcmd=_T("\"C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe\"");
					if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
					{
						IEcmd=_T("");
						IEcmd=_T("\"C:\\Program Files\\Internet Explorer\\iexplore.exe\"");
						::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW);
					}
				}
			}
		}
		return;
	}
	else if(this->SettingConf.m_iSolutionType==PROC_LFirefox)
	{
		//firefoxがインストールされていない。
		if(!IsFirefoxInstalled())
		{
			strMsg=_T("Mozilla Firefoxがインストールされていないか、設定されていないため起動できません。");
			ShowTimeoutMessageBox(strMsg,m_strThisAppName,MB_OK|MB_ICONERROR,6000);
			return;
		}
	}
	else if(this->SettingConf.m_iSolutionType==PROC_LChrome)
	{
		//Chromeがインストールされていない。
		if(!IsChromeInstalled())
		{
			strMsg=_T("Google Chromeがインストールされていないか、設定されていないため起動できません。");
			ShowTimeoutMessageBox(strMsg,m_strThisAppName,MB_OK|MB_ICONERROR,6000);
			return;
		}
	}
	else if(this->SettingConf.m_iSolutionType==PROC_LCustom)
	{
		if(!IsCustomInstalled(this->SettingConf.m_strCustomBrowserPath))
		{
			strMsg.Format(_T("指定されたブラウザーが見つからないため起動できません。\n%s"),SettingConf.m_strCustomBrowserPath);
			ShowTimeoutMessageBox(strMsg,m_strThisAppName,MB_OK|MB_ICONERROR,6000);
			return;
		}
	}
//	else if(this->SettingConf.m_iSolutionType==PROC_LDefaultBrowser)
//	else if(this->SettingConf.m_iSolutionType==PROC_LEdge)
	Exec_LocalBrowser(m_CommandParam);
}

BOOL CRedirectApp::IsResourceCAP_IE()
{
	BOOL bRet=FALSE;
	//////////////////////Resource CAPの確認////////////////////////
	theApp.ReadResourceCAPSetting();
	CString strMsg;
	CIEStatusHelper IEStatus;
	CString strCaption;
	strCaption.Format(_T("ThinBridge Launcher %s"),ResourceCAPConf.m_strReadSettingType);
	//tab CAP
	if(ResourceCAPConf.m_bTabCntCAP)
	{
		DWORD dwTabCnt=0;
		dwTabCnt = IEStatus.GetTabCnt();
		//Tab Limit
		if(dwTabCnt >= ResourceCAPConf.m_uiTabCntMAX-1)//-1は、これからTabを追加するので。-1の余裕が無いとそもそもNG
		{
			if(ResourceCAPConf.m_uiTab_MAX_ShowTime>0)
			{
				strMsg=ResourceCAPConf.m_strTab_MAX_Msg;
				ShowTimeoutMessageBox(strMsg,strCaption,MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL,ResourceCAPConf.m_uiTab_MAX_ShowTime*1000);
			}
			return TRUE;
		}
	}
	if(ResourceCAPConf.m_bMemUsageCAP)
	{
		size_t iMem=0;
		iMem = IEStatus.GetMemoryUsageSize();
		TCHAR memorysize[24] = _T("");
		TCHAR WarmMemorysize[24] = _T("");
		TCHAR LimitMemorysize[24] = _T("");
		::StrFormatByteSize(iMem, memorysize, 24);
		::StrFormatByteSize(ResourceCAPConf.m_uiMemWARM*1024*1024,WarmMemorysize,24);
		::StrFormatByteSize(ResourceCAPConf.m_uiMemMAX*1024*1024,LimitMemorysize,24);

		//Mem Limit
		if(iMem >= ResourceCAPConf.m_uiMemMAX*1024*1024)
		{
			if(ResourceCAPConf.m_uiMem_MAX_ShowTime>0)
			{
				strMsg=ResourceCAPConf.m_strMem_MAX_Msg;
				ShowTimeoutMessageBox(strMsg,strCaption,MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL,ResourceCAPConf.m_uiMem_MAX_ShowTime*1000);
			}
			return TRUE;
		}
	}
	return bRet;
}

void CRedirectApp::ShowPlusSettingDlgEx()
{
	////////////////////////////////////////////////////
	if(m_pDlgMsg)
	{
		delete m_pDlgMsg;
		m_pDlgMsg=NULL;
	}
	m_pDlgMsg = new ProgressDlg;
	m_pDlgMsg->m_iSolutionType=PROC_LCustom;
	m_pDlgMsg->Create(MAKEINTRESOURCE(IDD_DIALOG1));
	m_pDlgMsg->ShowWindow(SW_SHOW);
	m_pDlgMsg->m_Prg.SetRange( 0, 2 );
	m_pDlgMsg->m_Prg.SetStep( 1 );
	m_pDlgMsg->m_Prg.StepIt();
	m_pDlgMsg->SetMsg(_T("設定画面 起動中...."));
	//////////////////////////////////////////////////////

	theApp.SettingConfMod.Copy(&theApp.SettingConf);


	//Resource
	theApp.ReadResourceCAPSetting();
	theApp.ResourceCAPConfMod.Copy(&theApp.ResourceCAPConf);

	m_RedirectList.SetArrayData(m_strRedirectFilePath);
	m_RedirectListSaveData.InitSaveDataAll();
	CSettingsDialog SettingDlg;
	CString strTitle;
	SettingDlg.SetLogoText(theApp.m_strThisAppName);
	strTitle.Format(_T("%s Settings [%s]"),theApp.m_strThisAppName,m_strRedirectFilePath);
	strTitle.Replace(_T("\\"),_T("/"));
	SettingDlg.SetTitle(strTitle);

	SettingDlg.AddPage(RUNTIME_CLASS(CDlgRemoteSettingGEN),_T("全般設定"), IDD_DLG_REMOTE,_T("全般設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgETC),_T("URLリダイレクト全般設定"), IDD_DLG_ETC,_T("全般設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgEco),_T("リソース設定"), IDD_DLG_ECO,_T("全般設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgRemoteSetting),_T("リモートブラウザー設定"), IDD_DLG_REMOTE,_T("リモートブラウザー設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgSetRDP),_T("Microsoft RDP 設定"), IDD_DLG_RDP,_T("リモートブラウザー設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgSetVMW),_T("VMware Horizon 設定"), IDD_DLG_VMW,_T("リモートブラウザー設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgSetCX),_T("Citrix XenApp 設定"), IDD_DLG_CX,_T("リモートブラウザー設定"));

	SettingDlg.AddPage(RUNTIME_CLASS(CDlgRemoteSettingRD),_T("URLリダイレクト設定"), IDD_DLG_REMOTE,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgRD),_T("Microsoft RDP (リモート)"), IDD_DLG_RD_RDP,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgVM),_T("VMware Horizon (リモート)"), IDD_DLG_RD_RDP,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCTX),_T("Citrix XenApp (リモート)"), IDD_DLG_RD_RDP,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgFF),_T("Mozilla Firefox (ローカル)"), IDD_DLG_RD_RDP,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCHR),_T("Google Chrome (ローカル)"), IDD_DLG_RD_RDP,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgEDG),_T("Microsoft Edge (ローカル)"), IDD_DLG_RD_RDP,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU01),_T("指定ブラウザー01 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU02),_T("指定ブラウザー02 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU03),_T("指定ブラウザー03 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU04),_T("指定ブラウザー04 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU05),_T("指定ブラウザー05 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU06),_T("指定ブラウザー06 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU07),_T("指定ブラウザー07 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU08),_T("指定ブラウザー08 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU09),_T("指定ブラウザー09 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU10),_T("指定ブラウザー10 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU11),_T("指定ブラウザー11 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU12),_T("指定ブラウザー12 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU13),_T("指定ブラウザー13 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU14),_T("指定ブラウザー14 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU15),_T("指定ブラウザー15 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU16),_T("指定ブラウザー16 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU17),_T("指定ブラウザー17 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU18),_T("指定ブラウザー18 (ローカル)"), IDD_DLG_RD_CUSTOM,_T("URLリダイレクト設定"));

	//Office365対応
	if (PathFileExists(m_strO365ToolFullPath))
	{
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgChromeSwitcher), _T("Chrome(自動切り換え)"), IDD_DLG_RD_CH_SWITCH, _T("URLリダイレクト設定"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgO365), _T("Office365 WebApps"), IDD_DLG_RD_O365, _T("URLリダイレクト設定"));
	}
	else
	{
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU19), _T("指定ブラウザー19 (ローカル)"), IDD_DLG_RD_CUSTOM, _T("URLリダイレクト設定"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU20), _T("指定ブラウザー20 (ローカル)"), IDD_DLG_RD_CUSTOM, _T("URLリダイレクト設定"));
	}

	m_pMainWnd = &SettingDlg;

	SettingDlg.DoModal();
//	if(SettingDlg.DoModal() == IDOK)
//	{
//	}
}

void CRedirectApp::ShowPlusSettingDlgCAP()
{
	////////////////////////////////////////////////////
	if(m_pDlgMsg)
	{
		delete m_pDlgMsg;
		m_pDlgMsg=NULL;
	}
	m_pDlgMsg = new ProgressDlg;
	m_pDlgMsg->m_iSolutionType=PROC_LCustom;
	m_pDlgMsg->Create(MAKEINTRESOURCE(IDD_DIALOG1));
	m_pDlgMsg->ShowWindow(SW_SHOW);
	m_pDlgMsg->m_Prg.SetRange( 0, 2 );
	m_pDlgMsg->m_Prg.SetStep( 1 );
	m_pDlgMsg->m_Prg.StepIt();
	m_pDlgMsg->SetMsg(_T("設定画面 起動中...."));
	//////////////////////////////////////////////////////

	theApp.SettingConfMod.Copy(&theApp.SettingConf);

	//Resource
	theApp.ReadResourceCAPSetting();
	theApp.ResourceCAPConfMod.Copy(&theApp.ResourceCAPConf);

	CSettingsDialogCAP SettingDlg;
	CString strTitle;
	SettingDlg.SetLogoText(theApp.m_strThisAppName);
	strTitle.Format(_T("%s Settings"),theApp.m_strThisAppName);
	SettingDlg.SetTitle(strTitle);

	SettingDlg.AddPage(RUNTIME_CLASS(CDlgEco),_T("リソース設定"), IDD_DLG_ECO,_T("リソース設定"));
	SettingDlg.AddPage(RUNTIME_CLASS(CDlgEtcCAP),_T("転送先ブラウザー設定(ローカル)"), IDD_DLG_ETC_CAP,_T("転送先ブラウザー設定(ローカル)"));
	m_pMainWnd = &SettingDlg;

	SettingDlg.DoModal();
//	if(SettingDlg.DoModal() == IDOK)
//	{
//	}
}

void CRedirectApp::ReadTemplate()
{
	CStdioFile in;
	CString strTemp;
	m_strRdp_Template_File_Data.Empty();
	if(in.Open(m_strTemplate_FileFullPath,CFile::modeRead|CFile::shareDenyWrite))
	{
		while(in.ReadString(strTemp))
		{
			strTemp.TrimLeft();
			strTemp.TrimRight();
			m_strRdp_Template_File_Data += strTemp;
			m_strRdp_Template_File_Data += _T("\n");
		}
		in.Close();
	}
	//theApp.WriteDebugTraceDateTime(_T("ReadTemplate"),DEBUG_LOG_TYPE_GE);
	//theApp.WriteDebugTraceDateTime(m_strRdp_Template_File_Data,DEBUG_LOG_TYPE_GE);
}
void CRedirectApp::ReadRDP_ReadOnceFile()
{
	CStdioFile in;
	CString strTemp;
	m_strRdp_ReadOnce_File_Data.Empty();
	if(in.Open(m_strRdp_ReadOnce_FileFullPath,CFile::modeRead|CFile::shareDenyWrite))
	{
		while(in.ReadString(strTemp))
		{
			strTemp.TrimLeft();
			strTemp.TrimRight();
			m_strRdp_ReadOnce_File_Data += strTemp;
			m_strRdp_ReadOnce_File_Data += _T("\n");
		}
		in.Close();
	}
	::DeleteFile(m_strRdp_ReadOnce_FileFullPath);
	SetLastError(NO_ERROR);

	//theApp.WriteDebugTraceDateTime(_T("ReadRDP_ReadOnceFile"),DEBUG_LOG_TYPE_GE);
	//theApp.WriteDebugTraceDateTime(m_strRdp_ReadOnce_File_Data,DEBUG_LOG_TYPE_GE);
}

void CRedirectApp::ReadSetting()
{
	CStdioFile in;
	CString strTemp;
	CString strLogMsgLine;
	if(in.Open(m_strSetting_FileFullPath,CFile::modeRead|CFile::shareDenyWrite))
	{
		CString strTemp2;
		CString strTemp3;
		CStringArray strArray;
		while(in.ReadString(strTemp))
		{
			strTemp.TrimLeft();
			strTemp.TrimRight();
			strArray.RemoveAll();
			SBUtil::Split(strArray,strTemp,_T("="));
			strLogMsgLine += strTemp;
			strLogMsgLine += "\n";

			if(strArray.GetSize() >= 2)
			{
				strTemp2=strArray.GetAt(0);
				strTemp2.TrimLeft();
				strTemp2.TrimRight();

				strTemp3=strArray.GetAt(1);
				strTemp3.TrimLeft();
				strTemp3.TrimRight();

				if(strTemp2.Find(_T(";"))==0)
				{
					;
				}
				else if(strTemp2.Find(_T("#"))==0)
				{
					;
				}
				else
				{
					if(strTemp2.CompareNoCase(_T("ServerName"))==0)
					{
						SettingConf.m_strServerName=strTemp3;
					}
					else if(strTemp2.CompareNoCase(_T("RemoteAppName"))==0)
					{
						SettingConf.m_strRemoteAppName=strTemp3;
					}
					else if(strTemp2.CompareNoCase(_T("RemoteAppPath"))==0)
					{
						SettingConf.m_strRemoteAppPath=strTemp3;
					}
					else if(strTemp2.CompareNoCase(_T("RemoteAppCommandLine"))==0)
					{
						SettingConf.m_strRemoteAppCommandLine=strTemp3;
					}
					else if(strTemp2.CompareNoCase(_T("RemoteAppMode"))==0)
					{
						if(strTemp3==_T("1"))
							SettingConf.m_bRemoteAppMode=TRUE;
						else
							SettingConf.m_bRemoteAppMode=FALSE;
					}
					else if(strTemp2.CompareNoCase(_T("Redirect_Clipboard"))==0)
					{
						if(strTemp3==_T("1"))
							SettingConf.m_bRedirect_Clipboard=TRUE;
						else
							SettingConf.m_bRedirect_Clipboard=FALSE;
					}
					else if(strTemp2.CompareNoCase(_T("Redirect_Printer"))==0)
					{
						if(strTemp3==_T("1"))
							SettingConf.m_bRedirect_Printer=TRUE;
						else
							SettingConf.m_bRedirect_Printer=FALSE;
					}
					else if(strTemp2.CompareNoCase(_T("Redirect_Drive"))==0)
					{
						if(strTemp3==_T("1"))
							SettingConf.m_bRedirect_Drive=TRUE;
						else
							SettingConf.m_bRedirect_Drive=FALSE;
					}
					else if(strTemp2.CompareNoCase(_T("Enable_Log"))==0)
					{
						if(strTemp3==_T("1"))
							SettingConf.m_bEnableDebugLog=TRUE;
						else
							SettingConf.m_bEnableDebugLog=FALSE;

					}
					else if(strTemp2.CompareNoCase(_T("Solution_Type"))==0)
					{
						if(strTemp3.CompareNoCase(_T("RDP"))==0)
							SettingConf.m_iSolutionType=PROC_RDP;
						else if(strTemp3.CompareNoCase(_T("VMWARE"))==0)
							SettingConf.m_iSolutionType=PROC_VMWARE;
						else if(strTemp3.CompareNoCase(_T("CITRIX"))==0)
							SettingConf.m_iSolutionType=PROC_CITRIX;
						else if(strTemp3.CompareNoCase(_T("DefaultBrowser"))==0)
							SettingConf.m_iSolutionType=PROC_LDefaultBrowser;
						else if(strTemp3.CompareNoCase(_T("IE"))==0)
							SettingConf.m_iSolutionType=PROC_LIE;
						else if(strTemp3.CompareNoCase(_T("Firefox"))==0)
							SettingConf.m_iSolutionType=PROC_LFirefox;
						else if(strTemp3.CompareNoCase(_T("Chrome"))==0)
							SettingConf.m_iSolutionType=PROC_LChrome;
						else if(strTemp3.CompareNoCase(_T("Edge"))==0)
							SettingConf.m_iSolutionType=PROC_LEdge;
						else if(strTemp3.CompareNoCase(_T("Custom"))==0)
							SettingConf.m_iSolutionType=PROC_LCustom;

					}
					else if(strTemp2.CompareNoCase(_T("v_ConnectionServerName"))==0)
					{
						SettingConf.m_strHorizon_ConnectionServerName=strTemp3;
					}
					else if(strTemp2.CompareNoCase(_T("v_AppName"))==0)
					{
						SettingConf.m_strHorizon_AppName=strTemp3;
					}
					else if(strTemp2.CompareNoCase(_T("CustomBrowserPath"))==0)
					{
						SettingConf.m_strCustomBrowserPath=strTemp3;
					}
					else if(strTemp2.CompareNoCase(_T("CX_AppName"))==0)
					{
						SettingConf.m_strCitrix_AppName=strTemp3;
					}
					else if(strTemp2.CompareNoCase(_T("KeyCombination"))==0)
					{
						if(!strTemp3.IsEmpty())
							SettingConf.m_iKeyCombination=_ttoi(strTemp3);
						else
							SettingConf.m_iKeyCombination=0;
					}

				}
			}
		}
		in.Close();
	}
	//theApp.WriteDebugTraceDateTime(_T("=================="),DEBUG_LOG_TYPE_GE);
	//theApp.WriteDebugTraceDateTime(_T("ReadSetting--START--"),DEBUG_LOG_TYPE_GE);
	//theApp.WriteDebugTraceDateTime(strLogMsgLine,DEBUG_LOG_TYPE_GE);
	//theApp.WriteDebugTraceDateTime(_T("ReadSetting--END=="),DEBUG_LOG_TYPE_GE);
}

BOOL CRedirectApp::ReadResourceCAPSettingFromReg()
{
//	TCHAR REG_SETTING_KEY[]=_T("SOFTWARE\\ThinBridge");
	TCHAR REG_SETTING_KEY[]=_T("SOFTWARE\\Policies\\ThinBridge\\RCAP");
	BOOL bRet=FALSE;
	DWORD Val=0;
	HKEY  hKey={0};
	LONG lResult=0L;
	DWORD dwType=0;
	DWORD dwSize=0;
	DWORD iSize=0;
	CString strVal;
	TCHAR* pVal=NULL;

	lResult=RegOpenKeyEx(HKEY_CURRENT_USER,REG_SETTING_KEY,0,KEY_READ,&hKey);
	if(lResult == ERROR_SUCCESS)
	{
		bRet=TRUE;
		Val=0;
		lResult=RegQueryValueEx(hKey,_T("EnableIETabLimit"),NULL,&dwType,NULL,&dwSize);
		if(lResult == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey,_T("EnableIETabLimit"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
			if(Val!=0)
				ResourceCAPConf.m_bTabCntCAP=TRUE;
			else
				ResourceCAPConf.m_bTabCntCAP=FALSE;
		}

		Val=0;
		lResult=RegQueryValueEx(hKey,_T("IETabLimit_WARNING"),NULL,&dwType,NULL,&dwSize);
		if(lResult == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey,_T("IETabLimit_WARNING"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
			int iW=Val;
			if(2<=iW && iW <=99)
				ResourceCAPConf.m_uiTabCntWARM=iW;
			else
				ResourceCAPConf.m_uiTabCntWARM=10;
		}

		Val=0;
		lResult=RegQueryValueEx(hKey,_T("IETabLimit_MAX"),NULL,&dwType,NULL,&dwSize);
		if(lResult == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey,_T("IETabLimit_MAX"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
			int iW=Val;
			if(3<=iW && iW <=100)
				ResourceCAPConf.m_uiTabCntMAX=iW;
			else
				ResourceCAPConf.m_uiTabCntMAX=15;
		}

		iSize=0;
		strVal.Empty();
		pVal=NULL;
		RegQueryValueEx(hKey,_T("IETabLimit_WARNING_MSG"),NULL,&dwType,NULL,&iSize);
		if(iSize>0)
		{
			iSize+=1;//+1=null
			pVal = new TCHAR[iSize];
			memset(pVal,0x00,sizeof(TCHAR)*iSize);
			RegQueryValueEx(hKey,_T("IETabLimit_WARNING_MSG"),NULL,&dwType,(LPBYTE)pVal, &iSize);
			strVal = pVal;
			delete [] pVal;
			if(!strVal.IsEmpty())
			{
				strVal.Replace(_T("\r\n"),_T("\n"));
				strVal.Replace(_T("\\n"),_T("\n"));
				ResourceCAPConf.m_strTab_WARM_Msg=strVal;
			}
		}

		iSize=0;
		strVal.Empty();
		pVal=NULL;
		RegQueryValueEx(hKey,_T("IETabLimit_MAX_MSG"),NULL,&dwType,NULL,&iSize);
		if(iSize>0)
		{
			iSize+=1;//+1=null
			pVal = new TCHAR[iSize];
			memset(pVal,0x00,sizeof(TCHAR)*iSize);
			RegQueryValueEx(hKey,_T("IETabLimit_MAX_MSG"),NULL,&dwType,(LPBYTE)pVal, &iSize);
			strVal = pVal;
			delete [] pVal;
			if(!strVal.IsEmpty())
			{
				strVal.Replace(_T("\r\n"),_T("\n"));
				strVal.Replace(_T("\\n"),_T("\n"));
				ResourceCAPConf.m_strTab_MAX_Msg=strVal;
			}
		}

		Val=0;
		lResult=RegQueryValueEx(hKey,_T("IETabLimit_WARNING_MSG_TIME"),NULL,&dwType,NULL,&dwSize);
		if(lResult == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey,_T("IETabLimit_WARNING_MSG_TIME"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
			int iW=Val;
			if(0<=iW && iW <=60)
				ResourceCAPConf.m_uiTab_WARM_ShowTime=iW;
			else
				ResourceCAPConf.m_uiTab_WARM_ShowTime=5;
		}

		Val=0;
		lResult=RegQueryValueEx(hKey,_T("IETabLimit_MAX_MSG_TIME"),NULL,&dwType,NULL,&dwSize);
		if(lResult == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey,_T("IETabLimit_MAX_MSG_TIME"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
			int iW=Val;
			if(0<=iW && iW <=60)
				ResourceCAPConf.m_uiTab_MAX_ShowTime=iW;
			else
				ResourceCAPConf.m_uiTab_MAX_ShowTime=5;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		Val=0;
		lResult=RegQueryValueEx(hKey,_T("EnableIEMemLimit"),NULL,&dwType,NULL,&dwSize);
		if(lResult == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey,_T("EnableIEMemLimit"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
			if(Val!=0)
				ResourceCAPConf.m_bMemUsageCAP=TRUE;
			else
				ResourceCAPConf.m_bMemUsageCAP=FALSE;
		}

		Val=0;
		lResult=RegQueryValueEx(hKey,_T("IEMemLimit_WARNING"),NULL,&dwType,NULL,&dwSize);
		if(lResult == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey,_T("IEMemLimit_WARNING"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
			int iW=Val;
			if(100<=iW && iW <=1000)
				ResourceCAPConf.m_uiMemWARM=iW;
			else
				ResourceCAPConf.m_uiMemWARM=600;
		}

		Val=0;
		lResult=RegQueryValueEx(hKey,_T("IEMemLimit_MAX"),NULL,&dwType,NULL,&dwSize);
		if(lResult == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey,_T("IEMemLimit_MAX"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
			int iW=Val;
			if(200<=iW && iW <=2000)
				ResourceCAPConf.m_uiMemMAX=iW;
			else
				ResourceCAPConf.m_uiMemMAX=1024;
		}

		iSize=0;
		strVal.Empty();
		pVal=NULL;
		RegQueryValueEx(hKey,_T("IEMemLimit_WARNING_MSG"),NULL,&dwType,NULL,&iSize);
		if(iSize>0)
		{
			iSize+=1;//+1=null
			pVal = new TCHAR[iSize];
			memset(pVal,0x00,sizeof(TCHAR)*iSize);
			RegQueryValueEx(hKey,_T("IEMemLimit_WARNING_MSG"),NULL,&dwType,(LPBYTE)pVal, &iSize);
			strVal = pVal;
			delete [] pVal;
			if(!strVal.IsEmpty())
			{
				strVal.Replace(_T("\r\n"),_T("\n"));
				strVal.Replace(_T("\\n"),_T("\n"));
				ResourceCAPConf.m_strMem_WARM_Msg=strVal;
			}
		}

		iSize=0;
		strVal.Empty();
		pVal=NULL;
		RegQueryValueEx(hKey,_T("IEMemLimit_MAX_MSG"),NULL,&dwType,NULL,&iSize);
		if(iSize>0)
		{
			iSize+=1;//+1=null
			pVal = new TCHAR[iSize];
			memset(pVal,0x00,sizeof(TCHAR)*iSize);
			RegQueryValueEx(hKey,_T("IEMemLimit_MAX_MSG"),NULL,&dwType,(LPBYTE)pVal, &iSize);
			strVal = pVal;
			delete [] pVal;
			if(!strVal.IsEmpty())
			{
				strVal.Replace(_T("\r\n"),_T("\n"));
				strVal.Replace(_T("\\n"),_T("\n"));
				ResourceCAPConf.m_strMem_MAX_Msg=strVal;
			}
		}

		Val=0;
		lResult=RegQueryValueEx(hKey,_T("IEMemLimit_WARNING_MSG_TIME"),NULL,&dwType,NULL,&dwSize);
		if(lResult == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey,_T("IEMemLimit_WARNING_MSG_TIME"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
			int iW=Val;
			if(0<=iW && iW <=60)
				ResourceCAPConf.m_uiMem_WARM_ShowTime=iW;
			else
				ResourceCAPConf.m_uiMem_WARM_ShowTime=5;
		}

		Val=0;
		lResult=RegQueryValueEx(hKey,_T("IEMemLimit_MAX_MSG_TIME"),NULL,&dwType,NULL,&dwSize);
		if(lResult == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey,_T("IEMemLimit_MAX_MSG_TIME"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
			int iW=Val;
			if(0<=iW && iW <=60)
				ResourceCAPConf.m_uiMem_MAX_ShowTime=iW;
			else
				ResourceCAPConf.m_uiMem_MAX_ShowTime=5;
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		Val=0;
		lResult=RegQueryValueEx(hKey,_T("EnableIEPriority"),NULL,&dwType,NULL,&dwSize);
		if(lResult == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey,_T("EnableIEPriority"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
			if(Val!=0)
				ResourceCAPConf.m_bCPUPriorityCAP=TRUE;
			else
				ResourceCAPConf.m_bCPUPriorityCAP=FALSE;
		}

		Val=0;
		lResult=RegQueryValueEx(hKey,_T("IEPriority"),NULL,&dwType,NULL,&dwSize);
		if(lResult == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey,_T("IEPriority"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
			int iW=Val;
			if(0<=iW && iW <=4)
				ResourceCAPConf.m_uiCPUPriority=iW;
			else
				ResourceCAPConf.m_uiCPUPriority=2;
		}
		RegCloseKey(hKey);
	}
	return bRet;
}
void CRedirectApp::ReadResourceCAPSetting()
{
	//HKEY_CUの設定を最優先させる。
	//2016-09-30
	if(ReadResourceCAPSettingFromReg())
	{
		ResourceCAPConf.m_strReadSettingType=_T("(GPO)");
		return;
	}

	CStdioFile in;
	CString strTemp;
	CString strLogMsgLine;
	if(in.Open(m_strResourceCAPFullPath,CFile::modeRead|CFile::shareDenyWrite))
	{
		CString strTemp2;
		CString strTemp3;
		CStringArray strArray;
		while(in.ReadString(strTemp))
		{
			strTemp.TrimLeft();
			strTemp.TrimRight();
			strArray.RemoveAll();
			SBUtil::Split(strArray,strTemp,_T("="));
			strLogMsgLine += strTemp;
			strLogMsgLine += "\n";

			if(strArray.GetSize() >= 2)
			{
				strTemp2=strArray.GetAt(0);
				strTemp2.TrimLeft();
				strTemp2.TrimRight();

				strTemp3=strArray.GetAt(1);
				strTemp3.TrimLeft();
				strTemp3.TrimRight();

				if(strTemp2.Find(_T(";"))==0)
				{
					;
				}
				else if(strTemp2.Find(_T("#"))==0)
				{
					;
				}
				else
				{
					if(strTemp2.CompareNoCase(_T("EnableIETabLimit"))==0)
					{
						ResourceCAPConf.m_strReadSettingType=_T("(INI)");
						if(strTemp3==_T("1"))
							ResourceCAPConf.m_bTabCntCAP=TRUE;
						else
							ResourceCAPConf.m_bTabCntCAP=FALSE;
					}
					else if(strTemp2.CompareNoCase(_T("IETabLimit_WARNING"))==0)
					{
						int iW=0;
						iW = _ttoi(strTemp3);
						if(2<=iW && iW <=99)
							ResourceCAPConf.m_uiTabCntWARM=iW;
						else
							ResourceCAPConf.m_uiTabCntWARM=10;
					}
					else if(strTemp2.CompareNoCase(_T("IETabLimit_MAX"))==0)
					{
						int iW=0;
						iW = _ttoi(strTemp3);
						if(3<=iW && iW <=100)
							ResourceCAPConf.m_uiTabCntMAX=iW;
						else
							ResourceCAPConf.m_uiTabCntMAX=15;
					}
					else if(strTemp2.CompareNoCase(_T("IETabLimit_WARNING_MSG"))==0)
					{
						if(!strTemp3.IsEmpty())
						{
							strTemp3.Replace(_T("\\n"),_T("\n"));
							ResourceCAPConf.m_strTab_WARM_Msg=strTemp3;
						}
					}
					else if(strTemp2.CompareNoCase(_T("IETabLimit_MAX_MSG"))==0)
					{
						if(!strTemp3.IsEmpty())
						{
							strTemp3.Replace(_T("\\n"),_T("\n"));
							ResourceCAPConf.m_strTab_MAX_Msg=strTemp3;
						}
					}
					else if(strTemp2.CompareNoCase(_T("IETabLimit_WARNING_MSG_TIME"))==0)
					{
						if(!strTemp3.IsEmpty())
						{
							int iW=0;
							iW = _ttoi(strTemp3);
							if(0<=iW && iW <=60)
								ResourceCAPConf.m_uiTab_WARM_ShowTime=iW;
							else
								ResourceCAPConf.m_uiTab_WARM_ShowTime=5;
						}
					}
					else if(strTemp2.CompareNoCase(_T("IETabLimit_MAX_MSG_TIME"))==0)
					{
						if(!strTemp3.IsEmpty())
						{
							int iW=0;
							iW = _ttoi(strTemp3);
							if(0<=iW && iW <=60)
								ResourceCAPConf.m_uiTab_MAX_ShowTime=iW;
							else
								ResourceCAPConf.m_uiTab_MAX_ShowTime=5;
						}
					}
					//////////////////////////////////////////////////////////////////////
					else if(strTemp2.CompareNoCase(_T("EnableIEMemLimit"))==0)
					{
						if(strTemp3==_T("1"))
							ResourceCAPConf.m_bMemUsageCAP=TRUE;
						else
							ResourceCAPConf.m_bMemUsageCAP=FALSE;
					}
					else if(strTemp2.CompareNoCase(_T("IEMemLimit_WARNING"))==0)
					{
						int iW=0;
						iW = _ttoi(strTemp3);
						if(100<=iW && iW <=1000)
							ResourceCAPConf.m_uiMemWARM=iW;
						else
							ResourceCAPConf.m_uiMemWARM=600;
					}
					else if(strTemp2.CompareNoCase(_T("IEMemLimit_MAX"))==0)
					{
						int iW=0;
						iW = _ttoi(strTemp3);
						if(200<=iW && iW <=2000)
							ResourceCAPConf.m_uiMemMAX=iW;
						else
							ResourceCAPConf.m_uiMemMAX=1024;
					}
					else if(strTemp2.CompareNoCase(_T("IEMemLimit_WARNING_MSG"))==0)
					{
						if(!strTemp3.IsEmpty())
						{
							strTemp3.Replace(_T("\\n"),_T("\n"));
							ResourceCAPConf.m_strMem_WARM_Msg=strTemp3;
						}
					}
					else if(strTemp2.CompareNoCase(_T("IEMemLimit_MAX_MSG"))==0)
					{
						if(!strTemp3.IsEmpty())
						{
							strTemp3.Replace(_T("\\n"),_T("\n"));
							ResourceCAPConf.m_strMem_MAX_Msg=strTemp3;
						}
					}
					else if(strTemp2.CompareNoCase(_T("IEMemLimit_WARNING_MSG_TIME"))==0)
					{
						if(!strTemp3.IsEmpty())
						{
							int iW=0;
							iW = _ttoi(strTemp3);
							if(0<=iW && iW <=60)
								ResourceCAPConf.m_uiMem_WARM_ShowTime=iW;
							else
								ResourceCAPConf.m_uiMem_WARM_ShowTime=5;
						}
					}
					else if(strTemp2.CompareNoCase(_T("IEMemLimit_MAX_MSG_TIME"))==0)
					{
						if(!strTemp3.IsEmpty())
						{
							int iW=0;
							iW = _ttoi(strTemp3);
							if(0<=iW && iW <=60)
								ResourceCAPConf.m_uiMem_MAX_ShowTime=iW;
							else
								ResourceCAPConf.m_uiMem_MAX_ShowTime=5;
						}
					}
					/////////////////////////////////////////////////////////////////////////////////
					else if(strTemp2.CompareNoCase(_T("EnableIEPriority"))==0)
					{
						if(strTemp3==_T("1"))
							ResourceCAPConf.m_bCPUPriorityCAP=TRUE;
						else
							ResourceCAPConf.m_bCPUPriorityCAP=FALSE;
					}
					else if(strTemp2.CompareNoCase(_T("IEPriority"))==0)
					{
						int iW=0;
						iW = _ttoi(strTemp3);
						if(0<=iW && iW <=4)
							ResourceCAPConf.m_uiCPUPriority=iW;
						else
							ResourceCAPConf.m_uiCPUPriority=2;
					}
				}
			}
		}
		in.Close();
	}
}
void CRedirectApp::CreateRDPFile(CString& strCommand)
{
	//theApp.WriteDebugTraceDateTime(_T("CreateRDPFile--START--"),DEBUG_LOG_TYPE_GE);
	//theApp.WriteDebugTraceDateTime(strCommand,DEBUG_LOG_TYPE_GE);
	CStdioFile out;
	if(out.Open(m_strRDP_FileFullPath,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
	{
		CString strWriteData;
		//接続先のコンピューター名（or IPアドレス）
		if(!SettingConf.m_strServerName.IsEmpty())
		{
			strWriteData.Format(_T("full address:s:%s\n"),SettingConf.m_strServerName);
			out.WriteString(strWriteData);
		}

		//RemoteAppモードを有効
		strWriteData.Format(_T("remoteapplicationmode:i:%d\n"),SettingConf.m_bRemoteAppMode?1:0);
		out.WriteString(strWriteData);

		//RemoteAppの場合だけ。
		if(SettingConf.m_bRemoteAppMode)
		{
			//RemoteAppのアプリ名
			if(!SettingConf.m_strRemoteAppName.IsEmpty())
			{
				strWriteData.Format(_T("remoteapplicationname:s:%s\n"),SettingConf.m_strRemoteAppName);
				out.WriteString(strWriteData);
			}

			//RemoteAppで実行するEXEのフルパス
			if(!SettingConf.m_strRemoteAppPath.IsEmpty())
			{
				strWriteData.Format(_T("remoteapplicationprogram:s:%s\n"),SettingConf.m_strRemoteAppPath);
				out.WriteString(strWriteData);
				strWriteData.Format(_T("alternate shell:s:%s\n"),SettingConf.m_strRemoteAppPath);
				out.WriteString(strWriteData);
			}

			//CommandLine
			//if(!m_strRemoteAppCommandLine.IsEmpty())
			//{
			//	strWriteData.Format(_T("remoteapplicationcmdline:s:%s\n"),m_strRemoteAppCommandLine);
			//	out.WriteString(strWriteData);
			//}

			//引数が空。
			if(strCommand.IsEmpty())
			{
				//GUIで設定したコマンドラインあり。
				if(!SettingConf.m_strRemoteAppCommandLine.IsEmpty())
				{
					strWriteData.Format(_T("remoteapplicationcmdline:s:%s\n"),SettingConf.m_strRemoteAppCommandLine);
					out.WriteString(strWriteData);
				}
			}
			else
			{
				//GUIで設定したコマンドラインあり。
				if(!SettingConf.m_strRemoteAppCommandLine.IsEmpty())
				{
					strWriteData.Format(_T("remoteapplicationcmdline:s:%s %s\n"),SettingConf.m_strRemoteAppCommandLine,strCommand);
					out.WriteString(strWriteData);
				}
				else
				{
					strWriteData.Format(_T("remoteapplicationcmdline:s:%s\n"),strCommand);
					out.WriteString(strWriteData);
				}
			}
		}

		//クリップボードのリダイレクト
		strWriteData.Format(_T("redirectclipboard:i:%d\n"),SettingConf.m_bRedirect_Clipboard?1:0);
		out.WriteString(strWriteData);

		//Printerのリダイレクト
		strWriteData.Format(_T("redirectprinters:i:%d\n"),SettingConf.m_bRedirect_Printer?1:0);
		out.WriteString(strWriteData);

		//ローカルのディスクドライブをリモートに接続する
		strWriteData.Format(_T("RedirectDrives:i:%d\n"),SettingConf.m_bRedirect_Drive?1:0);
		out.WriteString(strWriteData);

		//templateファイルの内容を追加
		out.WriteString(m_strRdp_Template_File_Data);

		//ReadOnceファイルの内容を追加
		out.WriteString(m_strRdp_ReadOnce_File_Data);
		out.Close();
	}
	theApp.Exec_MSTSC(m_strRDP_FileFullPath);
	//theApp.WriteDebugTraceDateTime(_T("CreateRDPFile-Thread_Running_Watch--START"),DEBUG_LOG_TYPE_GE);

	if(m_pReExecThread)
	{
		for(int j=0;j<1000;j++)
		{
			MSG    msg={0};
			if(::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
			{
				AfxGetThread()->PumpMessage();
			}
			if(m_pReExecThread->bRun)
			{
				Sleep(60);
			}
			else
			{
				break;
			}
		}
	}
	CString strLogMsg;
	strLogMsg.Format(_T("MS-RDP\t%s"),strCommand);
	theApp.WriteDebugTraceDateTime(strLogMsg,DEBUG_LOG_LEVEL_OUTPUT_URL);

	//theApp.WriteDebugTraceDateTime(_T("CreateRDPFile-Thread_Running_Watch--END"),DEBUG_LOG_TYPE_GE);
	//theApp.WriteDebugTraceDateTime(_T("CreateRDPFile--END=="),DEBUG_LOG_TYPE_GE);

}
void CRedirectApp::Exec_MSTSC(CString& strRDPFile)
{
	//theApp.WriteDebugTraceDateTime(_T("Exec_MSTSC--START"),DEBUG_LOG_TYPE_GE);
	if(m_pDlgMsg)
	{
		delete m_pDlgMsg;
		m_pDlgMsg=NULL;
	}
	m_pDlgMsg = new ProgressDlg;
	m_pDlgMsg->m_iSolutionType=PROC_RDP;
	m_pDlgMsg->Create(MAKEINTRESOURCE(IDD_DIALOG1));

	m_pDlgMsg->ShowWindow(SW_SHOW);
	m_pDlgMsg->m_Prg.SetRange( 0, 10 );
	m_pDlgMsg->m_Prg.SetStep( 1 );
	m_pDlgMsg->SetMsg(_T("起動準備中...."));
	m_pDlgMsg->m_Prg.StepIt();
	m_pDlgMsg->SetMsg(strRDPFile);
	::Sleep(100);
	if(m_pReExecThread)
	{
		delete m_pReExecThread;
		m_pReExecThread = NULL;
	}
	m_pDlgMsg->m_Prg.StepIt();
	m_pReExecThread = new CCRre();
	::Sleep(100);
	m_pReExecThread->m_iSolutionType=PROC_RDP;
	m_pReExecThread->m_pDlgMsg=m_pDlgMsg;
	m_pReExecThread->m_strFilePath=strRDPFile;
	m_pDlgMsg->m_Prg.StepIt();
	::Sleep(100);
	m_pDlgMsg->SetMsg(_T("MS-RDP 起動中...."));
	::Sleep(100);
	m_pDlgMsg->m_Prg.StepIt();
	::Sleep(100);
	m_pDlgMsg->m_Prg.StepIt();
	m_pDlgMsg->m_Prg.StepIt();
	//theApp.WriteDebugTraceDateTime(_T("Exec_MSTSC-Thread_EXEC--START"),DEBUG_LOG_TYPE_GE);
	m_pReExecThread->Exec();
	//theApp.WriteDebugTraceDateTime(_T("Exec_MSTSC-Thread_EXEC--END"),DEBUG_LOG_TYPE_GE);
	::Sleep(100);
	//theApp.WriteDebugTraceDateTime(_T("Exec_MSTSC--END"),DEBUG_LOG_TYPE_GE);

	//2秒後にRDPファイルを削除する。
	::Sleep(1000*3);
	::DeleteFile(strRDPFile);
	SetLastError(NO_ERROR);
	//削除したファイルが存在するか念のため確認
	if(PathFileExists(strRDPFile))
	{
		//最大60回 削除処理をリトライする。
		for(int iDup=0;iDup<60;iDup++)
		{
			::DeleteFile(strRDPFile);
			SetLastError(NO_ERROR);
			//ファイル存在チェック
			if(!PathFileExists(strRDPFile))
			{
				//存在しない。削除されたら抜ける
				break;
			}
			//10回に1回は、30秒待つ
			if(iDup%10==0)//初回ループで0除算になるが、剰余の場合は0除算エラーは発生しない。MS VS2010
			{
				::Sleep(30*1000);
			}
			else
			{
				//1秒待機してからリトライ
				::Sleep(1000);
			}
		}
	}

}

void CRedirectApp::Exec_VMwareHorizon_Start(CString& CommandParam)
{
	//theApp.WriteDebugTraceDateTime(_T("Exec_VMwareHorizon_Start--START"),DEBUG_LOG_TYPE_GE);
	Exec_VMware_Horizon(SettingConf.m_strHorizon_ConnectionServerName,SettingConf.m_strHorizon_AppName,CommandParam);
	//theApp.WriteDebugTraceDateTime(_T("Exec_VMwareHorizon_Start-Thread_Running_Watch--START"),DEBUG_LOG_TYPE_GE);

	if(m_pReExecThread)
	{
		for(int j=0;j<8800;j++)
		{
			MSG    msg={0};
			if(::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
			{
				AfxGetThread()->PumpMessage();
			}
			if(m_pReExecThread->bRun)
			{
				Sleep(15);
			}
			else
			{
				break;
			}
		}
	}
	if(m_pDlgMsg)
	{
		delete m_pDlgMsg;
		m_pDlgMsg=NULL;
	}
	//::Sleep(2*1000);
	CString strLogMsg;
	strLogMsg.Format(_T("VMware-Horizon\t%s"),CommandParam);
	theApp.WriteDebugTraceDateTime(strLogMsg,DEBUG_LOG_LEVEL_OUTPUT_URL);

	//theApp.WriteDebugTraceDateTime(_T("Exec_VMwareHorizon_Start-Thread_Running_Watch--END"),DEBUG_LOG_TYPE_GE);
	//theApp.WriteDebugTraceDateTime(_T("Exec_VMwareHorizon_Start--END=="),DEBUG_LOG_TYPE_GE);
}
void CRedirectApp::Exec_VMware_Horizon(CString& strServerHostName,CString& strAppName,CString& strURL)
{
	//theApp.WriteDebugTraceDateTime(_T("Exec_VMware-Horizon--START"),DEBUG_LOG_TYPE_GE);
	if(m_pDlgMsg)
	{
		delete m_pDlgMsg;
		m_pDlgMsg=NULL;
	}
	m_pDlgMsg = new ProgressDlg;
	m_pDlgMsg->m_iSolutionType=PROC_VMWARE;
	m_pDlgMsg->Create(MAKEINTRESOURCE(IDD_DIALOG1));

	m_pDlgMsg->ShowWindow(SW_SHOW);
	m_pDlgMsg->m_Prg.SetRange( 0, 10 );
	m_pDlgMsg->m_Prg.SetStep( 1 );
	m_pDlgMsg->SetMsg(_T("起動準備中...."));
	m_pDlgMsg->m_Prg.StepIt();
	m_pDlgMsg->SetMsg(strServerHostName);
	::Sleep(100);
	if(m_pReExecThread)
	{
		delete m_pReExecThread;
		m_pReExecThread = NULL;
	}
	m_pDlgMsg->m_Prg.StepIt();
	m_pReExecThread = new CCRre();

	::Sleep(100);
	m_pReExecThread->m_iSolutionType=PROC_VMWARE;
	m_pReExecThread->m_pDlgMsg=m_pDlgMsg;
	m_pReExecThread->m_strHorizon_ConnectionServerName=strServerHostName;
	m_pReExecThread->m_strHorizon_AppName=strAppName;
	m_pReExecThread->m_strURL=strURL;
	m_pDlgMsg->m_Prg.StepIt();
	m_pDlgMsg->SetMsg(_T("VMware Horizon Client 起動中...."));
	m_pDlgMsg->m_Prg.StepIt();
	//theApp.WriteDebugTraceDateTime(_T("Exec_VMware-Horizon-Thread_EXEC--START"),DEBUG_LOG_TYPE_GE);
	m_pReExecThread->Exec();
	//theApp.WriteDebugTraceDateTime(_T("Exec_VMware-Horizon-Thread_EXEC--END"),DEBUG_LOG_TYPE_GE);
	//theApp.WriteDebugTraceDateTime(_T("Exec_VMware-Horizon--END"),DEBUG_LOG_TYPE_GE);
}


void CRedirectApp::Exec_CitixXenApp_Start(CString& CommandParam)
{
	//theApp.WriteDebugTraceDateTime(_T("Exec_CitixXenApp_Start--START"),DEBUG_LOG_TYPE_GE);
	Exec_Citrix_XenApp(SettingConf.m_strCitrix_AppName,CommandParam);
	//theApp.WriteDebugTraceDateTime(_T("Exec_CitixXenApp_Start-Thread_Running_Watch--START"),DEBUG_LOG_TYPE_GE);

	if(m_pReExecThread)
	{
		for(int j=0;j<8800;j++)
		{
			MSG    msg={0};
			if(::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
			{
				AfxGetThread()->PumpMessage();
			}
			if(m_pReExecThread->bRun)
			{
				Sleep(15);
			}
			else
			{
				break;
			}
		}
	}
	if(m_pDlgMsg)
	{
		delete m_pDlgMsg;
		m_pDlgMsg=NULL;
	}
	//::Sleep(2*1000);
	CString strLogMsg;
	strLogMsg.Format(_T("Citrix-XenApp\t%s"),CommandParam);
	theApp.WriteDebugTraceDateTime(strLogMsg,DEBUG_LOG_LEVEL_OUTPUT_URL);

	//theApp.WriteDebugTraceDateTime(_T("Exec_CitixXenApp_Start-Thread_Running_Watch--END"),DEBUG_LOG_TYPE_GE);
	//theApp.WriteDebugTraceDateTime(_T("Exec_CitixXenApp_Start--END=="),DEBUG_LOG_TYPE_GE);
}

void CRedirectApp::Exec_Citrix_XenApp(CString& strAppName,CString& strURL)
{
	//theApp.WriteDebugTraceDateTime(_T("Exec_Citrix_XenApp--START"),DEBUG_LOG_TYPE_GE);
	if(m_pDlgMsg)
	{
		delete m_pDlgMsg;
		m_pDlgMsg=NULL;
	}
	m_pDlgMsg = new ProgressDlg;
	m_pDlgMsg->m_iSolutionType=PROC_CITRIX;
	m_pDlgMsg->Create(MAKEINTRESOURCE(IDD_DIALOG1));

	m_pDlgMsg->ShowWindow(SW_SHOW);
	m_pDlgMsg->m_Prg.SetRange( 0, 10 );
	m_pDlgMsg->m_Prg.SetStep( 1 );
	m_pDlgMsg->SetMsg(_T("起動準備中...."));
	m_pDlgMsg->m_Prg.StepIt();
	m_pDlgMsg->SetMsg(_T("Citrix XenApp"));
	::Sleep(100);
	if(m_pReExecThread)
	{
		delete m_pReExecThread;
		m_pReExecThread = NULL;
	}
	m_pDlgMsg->m_Prg.StepIt();
	m_pReExecThread = new CCRre();

	::Sleep(100);
	m_pReExecThread->m_iSolutionType=PROC_CITRIX;
	m_pReExecThread->m_pDlgMsg=m_pDlgMsg;
	m_pReExecThread->m_strCitrix_AppName=strAppName;
	m_pReExecThread->m_strURL=strURL;
	m_pDlgMsg->m_Prg.StepIt();
	m_pDlgMsg->SetMsg(_T("Citrix XenApp 接続中...."));
	m_pDlgMsg->m_Prg.StepIt();
	//theApp.WriteDebugTraceDateTime(_T("Exec_Citrix_XenApp-Thread_EXEC--START"),DEBUG_LOG_TYPE_GE);
	m_pReExecThread->Exec();
	//theApp.WriteDebugTraceDateTime(_T("Exec_Citrix_XenApp-Thread_EXEC--END"),DEBUG_LOG_TYPE_GE);
	//theApp.WriteDebugTraceDateTime(_T("Exec_Citrix_XenApp--END"),DEBUG_LOG_TYPE_GE);
}



void CRedirectApp::Exec_LocalBrowser(CString& strURL)
{
	//theApp.WriteDebugTraceDateTime(_T("Exec_LocalBrowser--START"),DEBUG_LOG_TYPE_GE);
	Exec_Local_Browser(SettingConf.m_iSolutionType,SettingConf.m_strCustomBrowserPath,strURL);
	//theApp.WriteDebugTraceDateTime(_T("Exec_LocalBrowser-Thread_Running_Watch--START"),DEBUG_LOG_TYPE_GE);

	if(m_pReExecThread)
	{
		for(int j=0;j<1000;j++)
		{
			MSG    msg={0};
			if(::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
			{
				AfxGetThread()->PumpMessage();
			}
			if(m_pReExecThread->bRun)
			{
				Sleep(60);
			}
			else
			{
				break;
			}
		}
	}
	//theApp.WriteDebugTraceDateTime(_T("Exec_LocalBrowser-Thread_Running_Watch--END"),DEBUG_LOG_TYPE_GE);
	//theApp.WriteDebugTraceDateTime(_T("Exec_LocalBrowser--END=="),DEBUG_LOG_TYPE_GE);
}

void CRedirectApp::Exec_Local_Browser(UINT iSolutionType,CString& strCustomBrowserPath,CString& strURL)
{
	//theApp.WriteDebugTraceDateTime(_T("Exec_Local_Browser--START"),DEBUG_LOG_TYPE_GE);
	if(m_pDlgMsg)
	{
		delete m_pDlgMsg;
		m_pDlgMsg=NULL;
	}
	m_pDlgMsg = new ProgressDlg;
	m_pDlgMsg->m_iSolutionType=iSolutionType;
	m_pDlgMsg->Create(MAKEINTRESOURCE(IDD_DIALOG1));

	m_pDlgMsg->ShowWindow(SW_SHOW);
	m_pDlgMsg->m_Prg.SetRange( 0, 10 );
	m_pDlgMsg->m_Prg.SetStep( 1 );
	m_pDlgMsg->SetMsg(_T("起動準備中...."));
	m_pDlgMsg->m_Prg.StepIt();
	m_pDlgMsg->SetMsg(_T("ローカルブラウザー起動中"));
	::Sleep(100);
	if(m_pReExecThread)
	{
		delete m_pReExecThread;
		m_pReExecThread = NULL;
	}
	m_pDlgMsg->m_Prg.StepIt();
	m_pReExecThread = new CCRre();
	::Sleep(100);
	m_pReExecThread->m_iSolutionType=iSolutionType;
	m_pReExecThread->m_pDlgMsg=m_pDlgMsg;
	m_pReExecThread->m_strCustomBrowserPath=strCustomBrowserPath;
	m_pReExecThread->m_strURL=strURL;
	m_pDlgMsg->m_Prg.StepIt();
	::Sleep(100);

	CString strLogMsg;

	if(iSolutionType==PROC_LDefaultBrowser)
	{
		m_pDlgMsg->SetMsg(_T("DefaultBrowser 起動中...."));
		strLogMsg.Format(_T("DefaultBrowser\t%s"),strURL);
	}
	else if(iSolutionType==PROC_LIE)
	{
		m_pDlgMsg->SetMsg(_T("Internet Explorer 起動中...."));
		strLogMsg.Format(_T("IE\t%s"),strURL);
	}
	else if(iSolutionType==PROC_LFirefox)
	{
		m_pDlgMsg->SetMsg(_T("Mozilla Firefox 起動中...."));
		strLogMsg.Format(_T("Firefox\t%s"),strURL);
	}
	else if(iSolutionType==PROC_LChrome)
	{
		m_pDlgMsg->SetMsg(_T("Google Chrome 起動中...."));
		strLogMsg.Format(_T("Chrome\t%s"),strURL);
	}
	else if(iSolutionType==PROC_LEdge)
	{
		m_pDlgMsg->SetMsg(_T("Microsoft Edge 起動中...."));
		strLogMsg.Format(_T("Edge\t%s"),strURL);
	}
	else if(iSolutionType==PROC_LCustom)
	{
		m_pDlgMsg->SetMsg(strCustomBrowserPath);
		strLogMsg.Format(_T("Custom\t%s"),strURL);
	}

	::Sleep(100);
	m_pDlgMsg->m_Prg.StepIt();
	::Sleep(100);
	m_pDlgMsg->m_Prg.StepIt();
	m_pDlgMsg->m_Prg.StepIt();
	//theApp.WriteDebugTraceDateTime(_T("Exec_Local_Browser-Thread_EXEC--START"),DEBUG_LOG_TYPE_GE);
	m_pReExecThread->Exec();
	//theApp.WriteDebugTraceDateTime(_T("Exec_Local_Browser-Thread_EXEC--END"),DEBUG_LOG_TYPE_GE);
	::Sleep(100);
	//theApp.WriteDebugTraceDateTime(_T("Exec_Local_Browser--END"),DEBUG_LOG_TYPE_GE);
	theApp.WriteDebugTraceDateTime(strLogMsg,DEBUG_LOG_LEVEL_OUTPUT_URL);

}

void CRedirectApp::SetIE_FullPath()
{
	HKEY  hKey={0};
	DWORD dwDisposition=0;
	LONG lResult=0L;
	CString strVal;
	DWORD dwType=0;

	lResult=RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\iexplore.exe"),
		0,KEY_READ,&hKey);
	if(lResult == ERROR_SUCCESS)
	{
		DWORD iSize=0;
		TCHAR* pVal=NULL;

		RegQueryValueEx(hKey,_T(""),NULL,&dwType,NULL,&iSize);
		if(iSize>0)
		{
			iSize+=1;//+1=null
			pVal = new TCHAR[iSize];
			memset(pVal,0x00,sizeof(TCHAR)*iSize);
			RegQueryValueEx(hKey,_T(""),NULL,&dwType,(LPBYTE)pVal, &iSize);
			strVal = pVal;
			delete [] pVal;
		}
		RegCloseKey(hKey);
	}
	m_strIE_FullPath=strVal;
}

void CRedirectApp::SetFirefox_FullPath()
{
	HKEY  hKey={0};
	DWORD dwDisposition=0;
	LONG lResult=0L;
	CString strVal;
	DWORD dwType=0;

	lResult=RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\firefox.exe"),
		0,KEY_READ,&hKey);
	if(lResult == ERROR_SUCCESS)
	{
		DWORD iSize=0;
		TCHAR* pVal=NULL;

		RegQueryValueEx(hKey,_T(""),NULL,&dwType,NULL,&iSize);
		if(iSize>0)
		{
			iSize+=1;//+1=null
			pVal = new TCHAR[iSize];
			memset(pVal,0x00,sizeof(TCHAR)*iSize);
			RegQueryValueEx(hKey,_T(""),NULL,&dwType,(LPBYTE)pVal, &iSize);
			strVal = pVal;
			delete [] pVal;
		}
		RegCloseKey(hKey);
	}
	m_strFirefox_FullPath=strVal;
}

void CRedirectApp::SetChrome_FullPath()
{
	HKEY  hKey={0};
	DWORD dwDisposition=0;
	LONG lResult=0L;
	CString strVal;
	DWORD dwType=0;

	lResult=RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe"),
		0,KEY_READ,&hKey);
	if(lResult == ERROR_SUCCESS)
	{
		DWORD iSize=0;
		TCHAR* pVal=NULL;

		RegQueryValueEx(hKey,_T(""),NULL,&dwType,NULL,&iSize);
		if(iSize>0)
		{
			iSize+=1;//+1=null
			pVal = new TCHAR[iSize];
			memset(pVal,0x00,sizeof(TCHAR)*iSize);
			RegQueryValueEx(hKey,_T(""),NULL,&dwType,(LPBYTE)pVal, &iSize);
			strVal = pVal;
			delete [] pVal;
		}
		RegCloseKey(hKey);
		m_strChrome_FullPath=strVal;
	}
	//HK_CUをチェック
	if(m_strChrome_FullPath.IsEmpty())
	{
		lResult=RegOpenKeyEx(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe"),
			0,KEY_ALL_ACCESS,&hKey);
		if(lResult == ERROR_SUCCESS)
		{
			DWORD iSize=0;
			TCHAR* pVal=NULL;

			RegQueryValueEx(hKey,_T(""),NULL,&dwType,NULL,&iSize);
			if(iSize>0)
			{
				iSize+=1;//+1=null
				pVal = new TCHAR[iSize];
				memset(pVal,0x00,sizeof(TCHAR)*iSize);
				RegQueryValueEx(hKey,_T(""),NULL,&dwType,(LPBYTE)pVal, &iSize);
				strVal = pVal;
				delete [] pVal;
			}
			RegCloseKey(hKey);
			m_strChrome_FullPath=strVal;
		}
	}
}

BOOL CRedirectApp::IsFirefoxInstalled()
{
	BOOL bRet=FALSE;
	SetFirefox_FullPath();
	if(!m_strFirefox_FullPath.IsEmpty())
	{
		if(PathFileExists(m_strFirefox_FullPath))
		{
			bRet = TRUE;
		}
	}
	return bRet;
}

BOOL CRedirectApp::IsChromeInstalled()
{
	BOOL bRet=FALSE;
	SetChrome_FullPath();
	if(!m_strChrome_FullPath.IsEmpty())
	{
		if(PathFileExists(m_strChrome_FullPath))
		{
			bRet = TRUE;
		}
	}
	return bRet;
}


BOOL CRedirectApp::IsCustomInstalled(CString strPath)
{
	BOOL bRet=FALSE;
	if(!strPath.IsEmpty())
	{
		if(PathFileExists(strPath))
		{
			bRet = TRUE;
		}
	}
	return bRet;
}

BOOL CRedirectApp::IsHorizonInstalled()
{
	BOOL bRet = FALSE;
	HKEY  hKey={0};
	DWORD dwDisposition=0;
	LONG lResult=0L;
	CString strVal;
	DWORD dwType=0;

	lResult=RegOpenKeyEx(HKEY_CLASSES_ROOT,_T("vmware-view\\shell\\open\\command"),
		0,KEY_READ,&hKey);

	if(lResult == ERROR_SUCCESS)
	{
		DWORD iSize=0;
		TCHAR* pVal=NULL;

		RegQueryValueEx(hKey,_T(""),NULL,&dwType,NULL,&iSize);
		if(iSize>0)
		{
			iSize+=1;//+1=null
			pVal = new TCHAR[iSize];
			memset(pVal,0x00,sizeof(TCHAR)*iSize);
			RegQueryValueEx(hKey,_T(""),NULL,&dwType,(LPBYTE)pVal, &iSize);
			strVal = pVal;
			delete [] pVal;
		}
		RegCloseKey(hKey);
	}
	if(!strVal.IsEmpty())
		bRet = TRUE;

	return bRet;
}

BOOL CRedirectApp::IsCitrixInstalled()
{
	BOOL bRet = FALSE;
	HKEY  hKey={0};
	DWORD dwDisposition=0;
	LONG lResult=0L;
	CString strVal;
	DWORD dwType=0;

	lResult=RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Citrix\\Dazzle"),
		0,KEY_READ,&hKey);
	if(lResult == ERROR_SUCCESS)
	{
		DWORD iSize=0;
		TCHAR* pVal=NULL;

		RegQueryValueEx(hKey,_T("Launcher"),NULL,&dwType,NULL,&iSize);
		if(iSize>0)
		{
			iSize+=1;//+1=null
			pVal = new TCHAR[iSize];
			memset(pVal,0x00,sizeof(TCHAR)*iSize);
			RegQueryValueEx(hKey,_T("Launcher"),NULL,&dwType,(LPBYTE)pVal, &iSize);
			strVal = pVal;
			delete [] pVal;
		}
		RegCloseKey(hKey);
	}

	if(strVal.IsEmpty())
		strVal = _T("c:\\program files\\Citrix\\ICA Client\\SelfServicePlugin\\SelfService.exe");

	if(PathFileExists(strVal))
	{
		bRet = TRUE;
	}

	return bRet;
}

int CRedirectApp::ExitInstance()
{
	if(m_pDlgMsg)
	{
		delete m_pDlgMsg;
		m_pDlgMsg=NULL;
	}
	if(m_pReExecThread)
	{
		delete m_pReExecThread;
		m_pReExecThread = NULL;
	}
	//theApp.WriteDebugTraceDateTime(_T("ExitInstance"),DEBUG_LOG_TYPE_GE);
	//theApp.WriteDebugTraceDateTime(_T("------------------"),DEBUG_LOG_TYPE_GE);
	return CWinApp::ExitInstance();
}
struct LANGANDCODEPAGE
{
	WORD wLanguage;
	WORD wCodePage;
} *lpTranslate;

CString CRedirectApp::GetVersionStr()
{
	CString strDLLPath;
	strDLLPath = theApp.m_strExeFullPath;
	CString strRet;
	DWORD dwDummy = 0;
	DWORD dwSize = 0;
	DWORD dwMajar = 0;
	DWORD dwMinor = 0;
	DWORD dwBuild = 0;
	DWORD dwPrivate = 0;

	strRet = theApp.m_strThisAppName;
	strRet += _T(" Version N/A");

	dwSize = ::GetFileVersionInfoSize(strDLLPath, &dwDummy);
	if (dwSize > 0)
	{
		PBYTE pData = new BYTE[dwSize];
		memset(pData, 0x00, dwSize);
		UINT TranslateLen = 0;
		if (::GetFileVersionInfo(strDLLPath, 0, dwSize, pData))
		{
			VerQueryValue(pData, _T("\\VarFileInfo\\Translation"),
				(LPVOID*)&lpTranslate, &TranslateLen);
			void *pvVersion = { 0 };
			UINT VersionLen = 0;

			for (UINT i = 0; i < TranslateLen / sizeof(*lpTranslate); i++)
			{
				//コードページを指定
				CString name;

				name.Format(_T("\\StringFileInfo\\%04x%04x\\%s"),
					lpTranslate[i].wLanguage,
					lpTranslate[i].wCodePage, _T("FileVersion"));
				if (VerQueryValue(pData, name, &pvVersion, &VersionLen))
				{
					CString strVersionStr((LPCTSTR)pvVersion);
					strRet.Format(_T("%s Version %s"), theApp.m_strThisAppName, strVersionStr);
					break;
				}
			}
		}
		delete[] pData;
	}
	return strRet;
}

typedef int (__stdcall * TMessageBoxTimeout)(HWND, LPCTSTR, LPCTSTR, UINT, WORD, DWORD);
void CRedirectApp::ShowTimeoutMessageBox(LPCTSTR strMsg,LPCTSTR strCaption,int iType,int iTimeOut)
{
	BOOL bFreeLibrary = FALSE;
	HMODULE hModule = {0};
	hModule = ::GetModuleHandle(_T("user32.dll"));
	if(!hModule)
	{
		hModule = ::LoadLibrary(_T("user32.dll"));
		if(hModule)
			bFreeLibrary = TRUE;
	}

	if(hModule)
	{
		TMessageBoxTimeout  MessageBoxTimeout;
		MessageBoxTimeout = (TMessageBoxTimeout) GetProcAddress(hModule, "MessageBoxTimeoutW");
		if(MessageBoxTimeout)
		{
			MessageBoxTimeout(NULL, strMsg,
                       strCaption, iType, LANG_NEUTRAL,iTimeOut);
		}
		else
		{
			::MessageBox(NULL,strMsg,strCaption,iType);
		}
		if(bFreeLibrary)
		{

			FreeLibrary(hModule);
			bFreeLibrary=FALSE;
			hModule=NULL;
		}

	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CCRre, CWinThread)

//DDE Callback
static HDDEDATA CALLBACK DDECallback(WORD wType, WORD wFmt, HCONV hConv, HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD lData1, DWORD lData2)
{
	return (HDDEDATA)0;
}

void CCRre::IEStart(CString& strURL)
{
	HSZ hszService={0};
	HSZ hszTopic={0};

	HCONV hConv={0};
	HDDEDATA hDDEData={0};
	DWORD m_dwDDEID = 0;


	if(!DdeInitialize(&m_dwDDEID,(PFNCALLBACK)MakeProcInstance((FARPROC)DDECallback, ghInstance),
		CBF_SKIP_ALLNOTIFICATIONS | APPCMD_CLIENTONLY, 0L) != DMLERR_NO_ERROR)
	{
		hszService = DdeCreateStringHandle(m_dwDDEID, _T("IEXPLORE"), CP_WINNEUTRAL);
		hszTopic = DdeCreateStringHandle(m_dwDDEID, _T("WWW_OpenURL"), CP_WINNEUTRAL);
		{
			hConv = DdeConnect(m_dwDDEID, hszService, hszTopic, NULL);
			DdeFreeStringHandle(m_dwDDEID, hszService);
 			DdeFreeStringHandle(m_dwDDEID, hszTopic);
			if(hConv)
			{
				CString cmd;
				cmd = strURL;
				DWORD result=0;
				hDDEData = DdeClientTransaction((LPBYTE)cmd.GetBuffer(0),static_cast<DWORD>((cmd.GetLength()+1)*2),hConv,0,0,XTYP_EXECUTE,30000,&result);
				DdeDisconnect(hConv);
				DdeUninitialize(m_dwDDEID);
				theApp.WriteDebugTraceDateTime(_T("IEStart---DDE-Success."),DEBUG_LOG_TYPE_GE);
				return;
			}
		}
	}

	if(::ShellExecute(NULL,_T("open"),_T("iexplore.exe"),strURL,NULL, SW_SHOW) <= HINSTANCE(32))
	{
		theApp.WriteDebugTraceDateTime(_T("IEStart Stage1 Failed."),DEBUG_LOG_TYPE_GE);
		CString IEcmd;
		IEcmd.Format(_T("iexplorer.exe %s"),strURL);
		if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
		{
			theApp.WriteDebugTraceDateTime(_T("IEStart Stage2 Failed."),DEBUG_LOG_TYPE_GE);
			IEcmd=_T("");
			IEcmd.Format(_T("\"%s\" %s"),theApp.m_strIE_FullPath,strURL);
			if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
			{
				theApp.WriteDebugTraceDateTime(_T("IEStart Stage3 Failed."),DEBUG_LOG_TYPE_GE);
				IEcmd=_T("");
				IEcmd.Format(_T("\"C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe\" %s"),strURL);
				if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
				{
					theApp.WriteDebugTraceDateTime(_T("IEStart Stage4 Failed."),DEBUG_LOG_TYPE_GE);
					IEcmd=_T("");
					IEcmd.Format(_T("\"C:\\Program Files\\Internet Explorer\\iexplore.exe\" %s"),strURL);
					::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW);
					theApp.WriteDebugTraceDateTime(_T("IEStart Stage5 Success."),DEBUG_LOG_TYPE_GE);
				}
				else
					theApp.WriteDebugTraceDateTime(_T("IEStart Stage4 Success."),DEBUG_LOG_TYPE_GE);
			}
			else
				theApp.WriteDebugTraceDateTime(_T("IEStart Stage3 Success."),DEBUG_LOG_TYPE_GE);
		}
		else
			theApp.WriteDebugTraceDateTime(_T("IEStart Stage2 Success."),DEBUG_LOG_TYPE_GE);

	}
	else
		theApp.WriteDebugTraceDateTime(_T("IEStart Stage1 Success."),DEBUG_LOG_TYPE_GE);

}

void CCRre::FirefoxStart(CString& strURL)
{
	if(::ShellExecute(NULL,_T("open"),_T("firefox.exe"),strURL,NULL, SW_SHOW) <= HINSTANCE(32))
	{
		theApp.WriteDebugTraceDateTime(_T("FirefoxStart Stage1 Failed."),DEBUG_LOG_TYPE_GE);
		CString IEcmd;
		IEcmd.Format(_T("firefox.exe %s"),m_strURL);
		if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
		{
			theApp.WriteDebugTraceDateTime(_T("FirefoxStart Stage2 Failed."),DEBUG_LOG_TYPE_GE);
			IEcmd=_T("");
			IEcmd.Format(_T("\"%s\" %s"),theApp.m_strFirefox_FullPath,strURL);
			if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
			{
				theApp.WriteDebugTraceDateTime(_T("FirefoxStart Stage3 Failed."),DEBUG_LOG_TYPE_GE);
				IEcmd=_T("");
				IEcmd.Format(_T("\"C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe\" %s"),strURL);
				if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
				{
					theApp.WriteDebugTraceDateTime(_T("FirefoxStart Stage4 Failed."),DEBUG_LOG_TYPE_GE);
					IEcmd=_T("");
					IEcmd.Format(_T("\"C:\\Program Files\\Mozilla Firefox\\firefox.exe\" %s"),strURL);
					::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW);
					theApp.WriteDebugTraceDateTime(_T("FirefoxStart Stage5 Success."),DEBUG_LOG_TYPE_GE);
				}
				else
					theApp.WriteDebugTraceDateTime(_T("FirefoxStart Stage4 Success."),DEBUG_LOG_TYPE_GE);
			}
			else
				theApp.WriteDebugTraceDateTime(_T("FirefoxStart Stage3 Success."),DEBUG_LOG_TYPE_GE);
		}
		else
			theApp.WriteDebugTraceDateTime(_T("FirefoxStart Stage2 Success."),DEBUG_LOG_TYPE_GE);
	}
	else
		theApp.WriteDebugTraceDateTime(_T("FirefoxStart Stage1 Success."),DEBUG_LOG_TYPE_GE);
}

void CCRre::ChromeStart(CString& strURL)
{
	if(::ShellExecute(NULL,_T("open"),_T("chrome.exe"),strURL,NULL, SW_SHOW) <= HINSTANCE(32))
	{
		theApp.WriteDebugTraceDateTime(_T("ChromeStart Stage1 Failed."),DEBUG_LOG_TYPE_GE);
		CString IEcmd;
		IEcmd.Format(_T("chrome.exe %s"),strURL);
		if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
		{
			theApp.WriteDebugTraceDateTime(_T("ChromeStart Stage2 Failed."),DEBUG_LOG_TYPE_GE);
			IEcmd=_T("");
			IEcmd.Format(_T("\"%s\" %s"),theApp.m_strChrome_FullPath,strURL);
			if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
			{
				theApp.WriteDebugTraceDateTime(_T("ChromeStart Stage3 Failed."),DEBUG_LOG_TYPE_GE);
				IEcmd=_T("");
				IEcmd.Format(_T("\"C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe\" %s"),strURL);
				if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
				{
					theApp.WriteDebugTraceDateTime(_T("ChromeStart Stage4 Failed."),DEBUG_LOG_TYPE_GE);
					IEcmd=_T("");
					IEcmd.Format(_T("\"C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe\" %s"),strURL);
					::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW);
					theApp.WriteDebugTraceDateTime(_T("ChromeStart Stage5 Success."),DEBUG_LOG_TYPE_GE);
				}
				else
					theApp.WriteDebugTraceDateTime(_T("ChromeStart Stage4 Success."),DEBUG_LOG_TYPE_GE);
			}
			else
				theApp.WriteDebugTraceDateTime(_T("ChromeStart Stage3 Success."),DEBUG_LOG_TYPE_GE);
		}
		else
			theApp.WriteDebugTraceDateTime(_T("ChromeStart Stage2 Success."),DEBUG_LOG_TYPE_GE);
	}
	else
		theApp.WriteDebugTraceDateTime(_T("ChromeStart Stage1 Success."),DEBUG_LOG_TYPE_GE);
}

void CCRre::EdgeStart(CString& strURL)
{
	CString IEcmd;
	IEcmd.Format(_T("microsoft-edge:%s"),strURL);
	::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW);
	theApp.WriteDebugTraceDateTime(_T("EdgeStart Stage1 Success."),DEBUG_LOG_TYPE_GE);
}

void CCRre::DefaultBrowserStart(CString& strURL)
{
	if(::ShellExecute(NULL,NULL,strURL,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
	{
		theApp.WriteDebugTraceDateTime(_T("DefaultBrowserStart Stage1 Failed."),DEBUG_LOG_TYPE_GE);
		if(::ShellExecute(NULL,_T("open"),strURL,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
		{
			theApp.WriteDebugTraceDateTime(_T("DefaultBrowserStart Stage2 Failed."),DEBUG_LOG_TYPE_GE);
			if(::ShellExecute(NULL,NULL,_T("explorer.exe"),strURL,NULL, SW_SHOW) <= HINSTANCE(32))
			{
				theApp.WriteDebugTraceDateTime(_T("DefaultBrowserStart Stage3 Failed."),DEBUG_LOG_TYPE_GE);
				::ShellExecute(NULL,_T("open"),_T("iexplore.exe"),strURL,NULL, SW_SHOW);
				theApp.WriteDebugTraceDateTime(_T("DefaultBrowserStart Stage4 Success."),DEBUG_LOG_TYPE_GE);
			}
			else
				theApp.WriteDebugTraceDateTime(_T("DefaultBrowserStart Stage3 Success."),DEBUG_LOG_TYPE_GE);
		}
		else
			theApp.WriteDebugTraceDateTime(_T("DefaultBrowserStart Stage2 Success."),DEBUG_LOG_TYPE_GE);
	}
	else
		theApp.WriteDebugTraceDateTime(_T("DefaultBrowserStart Stage1 Success."),DEBUG_LOG_TYPE_GE);
}

void CCRre::CustomBrowserStart(CString& strPath,CString& strURL)
{
	CString IEcmd;
	IEcmd=_T("");
	IEcmd.Format(_T("\"%s\" %s"),strPath,strURL);
	CString strPathQ;
	strPathQ.Format(_T("\"%s\""),strPath);
	STARTUPINFO si={0};
	PROCESS_INFORMATION pi={0};
	si.cb = sizeof( si );
	if(!CreateProcess( NULL,(LPTSTR)(LPCTSTR)IEcmd,NULL, NULL, FALSE, 0, NULL,NULL, &si, &pi ))
	{
		theApp.WriteDebugTraceDateTime(_T("CustomBrowserStart Stage1 Failed."),DEBUG_LOG_TYPE_GE);
		SetLastError(NO_ERROR);
		//Retry
		STARTUPINFO si2={0};
		PROCESS_INFORMATION pi2={0};
		si2.cb = sizeof( si2 );
		if(!CreateProcess( strPathQ,(LPTSTR)(LPCTSTR)strURL,NULL, NULL, FALSE, 0, NULL,NULL, &si2, &pi2 ))
		{
			theApp.WriteDebugTraceDateTime(_T("CustomBrowserStart Stage2 Failed."),DEBUG_LOG_TYPE_GE);
			SetLastError(NO_ERROR);
			if(::ShellExecute(NULL,_T("open"),strPathQ,strURL,NULL, SW_SHOW) <= HINSTANCE(32))
			{
				theApp.WriteDebugTraceDateTime(_T("CustomBrowserStart Stage3 Failed."),DEBUG_LOG_TYPE_GE);
				SetLastError(NO_ERROR);
				::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW);
				theApp.WriteDebugTraceDateTime(_T("CustomBrowserStart Stage4 Success."),DEBUG_LOG_TYPE_GE);
			}
			else
				theApp.WriteDebugTraceDateTime(_T("CustomBrowserStart Stage3 Success."),DEBUG_LOG_TYPE_GE);
		}
		else
		{
			theApp.WriteDebugTraceDateTime(_T("CustomBrowserStart Stage2 Success."),DEBUG_LOG_TYPE_GE);
			if(pi2.hThread)
			{
				CloseHandle( pi2.hThread );  // スレッドのハンドルは使わないのですぐ破棄
				pi2.hThread=0;
			}
			if(pi2.hProcess)
			{
				CloseHandle( pi2.hProcess ); // もうプロセスのハンドルは使わないので破棄
				pi2.hProcess=0;
			}
		}
	}
	else
	{
		theApp.WriteDebugTraceDateTime(_T("CustomBrowserStart Stage1 Success."),DEBUG_LOG_TYPE_GE);
		if(pi.hThread)
		{
			CloseHandle( pi.hThread );  // スレッドのハンドルは使わないのですぐ破棄
			pi.hThread=0;
		}
		if(pi.hProcess)
		{
			CloseHandle( pi.hProcess ); // もうプロセスのハンドルは使わないので破棄
			pi.hProcess=0;
		}
	}
}

void CCRre::VMwareViewStart(CString& strCommand)
{
	CString strViewProc;
	strViewProc = Get_ViewClient_ProtocolCommand();
	BOOL bExecFlg=FALSE;
	if(!strViewProc.IsEmpty())
	{
		if(strViewProc.Find(_T("%1")) > 0)
		{
			strViewProc.Replace(_T("%1"),strCommand);
			if(::ShellExecute(NULL,_T("open"),strCommand,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
			{
				theApp.WriteDebugTraceDateTime(_T("VMwareViewStart Stage1 Failed."),DEBUG_LOG_TYPE_GE);
				bExecFlg=FALSE;
			}
			else
			{
				theApp.WriteDebugTraceDateTime(_T("VMwareViewStart Stage1 Success."),DEBUG_LOG_TYPE_GE);
				bExecFlg=TRUE;
			}
		}
	}

	if(!bExecFlg)
	{
		if(::ShellExecute(NULL,NULL,strCommand,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
			theApp.WriteDebugTraceDateTime(_T("VMwareViewStart Stage2 Failed."),DEBUG_LOG_TYPE_GE);
		else
			theApp.WriteDebugTraceDateTime(_T("VMwareViewStart Stage2 Success."),DEBUG_LOG_TYPE_GE);
	}
}

void CCRre::CitrixXenAppStart(CString& strCommand)
{
	CString strCitrixSelfService;
	strCitrixSelfService = Get_Citrix_SelfserviceEXEPath();
	BOOL bExecFlg=FALSE;
	if(strCitrixSelfService.IsEmpty())
		strCitrixSelfService=_T("c:\\program files\\Citrix\\ICA Client\\SelfServicePlugin\\SelfService.exe");

	CString IEcmd;
	IEcmd=_T("");
	IEcmd.Format(_T("\"%s\" %s"),strCitrixSelfService,strCommand);
	CString strPathQ;
	strPathQ.Format(_T("\"%s\""),strCitrixSelfService);

	STARTUPINFO si={0};
	PROCESS_INFORMATION pi={0};
	si.cb = sizeof( si );
	if(!CreateProcess( NULL,(LPTSTR)(LPCTSTR)IEcmd,NULL, NULL, FALSE, 0, NULL,NULL, &si, &pi ))
	{
		theApp.WriteDebugTraceDateTime(_T("CitrixXenAppStart Stage1 Failed."),DEBUG_LOG_TYPE_GE);
		SetLastError(NO_ERROR);
		//Retry
		STARTUPINFO si2={0};
		PROCESS_INFORMATION pi2={0};
		si2.cb = sizeof( si2 );
		if(!CreateProcess( strPathQ,(LPTSTR)(LPCTSTR)strCommand,NULL, NULL, FALSE, 0, NULL,NULL, &si2, &pi2 ))
		{
			theApp.WriteDebugTraceDateTime(_T("CitrixXenAppStart Stage2 Failed."),DEBUG_LOG_TYPE_GE);
			SetLastError(NO_ERROR);
			if(::ShellExecute(NULL,_T("open"),strPathQ,strCommand,NULL, SW_SHOW) <= HINSTANCE(32))
			{
				theApp.WriteDebugTraceDateTime(_T("CitrixXenAppStart Stage3 Failed."),DEBUG_LOG_TYPE_GE);
				SetLastError(NO_ERROR);
				::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW);
				theApp.WriteDebugTraceDateTime(_T("CitrixXenAppStart Stage4 Success."),DEBUG_LOG_TYPE_GE);
			}
			else
				theApp.WriteDebugTraceDateTime(_T("CitrixXenAppStart Stage3 Success."),DEBUG_LOG_TYPE_GE);
		}
		else
		{
			theApp.WriteDebugTraceDateTime(_T("CitrixXenAppStart Stage2 Success."),DEBUG_LOG_TYPE_GE);
			if(pi2.hThread)
			{
				CloseHandle( pi2.hThread );  // スレッドのハンドルは使わないのですぐ破棄
				pi2.hThread=0;
			}
			if(pi2.hProcess)
			{
				CloseHandle( pi2.hProcess ); // もうプロセスのハンドルは使わないので破棄
				pi2.hProcess=0;
			}
		}
	}
	else
	{
		theApp.WriteDebugTraceDateTime(_T("CitrixXenAppStart Stage1 Success."),DEBUG_LOG_TYPE_GE);
		if(pi.hThread)
		{
			CloseHandle( pi.hThread );  // スレッドのハンドルは使わないのですぐ破棄
			pi.hThread=0;
		}
		if(pi.hProcess)
		{
			CloseHandle( pi.hProcess ); // もうプロセスのハンドルは使わないので破棄
			pi.hProcess=0;
		}
	}
}

CString CCRre::Get_ViewClient_ProtocolCommand()
{
	HKEY  hKey={0};
	DWORD dwDisposition=0;
	LONG lResult=0L;
	CString strVal;
	DWORD dwType=0;

	lResult=RegOpenKeyEx(HKEY_CLASSES_ROOT,_T("vmware-view\\shell\\open\\command"),
		0,KEY_READ,&hKey);
	if(lResult == ERROR_SUCCESS)
	{
		DWORD iSize=0;
		TCHAR* pVal=NULL;

		RegQueryValueEx(hKey,_T(""),NULL,&dwType,NULL,&iSize);
		if(iSize>0)
		{
			iSize+=1;//+1=null
			pVal = new TCHAR[iSize];
			memset(pVal,0x00,sizeof(TCHAR)*iSize);
			RegQueryValueEx(hKey,_T(""),NULL,&dwType,(LPBYTE)pVal, &iSize);
			strVal = pVal;
			delete [] pVal;
		}
		RegCloseKey(hKey);
	}
	return strVal;
}

CString CCRre::Get_Citrix_SelfserviceEXEPath()
{
	HKEY  hKey={0};
	DWORD dwDisposition=0;
	LONG lResult=0L;
	CString strVal;
	DWORD dwType=0;

	lResult=RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Citrix\\Dazzle"),
		0,KEY_READ,&hKey);
	if(lResult == ERROR_SUCCESS)
	{
		DWORD iSize=0;
		TCHAR* pVal=NULL;

		RegQueryValueEx(hKey,_T("Launcher"),NULL,&dwType,NULL,&iSize);
		if(iSize>0)
		{
			iSize+=1;//+1=null
			pVal = new TCHAR[iSize];
			memset(pVal,0x00,sizeof(TCHAR)*iSize);
			RegQueryValueEx(hKey,_T("Launcher"),NULL,&dwType,(LPBYTE)pVal, &iSize);
			strVal = pVal;
			delete [] pVal;
		}
		RegCloseKey(hKey);
	}
	return strVal;
}


CCRre::CCRre(HWND hwnd)
{
	// スレッドの起動待ち
	CreateThread();
	CSingleLock sLock( &m_event, TRUE );
	m_bAutoDelete = FALSE;
	m_pDlgMsg=NULL;
//	m_hWndParent=hwnd;
	m_dwThreadId=NULL;
	bRun=FALSE;
}
CCRre::CCRre()
{
	// スレッドの起動待ち
	CreateThread();
	CSingleLock sLock( &m_event, TRUE );
	m_bAutoDelete = FALSE;
	m_pDlgMsg=NULL;
//	m_hWndParent=NULL;
	m_dwThreadId=NULL;
	bRun=FALSE;
	m_iSolutionType=PROC_RDP;
}

CCRre::~CCRre()
{
	// スレッドの消滅待ち
	if(!m_bAutoDelete )
	{
		PostThreadMessage( WM_QUIT, 0, 0 );
//		::WaitForSingleObject( m_hThread, INFINITE );
		DWORD waitResult = ::WaitForSingleObject(m_hThread, 15000 );
		if (waitResult == WAIT_TIMEOUT)
		{
			theApp.WriteDebugTraceDateTime(_T("CCRre::~CCRre(WAIT_TIMEOUT)"),DEBUG_LOG_TYPE_EX);
		}
		//bRun=FALSE;
	}
}

BOOL CCRre::InitInstance()
{
	m_event.SetEvent();
	return TRUE;
}

int CCRre::ExitInstance()
{
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCRre, CWinThread)
	//{{AFX_MSG_MAP(CCRre)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加します。
	//}}AFX_MSG_MAP
	ON_THREAD_MESSAGE(MSG_THREAD_START,OnExec)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCRre メッセージ ハンドラ
void CCRre::Exec()
{
	//theApp.WriteDebugTraceDateTime(_T("Exec--START"),DEBUG_LOG_TYPE_GE);
	MSG msgDummy={0};
	PeekMessage(&msgDummy, NULL, 0, 0, PM_NOREMOVE);
	PostThreadMessage( MSG_THREAD_START, 0, 0 );
	::Sleep(100);

	CString strCommand;
	if(m_pDlgMsg)
	{
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
	}

	for(;;)
	{
		if(PROC_RDP==m_iSolutionType)
		{
			STARTUPINFO si={0};
			PROCESS_INFORMATION pi={0};
			si.cb = sizeof( si );

			if(!m_strFilePath.IsEmpty())
				strCommand.Format(_T("\"%s\" \"%s\""),_T("mstsc.exe"),m_strFilePath);
			else
				strCommand.Format(_T("\"%s\""),_T("mstsc.exe"));
			if(m_pDlgMsg)m_pDlgMsg->m_Prg.StepIt();
			if(!CreateProcess( NULL,(LPTSTR)(LPCTSTR)strCommand,NULL, NULL, FALSE, 0, NULL,NULL, &si, &pi ))
			{
				theApp.WriteDebugTraceDateTime(_T("MSTSCStart Stage1 Failed."),DEBUG_LOG_TYPE_GE);
				SetLastError(NO_ERROR);
				//Retry
				if(!CreateProcess( _T("mstsc.exe"),(LPTSTR)(LPCTSTR)m_strFilePath,NULL, NULL, FALSE, 0, NULL,NULL, &si, &pi ))
				{
					theApp.WriteDebugTraceDateTime(_T("MSTSCStart Stage2 Failed."),DEBUG_LOG_TYPE_GE);
					SetLastError(NO_ERROR);
					if(::ShellExecute(NULL,_T("open"),_T("mstsc.exe"),m_strFilePath,NULL, SW_SHOW) <= HINSTANCE(32))
					{
						theApp.WriteDebugTraceDateTime(_T("MSTSCStart Stage3 Failed."),DEBUG_LOG_TYPE_GE);
						SetLastError(NO_ERROR);
						::ShellExecute(NULL,NULL,m_strFilePath,NULL,NULL, SW_SHOW);
						theApp.WriteDebugTraceDateTime(_T("MSTSCStart Stage4 Success."),DEBUG_LOG_TYPE_GE);
					}
					else
						theApp.WriteDebugTraceDateTime(_T("MSTSCStart Stage3 Success."),DEBUG_LOG_TYPE_GE);
				}
				else
					theApp.WriteDebugTraceDateTime(_T("MSTSCStart Stage2 Success."),DEBUG_LOG_TYPE_GE);

			}
			else
				theApp.WriteDebugTraceDateTime(_T("MSTSCStart Stage1 Success."),DEBUG_LOG_TYPE_GE);
			if(pi.hThread)
			{
				CloseHandle( pi.hThread );  // スレッドのハンドルは使わないのですぐ破棄
				pi.hThread=0;
			}
			if(pi.hProcess)
			{
				CloseHandle( pi.hProcess ); // もうプロセスのハンドルは使わないので破棄
				pi.hProcess=0;
			}
			break;
		}
		else if(PROC_VMWARE==m_iSolutionType)
		{
			CString strAppNameEncode;
			CString strURLEncode;
			strAppNameEncode = theApp.ConvertUTF8_URLEncode(m_strHorizon_AppName);
			if(this->m_strURL.IsEmpty())
			{
				strCommand.Format(_T("vmware-view://%s/%s"),m_strHorizon_ConnectionServerName,strAppNameEncode);
			}
			else
			{
				strURLEncode = theApp.ConvertUTF8_URLEncode(m_strURL);
				strCommand.Format(_T("vmware-view://%s/%s?url=%s"),m_strHorizon_ConnectionServerName,strAppNameEncode,strURLEncode);
			}
			//theApp.WriteDebugTraceDateTime(_T("OnExec--START--VMwareViewStart(strCommand)"),DEBUG_LOG_TYPE_GE);
			this->VMwareViewStart(strCommand);
			//theApp.WriteDebugTraceDateTime(strCommand,DEBUG_LOG_TYPE_GE);
			break;
		}
		else if(PROC_CITRIX==m_iSolutionType)
		{
			if(this->m_strURL.IsEmpty())
			{
				strCommand.Format(_T("-qlaunch \"%s\""),m_strCitrix_AppName);
			}
			else
			{
				strCommand.Format(_T("-qlaunch \"%s\" %s"),m_strCitrix_AppName,m_strURL);
			}
			//theApp.WriteDebugTraceDateTime(_T("OnExec--START--CitrixXenAppStart(strCommand)"),DEBUG_LOG_TYPE_GE);
			this->CitrixXenAppStart(strCommand);
			//theApp.WriteDebugTraceDateTime(strCommand,DEBUG_LOG_TYPE_GE);
			break;
		}

		else if(PROC_LDefaultBrowser==m_iSolutionType)
		{
			this->DefaultBrowserStart(m_strURL);
			//theApp.WriteDebugTraceDateTime(m_strURL,DEBUG_LOG_TYPE_GE);
			break;
		}
		else if(PROC_LIE==m_iSolutionType)
		{
			this->IEStart(m_strURL);
			//theApp.WriteDebugTraceDateTime(m_strURL,DEBUG_LOG_TYPE_GE);
			break;
		}
		else if(PROC_LFirefox==m_iSolutionType)
		{
			this->FirefoxStart(m_strURL);
			//theApp.WriteDebugTraceDateTime(m_strURL,DEBUG_LOG_TYPE_GE);
			break;
		}
		else if(PROC_LChrome==m_iSolutionType)
		{
			this->ChromeStart(m_strURL);
			//theApp.WriteDebugTraceDateTime(m_strURL,DEBUG_LOG_TYPE_GE);
			break;
		}
		else if(PROC_LEdge==m_iSolutionType)
		{
			this->EdgeStart(m_strURL);
			//theApp.WriteDebugTraceDateTime(m_strURL,DEBUG_LOG_TYPE_GE);
			break;
		}
		else if(PROC_LCustom==m_iSolutionType)
		{
			this->CustomBrowserStart(m_strCustomBrowserPath,m_strURL);
			//theApp.WriteDebugTraceDateTime(m_strURL,DEBUG_LOG_TYPE_GE);
			break;
		}
		break;
	}
	::Sleep(100);
	if(m_pDlgMsg)
	{
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
	}
	//theApp.WriteDebugTraceDateTime(_T("Exec--END"),DEBUG_LOG_TYPE_GE);
	return;
}

void CCRre::OnExec(WPARAM wParam, LPARAM lParam)
{
	//theApp.WriteDebugTraceDateTime(_T("OnExec--START"),DEBUG_LOG_TYPE_GE);
	bRun=TRUE;
	if(m_pDlgMsg)
	{
		::Sleep(100);
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		::Sleep(100);
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		::Sleep(100);
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		::Sleep(100);
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		m_pDlgMsg->m_Prg.StepIt();
		::Sleep(100);
		m_pDlgMsg->ShowWindow(SW_HIDE);
	}
	bRun=FALSE;
	//theApp.WriteDebugTraceDateTime(_T("OnExec--END"),DEBUG_LOG_TYPE_GE);
	return;
}

//////////////////////////////////////////////////////

/*
#接続先のコンピューター名（or IPアドレス）を指定する。
full address:s:@@REMOTE_SERVER@@

##RemoteApp関連
#RemoteAppモードを有効
remoteapplicationmode:i:1

#RemoteAppのアプリ名
remoteapplicationname:s:@@Remote_AppName@@

#RemoteAppで実行するEXEのフルパス
remoteapplicationprogram:s:@@Remote_AppPath@@
alternate shell:s:@@Remote_AppPath@@

#CommandLine
remoteapplicationcmdline:s:@@Remote_App_CmdLine@@

#作業フォルダ
shell working directory:s:

##Redirect関連
#Printerのリダイレクト 0無効 1有効
redirectprinters:i:@@Redirect_Printer@@

#クリップボードのリダイレクト 0無効 1有効
redirectclipboard:i:@@Redirect_Clipboard@@

#ローカルのディスクドライブをリモートに接続する
RedirectDrives:i@@Redirect_Drive@@

#シリアルポートのリダイレクト 0無効 1有効
redirectcomports:i:0

#スマートカードのリダイレクト 0無効 1有効
redirectsmartcards:i:0


#データを転送するときに圧縮する。
compression:i:1

#接続品質 1：モデム、2：低速、3：衛星、4：高速、5：WAN、6：LAN 7:自動検出
connection type:i:7

#リモートデスクトップのDirectXリダイレクト機能
RedirectDirectX:i:1

#ネットワーク自動調整
networkautodetect:i:1
bandwidthautodetect:i:1

#フルスクリーンモードで接続した際に、接続バーを表示するか制御するパラメータ
displayconnectionbar:i:1

enableworkspacereconnect:i:0

#背景を表示するかどうかを指定する
disable wallpaper:i:0

#ClearType機能を有効にするかを指定する
allow font smoothing:i:1

#デスクトップコンポジション機能を有効にするかを指定する
allow desktop composition:i:0

#ドラッグ中にウインドウの内容を表示
disable full window drag:i:1

#メニューとウインドウアニメーション
disable menu anims:i:1

#テーマ機能を抑制するかを指定する
disable themes:i:0

#カーソルのアニメーション効果を抑制するか
disable cursor setting:i:0

bitmapcachepersistenable:i:1

#リモートオーディオ再生
audiomode:i:0

#切断された場合に自動的に再接続する
autoreconnection enabled:i:1

#サーバー認証でポリシーが最低要件を満たしていない場合の動作を指定す
authentication level:i:2

#リモートコンピュータにゲートウェイの資格情報を使用する
prompt for credentials:i:0

#セキュリティレイヤ接続は無効でSSLを使用する
negotiate security layer:i:1


*/



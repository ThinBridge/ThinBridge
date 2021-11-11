
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

	m_strThisAppName=_T("ThinBridge");
	SetAppID(m_strThisAppName);
	m_bCitrixCustomEnv = FALSE;
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

	m_strExeFolderPath =szDrive;
	m_strExeFolderPath += szDir;

	m_strO365ToolFullPath = szDrive;
	m_strO365ToolFullPath += szDir;
	CString strCitrixCustomEnvFlgEXE;
	strCitrixCustomEnvFlgEXE = m_strO365ToolFullPath;
	strCitrixCustomEnvFlgEXE += _T("TBo365URLSync.exe");
	//
	m_bCitrixCustomEnv = PathFileExists(strCitrixCustomEnvFlgEXE);

	m_strO365ToolFullPath += _T("TBo365URLSyncSetting.exe");


	m_strChromeSwitcherFullPath = szDrive;
	m_strChromeSwitcherFullPath += szDir;
	m_strChromeSwitcherFullPath += _T("TBChromeSwitcher.exe");


	TCHAR szTemp[MAX_PATH+1]={0};
	::GetTempPath(MAX_PATH,szTemp);
	CString strTempPath;

	strTempPath = szTemp;
	strTempPath += theApp.m_strThisAppName;
	::CreateDirectory(strTempPath, NULL);
	SetLastError(NO_ERROR);

	//template�̓Ǎ�
	m_strTemplate_FileFullPath = szDrive;
	m_strTemplate_FileFullPath += szDir;
	m_strTemplate_FileFullPath += _T("rdp_template.conf");

	m_strTestConnect_FileFullPath = strTempPath;
	m_strTestConnect_FileFullPath += _T("\\Test_Connect.rdp");

	//ReadOnce�̃p�X�Z�b�g
	m_strRdp_ReadOnce_FileFullPath = szDrive;
	m_strRdp_ReadOnce_FileFullPath += szDir;
	m_strRdp_ReadOnce_FileFullPath += _T("rdp_readonce.conf");

	//RDP�t�@�C���̃p�X
	m_strRDP_FileFullPath = strTempPath;
	m_strRDP_FileFullPath += _T("\\");
	m_strRDP_FileFullPath += theApp.m_strThisAppName;

	//���[�v�h�~
	m_strLoopBlockFilePath=strTempPath;
	m_strLoopBlockFilePath+= _T("\\STP.dat");
	m_LoopBlock.SetFilePath(m_strLoopBlockFilePath);

	//PID��t������B��ʂɓ����N�������ƁA�R���t���N�g����\�������邽�߁B
	//�^�C�~���O�ɂ��폜�ł��Ȃ��P�[�X���l�����邽�߁A��1���̐���0-9�𗘗p���鎖�ɂ���B
	//�ő��16��RDP�t�@�C�������[�e�[�V�����B
	DWORD dPID = 0;
	dPID = ::GetCurrentProcessId();
	CString strPID;
	strPID.Format(_T("%04d"),dPID);
	//m_strRDP_FileFullPath +=strPID;
	m_strRDP_FileFullPath +=strPID.Right(1);
	m_strRDP_FileFullPath +=_T(".RDP");
	//�t�@�C�������ɑ��݂��邩�m�F
	if(PathFileExists(m_strRDP_FileFullPath))
	{
		//���ɁA���̃t�@�C�������݂��Ă���ꍇ�B
		//PID���ė��p���ꂽ�ꍇ�ɁA�d������\�����l������(�����Ƃ͎v�����A�O�ɂ͔O��)

		//RDP�t�@�C�����i�[����t�H���_�[�p�X
		CString strRDP_FileTemp;
		strRDP_FileTemp = strTempPath;

		//�t�@�C����(�ꎞ�t�@�C��)
		CString strRDPFileNameTemp;
		strRDPFileNameTemp = theApp.m_strThisAppName;
		strRDPFileNameTemp += strPID;

		//���݃`�F�b�N����t���p�X�i�[
		CString strRDP_FileTempFullPath;

		for(int iDup=0;iDup<1000;iDup++)
		{
			//<PID>-00�`9999.RDP�͈̔͂��m�F
			strRDP_FileTempFullPath.Format(_T("%s\\%s-%02d.RDP"),strRDP_FileTemp,strRDPFileNameTemp,iDup);
			//�t�@�C���̑��݃`�F�b�N
			if(!PathFileExists(strRDP_FileTempFullPath))
			{
				//���̃t�@�C����������Ȃ��ꍇ
				//������Ȃ��̂ŁA���̃t�@�C�����𗘗p��������B(�n�߂Ɍ������ԍ��𗘗p)
				m_strRDP_FileFullPath=strRDP_FileTempFullPath;
				break;
			}
		}
	}

	//ThinBridgePlus ini
	m_strRedirectFilePath = szDrive;
	m_strRedirectFilePath += szDir;
	m_strRedirectFilePath +=_T("ThinBridgeBHO.ini");


	return bRet;
}

void CRedirectApp::InitCommandParamOptionParam()
{
	CString Command1;
	CString Command2;
	CString Command3;

	//�R�}���h���C������B
	if(m_lpCmdLine[0] != '\0')
	{
		//�p�����[�^��1����
		if(__argc ==2)
		{
			Command1 = CString(__wargv[1]);
			Command1.Replace(_T("\""),_T(""));
			Command1.TrimLeft();
			Command1.TrimRight();

			if(!Command1.IsEmpty())
			{
				//URL��FilePath�̏ꍇ�́A�����I��m_CommandParam�Ƃ���B
				if(SBUtil::IsURL(Command1))
				{
					m_CommandParam=Command1;
				}
				//filepath
				else if(Command1.Find(_T(":"))==1)
				{
					m_CommandParam=Command1;
				}
				//-�́A�I�v�V����
				else if(Command1.Find(_T("-"))==0)
				{
					m_OptionParam=Command1;
				}
				// /�́A�I�v�V����
				else if(Command1.Find(_T("/"))==0)
				{
					m_OptionParam=Command1;
				}
			}
		}
		//�R�}���h���C����3�ȏ�A0�Ԃ́AEXE�p�X
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
				//URL��FilePath�̏ꍇ�́A�����I��m_CommandParam�Ƃ���B
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
				//URL��FilePath�̏ꍇ�́A�����I��m_CommandParam�Ƃ���B
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
		//�R�}���h���C����3�ȏ�A0�Ԃ́AEXE�p�X
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
				//URL��FilePath�̏ꍇ�́A�����I��m_CommandParam�Ƃ���B
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
				//URL��FilePath�̏ꍇ�́A�����I��m_CommandParam�Ƃ���B
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
				//URL��FilePath�̏ꍇ�́A�����I��m_CommandParam�Ƃ���B
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
}
void CRedirectApp::InitExecCommandParam()
{
	CString strMsg;
	if(this->SettingConf.m_iSolutionType==PROC_RDP)
	{
		m_RedirectList.SetGlobalData(m_strRedirectFilePath);
		SetLastError(NO_ERROR);
		this->ReadTemplate();
		this->ReadRDP_ReadOnceFile();	
		//RDP�t�@�C����TEMP�ɏo�͂�MSTSC.exe�����s����B		
		CreateRDPFile(m_CommandParam);
	}
	else if(this->SettingConf.m_iSolutionType==PROC_VMWARE)
	{
		m_RedirectList.SetGlobalData(m_strRedirectFilePath);
		//horizon client���C���X�g�[������Ă��Ȃ��B
		if(!IsHorizonInstalled())
		{
			strMsg=_T("VMware Horizon Client���C���X�g�[������Ă��Ȃ����A�ݒ肳��Ă��Ȃ����ߋN���ł��܂���B");
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
		m_RedirectList.SetGlobalData(m_strRedirectFilePath);
		//citrix client���C���X�g�[������Ă��Ȃ��B
		if(!IsCitrixInstalled())
		{
			strMsg=_T("Citrix���C���X�g�[������Ă��Ȃ����A�ݒ肳��Ă��Ȃ����ߋN���ł��܂���B");
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
		m_RedirectList.SetGlobalData(m_strRedirectFilePath);
		SetIE_FullPath();
		Exec_LocalBrowser(m_CommandParam);
	}
	else if(this->SettingConf.m_iSolutionType==PROC_LFirefox)
	{
		//firefox���C���X�g�[������Ă��Ȃ��B
		if(!IsFirefoxInstalled())
		{
			strMsg=_T("Mozilla Firefox���C���X�g�[������Ă��Ȃ����A�ݒ肳��Ă��Ȃ����ߋN���ł��܂���B");
			ShowTimeoutMessageBox(strMsg,m_strThisAppName,MB_OK|MB_ICONERROR,6000);
			return;
		}
		Exec_LocalBrowser(m_CommandParam);
	}
	else if(this->SettingConf.m_iSolutionType==PROC_LChrome)
	{
		//Chrome���C���X�g�[������Ă��Ȃ��B
		if(!IsChromeInstalled())
		{
			strMsg=_T("Google Chrome���C���X�g�[������Ă��Ȃ����A�ݒ肳��Ă��Ȃ����ߋN���ł��܂���B");
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
			strMsg.Format(_T("�w�肳�ꂽ�u���E�U�[��������Ȃ����ߋN���ł��܂���B\n%s"),SettingConf.m_strCustomBrowserPath);
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

	ShowPlusSettingDlgEx();
	return;
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

	//ESC��������Ă���ꍇ�́A���_�C���N�g�����Ȃ��B
	if(::GetKeyState(VK_ESCAPE)<0)
		return FALSE;

	//�e��t�@�C������Ǎ�
	if(!this->InitBaseFunction())
		return FALSE;

	//�R�}���h���C���̃Z�b�g
	this->InitCommandParamOptionParam();

	//�����Z�b�g����Ă���ꍇ ThinBridgeBHO�����Call
	//�����ŁAthis->SettingConf.m_iSolutionType�������I�ɏ㏑������B
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
			//���΃p�X��OK�ɂ���B
			if (!IsCustomInstalled(this->SettingConf.m_strCustomBrowserPath))
			{
				if (m_OptionParam.Find(_T("..\\")) >= 0
					|| m_OptionParam.Find(_T("../")) >= 0
					|| m_OptionParam.Find(_T(".\\")) >= 0
					|| m_OptionParam.Find(_T("./")) >= 0
					|| m_OptionParam.Find(_T("/")) >= 0
					|| m_OptionParam.Find(_T("\\.\\")) >= 0
					|| m_OptionParam.Find(_T("/./")) >= 0
					|| m_OptionParam.Find(_T("\\..\\")) >= 0
					|| m_OptionParam.Find(_T("/../")) >= 0
					|| m_OptionParam.Find(_T("\\\\")) >= 0
					)
				{
					//�댯�ȃp�X�ݒ��NG �󔒂ɂ��Ă��܂��B
					this->SettingConf.m_strCustomBrowserPath.Empty();
				}
				else
				{
					//���ϐ��̉���
					if (m_OptionParam.Find(_T("%")) >= 0)
					{
						TCHAR szExpPath[MAX_PATH * 2] = { 0 };
						ExpandEnvironmentStrings(m_OptionParam, szExpPath, MAX_PATH * 2);
						m_OptionParam = szExpPath;
					}
					//���΃p�X�̏ꍇ
					if (PathIsRelative(m_OptionParam))
					{
						this->SettingConf.m_strCustomBrowserPath = m_strExeFolderPath;
						this->SettingConf.m_strCustomBrowserPath += m_OptionParam;
					}
				}
			}
		}
	}

	//CommandLine������ꍇ�B	
	if(!m_CommandParam.IsEmpty())
	{
		CString strCmd;
		CString strMutexStringTemp;
		CString strMutexString;
		CString strCommandParam;
		strCommandParam= m_CommandParam;
		strCommandParam.Replace(_T("\\"),_T("/"));
		strMutexStringTemp.Format(_T("Local\\TB-%s-%s"), m_OptionParam, strCommandParam);
		SBUtil::GetDivChar(strMutexStringTemp,MAX_PATH, strMutexString,FALSE);
		//Mutex�m�F
		HANDLE hMutex = NULL;
		hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, strMutexString);
		if(hMutex)
		{
			//��ɋN�����Ă�����̂�����B����̓��_�C���N�g���Ȃ��B�A���Ăяo���̉\���������B
			strCmd.Format(_T("[Duplicate Call]%s %s "), m_CommandParam, m_OptionParam);
			WriteDebugTraceDateTime(strCmd);
			return FALSE;
		}
		else
		{
			hMutex = ::CreateMutex(NULL, FALSE, strMutexString);
		}

		if (SBUtil::InThinApp())
			strCmd.Format(_T("[VOS] %s %s"), m_CommandParam, m_OptionParam);
		else
			strCmd.Format(_T("%s %s"), m_CommandParam, m_OptionParam);
		WriteDebugTraceDateTime(strCmd);

		UINT uRet=0;
		uRet = m_LoopBlock.CheckLoop(strCmd);
		if(uRet == LB_OK)
		{
			this->InitExecCommandParam();
		}
		else if(uRet == LB_NG)
		{
			if (hMutex)
			{
				ReleaseMutex(hMutex);
				CloseHandle(hMutex);
				hMutex = NULL;
			}
			CString strSafeGuardMsg;
			strSafeGuardMsg.Format(_T("%s\n%s"), _T("�Z���ԂɘA�������u���E�U�[ ���_�C���N�g�����o���܂����B\n\n�u���E�U�[ ���_�C���N�g�����𒆒f���܂��B\n�������Ԃ������Ă���Ď��s���Ă��������B"), m_CommandParam);
			ShowTimeoutMessageBox(strSafeGuardMsg, m_strThisAppName, MB_OK | MB_ICONWARNING| MB_SYSTEMMODAL,10000);
		}
		if (hMutex)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			hMutex = NULL;
		}
		return FALSE;
	}
	//Command Line�Ȃ�
	else
	{
		//�R�}���h���C���������ꍇ�ŁAEXE�̖��̂�ThinBridgeSetting.exe�̏ꍇ��
		//�ݒ��ʂ�\������B
		if(m_strExeFileName.CompareNoCase(_T("ThinBridgeSetting.exe"))==0
		|| m_OptionParam.CompareNoCase(_T("/Config")) == 0)
		{
			this->InitShowSettingDlg();
		}
		//������ThinBridge.exe�����s�����ꍇ�́AIE�̃X�^�[�g�y�[�W���J���B
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

	//�����[�g�n���ݒ肳��Ă���ꍇ�́AIE������
	//�����l��RDP�Ȃ̂ŁAsetting.conf���Ȃ��ꍇ�ɁARDP�Őڑ����悤�Ƃ��Ă��܂����߁B
	if(this->SettingConf.m_iSolutionType==PROC_RDP
	||this->SettingConf.m_iSolutionType==PROC_VMWARE
	||this->SettingConf.m_iSolutionType==PROC_CITRIX
	)
	{
		//2021-06-18 IE��EOS�ɑΉ����Ă���
		//this->SettingConf.m_iSolutionType=PROC_LIE;
		this->SettingConf.m_iSolutionType = PROC_LDefaultBrowser;
	}

	//Local IE
	if(this->SettingConf.m_iSolutionType==PROC_LIE)
	{
		SetIE_FullPath();
		//IE��ݒ肵���ꍇ�́A�K���V����IE Window��\������B
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
		//firefox���C���X�g�[������Ă��Ȃ��B
		if(!IsFirefoxInstalled())
		{
			strMsg=_T("Mozilla Firefox���C���X�g�[������Ă��Ȃ����A�ݒ肳��Ă��Ȃ����ߋN���ł��܂���B");
			ShowTimeoutMessageBox(strMsg,m_strThisAppName,MB_OK|MB_ICONERROR,6000);
			return;
		}
	}
	else if(this->SettingConf.m_iSolutionType==PROC_LChrome)
	{
		//Chrome���C���X�g�[������Ă��Ȃ��B
		if(!IsChromeInstalled())
		{
			strMsg=_T("Google Chrome���C���X�g�[������Ă��Ȃ����A�ݒ肳��Ă��Ȃ����ߋN���ł��܂���B");
			ShowTimeoutMessageBox(strMsg,m_strThisAppName,MB_OK|MB_ICONERROR,6000);
			return;
		}
	}
	else if(this->SettingConf.m_iSolutionType==PROC_LCustom)
	{
		if(!IsCustomInstalled(this->SettingConf.m_strCustomBrowserPath))
		{
			strMsg.Format(_T("�w�肳�ꂽ�u���E�U�[��������Ȃ����ߋN���ł��܂���B\n%s"),SettingConf.m_strCustomBrowserPath);
			ShowTimeoutMessageBox(strMsg,m_strThisAppName,MB_OK|MB_ICONERROR,6000);
			return;
		}
	}
//	else if(this->SettingConf.m_iSolutionType==PROC_LDefaultBrowser)
//	else if(this->SettingConf.m_iSolutionType==PROC_LEdge)
	Exec_LocalBrowser(m_CommandParam);
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
	m_pDlgMsg->SetMsg(_T("�ݒ��� �N����...."));
	//////////////////////////////////////////////////////

	m_RedirectList.SetArrayData(m_strRedirectFilePath, theApp.m_bCitrixCustomEnv);
	m_RedirectListSaveData.InitSaveDataAll(theApp.m_bCitrixCustomEnv);
	CSettingsDialog SettingDlg;
	CString strTitle;
	SettingDlg.SetLogoText(theApp.m_strThisAppName);
	strTitle.Format(_T("%s Settings [%s]"),theApp.m_strThisAppName,m_strRedirectFilePath);
	strTitle.Replace(_T("\\"),_T("/"));
	SettingDlg.SetTitle(strTitle);

	//Citrix������t���O
	if(theApp.m_bCitrixCustomEnv)
	{
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgETC), _T("URL���_�C���N�g�S�ʐݒ�"), IDD_DLG_ETC, _T(""));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCHR), _T("Google Chrome (���[�J����p)"), IDD_DLG_RD_RDP, _T(""));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgChromeSwitcher), _T("Google Chrome(�����؂芷��)"), IDD_DLG_RD_CH_SWITCH, _T(""));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgO365), _T("Office365 WebApps"), IDD_DLG_RD_O365, _T("Google Chrome(�����؂芷��)"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgDMZ), _T("���pURL(SSO/SAML�Ή��T�C�g)"), IDD_DLG_RD_DMZ, _T(""));
	}
	else
	{
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgETC), _T("URL���_�C���N�g�S�ʐݒ�"), IDD_DLG_ETC, _T(""));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgRemoteSetting), _T("�����[�g�u���E�U�[�ݒ�"), IDD_DLG_REMOTE, _T("�����[�g�u���E�U�[�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgSetRDP), _T("Microsoft RDP �ݒ�"), IDD_DLG_RDP, _T("�����[�g�u���E�U�[�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgSetVMW), _T("VMware Horizon �ݒ�"), IDD_DLG_VMW, _T("�����[�g�u���E�U�[�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgSetCX), _T("Citrix Virtual Apps/XenApp �ݒ�"), IDD_DLG_CX, _T("�����[�g�u���E�U�[�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgRemoteSettingRD), _T("URL���_�C���N�g�ݒ�"), IDD_DLG_REMOTE, _T("URL���_�C���N�g�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgRD), _T("Microsoft RDP (�����[�g)"), IDD_DLG_RD_RDP, _T("URL���_�C���N�g�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgVM), _T("VMware Horizon (�����[�g)"), IDD_DLG_RD_RDP, _T("URL���_�C���N�g�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCTX), _T("Citrix Virtual Apps (�����[�g)"), IDD_DLG_RD_RDP, _T("URL���_�C���N�g�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgFF), _T("Mozilla Firefox (���[�J��)"), IDD_DLG_RD_RDP, _T("URL���_�C���N�g�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCHR), _T("Google Chrome (���[�J��)"), IDD_DLG_RD_RDP, _T("URL���_�C���N�g�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgEDG), _T("Microsoft Edge (���[�J��)"), IDD_DLG_RD_RDP, _T("URL���_�C���N�g�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgIE), _T("Internet Explorer (���[�J��)"), IDD_DLG_RD_RDP, _T("URL���_�C���N�g�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgO365), _T("Office365 WebApps"), IDD_DLG_RD_O365, _T("URL���_�C���N�g�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU01), _T("�w��u���E�U�[01 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU02), _T("�w��u���E�U�[02 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU03), _T("�w��u���E�U�[03 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU04), _T("�w��u���E�U�[04 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU05), _T("�w��u���E�U�[05 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgDMZ), _T("���pURL(SSO/SAML�Ή��T�C�g)"), IDD_DLG_RD_DMZ, _T("URL���_�C���N�g�ݒ�"));
		SettingDlg.AddPage(RUNTIME_CLASS(CDlgDefault), _T("���̑�(����`URL)"), IDD_DLG_RD_Default, _T("URL���_�C���N�g�ݒ�"));

//		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU06), _T("�w��u���E�U�[06 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
//		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU07), _T("�w��u���E�U�[07 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
//		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU08), _T("�w��u���E�U�[08 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
//		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU09), _T("�w��u���E�U�[09 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
//		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU10), _T("�w��u���E�U�[10 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
//		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU11), _T("�w��u���E�U�[11 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
//		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU12), _T("�w��u���E�U�[12 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
//		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU13), _T("�w��u���E�U�[13 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
//		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU14), _T("�w��u���E�U�[14 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
//		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU15), _T("�w��u���E�U�[15 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
//		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU16), _T("�w��u���E�U�[16 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
//		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU17), _T("�w��u���E�U�[17 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));

//		�ȉ��͗��p�s�� ���Ŋ��Ɏg���Ă��邽��
//		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU18), _T("�w��u���E�U�[18 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
//		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU19), _T("�w��u���E�U�[19 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
//		SettingDlg.AddPage(RUNTIME_CLASS(CDlgCU20), _T("�w��u���E�U�[20 (���[�J��)"), IDD_DLG_RD_CUSTOM, _T("URL���_�C���N�g�ݒ�"));
	}

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
}

//void CRedirectApp::ReadSetting()
//{
//	CStdioFile in;
//	CString strTemp;
//	CString strLogMsgLine;
//	if(in.Open(m_strSetting_FileFullPath,CFile::modeRead|CFile::shareDenyWrite))
//	{
//		CString strTemp2;
//		CString strTemp3;
//		CStringArray strArray;
//		while(in.ReadString(strTemp))
//		{
//			strTemp.TrimLeft();
//			strTemp.TrimRight();
//			strArray.RemoveAll();
//			SBUtil::Split(strArray,strTemp,_T("="));
//			strLogMsgLine += strTemp;
//			strLogMsgLine += "\n";
//
//			if(strArray.GetSize() >= 2)
//			{
//				strTemp2=strArray.GetAt(0);
//				strTemp2.TrimLeft();
//				strTemp2.TrimRight();
//
//				strTemp3=strArray.GetAt(1);
//				strTemp3.TrimLeft();
//				strTemp3.TrimRight();
//
//				if(strTemp2.Find(_T(";"))==0)
//				{
//					;
//				}
//				else if(strTemp2.Find(_T("#"))==0)
//				{
//					;
//				}
//				else
//				{
//					if(strTemp2.CompareNoCase(_T("ServerName"))==0)
//					{
//						SettingConf.m_strServerName=strTemp3;
//					}
//					else if(strTemp2.CompareNoCase(_T("RemoteAppName"))==0)
//					{
//						SettingConf.m_strRemoteAppName=strTemp3;
//					}
//					else if(strTemp2.CompareNoCase(_T("RemoteAppPath"))==0)
//					{
//						SettingConf.m_strRemoteAppPath=strTemp3;
//					}
//					else if(strTemp2.CompareNoCase(_T("RemoteAppCommandLine"))==0)
//					{
//						SettingConf.m_strRemoteAppCommandLine=strTemp3;
//					}
//					else if(strTemp2.CompareNoCase(_T("RemoteAppMode"))==0)
//					{
//						if(strTemp3==_T("1"))
//							SettingConf.m_bRemoteAppMode=TRUE;
//						else
//							SettingConf.m_bRemoteAppMode=FALSE;
//					}
//					else if(strTemp2.CompareNoCase(_T("Redirect_Clipboard"))==0)
//					{
//						if(strTemp3==_T("1"))
//							SettingConf.m_bRedirect_Clipboard=TRUE;
//						else
//							SettingConf.m_bRedirect_Clipboard=FALSE;
//					}
//					else if(strTemp2.CompareNoCase(_T("Redirect_Printer"))==0)
//					{
//						if(strTemp3==_T("1"))
//							SettingConf.m_bRedirect_Printer=TRUE;
//						else
//							SettingConf.m_bRedirect_Printer=FALSE;
//					}
//					else if(strTemp2.CompareNoCase(_T("Redirect_Drive"))==0)
//					{
//						if(strTemp3==_T("1"))
//							SettingConf.m_bRedirect_Drive=TRUE;
//						else
//							SettingConf.m_bRedirect_Drive=FALSE;
//					}
//					else if(strTemp2.CompareNoCase(_T("Solution_Type"))==0)
//					{
//						if(strTemp3.CompareNoCase(_T("RDP"))==0)
//							SettingConf.m_iSolutionType=PROC_RDP;
//						else if(strTemp3.CompareNoCase(_T("VMWARE"))==0)
//							SettingConf.m_iSolutionType=PROC_VMWARE;
//						else if(strTemp3.CompareNoCase(_T("CITRIX"))==0)
//							SettingConf.m_iSolutionType=PROC_CITRIX;
//						else if(strTemp3.CompareNoCase(_T("DefaultBrowser"))==0)
//							SettingConf.m_iSolutionType=PROC_LDefaultBrowser;
//						else if(strTemp3.CompareNoCase(_T("IE"))==0)
//							SettingConf.m_iSolutionType=PROC_LIE;
//						else if(strTemp3.CompareNoCase(_T("Firefox"))==0)
//							SettingConf.m_iSolutionType=PROC_LFirefox;
//						else if(strTemp3.CompareNoCase(_T("Chrome"))==0)
//							SettingConf.m_iSolutionType=PROC_LChrome;
//						else if(strTemp3.CompareNoCase(_T("Edge"))==0)
//							SettingConf.m_iSolutionType=PROC_LEdge;
//						else if(strTemp3.CompareNoCase(_T("Custom"))==0)
//							SettingConf.m_iSolutionType=PROC_LCustom;
//
//					}
//					else if(strTemp2.CompareNoCase(_T("v_ConnectionServerName"))==0)
//					{
//						SettingConf.m_strHorizon_ConnectionServerName=strTemp3;
//					}
//					else if(strTemp2.CompareNoCase(_T("v_AppName"))==0)
//					{
//						SettingConf.m_strHorizon_AppName=strTemp3;
//					}
//					else if(strTemp2.CompareNoCase(_T("CustomBrowserPath"))==0)
//					{
//						SettingConf.m_strCustomBrowserPath=strTemp3;
//					}
//					else if(strTemp2.CompareNoCase(_T("CX_AppName"))==0)
//					{
//						SettingConf.m_strCitrix_AppName=strTemp3;
//					}
//
//				}
//			}
//		}
//		in.Close();
//	}
//}


void CRedirectApp::CreateRDPFile(CString& strCommand)
{
	CStdioFile out;
	if(out.Open(m_strRDP_FileFullPath,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
	{
		CString strWriteData;
		//�ڑ���̃R���s���[�^�[���ior IP�A�h���X�j
		if(!m_RedirectList.m_strRDP_ServerName.IsEmpty())
		{
			strWriteData.Format(_T("full address:s:%s\n"), m_RedirectList.m_strRDP_ServerName);
			out.WriteString(strWriteData);
		}

		//RemoteApp���[�h��L��
		strWriteData.Format(_T("remoteapplicationmode:i:%d\n"), m_RedirectList.m_bRemoteAppMode?1:0);
		out.WriteString(strWriteData);

		//RemoteApp�̏ꍇ�����B
		if(m_RedirectList.m_bRemoteAppMode)
		{
			//RemoteApp�̃A�v����
			if(!m_RedirectList.m_strRemoteAppName.IsEmpty())
			{
				strWriteData.Format(_T("remoteapplicationname:s:%s\n"), m_RedirectList.m_strRemoteAppName);
				out.WriteString(strWriteData);
			}

			//RemoteApp�Ŏ��s����EXE�̃t���p�X
			if(!m_RedirectList.m_strRemoteAppPath.IsEmpty())
			{
				strWriteData.Format(_T("remoteapplicationprogram:s:%s\n"), m_RedirectList.m_strRemoteAppPath);
				out.WriteString(strWriteData);
				strWriteData.Format(_T("alternate shell:s:%s\n"), m_RedirectList.m_strRemoteAppPath);
				out.WriteString(strWriteData);
			}

			//��������B
			if(strCommand.IsEmpty())
			{
				//GUI�Őݒ肵���R�}���h���C������B
				if(!m_RedirectList.m_strRemoteAppCommandLine.IsEmpty())
				{
					strWriteData.Format(_T("remoteapplicationcmdline:s:%s\n"), m_RedirectList.m_strRemoteAppCommandLine);
					out.WriteString(strWriteData);
				}
			}
			else
			{
				//GUI�Őݒ肵���R�}���h���C������B
				if(!m_RedirectList.m_strRemoteAppCommandLine.IsEmpty())
				{
					strWriteData.Format(_T("remoteapplicationcmdline:s:%s %s\n"), m_RedirectList.m_strRemoteAppCommandLine,strCommand);
					out.WriteString(strWriteData);
				}
				else
				{
					strWriteData.Format(_T("remoteapplicationcmdline:s:%s\n"),strCommand);
					out.WriteString(strWriteData);
				}
			}
		}

		//�N���b�v�{�[�h�̃��_�C���N�g
		strWriteData.Format(_T("redirectclipboard:i:%d\n"), m_RedirectList.m_bRedirect_Clipboard?1:0);
		out.WriteString(strWriteData);

		//Printer�̃��_�C���N�g
		strWriteData.Format(_T("redirectprinters:i:%d\n"), m_RedirectList.m_bRedirect_Printer?1:0);
		out.WriteString(strWriteData);

		//���[�J���̃f�B�X�N�h���C�u�������[�g�ɐڑ�����
		strWriteData.Format(_T("RedirectDrives:i:%d\n"), m_RedirectList.m_bRedirect_Drive?1:0);
		out.WriteString(strWriteData);

		//template�t�@�C���̓��e��ǉ�
		out.WriteString(m_strRdp_Template_File_Data);

		//ReadOnce�t�@�C���̓��e��ǉ�
		out.WriteString(m_strRdp_ReadOnce_File_Data);
		out.Close();
	}
	theApp.Exec_MSTSC(m_strRDP_FileFullPath);

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
}
void CRedirectApp::Exec_MSTSC(CString& strRDPFile)
{
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
	m_pDlgMsg->SetMsg(_T("�N��������...."));
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
	m_pDlgMsg->SetMsg(_T("MS-RDP �N����...."));
	::Sleep(100);
	m_pDlgMsg->m_Prg.StepIt();
	::Sleep(100);
	m_pDlgMsg->m_Prg.StepIt();
	m_pDlgMsg->m_Prg.StepIt();
	m_pReExecThread->Exec();
	::Sleep(100);

	//2�b���RDP�t�@�C�����폜����B
	::Sleep(1000*3);
	::DeleteFile(strRDPFile);
	SetLastError(NO_ERROR);
	//�폜�����t�@�C�������݂��邩�O�̂��ߊm�F
	if(PathFileExists(strRDPFile))
	{
		//�ő�60�� �폜���������g���C����B
		for(int iDup=0;iDup<60;iDup++)
		{
			::DeleteFile(strRDPFile);
			SetLastError(NO_ERROR);
			//�t�@�C�����݃`�F�b�N
			if(!PathFileExists(strRDPFile))
			{
				//���݂��Ȃ��B�폜���ꂽ�甲����
				break;
			}
			//10���1��́A30�b�҂�
			if(iDup%10==0)//���񃋁[�v��0���Z�ɂȂ邪�A��]�̏ꍇ��0���Z�G���[�͔������Ȃ��BMS VS2010
			{
				::Sleep(30*1000);
			}
			else
			{
				//1�b�ҋ@���Ă��烊�g���C
				::Sleep(1000);
			}
		}
	}

}

void CRedirectApp::Exec_VMwareHorizon_Start(CString& CommandParam)
{
	Exec_VMware_Horizon(m_RedirectList.m_strHorizon_ConnectionServerName, m_RedirectList.m_strHorizon_AppName,CommandParam);
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
}
void CRedirectApp::Exec_VMware_Horizon(CString& strServerHostName,CString& strAppName,CString& strURL)
{
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
	m_pDlgMsg->SetMsg(_T("�N��������...."));
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
	m_pDlgMsg->SetMsg(_T("VMware Horizon Client �N����...."));
	m_pDlgMsg->m_Prg.StepIt();
	m_pReExecThread->Exec();
}


void CRedirectApp::Exec_CitixXenApp_Start(CString& CommandParam)
{
	Exec_Citrix_XenApp(m_RedirectList.m_strCitrix_AppName,CommandParam);

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
}

void CRedirectApp::Exec_Citrix_XenApp(CString& strAppName,CString& strURL)
{
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
	m_pDlgMsg->SetMsg(_T("�N��������...."));
	m_pDlgMsg->m_Prg.StepIt();
	m_pDlgMsg->SetMsg(_T("Citrix"));
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
	m_pDlgMsg->SetMsg(_T("Citrix �ڑ���...."));
	m_pDlgMsg->m_Prg.StepIt();
	m_pReExecThread->Exec();
}



void CRedirectApp::Exec_LocalBrowser(CString& strURL)
{
	Exec_Local_Browser(SettingConf.m_iSolutionType,SettingConf.m_strCustomBrowserPath,strURL);

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
}

void CRedirectApp::Exec_Local_Browser(UINT iSolutionType,CString& strCustomBrowserPath,CString& strURL)
{
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
	m_pDlgMsg->SetMsg(_T("�N��������...."));
	m_pDlgMsg->m_Prg.StepIt();
	m_pDlgMsg->SetMsg(_T("���[�J���u���E�U�[�N����"));
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
		m_pDlgMsg->SetMsg(_T("DefaultBrowser �N����...."));
		strLogMsg.Format(_T("DefaultBrowser\t%s"),strURL);
	}
	else if(iSolutionType==PROC_LIE)
	{
		m_pDlgMsg->SetMsg(_T("Internet Explorer �N����...."));
		strLogMsg.Format(_T("IE\t%s"),strURL);
	}
	else if(iSolutionType==PROC_LFirefox)
	{
		m_pDlgMsg->SetMsg(_T("Mozilla Firefox �N����...."));
		strLogMsg.Format(_T("Firefox\t%s"),strURL);
	}
	else if(iSolutionType==PROC_LChrome)
	{
		m_pDlgMsg->SetMsg(_T("Google Chrome �N����...."));
		strLogMsg.Format(_T("Chrome\t%s"),strURL);
	}
	else if(iSolutionType==PROC_LEdge)
	{
		m_pDlgMsg->SetMsg(_T("Microsoft Edge �N����...."));
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
	m_pReExecThread->Exec();
	::Sleep(100);
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
	//HK_CU���`�F�b�N
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
				//�R�[�h�y�[�W���w��
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
	TCHAR buf[1024];
	_tcscpy_s(buf, 1024, (LPTSTR)strURL);

	HCONV hConv={0};
	HDDEDATA hDDEData={0};
	DWORD m_dwDDEID = 0;

	//DDE�Ăяo�����L���ȏꍇ Default�L��
	if(!theApp.m_RedirectList.m_bDisableIE_DDE)
	{
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
					hDDEData = DdeClientTransaction((LPBYTE)buf,1024*sizeof(TCHAR),hConv,0,0,XTYP_EXECUTE,30000,&result);
					if (hDDEData)
						DdeFreeDataHandle(hDDEData);
					DdeDisconnect(hConv);
					DdeUninitialize(m_dwDDEID);
					if (SBUtil::InThinApp())
						theApp.WriteDebugTraceDateTime(_T("[VOS] IE DDE Start"));
					else
						theApp.WriteDebugTraceDateTime(_T("IE DDE Start"));
					return;
				}
			}
		}
	}
	if(::ShellExecute(NULL,_T("open"),_T("iexplore.exe"),strURL,NULL, SW_SHOW) <= HINSTANCE(32))
	{
		CString IEcmd;
		IEcmd.Format(_T("iexplorer.exe %s"),strURL);
		if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
		{
			IEcmd=_T("");
			IEcmd.Format(_T("\"%s\" %s"),theApp.m_strIE_FullPath,strURL);
			if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
			{
				IEcmd=_T("");
				IEcmd.Format(_T("\"C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe\" %s"),strURL);
				if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
				{
					IEcmd=_T("");
					IEcmd.Format(_T("\"C:\\Program Files\\Internet Explorer\\iexplore.exe\" %s"),strURL);
					::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW);
				}
			}
			else
			{
				if (SBUtil::InThinApp())
					theApp.WriteDebugTraceDateTime(_T("[VOS] IE ShellExecute2 Start"));
				else
					theApp.WriteDebugTraceDateTime(_T("IE ShellExecute2 Start"));
			}
		}
	}
	else
	{
		if (SBUtil::InThinApp())
			theApp.WriteDebugTraceDateTime(_T("[VOS] IE ShellExecute Start"));
		else
			theApp.WriteDebugTraceDateTime(_T("IE ShellExecute Start"));
	}
}

void CCRre::FirefoxStart(CString& strURL)
{
	if(::ShellExecute(NULL,_T("open"),_T("firefox.exe"),strURL,NULL, SW_SHOW) <= HINSTANCE(32))
	{
		CString IEcmd;
		IEcmd.Format(_T("firefox.exe %s"),m_strURL);
		if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
		{
			IEcmd=_T("");
			IEcmd.Format(_T("\"%s\" %s"),theApp.m_strFirefox_FullPath,strURL);
			if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
			{
				IEcmd=_T("");
				IEcmd.Format(_T("\"C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe\" %s"),strURL);
				if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
				{
					IEcmd=_T("");
					IEcmd.Format(_T("\"C:\\Program Files\\Mozilla Firefox\\firefox.exe\" %s"),strURL);
					::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW);
				}
			}
		}
	}
}

void CCRre::ChromeStart(CString& strURL)
{
	if(::ShellExecute(NULL,_T("open"),_T("chrome.exe"),strURL,NULL, SW_SHOW) <= HINSTANCE(32))
	{
		CString IEcmd;
		IEcmd.Format(_T("chrome.exe %s"),strURL);
		if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
		{
			IEcmd=_T("");
			IEcmd.Format(_T("\"%s\" %s"),theApp.m_strChrome_FullPath,strURL);
			if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
			{
				IEcmd=_T("");
				IEcmd.Format(_T("\"C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe\" %s"),strURL);
				if(::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
				{
					IEcmd=_T("");
					IEcmd.Format(_T("\"C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe\" %s"),strURL);
					::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW);
				}
			}
		}
	}
}

void CCRre::EdgeStart(CString& strURL)
{
	CString IEcmd;
	IEcmd.Format(_T("microsoft-edge:%s"),strURL);
	::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW);
}

void CCRre::DefaultBrowserStart(CString& strURL)
{
	if(::ShellExecute(NULL,NULL,strURL,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
	{
		if(::ShellExecute(NULL,_T("open"),strURL,NULL,NULL, SW_SHOW) <= HINSTANCE(32))
		{
			if(::ShellExecute(NULL,NULL,_T("explorer.exe"),strURL,NULL, SW_SHOW) <= HINSTANCE(32))
			{
				::ShellExecute(NULL,_T("open"),_T("iexplore.exe"),strURL,NULL, SW_SHOW);
			}
		}
	}
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
		SetLastError(NO_ERROR);
		//Retry
		STARTUPINFO si2={0};
		PROCESS_INFORMATION pi2={0};
		si2.cb = sizeof( si2 );
		if(!CreateProcess( strPathQ,(LPTSTR)(LPCTSTR)strURL,NULL, NULL, FALSE, 0, NULL,NULL, &si2, &pi2 ))
		{
			SetLastError(NO_ERROR);
			if(::ShellExecute(NULL,_T("open"),strPathQ,strURL,NULL, SW_SHOW) <= HINSTANCE(32))
			{
				SetLastError(NO_ERROR);
				::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW);
			}
		}
		else
		{
			if(pi2.hThread)
			{
				CloseHandle( pi2.hThread );  // �X���b�h�̃n���h���͎g��Ȃ��̂ł����j��
				pi2.hThread=0;
			}
			if(pi2.hProcess)
			{
				CloseHandle( pi2.hProcess ); // �����v���Z�X�̃n���h���͎g��Ȃ��̂Ŕj��
				pi2.hProcess=0;
			}
		}
	}
	else
	{
		if(pi.hThread)
		{
			CloseHandle( pi.hThread );  // �X���b�h�̃n���h���͎g��Ȃ��̂ł����j��
			pi.hThread=0;
		}
		if(pi.hProcess)
		{
			CloseHandle( pi.hProcess ); // �����v���Z�X�̃n���h���͎g��Ȃ��̂Ŕj��
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
				bExecFlg=FALSE;
			}
			else
			{
				bExecFlg=TRUE;
			}
		}
	}

	if(!bExecFlg)
	{
		::ShellExecute(NULL,NULL,strCommand,NULL,NULL, SW_SHOW);
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
		SetLastError(NO_ERROR);
		//Retry
		STARTUPINFO si2={0};
		PROCESS_INFORMATION pi2={0};
		si2.cb = sizeof( si2 );
		if(!CreateProcess( strPathQ,(LPTSTR)(LPCTSTR)strCommand,NULL, NULL, FALSE, 0, NULL,NULL, &si2, &pi2 ))
		{
			SetLastError(NO_ERROR);
			if(::ShellExecute(NULL,_T("open"),strPathQ,strCommand,NULL, SW_SHOW) <= HINSTANCE(32))
			{
				SetLastError(NO_ERROR);
				::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW);
			}
		}
		else
		{
			if(pi2.hThread)
			{
				CloseHandle( pi2.hThread );  // �X���b�h�̃n���h���͎g��Ȃ��̂ł����j��
				pi2.hThread=0;
			}
			if(pi2.hProcess)
			{
				CloseHandle( pi2.hProcess ); // �����v���Z�X�̃n���h���͎g��Ȃ��̂Ŕj��
				pi2.hProcess=0;
			}
		}
	}
	else
	{
		if(pi.hThread)
		{
			CloseHandle( pi.hThread );  // �X���b�h�̃n���h���͎g��Ȃ��̂ł����j��
			pi.hThread=0;
		}
		if(pi.hProcess)
		{
			CloseHandle( pi.hProcess ); // �����v���Z�X�̃n���h���͎g��Ȃ��̂Ŕj��
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
	// �X���b�h�̋N���҂�
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
	// �X���b�h�̋N���҂�
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
	// �X���b�h�̏��ő҂�
	if(!m_bAutoDelete )
	{
		PostThreadMessage( WM_QUIT, 0, 0 );
		DWORD waitResult = ::WaitForSingleObject(m_hThread, 15000 );
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
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ����܂��B
	//}}AFX_MSG_MAP
	ON_THREAD_MESSAGE(MSG_THREAD_START,OnExec)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCRre ���b�Z�[�W �n���h��
void CCRre::Exec()
{
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
				SetLastError(NO_ERROR);
				//Retry
				if(!CreateProcess( _T("mstsc.exe"),(LPTSTR)(LPCTSTR)m_strFilePath,NULL, NULL, FALSE, 0, NULL,NULL, &si, &pi ))
				{
					SetLastError(NO_ERROR);
					if(::ShellExecute(NULL,_T("open"),_T("mstsc.exe"),m_strFilePath,NULL, SW_SHOW) <= HINSTANCE(32))
					{
						SetLastError(NO_ERROR);
						::ShellExecute(NULL,NULL,m_strFilePath,NULL,NULL, SW_SHOW);
					}
				}
			}
			if(pi.hThread)
			{
				CloseHandle( pi.hThread );  // �X���b�h�̃n���h���͎g��Ȃ��̂ł����j��
				pi.hThread=0;
			}
			if(pi.hProcess)
			{
				CloseHandle( pi.hProcess ); // �����v���Z�X�̃n���h���͎g��Ȃ��̂Ŕj��
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
			this->VMwareViewStart(strCommand);
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
			this->CitrixXenAppStart(strCommand);
			break;
		}

		else if(PROC_LDefaultBrowser==m_iSolutionType)
		{
			this->DefaultBrowserStart(m_strURL);
			break;
		}
		else if(PROC_LIE==m_iSolutionType)
		{
			this->IEStart(m_strURL);
			break;
		}
		else if(PROC_LFirefox==m_iSolutionType)
		{
			this->FirefoxStart(m_strURL);
			break;
		}
		else if(PROC_LChrome==m_iSolutionType)
		{
			this->ChromeStart(m_strURL);
			break;
		}
		else if(PROC_LEdge==m_iSolutionType)
		{
			this->EdgeStart(m_strURL);
			break;
		}
		else if(PROC_LCustom==m_iSolutionType)
		{
			this->CustomBrowserStart(m_strCustomBrowserPath,m_strURL);
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
	return;
}

void CCRre::OnExec(WPARAM wParam, LPARAM lParam)
{
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
	return;
}

//////////////////////////////////////////////////////

/*
#�ڑ���̃R���s���[�^�[���ior IP�A�h���X�j���w�肷��B
full address:s:@@REMOTE_SERVER@@

##RemoteApp�֘A
#RemoteApp���[�h��L��
remoteapplicationmode:i:1

#RemoteApp�̃A�v����
remoteapplicationname:s:@@Remote_AppName@@

#RemoteApp�Ŏ��s����EXE�̃t���p�X
remoteapplicationprogram:s:@@Remote_AppPath@@
alternate shell:s:@@Remote_AppPath@@

#CommandLine
remoteapplicationcmdline:s:@@Remote_App_CmdLine@@

#��ƃt�H���_
shell working directory:s:

##Redirect�֘A
#Printer�̃��_�C���N�g 0���� 1�L��
redirectprinters:i:@@Redirect_Printer@@

#�N���b�v�{�[�h�̃��_�C���N�g 0���� 1�L��
redirectclipboard:i:@@Redirect_Clipboard@@

#���[�J���̃f�B�X�N�h���C�u�������[�g�ɐڑ�����
RedirectDrives:i@@Redirect_Drive@@

#�V���A���|�[�g�̃��_�C���N�g 0���� 1�L��
redirectcomports:i:0

#�X�}�[�g�J�[�h�̃��_�C���N�g 0���� 1�L��
redirectsmartcards:i:0


#�f�[�^��]������Ƃ��Ɉ��k����B
compression:i:1

#�ڑ��i�� 1�F���f���A2�F�ᑬ�A3�F�q���A4�F�����A5�FWAN�A6�FLAN 7:�������o
connection type:i:7

#�����[�g�f�X�N�g�b�v��DirectX���_�C���N�g�@�\
RedirectDirectX:i:1

#�l�b�g���[�N��������
networkautodetect:i:1
bandwidthautodetect:i:1

#�t���X�N���[�����[�h�Őڑ������ۂɁA�ڑ��o�[��\�����邩���䂷��p�����[�^
displayconnectionbar:i:1

enableworkspacereconnect:i:0

#�w�i��\�����邩�ǂ������w�肷��
disable wallpaper:i:0

#ClearType�@�\��L���ɂ��邩���w�肷��
allow font smoothing:i:1

#�f�X�N�g�b�v�R���|�W�V�����@�\��L���ɂ��邩���w�肷��
allow desktop composition:i:0

#�h���b�O���ɃE�C���h�E�̓��e��\��
disable full window drag:i:1

#���j���[�ƃE�C���h�E�A�j���[�V����
disable menu anims:i:1

#�e�[�}�@�\��}�����邩���w�肷��
disable themes:i:0

#�J�[�\���̃A�j���[�V�������ʂ�}�����邩
disable cursor setting:i:0

bitmapcachepersistenable:i:1

#�����[�g�I�[�f�B�I�Đ�
audiomode:i:0

#�ؒf���ꂽ�ꍇ�Ɏ����I�ɍĐڑ�����
autoreconnection enabled:i:1

#�T�[�o�[�F�؂Ń|���V�[���Œ�v���𖞂����Ă��Ȃ��ꍇ�̓�����w�肷
authentication level:i:2

#�����[�g�R���s���[�^�ɃQ�[�g�E�F�C�̎��i�����g�p����
prompt for credentials:i:0

#�Z�L�����e�B���C���ڑ��͖�����SSL���g�p����
negotiate security layer:i:1


*/



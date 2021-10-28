
// ThinBridgeRuleUpdater.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "ThinBridgeRuleUpdater.h"
#include "ThinBridgeRuleUpdaterDlg.h"
#include "ThinBridgeRuleUpdaterHelper.h"
#include "locale.h"
#include "psapi.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CThinBridgeRuleUpdaterApp

BEGIN_MESSAGE_MAP(CThinBridgeRuleUpdaterApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CThinBridgeRuleUpdaterApp コンストラクション

CThinBridgeRuleUpdaterApp::CThinBridgeRuleUpdaterApp()
{
	m_iWriteThinBridgeBHO_ResultCode=0;
}


// 唯一の CThinBridgeRuleUpdaterApp オブジェクトです。

CThinBridgeRuleUpdaterApp theApp;


// CThinBridgeRuleUpdaterApp 初期化
#include "wtsapi32.h"
#pragma comment(lib, "wtsapi32.lib")
#include <userenv.h>
#pragma comment (lib, "userenv.lib")
void CThinBridgeRuleUpdaterApp::SetActiveUserNameSID()
{
	m_strCurrentSID= GetCurrentProcessStringSid();
	m_strSID= m_strCurrentSID;
	GetUserNameData();
	PWTS_SESSION_INFOW pStSesInf = NULL;
	DWORD dwCount = 0;
	DWORD dwSesId = 0;
	DWORD dwSize = 0;
	LPTSTR lpNameTmp = NULL;
	// ログオンユーザのセッションリストを取得する
	if (::WTSEnumerateSessionsW(NULL,
		0,                    // 予約（必ず0を指定）
		1,                    // 列挙要求のバージョン(必ず1を指定)
		&pStSesInf,           // 構造体の配列へのポインタを受取る
		&dwCount) == FALSE) // 構造体の数格納用(セッションリスト数)
	{
		// 取得失敗
		return;
	}
	// セッションリスト数分繰り返す
	for (DWORD dwLoop = 0; dwLoop < dwCount; dwLoop++)
	{
		// セッションリストから順次セッション情報を取得する
		if (::WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE,      // ターミナルサーバハンドル
			pStSesInf[dwLoop].SessionId,  // セッションの識別子
			WTSUserName,                    // セッションに関連付けられたユーザー受取を指定
			&lpNameTmp,                     // ユーザ名ポインタ格納用
			&dwSize) == TRUE)             // 受取のデータサイズ
		{
			CString strUserName;
			strUserName = lpNameTmp;
			// 現在アクティブなセッションのIDを取得
			dwSesId = ::WTSGetActiveConsoleSessionId();
			// セッションIDを比較しアクティブなセッションIDと同一ならユーザ名を表示する
			if (pStSesInf[dwLoop].SessionId == dwSesId)
			{
				if(!strUserName.IsEmpty())
				{
					LPTSTR lpDomainTmp = NULL;
					if (::WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE,      // ターミナルサーバハンドル
						pStSesInf[dwLoop].SessionId,  // セッションの識別子
						WTSDomainName,                    // セッションに関連付けられたユーザー受取を指定
						&lpDomainTmp,                     // ユーザ名ポインタ格納用
						&dwSize) == TRUE)             // 受取のデータサイズ
					{
						CString strDomainName;
						strDomainName = lpDomainTmp;
						if (lpDomainTmp != NULL)
						{
							// 解放
							WTSFreeMemory(lpDomainTmp);
							lpDomainTmp = NULL;
						}
						HANDLE hToken={0};
						WTSQueryUserToken(dwSesId, &hToken);
						if(hToken)
						{
							m_strSID =GetTokenStringSid(hToken);
							WriteThinBridgeFlg(_T("LOGON_SID"),m_strSID);
							WriteThinBridgeFlg(_T("CURRENT_SID"), m_strCurrentSID);
							WriteThinBridgeFlg(_T("EXEC_USER"),GetUserNameData());
							CloseHandle(hToken);
							//LPVOID              lpEnvironment={0};
							//STARTUPINFO         startupInfo={0};
							//PROCESS_INFORMATION processInformation={0};
							//CreateEnvironmentBlock(&lpEnvironment, hToken, TRUE);

							//ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
							//startupInfo.cb = sizeof(STARTUPINFO);
							//const WCHAR szD[]= _T("winsta0\\default");
							//startupInfo.lpDesktop = (LPWSTR)szD;
							//CreateProcessAsUser(hToken, L"C:\\Windows\\notepad.exe", NULL, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT, lpEnvironment, NULL, &startupInfo, &processInformation);
							//DestroyEnvironmentBlock(lpEnvironment);
							//CloseHandle(hToken);
							//CloseHandle(processInformation.hThread);
							//CloseHandle(processInformation.hProcess);
						}
						m_strDomainUserName = strDomainName + _T("/") + strUserName;
						WriteThinBridgeFlg(_T("LOGON_USER"), m_strDomainUserName);
					}
				}
			}
		}
		else
		{
			// 取得失敗
			return;
		}
	}
	if (lpNameTmp != NULL)
	{
		// 解放
		WTSFreeMemory(lpNameTmp);
		lpNameTmp = NULL;
	}
	if (pStSesInf != NULL)
	{
		// 解放
		WTSFreeMemory(pStSesInf);
		pStSesInf = NULL;
	}
	return;
}

int CThinBridgeRuleUpdaterApp::ExecActiveSession()
{
	//タスクスケジューラからの実行の場合に絞る。
	if(m_strCurrentSID != _T("S-1-5-18"))
		return 1;

	PWTS_SESSION_INFOW pStSesInf = NULL;
	DWORD dwCount = 0;
	DWORD dwSesId = 0;
	DWORD dwSize = 0;
	// ログオンユーザのセッションリストを取得する
	if (::WTSEnumerateSessionsW(NULL,
		0,                    // 予約（必ず0を指定）
		1,                    // 列挙要求のバージョン(必ず1を指定)
		&pStSesInf,           // 構造体の配列へのポインタを受取る
		&dwCount) == FALSE) // 構造体の数格納用(セッションリスト数)
	{
		// 取得失敗
		return 2;
	}
	int iRet=0;
	// セッションリスト数分繰り返す
	for (DWORD dwLoop = 0; dwLoop < dwCount; dwLoop++)
	{
		iRet=3;
		// 現在アクティブなセッションのIDを取得
		dwSesId = ::WTSGetActiveConsoleSessionId();
		// セッションIDを比較しアクティブなセッションIDと同一ならユーザ名を表示する
		if (pStSesInf[dwLoop].SessionId == dwSesId)
		{
			iRet = 4;
			HANDLE hToken = { 0 };
			WTSQueryUserToken(pStSesInf[dwLoop].SessionId, &hToken);
			if (hToken)
			{
				iRet = 5;
				CString strSID = GetTokenStringSid(hToken);
				//systemアカウントのSIDでは無い。ログインセッションがある。
				if (strSID != _T("S-1-5-18"))
				{
					//ログインユーザーセッションでこのプログラムを起動する。
					//権限絡み（CIFSなどへのアクセス）
					LPVOID              lpEnvironment={0};
					STARTUPINFO         startupInfo={0};
					PROCESS_INFORMATION processInformation={0};
					CreateEnvironmentBlock(&lpEnvironment, hToken, TRUE);
					ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
					startupInfo.cb = sizeof(STARTUPINFO);
					const WCHAR szD[]= _T("winsta0\\default");
					startupInfo.lpDesktop = (LPWSTR)szD;
					CreateProcessAsUser(hToken, m_strExeFullPath, NULL, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT, lpEnvironment, NULL, &startupInfo, &processInformation);
					DestroyEnvironmentBlock(lpEnvironment);
					CloseHandle(processInformation.hThread);
					CloseHandle(processInformation.hProcess);
					iRet=200;	
					CloseHandle(hToken);
					break;
				}
				CloseHandle(hToken);
			}
		}
	}
	//RDP対応
	//Console Sessionが取れない場合は、はじめのユーザーで実行してみる。
	if(iRet!=200)
	{
		// セッションリスト数分繰り返す
		for (DWORD dwLoop = 0; dwLoop < dwCount; dwLoop++)
		{
			iRet = 6;
			HANDLE hToken = { 0 };
			WTSQueryUserToken(pStSesInf[dwLoop].SessionId, &hToken);
			if (hToken)
			{
				iRet = 7;
				CString strSID = GetTokenStringSid(hToken);
				//systemアカウントのSIDでは無い。ログインセッションがある。
				if (strSID != _T("S-1-5-18"))
				{
					//ログインユーザーセッションでこのプログラムを起動する。
					//権限絡み（CIFSなどへのアクセス）
					LPVOID              lpEnvironment = { 0 };
					STARTUPINFO         startupInfo = { 0 };
					PROCESS_INFORMATION processInformation = { 0 };
					CreateEnvironmentBlock(&lpEnvironment, hToken, TRUE);
					ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
					startupInfo.cb = sizeof(STARTUPINFO);
					const WCHAR szD[] = _T("winsta0\\default");
					startupInfo.lpDesktop = (LPWSTR)szD;
					CreateProcessAsUser(hToken, m_strExeFullPath, NULL, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT, lpEnvironment, NULL, &startupInfo, &processInformation);
					DestroyEnvironmentBlock(lpEnvironment);
					CloseHandle(processInformation.hThread);
					CloseHandle(processInformation.hProcess);
					iRet = 200;
					CloseHandle(hToken);
					break;
				}
				CloseHandle(hToken);
			}
		}
	}
	if (pStSesInf != NULL)
	{
		// 解放
		WTSFreeMemory(pStSesInf);
		pStSesInf = NULL;
	}
	return iRet;
}

BOOL CThinBridgeRuleUpdaterApp::InitInstance()
{
	_wsetlocale(LC_ALL, _T("jpn"));

	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	::OleInitialize(NULL);
	if (!AfxOleInit())
	{
		return FALSE;
	}
	AfxOleGetMessageFilter()->EnableNotRespondingDialog(FALSE);
	AfxOleGetMessageFilter()->EnableBusyDialog(FALSE);

	INITCOMMONCONTROLSEX InitCtrls={0};
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	AfxEnableControlContainer();

	TCHAR szPath[MAX_PATH+1]={0};
	TCHAR szDrive[_MAX_DRIVE]={0};
	TCHAR szDir[_MAX_DIR]={0};
	TCHAR szFileName[_MAX_FNAME]={0};
	TCHAR szExt[_MAX_EXT]={0};
	GetModuleFileName(AfxGetInstanceHandle(),szPath,MAX_PATH);
	_tsplitpath_s(szPath, szDrive,_MAX_DRIVE, szDir,_MAX_DIR, szFileName,_MAX_FNAME, szExt,_MAX_EXT); 

	m_strExeFullPath=szPath;
	m_strExeNameNoExt = szFileName;

	m_strExeFolderPath=szDrive;
	m_strExeFolderPath+= szDir;

	//設定データのロード
	m_strSetting_FileFullPath = szDrive;
	m_strSetting_FileFullPath += szDir;
	m_strSetting_FileFullPath += _T("ThinBridgeRuleUpdater.conf");
	this->ReadSetting();

	m_strThinBridgeBHOFileFullPath = szDrive;
	m_strThinBridgeBHOFileFullPath += szDir;
	m_strThinBridgeBHOFileFullPath += _T("ThinBridgeBHO.ini");

	m_strExecLogFileFullPath = szDrive;
	m_strExecLogFileFullPath += szDir;
	m_strExecLogFileFullPath += _T("TBUpdateLog\\");
	::CreateDirectory(m_strExecLogFileFullPath, NULL);
	m_strExecLogFileDir= m_strExecLogFileFullPath;
	CTime time = CTime::GetCurrentTime();
	CString strLogFileNow;
	strLogFileNow.Format(_T("ThinBridgeRuleUpdater%s.log"), time.Format(_T("%Y-%m-%d")));
	m_strExecLogFileFullPath += strLogFileNow;
	this->LogRotate(m_strExecLogFileDir);

	SetActiveUserNameSID();

	CString strMsg;
	CString Command1;
	CString Command2;
	//コマンドラインあり。
	if(m_lpCmdLine[0] != '\0')
	{
		//パラメータが1つ以上
		if(__argc >=2)
		{
			Command1 = CString(__wargv[1]);
			Command1.Replace(_T("\""),_T(""));
			Command1.TrimLeft();
			Command1.TrimRight();
		}
		if (Command1.CompareNoCase(_T("/Config")) == 0)
		{
			CThinBridgeRuleUpdaterDlg dlg;
			m_pMainWnd = &dlg;
			dlg.DoModal();
			return FALSE;
		}
	}
	else
	{
		if (m_strExeNameNoExt.CompareNoCase(_T("ThinBridgeRuleUpdaterSetting")) == 0)
		{
			CThinBridgeRuleUpdaterDlg dlg;
			m_pMainWnd = &dlg;
			dlg.DoModal();
			return FALSE;
		}

		//タスクスケジューラからの起動の場合
		//タスクスケジューラで1時間に1回 実行されるため負荷を分散するために、ランダムでWaitする。0-10分の範囲
		int nRand = 0;
		CString strWaitTime;
		CTime time = CTime::GetCurrentTime();

		//systemアカウントのSID
		if (m_strCurrentSID == _T("S-1-5-18"))
		{
			CTime cTime = CTime::GetCurrentTime();
			int iMin=0;
			iMin = cTime.GetMinute();
			BOOL bWait=FALSE;
			srand(GetTickCount());

			//ログイン時にもタスクスケジューラ経由でこれが動く。
			//タスクスケジューラ経由で実行しているので判断ができないため。
			//30分の間は、スケジューラの30分起動なので0-10分Waitする。負荷分散(VDI)
			if (InCtrixVDI())
			{
				if (iMin == 30)
				{
					//VDIで30分の起動でタスクスケジュールからの場合　ログオンタスクの場合と定期実行が重なる場合がある。
					//VDI側は、ドライブがリフレッシュされるのでThinBridgeBHO.iniが残らない。
					//ファイルがない場合は、Waitせずにすぐに実行する。
					if (PathFileExists(m_strThinBridgeBHOFileFullPath))
					{
						bWait = TRUE;
						//1分(60秒)の中でランダム値でWaitさせる。少しでも負荷を分散したい。
						nRand = rand() % 60;
						DWORD dWaitTime = 0;
						dWaitTime = nRand * 1000;//秒単位
						WaitSec(dWaitTime);
					}
					else
					{
						bWait = FALSE;
					}
				}
			}
			//00分の間は、スケジューラの00分起動なので0-10分Waitする。負荷分散(LTSC)
			else
			{
				//端末側は、ログインに関してスタートアップのショートカットで実行されるので
				//タスク実行は定期実行のみ。ある程度遅延がある可能性を考慮し0-1分の間の場合はWaitする。
				if (iMin == 0|| iMin == 1)
				{
					//ファイルがない場合は、Waitせずにすぐに実行する。
					if (PathFileExists(m_strThinBridgeBHOFileFullPath))
					{
						bWait = TRUE;
						//1分(60秒)の中でランダム値でWaitさせる。少しでも負荷を分散したい。
						nRand = rand() % 60;
						DWORD dWaitTime = 0;
						dWaitTime = nRand * 1000;//秒単位
						WaitSec(dWaitTime);
					}
					else
					{
						bWait = FALSE;
					}
				}
			}
			if(bWait)
			{
				EmptyWorkingSet(GetCurrentProcess());
				if(SettingConf.m_iInterval!=0)
				{
					nRand = rand() % SettingConf.m_iInterval;
					DWORD dWaitTime = 0;
					dWaitTime = nRand * 1000*60;//分単位
					WaitSec(dWaitTime);
				}
			}
			else
			{
				//ログインがトリガーになっている可能性が高いのでWaitなし。
				nRand=0;
			}

			int iRet = 0;
			iRet = ExecActiveSession();
			//実行に失敗した場合は、System権限で実行する
			//200 OKの場合は、ログインセッションでこのEXEが実行されるので、ここで終了
			//失敗している場合は、そのまま流す。
			if (iRet==200)
			{
				return FALSE;
			}
		}
		//2重起動制限
		HANDLE hMutexCtx = NULL;
		hMutexCtx = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("Local\\TBRuleUpdateFlg"));
		if (hMutexCtx != NULL)
		{
			//既に起動しているので、起動しているプロセスに任せる
			return FALSE;
		}
		//Mutex作成
		if (!m_hMutex)
		{
			m_hMutex = ::CreateMutex(NULL, FALSE, _T("Local\\TBRuleUpdateFlg"));
		}
		//strWaitTime.Format(_T(" 待機(%d分)"), nRand);
		CString strResult;
		strResult = WriteThinBridgeBHO(&SettingConf, TRUE, FALSE);
		CStdioFile out;
		if (out.Open(m_strExecLogFileFullPath, CFile::modeWrite | CFile::shareDenyNone | CFile::modeCreate | CFile::modeNoTruncate))
		{
			TRY
			{
				out.SeekToEnd();
				out.WriteString(_T("\n自動更新//////////////////////////////////////////////////////////////////////////////////////////////////\n"));
				out.WriteString(time.Format(_T("%Y-%m-%d %H:%M:%S")));
				out.WriteString(strWaitTime);
				out.WriteString(_T("\n処理結果："));
				out.WriteString(GetErrorMsg());
				out.WriteString(_T("\n"));
				out.WriteString(_T("ExecUser:["));
				out.WriteString(m_strExecUserName);
				out.WriteString(_T("]"));
				out.WriteString(_T("LogonUser:["));
				out.WriteString(m_strDomainUserName);
				out.WriteString(_T("]"));
				out.WriteString(_T("LogonUserSID:["));
				out.WriteString(m_strSID);
				out.WriteString(_T("]"));
				out.WriteString(_T("CurrentSID:["));
				out.WriteString(m_strCurrentSID);
				out.WriteString(_T("]\n"));
				out.WriteString(_T("SettingType:"));
				out.WriteString(SettingConf.IsGPO()?_T("GPO\n"):_T("File\n"));
				strResult.Replace(_T("\r\n"), _T("\n"));
				out.WriteString(strResult);
				out.Close();
			}
				CATCH(CFileException, eP) {}
			END_CATCH
		}
	}
	return FALSE;
}

void CThinBridgeRuleUpdaterApp::ReadSetting()
{
	//GPOを優先させる
	if(!SettingConf.ReadSettingGPO())
		SettingConf.ReadSetting(m_strSetting_FileFullPath);
}
CString CThinBridgeRuleUpdaterApp::WriteThinBridgeBHO(CConfData* pSettingConf,BOOL bWriteBHO_ini,BOOL bFromGUI)
{
	m_iWriteThinBridgeBHO_ResultCode=0;
	_wsetlocale(LC_ALL, _T("jpn"));

	CString strRet;
	if (pSettingConf == NULL)
	{
		strRet.Format(_T("エラー：設定データが不正な状態です。\r\n%s\r\n"), m_strSetting_FileFullPath);
		m_iWriteThinBridgeBHO_ResultCode = ERR_THINBRIDGE_BHO_INI_NOT_FOUND;
		return strRet;
	}

	if (pSettingConf->m_strConfigServerURL1.IsEmpty())
	{
		m_iWriteThinBridgeBHO_ResultCode = INFO_CONFIG_SERVER_NOT_SETTING;
		return strRet;
	}

	CString strConfigServerURL;
	ThinBridgeRuleUpdaterHelper TBRuleUpdate;

	CString strServerIndex;
	strConfigServerURL = pSettingConf->m_strConfigServerURL1;
	int iRet = 0;
	CString strLineMsg;
	CString strLogTry;
	CStringArray strConfigServerArry;
	srand(GetTickCount());

	strConfigServerArry.Add(pSettingConf->m_strConfigServerURL1);
	if (!pSettingConf->m_strConfigServerURL2.IsEmpty())
		strConfigServerArry.Add(pSettingConf->m_strConfigServerURL2);
	if (!pSettingConf->m_strConfigServerURL3.IsEmpty())
		strConfigServerArry.Add(pSettingConf->m_strConfigServerURL3);
	if (!pSettingConf->m_strConfigServerURL4.IsEmpty())
		strConfigServerArry.Add(pSettingConf->m_strConfigServerURL4);
	if (!pSettingConf->m_strConfigServerURL5.IsEmpty())
		strConfigServerArry.Add(pSettingConf->m_strConfigServerURL5);
	if (!pSettingConf->m_strConfigServerURL6.IsEmpty())
		strConfigServerArry.Add(pSettingConf->m_strConfigServerURL6);

	CStringArray strConfigServerArryExec;
	int iSetServerCnt = 0;
	iSetServerCnt = strConfigServerArry.GetSize();

	int nRandServer = 0;
	//1つのみ。
	if (iSetServerCnt == 1)
	{
		strConfigServerArryExec.Add(pSettingConf->m_strConfigServerURL1);
	}
	else
	{
		//ランダム
		if(pSettingConf->m_iServerConnectionRule==0)
		{
			CString strSvrTemp;
			CStringArray strConfigServerArryWork;
			for (int ii = 0; ii < 20; ii++)
			{
				strSvrTemp.Empty();
				int iSetServerCntAll = strConfigServerArry.GetSize();
				for (int j = iSetServerCntAll; j > 0; j--)
				{
					nRandServer = rand() % iSetServerCntAll;
					strSvrTemp = strConfigServerArry.GetAt(nRandServer);
					if (!strSvrTemp.IsEmpty())
					{
						strConfigServerArryExec.Add(strSvrTemp);
						strConfigServerArry.SetAt(nRandServer, _T(""));
					}
				}
				strConfigServerArryWork.RemoveAll();
				for (int i = 0; i < strConfigServerArry.GetSize(); i++)
				{
					strSvrTemp = strConfigServerArry.GetAt(i);
					if (!strSvrTemp.IsEmpty())
						strConfigServerArryWork.Add(strSvrTemp);
				}
				if (strConfigServerArryWork.GetSize() > 0)
				{
					strConfigServerArry.RemoveAll();
					strConfigServerArry.Copy(strConfigServerArryWork);
				}
				int iSetServerCntNow = strConfigServerArryExec.GetSize();
				if (iSetServerCntNow == iSetServerCnt)
					break;
			}
			if (strConfigServerArryExec.GetSize() != iSetServerCnt)
			{
				for (int i = 0; i < strConfigServerArry.GetSize(); i++)
				{
					strSvrTemp.Empty();
					strSvrTemp = strConfigServerArry.GetAt(i);
					if (!strSvrTemp.IsEmpty())
					{
						strConfigServerArryExec.Add(strSvrTemp);
						strConfigServerArry.SetAt(nRandServer, _T(""));
					}
				}
			}
		}
		//リスト順
		else
		{
			strConfigServerArryExec.Copy(strConfigServerArry);
		}
		CString strMsgTemp;
		for (int i = 0; i < strConfigServerArryExec.GetSize(); i++)
		{
			strMsgTemp.Format(_T("取得順%d：%s\r\n"), i + 1, strConfigServerArryExec.GetAt(i));
			strServerIndex += strMsgTemp;
		}
	}

	//GUIからの場合
	if(bFromGUI)
	{
		//複数サーバー冗長化
		for (int j = 0; j < strConfigServerArryExec.GetSize(); j++)
		{
			strConfigServerURL = strConfigServerArryExec.GetAt(j);
			iRet = TBRuleUpdate.ExecMain(strConfigServerURL);
			if (iRet == OK_SERVER)
			{
				strLineMsg.Format(_T("%d:OK %d %s %s"), j + 1, iRet, TBRuleUpdate.GetErrorMsg(iRet), strConfigServerURL);
				pSettingConf->m_strConfigServerURLOK = strConfigServerURL;
				strLogTry += LogFmt(strLineMsg);
				strRet += strLogTry;
				break;
			}
			strLineMsg.Format(_T("%d:Err %d %s %s"), j + 1, iRet, TBRuleUpdate.GetErrorMsg(iRet), strConfigServerURL);
			strLogTry += LogFmt(strLineMsg);
		}
		if (OK_SERVER != iRet)
		{
			strRet = _T("エラー：問題が発生しました。\r\n");
			strRet += strServerIndex;
			strRet += strLogTry;
			strRet += TBRuleUpdate.GetLog();
			m_iWriteThinBridgeBHO_ResultCode = ERR_CONFIG_SERVER;
			return strRet;
		}
	}
	else//タスクスケジューラ、スタートアップ、直接起動の場合はリトライをする。
	{
		//複数サーバー冗長化
		for(int j=0;j< strConfigServerArryExec.GetSize();j++)
		{
			strConfigServerURL = strConfigServerArryExec.GetAt(j);
			iRet = TBRuleUpdate.ExecMain(strConfigServerURL);
			if (iRet == OK_SERVER)
			{
				strLineMsg.Format(_T("%d:OK %d %s %s"), j + 1, iRet, TBRuleUpdate.GetErrorMsg(iRet), strConfigServerURL);
				pSettingConf->m_strConfigServerURLOK = strConfigServerURL;
				strLogTry += LogFmt(strLineMsg);
				strRet += strLogTry;
				break;
			}
			strLineMsg.Format(_T("%d:Err %d %s %s"), j + 1, iRet, TBRuleUpdate.GetErrorMsg(iRet), strConfigServerURL);
			strLogTry += LogFmt(strLineMsg);

			////10秒の間で待ってみる
			//int nRand = rand() % 10;
			//DWORD dWaitTime = 0;
			//dWaitTime = nRand * 1000;//秒単位
			//WaitSec(dWaitTime);
		}

		//retry 2
		if (OK_SERVER != iRet)
		{
			//10秒待ってみる
			WaitSec(1000*10);

			int nRand = rand() % 10;
			DWORD dWaitTime = 0;
			dWaitTime = nRand * 1000;//秒単位
			//+10秒くらいの間で待ってみる
			WaitSec(dWaitTime);

			for (int j = 0; j < strConfigServerArryExec.GetSize(); j++)
			{
				strConfigServerURL = strConfigServerArryExec.GetAt(j);
				iRet = TBRuleUpdate.ExecMain(strConfigServerURL);
				if (iRet == OK_SERVER)
				{
					strLineMsg.Format(_T("%d:OK %d %s %s"), j + 1, iRet, TBRuleUpdate.GetErrorMsg(iRet), strConfigServerURL);
					pSettingConf->m_strConfigServerURLOK = strConfigServerURL;
					strLogTry += LogFmt(strLineMsg);
					strRet += strLogTry;
					break;
				}
				strLineMsg.Format(_T("%d:Err %d %s %s"), j + 1, iRet, TBRuleUpdate.GetErrorMsg(iRet), strConfigServerURL);
				strLogTry += LogFmt(strLineMsg);

				////10秒の間で待ってみる
				//int nRand = rand() % 10;
				//DWORD dWaitTime = 0;
				//dWaitTime = nRand * 1000;//秒単位
				//WaitSec(dWaitTime);
			}
		}

		//retry 3
		if (OK_SERVER != iRet)
		{
			//10秒待ってみる
			WaitSec(1000 * 10);

			int nRand = rand() % 10;
			DWORD dWaitTime = 0;
			dWaitTime = nRand * 1000;//秒単位
			//+10秒くらいの間で待ってみる
			WaitSec(dWaitTime);

			for (int j = 0; j < strConfigServerArryExec.GetSize(); j++)
			{
				strConfigServerURL = strConfigServerArryExec.GetAt(j);
				iRet = TBRuleUpdate.ExecMain(strConfigServerURL);
				if (iRet == OK_SERVER)
				{
					strLineMsg.Format(_T("%d:OK %d %s %s"), j + 1, iRet, TBRuleUpdate.GetErrorMsg(iRet), strConfigServerURL);
					pSettingConf->m_strConfigServerURLOK = strConfigServerURL;
					strLogTry += LogFmt(strLineMsg);
					strRet += strLogTry;
					break;
				}
				strLineMsg.Format(_T("%d:Err %d %s %s"), j + 1, iRet, TBRuleUpdate.GetErrorMsg(iRet), strConfigServerURL);
				strLogTry += LogFmt(strLineMsg);

				////10秒の間で待ってみる
				//int nRand = rand() % 10;
				//DWORD dWaitTime = 0;
				//dWaitTime = nRand * 1000;//秒単位
				//WaitSec(dWaitTime);
			}
		}
		if (OK_SERVER != iRet)
		{
			strRet = _T("エラー：問題が発生しました。\r\n");
			strRet += strServerIndex;
			strRet += strLogTry;
			strRet += TBRuleUpdate.GetLog();
			m_iWriteThinBridgeBHO_ResultCode = ERR_CONFIG_SERVER;
			return strRet;
		}
	}
	strRet += TBRuleUpdate.GetLog();

	//ThinBridgeBHOのデータを全て読み込む
	CStdioFile in;
	CString strTemp;
	CString strLogMsgLine;

	m_strArrayBHODataOrg.RemoveAll();
	if(in.Open(m_strThinBridgeBHOFileFullPath, CFile::modeRead | CFile::shareDenyWrite))
	{
		CStringArray strArrayBHOData;
		while (in.ReadString(strTemp))
		{
			m_strArrayBHODataOrg.Add(strTemp);
		}
		in.Close();
	}
	//Diff
	BOOL bDiff=FALSE;
	CStringArray strArrayBHODataNew;
	SBUtil2::Split(&strArrayBHODataNew, TBRuleUpdate.m_strGetData,_T("\r\n"));

	//サーバーから取得した後にチェックしているのでここではやらない。
	////サーバーから取得ファイルの内容チェック
	//int iValidCnt=0;
	//for (int j = 0; j < strArrayBHODataNew.GetSize(); j++)
	//{
	//	strTemp= strArrayBHODataNew.GetAt(j);
	//	if (!strTemp.IsEmpty())
	//	{
	//		if(strTemp==_T("[GLOBAL]"))
	//			iValidCnt++;
	//		else if(strTemp == _T("[Chrome]"))
	//			iValidCnt++;
	//		else if (strTemp == _T("[CUSTOM18]"))
	//			iValidCnt++;
	//		else if (strTemp == _T("[CUSTOM19]"))
	//			iValidCnt++;
	//		else if (strTemp == _T("[CUSTOM20]"))
	//			iValidCnt++;
	//	}
	//}
	//if(iValidCnt < 3)
	//{
	//	strRet.Format(_T("エラー：サーバーから取得したThinBridge設定データが不正な状態です。\r\n"));
	//	strRet += TBRuleUpdate.GetLog();
	//	m_iWriteThinBridgeBHO_ResultCode = ERR_THINBRIDGE_BHO_INI_INVALID;
	//	return strRet;
	//}

	CString strDiffLine;
	CString strDiffLine2;
	if(m_strArrayBHODataOrg.GetSize()!= strArrayBHODataNew.GetSize())
		bDiff = TRUE;
	else
	{
		for (int j = 0; j < m_strArrayBHODataOrg.GetSize(); j++)
		{
			strDiffLine = m_strArrayBHODataOrg.GetAt(j);
			strDiffLine2 = strArrayBHODataNew.GetAt(j);
			if (strDiffLine != strDiffLine2)
			{
				bDiff=TRUE;
				break;
			}
		}
	}

	//変更あり
	if(bDiff)
	{
		//BHOの書き込み
		if(bWriteBHO_ini)
		{
			//file backup
			::CopyFile(m_strThinBridgeBHOFileFullPath + ".update_bak4", m_strThinBridgeBHOFileFullPath + ".update_bak5", FALSE);
			::CopyFile(m_strThinBridgeBHOFileFullPath + ".update_bak3", m_strThinBridgeBHOFileFullPath + ".update_bak4", FALSE);
			::CopyFile(m_strThinBridgeBHOFileFullPath + ".update_bak2", m_strThinBridgeBHOFileFullPath + ".update_bak3", FALSE);
			::CopyFile(m_strThinBridgeBHOFileFullPath + ".update_bak1", m_strThinBridgeBHOFileFullPath + ".update_bak2", FALSE);
			::CopyFile(m_strThinBridgeBHOFileFullPath, m_strThinBridgeBHOFileFullPath + ".update_bak1", FALSE);
			SetLastError(NO_ERROR);

			//読み取り専用のチェックを外す。
			DWORD dwAttributes=0;
			SetFileAttributes(m_strThinBridgeBHOFileFullPath, dwAttributes);

			CStdioFile out;
			if (out.Open(m_strThinBridgeBHOFileFullPath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
			{
				//command1
				CString strWriteData;
				for(int i=0; i< strArrayBHODataNew.GetSize();i++)
				{
					strWriteData.Format(_T("%s\n"), strArrayBHODataNew.GetAt(i));
					out.WriteString(strWriteData);
					strRet += strArrayBHODataNew.GetAt(i);
					strRet += _T("\r\n");
				}
				out.Close();
				CString strTemp1;
				strTemp1 = strRet;
				strRet = _T("【成功】\r\n設定ファイルの保存に成功しました。\r\n");
				strRet += strServerIndex;
				strRet +=strTemp1;
				m_iWriteThinBridgeBHO_ResultCode = SUCCESS_ALL;
				////////////////////////////////////////////////////////////////////////////////
				//差分を出力する。
				CTime time = CTime::GetCurrentTime();
				CString strLogFileNow;
				CString strHistoryDir;

				strHistoryDir = m_strExecLogFileDir;
				strHistoryDir += _T("History\\");
				::CreateDirectory(strHistoryDir, NULL);

				CString strBefore;
				strBefore = strHistoryDir;
				strLogFileNow.Format(_T("%s_%s_Before.ini"), _T("ThinBridgeBHO"), time.Format(_T("%Y-%m-%d-%H%M%S")));
				strBefore += strLogFileNow;
				::CopyFile(m_strThinBridgeBHOFileFullPath + ".update_bak1", strBefore, FALSE);
				SetFileAttributes(strBefore, 0);
				this->LogRotateETC(strHistoryDir, _T("ThinBridgeBHO_"), _T("_Before.ini"));

				CString strAfter;
				strAfter = strHistoryDir;
				strLogFileNow.Format(_T("%s_%s_After.ini"), _T("ThinBridgeBHO"), time.Format(_T("%Y-%m-%d-%H%M%S")));
				strAfter += strLogFileNow;
				::CopyFile(m_strThinBridgeBHOFileFullPath, strAfter, FALSE);
				SetFileAttributes(strAfter, 0);
				this->LogRotateETC(strHistoryDir, _T("ThinBridgeBHO_"), _T("_After.ini"));


				CString strDiffFile;
				strDiffFile = strHistoryDir;
				strLogFileNow.Format(_T("%s_%s_Diff.ini"), _T("ThinBridgeBHO"), time.Format(_T("%Y-%m-%d-%H%M%S")));
				strDiffFile += strLogFileNow;
				this->LogRotateETC(strHistoryDir, _T("ThinBridgeBHO_"), _T("_Diff.ini"));

				CompareFiles(strBefore, strAfter, strDiffFile);

			}
			else
			{
				CString strTemp1;
				strTemp1 = strRet;
				strRet = _T("エラー：ThinBridgeBHO.ini ファイル保存で問題が発生しました。\r\n");
				strRet += strServerIndex;
				strRet += strTemp1;
				strTemp.Format(_T("#Error2:ThinBridgeBHO.ini FileOpenError %s\r\n"), m_strThinBridgeBHOFileFullPath);
				strRet += strTemp;
				m_iWriteThinBridgeBHO_ResultCode = ERR_THINBRIDGE_BHO_INI_FILE_WRITE;
			}
			//読み取り専用|隠しファイルにする。
			dwAttributes = FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN;
			SetFileAttributes(m_strThinBridgeBHOFileFullPath, dwAttributes);

		}
		else
		{
			CString strTemp1;
			strTemp1 = strRet;
			strRet = _T("【成功】\r\n設定データ取得、差分チェックに成功しました。\r\n");
			CString strTemp2;
			strTemp2.Format(_T("取得元：%s\r\n更新先：%s\r\n"), strConfigServerURL, m_strThinBridgeBHOFileFullPath);
			strRet += strServerIndex;
			strRet += strTemp2;
			strRet += strTemp1;
			m_iWriteThinBridgeBHO_ResultCode = SUCCESS_NO_FILE_WRITE;
		}
	}
	else
	{
		CString strTemp1;
		strTemp1 = strRet;
		strRet =_T("【同一内容 変更なし】\r\n更新内容に差分がありませんでした。\r\n");
		strTemp1.Format(_T("取得元：%s\r\n更新先：%s\r\n"), strConfigServerURL, m_strThinBridgeBHOFileFullPath);
		strRet += strServerIndex;
		strRet += strTemp1;
		m_iWriteThinBridgeBHO_ResultCode=SUCCESS_SAME_DATA;
	}
	return strRet;
}

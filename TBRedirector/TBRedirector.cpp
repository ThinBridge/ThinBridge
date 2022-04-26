#include "pch.h"
#include "framework.h"
#include "TBRedirector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTBRedirectorApp, CWinApp)
END_MESSAGE_MAP()

CTBRedirectorApp::CTBRedirectorApp()
{
}
// 唯一の CTBRedirectorApp オブジェクト
CTBRedirectorApp theApp;

BOOL CTBRedirectorApp::InitInstance()
{
	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	::OleInitialize(NULL);
	if (!AfxOleInit())
	{
		return FALSE;
	}
	AfxOleGetMessageFilter()->EnableNotRespondingDialog(FALSE);
	AfxOleGetMessageFilter()->EnableBusyDialog(FALSE);
	CString logmsg;

	CWinApp::InitInstance();
	TCHAR szPath[MAX_PATH + 1] = { 0 };
	TCHAR szDrive[_MAX_DRIVE] = { 0 };
	TCHAR szDir[_MAX_DIR] = { 0 };
	TCHAR szFileName[_MAX_FNAME] = { 0 };
	TCHAR szExt[_MAX_EXT] = { 0 };
	GetModuleFileName(AfxGetInstanceHandle(), szPath, MAX_PATH);
	_tsplitpath_s(szPath, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFileName, _MAX_FNAME, szExt, _MAX_EXT);

	m_strExeFullPath = szPath;
	m_strExeNameNoExt = szFileName;

	TCHAR lpszPath[MAX_PATH * 2] = _T("");
	LPITEMIDLIST pidl = { 0 };
	if (::SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &pidl) == NOERROR)
	{
		::SHGetPathFromIDList(pidl, lpszPath);
		::CoTaskMemFree(pidl);
	}
	m_strUserDataPath = lpszPath;
	m_strUserDataPath += _T("\\ThinBridge\\");

	CString strFolderPath;
	strFolderPath=szDrive;
	strFolderPath+=szDir;

	m_strThinBridge_EXE_FullPath = strFolderPath;
	m_strThinBridge_EXE_FullPath += _T("ThinBridge.exe");

	m_strRedirectFilePath = strFolderPath;
	m_strRedirectFilePath += _T("ThinBridgeBHO.ini");

	CString strUserThinBridgeBHOINIFile;
	strUserThinBridgeBHOINIFile = m_strUserDataPath;
	strUserThinBridgeBHOINIFile += _T("ThinBridgeBHO.ini");
	//ユーザー設定を優先させる。
	if (PathFileExists(strUserThinBridgeBHOINIFile))
	{
		m_strRedirectFilePath = strUserThinBridgeBHOINIFile;
	}
	logmsg.Format(_T("ThinBridgeBHO.ini [%s]"), m_strRedirectFilePath);
	this->WriteDebugTraceDateTime(logmsg);

	logmsg = _T("InitInstance Stage:01");
	this->WriteDebugTraceDateTime(logmsg);

	//Delay load
	m_RedirectList.SetArrayData(m_strRedirectFilePath);
	logmsg = _T("InitInstance Stage:02");
	this->WriteDebugTraceDateTime(logmsg);

	CString Command1;
	CString Command2;
	//コマンドラインあり。
	if (m_lpCmdLine[0] != '\0')
	{
		//パラメータが1つだけ
		if (__argc == 2)
		{
			Command1 = CString(__wargv[1]);
			Command1.Replace(_T("\""), _T(""));
			Command1.TrimLeft();
			Command1.TrimRight();

			if (!Command1.IsEmpty())
			{
				//URLかFilePathの場合は、強制的にm_CommandParamとする。
				if (SBUtil::IsURL(Command1))
				{
					m_CommandParam = Command1;
				}
				//filepath
				else if (Command1.Find(_T(":")) == 1)
				{
					m_CommandParam = Command1;
				}
				//-は、オプション
				else if (Command1.Find(_T("-")) == 0)
				{
					m_OptionParam = Command1;
				}
				// /は、オプション
				else if (Command1.Find(_T("/")) == 0)
				{
					m_OptionParam = Command1;
				}
			}
		}
		if (!m_CommandParam.IsEmpty())
			this->ExecRedirect(m_CommandParam);
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif
	return FALSE;
}

void CTBRedirectorApp::ExecRedirect(LPCTSTR lpURL)
{
	if (!SBUtil::IsURL_HTTP(lpURL))
	{
//		CString strMsg;
//		strMsg.Format(_T("%s [%s]"), _T("URLが正しく設定されていません。"), lpURL);
//		::MessageBox(NULL, strMsg, m_strExeNameNoExt, MB_ICONERROR);
		CString strPathUpper;
		strPathUpper = _T("\\");
		strPathUpper += lpURL;
		strPathUpper.Replace(_T("/"),_T("\\"));
		strPathUpper.MakeUpper();
		if (strPathUpper.Find(_T("\\CMD.EXE")) >= 0
			|| strPathUpper.Find(_T("\\ARP.EXE")) >= 0
			|| strPathUpper.Find(_T("\\AT.EXE")) >= 0
			|| strPathUpper.Find(_T("\\BCDEDIT.EXE")) >= 0
			|| strPathUpper.Find(_T("\\CACLS.EXE")) >= 0
			|| strPathUpper.Find(_T("\\CERTREQ.EXE")) >= 0
			|| strPathUpper.Find(_T("\\CIPHER.EXE")) >= 0
			|| strPathUpper.Find(_T("\\CMD.EXE")) >= 0
			|| strPathUpper.Find(_T("\\CONTROL.EXE")) >= 0
			|| strPathUpper.Find(_T("\\CSCRIPT.EXE")) >= 0
			|| strPathUpper.Find(_T("\\DLLHOST.EXE")) >= 0
			|| strPathUpper.Find(_T("\\DOSX.EXE")) >= 0
			|| strPathUpper.Find(_T("\\EXPLORER.EXE")) >= 0
			|| strPathUpper.Find(_T("\\FORMAT.EXE")) >= 0
			|| strPathUpper.Find(_T("\\FSUTIL.EXE")) >= 0
			|| strPathUpper.Find(_T("\\FTP.EXE")) >= 0
			|| strPathUpper.Find(_T("\\IPCONFIG.EXE")) >= 0
			|| strPathUpper.Find(_T("\\ISCSICLI.EXE")) >= 0
			|| strPathUpper.Find(_T("\\MSCDEXNT.EXE")) >= 0
			|| strPathUpper.Find(_T("\\MSHTA.EXE")) >= 0
			|| strPathUpper.Find(_T("\\NBTSTAT.EXE")) >= 0
			|| strPathUpper.Find(_T("\\NET.EXE")) >= 0
			|| strPathUpper.Find(_T("\\NET1.EXE")) >= 0
			|| strPathUpper.Find(_T("\\NETSH.EXE")) >= 0
			|| strPathUpper.Find(_T("\\NETSTAT.EXE")) >= 0
			|| strPathUpper.Find(_T("\\NSLOOKUP.EXE")) >= 0
			|| strPathUpper.Find(_T("\\PING.EXE")) >= 0
			|| strPathUpper.Find(_T("\\POWERSHELL.EXE")) >= 0
			|| strPathUpper.Find(_T("\\QPROCESS.EXE")) >= 0
			|| strPathUpper.Find(_T("\\QUERY.EXE")) >= 0
			|| strPathUpper.Find(_T("\\REDIR.EXE")) >= 0
			|| strPathUpper.Find(_T("\\REG.EXE")) >= 0
			|| strPathUpper.Find(_T("\\REGEDIT.EXE")) >= 0
			|| strPathUpper.Find(_T("\\REGEDT32.EXE")) >= 0
			|| strPathUpper.Find(_T("\\ROUTE.EXE")) >= 0
			|| strPathUpper.Find(_T("\\RUNDLL32.EXE")) >= 0
			|| strPathUpper.Find(_T("\\SC.EXE")) >= 0
			|| strPathUpper.Find(_T("\\SCHTASKS.EXE")) >= 0
			|| strPathUpper.Find(_T("\\SDBINST.EXE")) >= 0
			|| strPathUpper.Find(_T("\\SUBST.EXE")) >= 0
			|| strPathUpper.Find(_T("\\SVCHOST.EXE")) >= 0
			|| strPathUpper.Find(_T("\\SYSPREP.EXE")) >= 0
			|| strPathUpper.Find(_T("\\SYSTEMINFO.EXE")) >= 0
			|| strPathUpper.Find(_T("\\TASKKILL.EXE")) >= 0
			|| strPathUpper.Find(_T("\\TASKLIST.EXE")) >= 0
			|| strPathUpper.Find(_T("\\TRACERT.EXE")) >= 0
			|| strPathUpper.Find(_T("\\VER.EXE")) >= 0
			|| strPathUpper.Find(_T("\\VSSADMIN.EXE")) >= 0
			|| strPathUpper.Find(_T("\\WHOAMI.EXE")) >= 0
			|| strPathUpper.Find(_T("\\WINRS.EXE")) >= 0
			|| strPathUpper.Find(_T("\\WMIC.EXE")) >= 0
			|| strPathUpper.Find(_T("\\WSCRIPT.EXE")) >= 0
			|| strPathUpper.Find(_T("\\WUSA.EXE")) >= 0
			|| strPathUpper.Find(_T("\\COMMAND.COM")) >= 0
			|| strPathUpper.Find(_T("\\EVENTVWR.EXE")) >= 0
			|| strPathUpper.Find(_T("\\PSEXEC.EXE")) >= 0
			|| strPathUpper.Find(_T("\\CMDKEY.EXE")) >= 0
			|| strPathUpper.Find(_T("\\MSIEXEC.EXE")) >= 0
			|| strPathUpper.Find(_T("\\TASKHOSTW.EXE")) >= 0
			|| strPathUpper.Find(_T("\\TASKHOST.EXE")) >= 0
			|| strPathUpper.Find(_T("\\BACKGROUNDTASKHOST.EXE")) >= 0
			|| strPathUpper.Find(_T("\\ATTRIB.EXE")) >= 0
			|| strPathUpper.Find(_T("\\ICACLS.EXE")) >= 0
			|| strPathUpper.Find(_T("\\ROBOCOPY.EXE")) >= 0
			|| strPathUpper.Find(_T("\\XCOPY.EXE")) >= 0
			|| strPathUpper.Find(_T("\\QWINSTA.EXE")) >= 0
			)
		{
			CString strMsg;
			strMsg.Format(_T("%s \n%s"), _T("ThinBridge 制限されたコマンドは実行できません"), lpURL);
			::MessageBox(NULL, strMsg, _T("ThinBridge"), MB_ICONERROR);
			return;
		}


		if (::ShellExecute(NULL, NULL, lpURL, NULL, NULL, SW_SHOW) <= HINSTANCE(32))
		{
			if (::ShellExecute(NULL, _T("open"), lpURL, NULL, NULL, SW_SHOW) <= HINSTANCE(32))
			{
				if (::ShellExecute(NULL, NULL, _T("explorer.exe"), lpURL, NULL, SW_SHOW) <= HINSTANCE(32))
				{
					::ShellExecute(NULL, _T("open"), _T("explorer.exe"), lpURL, NULL, SW_SHOW);
				}
			}
		}
		return;
	}
	CString logmsg;
	CString strURL = lpURL;
	CString strURL_FULL = strURL;
	strURL = SBUtil::Trim_URLOnly(strURL);

	//HitしたObjectコレクション格納
	CAtlArray<intptr_t> arr_RedirectBrowserHit;
	CURLRedirectDataClass* pRedirectData = NULL;

	BOOL bResultRedirectURL = FALSE;
	bResultRedirectURL = m_RedirectList.IsRedirect(strURL_FULL, TRUE, &arr_RedirectBrowserHit);
	//判定結果 リダイレクトする。
	if (bResultRedirectURL)
	{
		int iColMax = (int)arr_RedirectBrowserHit.GetCount();
		if (iColMax < 1)
			return;

		logmsg.Format(_T("#ThinBridge_Redirect(%d)\t%s"), iColMax, m_RedirectList.m_strHitReasonAll);
		this->WriteDebugTraceDateTime(logmsg);

		//始めにHitした物を使う。
		pRedirectData = (CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(0);
		if (pRedirectData)
		{
			for (int i = 0; i < iColMax; i++)
			{
				pRedirectData = (CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(i);
				if (pRedirectData)
				{
					this->OpenAnotherBrowser(pRedirectData, strURL_FULL);
					::Sleep(100);
				}
			}
			return;
		}
	}
	//HITしない
	else
	{
		//Defaultブラウザーへリダイレクトする。
		int imax = (int)m_RedirectList.m_arr_RedirectBrowser.GetCount();
		CURLRedirectDataClass* pRedirectData_Default = NULL;
		for (int i = 0; i < imax; i++)
		{
			pRedirectData = NULL;
			pRedirectData = (CURLRedirectDataClass*)m_RedirectList.m_arr_RedirectBrowser.GetAt(i);
			if (pRedirectData)
			{
				//Defaultサイト
				if (pRedirectData->m_strExecType == _T("Default"))
				{
					pRedirectData_Default = pRedirectData;
					break;
				}
			}
		}
		if (pRedirectData_Default)
		{
			//何かしら設定されている。
			if (!pRedirectData_Default->m_strExecExeFullPath.IsEmpty())
			{
				logmsg.Format(_T("#ThinBridge_Redirect Default[%s]"), pRedirectData_Default->m_strExecExeFullPath);
				this->WriteDebugTraceDateTime(logmsg);

				this->OpenAnotherBrowser(pRedirectData_Default, strURL_FULL);
				return;
			}
		}
		//何も設定されていない場合は、既定のブラウザーで開く。
		CString strDefBrowserName;
		CURLRedirectDataClass Rd;

		if (IsDefaultBrowser())//ThinBridgeを既定のブラウザーhttp/httpsにしている場合は、ループするので他にする。
		{
			strDefBrowserName = GetDefaultHTMLBrowser();
			if(strDefBrowserName.IsEmpty())
			{
				Rd.m_strExecType = _T("IE");
				this->OpenAnotherBrowser(&Rd, strURL_FULL);
				return;
			}
		}
		else
		{
			strDefBrowserName = GetDefaultBrowser();
		}
		strDefBrowserName = strDefBrowserName.MakeUpper();

		//MS-Edge
		if(strDefBrowserName.Find(_T("MSEDGE"))==0)
		{
			Rd.m_strExecType = _T("EDGE");
			this->OpenAnotherBrowser(&Rd, strURL_FULL);
		}
		//Chrome
		else if (strDefBrowserName.Find(_T("CHROME")) == 0)
		{
			Rd.m_strExecType = _T("CHROME");
			this->OpenAnotherBrowser(&Rd, strURL_FULL);
		}
		//Firefox
		else if (strDefBrowserName.Find(_T("FIREFOX")) == 0)
		{
			Rd.m_strExecType = _T("FIREFOX");
			this->OpenAnotherBrowser(&Rd, strURL_FULL);
		}
		//IE
		else if (strDefBrowserName.Find(_T("IE")) == 0)
		{
			Rd.m_strExecType = _T("IE");
			this->OpenAnotherBrowser(&Rd, strURL_FULL);
		}
		else
		{
			Rd.m_strExecType = _T("Default");
			this->OpenAnotherBrowser(&Rd, strURL_FULL);

		}
	}
}

void CTBRedirectorApp::OpenAnotherBrowser(CURLRedirectDataClass* pRedirectData, const CString& strURL)
{
	if (pRedirectData == NULL)return;

	if (SBUtil::IsURL_HTTP(strURL))
	{
		CString strExecTypeUpper;
		strExecTypeUpper = pRedirectData->m_strExecType;
		strExecTypeUpper.MakeUpper();
		CString strCommand;
		CString strCommand2;
		CString logmsg;

		if (strExecTypeUpper.CompareNoCase(_T("RDP")) == 0
			|| strExecTypeUpper.CompareNoCase(_T("VMWARE")) == 0
			|| strExecTypeUpper.CompareNoCase(_T("CITRIX")) == 0
			|| strExecTypeUpper.CompareNoCase(_T("FIREFOX")) == 0
			|| strExecTypeUpper.CompareNoCase(_T("CHROME")) == 0
			|| strExecTypeUpper.CompareNoCase(_T("EDGE")) == 0
			|| strExecTypeUpper.CompareNoCase(_T("IE")) == 0)
		{
			strCommand.Format(_T("\"%s\" \"%s\" \"%s\""), m_strThinBridge_EXE_FullPath, strURL, pRedirectData->m_strExecType);
			strCommand2.Format(_T("%s \"%s\""), strURL, pRedirectData->m_strExecType);
		}
		//Default
		else if (strExecTypeUpper.CompareNoCase(_T("Default")) == 0)
		{
			if (pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("RDP")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("VMWARE")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("CITRIX")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("FIREFOX")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("CHROME")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("IE")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("EDGE")) == 0)
			{
				strCommand.Format(_T("\"%s\" \"%s\" \"%s\""), m_strThinBridge_EXE_FullPath, strURL, pRedirectData->m_strExecExeFullPath);
				strCommand2.Format(_T("%s \"%s\""), strURL, pRedirectData->m_strExecExeFullPath);
			}
			else if(pRedirectData->m_strExecExeFullPath.IsEmpty())
			{
				strCommand.Format(_T("\"%s\" \"%s\" \"%s\""), m_strThinBridge_EXE_FullPath, strURL, pRedirectData->m_strExecType);
				strCommand2.Format(_T("%s \"%s\""), strURL, pRedirectData->m_strExecType);
			}
			else
			{
				strCommand.Format(_T("\"%s\" \"%s\" \"%s\""), m_strThinBridge_EXE_FullPath, strURL, pRedirectData->m_strExecExeFullPath);
				strCommand2.Format(_T("%s \"%s\""), strURL, pRedirectData->m_strExecExeFullPath);
			}
		}
		//Office365対応[CUSTOM20]
		else if (strExecTypeUpper.CompareNoCase(_T("CUSTOM20")) == 0)
		{
			if (pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("RDP")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("VMWARE")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("CITRIX")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("FIREFOX")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("CHROME")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("EDGE")) == 0)
			{
				strCommand.Format(_T("\"%s\" \"%s\" \"%s\""), m_strThinBridge_EXE_FullPath, strURL, pRedirectData->m_strExecExeFullPath);
				strCommand2.Format(_T("%s \"%s\""), strURL, pRedirectData->m_strExecExeFullPath);
			}
			else
			{
				strCommand.Format(_T("\"%s\" \"%s\" \"%s\""), m_strThinBridge_EXE_FullPath, strURL, pRedirectData->m_strExecExeFullPath);
				strCommand2.Format(_T("%s \"%s\""), strURL, pRedirectData->m_strExecExeFullPath);
			}
		}
		//Custom系の場合は、m_strExecExeFullPath
		else if (strExecTypeUpper.Find(_T("CUSTOM")) == 0)
		{
			strCommand.Format(_T("\"%s\" \"%s\" \"%s\""), m_strThinBridge_EXE_FullPath, strURL, pRedirectData->m_strExecExeFullPath);
			strCommand2.Format(_T("%s \"%s\""), strURL, pRedirectData->m_strExecExeFullPath);
		}
		else
		{
			strCommand.Format(_T("\"%s\" \"%s\" \"%s\""), m_strThinBridge_EXE_FullPath, strURL, pRedirectData->m_strExecType);
			strCommand2.Format(_T("%s \"%s\""), strURL, pRedirectData->m_strExecType);
		}


		logmsg.Format(_T("#ExecuteThinBridge\t%s"), strCommand);
		WriteDebugTraceDateTime(logmsg);

		STARTUPINFO si = { 0 };
		PROCESS_INFORMATION pi = { 0 };
		si.cb = sizeof(si);
		unsigned long ecode = 0;
		::Sleep(100);
		if (!CreateProcess(NULL, (LPTSTR)(LPCTSTR)strCommand, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
		{
			::Sleep(100);
			logmsg.Format(_T("ExecuteThinBridge:CreateProcess Retry1 0x%08x"), ::GetLastError());
			WriteDebugTraceDateTime(logmsg);
			SetLastError(NO_ERROR);
			//Retry
			if (!CreateProcess(m_strThinBridge_EXE_FullPath, (LPTSTR)(LPCTSTR)strCommand2, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
			{
				::Sleep(100);
				logmsg.Format(_T("ExecuteThinBridge:ShellExecute Retry2 0x%08x"), ::GetLastError());
				WriteDebugTraceDateTime(logmsg);
				SetLastError(NO_ERROR);
				if (::ShellExecute(NULL, _T("open"), m_strThinBridge_EXE_FullPath, strCommand2, NULL, SW_SHOW) <= HINSTANCE(32))
				{
					::Sleep(100);
					logmsg.Format(_T("ExecuteThinBridge:ShellExecute Retry3 0x%08x"), ::GetLastError());
					WriteDebugTraceDateTime(logmsg);
					SetLastError(NO_ERROR);
					::ShellExecute(NULL, NULL, strCommand, NULL, NULL, SW_SHOW);
				}
			}
		}
		if (pi.hThread)
		{
			CloseHandle(pi.hThread);
			pi.hThread = 0;
		}
		if (pi.hProcess)
		{
			CloseHandle(pi.hProcess);
			pi.hProcess = 0;
		}
	}
}



void CTBRedirectorApp::WriteDebugTraceDateTime(LPCTSTR msg)
{
	if (msg == NULL)return;
	if (_tcslen(msg) == 0)return;
	_wsetlocale(LC_ALL, _T("jpn"));
	CString strWriteLine;
	CTime time = CTime::GetCurrentTime();
	strWriteLine.Format(_T("ThinBridge:RedirectorEXE\t%s\t%s\n"), time.Format(_T("%Y-%m-%d %H:%M:%S")), msg);
	if (strWriteLine.GetLength() > 4000)
	{
		strWriteLine = strWriteLine.Mid(0, 4000);
		strWriteLine += _T("..DIV..");
	}
	OutputDebugString(strWriteLine);
}

BOOL CTBRedirectorApp::IsDefaultBrowser()
{
	BOOL bRet = FALSE;
	HKEY  hKey = { 0 };
	DWORD dwDisposition = 0;
	LONG lResult = 0L;
	TCHAR szProcName[] = _T("ThinBridgeURL");
	CString strVal;
	CString strVal2;

	DWORD dwType = 0;

	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\http\\UserChoice"),
		0, KEY_READ, &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		DWORD iSize = 0;
		TCHAR* pVal = NULL;

		RegQueryValueEx(hKey, _T("Progid"), NULL, &dwType, NULL, &iSize);
		if (iSize > 0)
		{
			iSize += 1;//+1=null
			pVal = new TCHAR[iSize];
			memset(pVal, 0x00, sizeof(TCHAR) * iSize);
			RegQueryValueEx(hKey, _T("Progid"), NULL, &dwType, (LPBYTE)pVal, &iSize);
			strVal = pVal;
			delete[] pVal;
		}
		RegCloseKey(hKey);
	}

	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\https\\UserChoice"),
		0, KEY_READ, &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		DWORD iSize = 0;
		TCHAR* pVal = NULL;

		RegQueryValueEx(hKey, _T("Progid"), NULL, &dwType, NULL, &iSize);
		if (iSize > 0)
		{
			iSize += 1;//+1=null
			pVal = new TCHAR[iSize];
			memset(pVal, 0x00, sizeof(TCHAR) * iSize);
			RegQueryValueEx(hKey, _T("Progid"), NULL, &dwType, (LPBYTE)pVal, &iSize);
			strVal2 = pVal;
			delete[] pVal;
		}
		RegCloseKey(hKey);
	}

	//既にThinBridgeがセットされている。
	if (strVal == szProcName && strVal2 == szProcName)
		bRet = TRUE;

	return bRet;
}

CString CTBRedirectorApp::GetDefaultBrowser()
{
	BOOL bRet = FALSE;
	HKEY  hKey = { 0 };
	DWORD dwDisposition = 0;
	LONG lResult = 0L;
	CString strVal;
	DWORD dwType = 0;

	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\http\\UserChoice"),
		0, KEY_READ, &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		DWORD iSize = 0;
		TCHAR* pVal = NULL;

		RegQueryValueEx(hKey, _T("Progid"), NULL, &dwType, NULL, &iSize);
		if (iSize > 0)
		{
			iSize += 1;//+1=null
			pVal = new TCHAR[iSize];
			memset(pVal, 0x00, sizeof(TCHAR) * iSize);
			RegQueryValueEx(hKey, _T("Progid"), NULL, &dwType, (LPBYTE)pVal, &iSize);
			strVal = pVal;
			delete[] pVal;
		}
		RegCloseKey(hKey);
	}
	return strVal;
}

//HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.htm\UserChoice
CString CTBRedirectorApp::GetDefaultHTMLBrowser()
{
	BOOL bRet = FALSE;
	HKEY  hKey = { 0 };
	DWORD dwDisposition = 0;
	LONG lResult = 0L;
	CString strVal;
	DWORD dwType = 0;

	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.htm\\UserChoice"),
		0, KEY_READ, &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		DWORD iSize = 0;
		TCHAR* pVal = NULL;

		RegQueryValueEx(hKey, _T("Progid"), NULL, &dwType, NULL, &iSize);
		if (iSize > 0)
		{
			iSize += 1;//+1=null
			pVal = new TCHAR[iSize];
			memset(pVal, 0x00, sizeof(TCHAR) * iSize);
			RegQueryValueEx(hKey, _T("Progid"), NULL, &dwType, (LPBYTE)pVal, &iSize);
			strVal = pVal;
			delete[] pVal;
		}
		RegCloseKey(hKey);
	}
	return strVal;
}

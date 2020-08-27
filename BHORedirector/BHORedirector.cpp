// BHORedirector.cpp : DLL エクスポートの実装です。


#include "stdafx.h"
#include "resource.h"
#include "BHORedirector_i.h"
#include "dllmain.h"
#include "BHORedirector.h"
#include "shlobj.h"
#include "shlwapi.h"
#pragma comment(lib, "ShLwApi.Lib")

// DLL を OLE によってアンロードできるようにするかどうかを指定します。
STDAPI DllCanUnloadNow(void){
	return _AtlModule.DllCanUnloadNow();
}
// 要求された型のオブジェクトを作成するクラス ファクトリを返します。
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv){
	return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}
// DllRegisterServer - エントリをシステム レジストリに追加します。
STDAPI DllRegisterServer(void){
	// オブジェクト、タイプ ライブラリおよびタイプ ライブラリ内のすべてのインターフェイスを登録します
	HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}

// DllUnregisterServer - エントリをレジストリから削除します。
STDAPI DllUnregisterServer(void){
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

// DllInstall - ユーザーおよびコンピューターごとのシステム レジストリ エントリを追加または削除します。
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine){
	HRESULT hr = E_FAIL;
	static const wchar_t szUserSwitch[] = L"user";
	if (pszCmdLine != NULL)	{
		if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0){
			ATL::AtlSetPerUserRegistration(true);
		}
	}
	if (bInstall){	
		hr = DllRegisterServer();
		if (FAILED(hr)){
			DllUnregisterServer();
		}
	}
	else{
		hr = DllUnregisterServer();
	}
	return hr;
}

// BHORedirector.cpp : CBHORedirector の実装
// CBHORedirector

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CBHORedirector::SetSite(IUnknown* pUnkSite)
{
	CString logmsg;
    if (pUnkSite != NULL)
    {
		m_IE_M_Ver=SBUtil::GetTridentVersion();

		logmsg.Format(_T("SetSite [0x%08x] Start"),pUnkSite);
		this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_GE,TRUE);

		//m_strUserDataPath
		TCHAR lpszPath[MAX_PATH*2] = _T("");
		LPITEMIDLIST pidl={0};
		if(::SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &pidl) == NOERROR)
		{
			::SHGetPathFromIDList(pidl, lpszPath);
			::CoTaskMemFree(pidl);
		}
		m_strUserDataPath=lpszPath;
		m_strUserDataPath+=_T("\\ThinBridge\\");

		//ThinApp環境
		if(SBUtil::InThinApp())// && m_IE_M_Ver==8)
		{
			m_bInVOS=TRUE;
			m_strThinBridge_EXE_FullPath=_AtlModule.m_gSZFullPath;
			m_strThinBridge_EXE_FullPath += _T("ExternalP.exe");
			m_strRedirect_Script_FilePath=_AtlModule.m_gSZFullPath;
			m_strRedirect_Script_FilePath+=_T("ThinBridgeBHO_VOS.tbs");
		}
		else
		{
			m_bInVOS=FALSE;
			m_strThinBridge_EXE_FullPath=_AtlModule.m_gSZFullPath;
			m_strThinBridge_EXE_FullPath += _T("ThinBridge.exe");

			m_strRedirect_Script_FilePath=_AtlModule.m_gSZFullPath;
			m_strRedirect_Script_FilePath+=_T("ThinBridgeBHO.tbs");

			CString strUserScriptFile;
			strUserScriptFile = m_strUserDataPath;
			strUserScriptFile += _T("ThinBridgeBHO.tbs");
			//ユーザー設定を優先させる。
			if(PathFileExists(strUserScriptFile))
			{
				m_strRedirect_Script_FilePath=strUserScriptFile;
			}
			logmsg.Format(_T("ThinBridgeBHO.tbs [%s]"),m_strRedirect_Script_FilePath);
			this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_EX,TRUE);
		}

		m_strRedirectFilePath=_AtlModule.m_gSZFullPath;
		m_strRedirectFilePath+=_T("ThinBridgeBHO.ini");

		m_strResourceCAPFullPath=_AtlModule.m_gSZFullPath;
		m_strResourceCAPFullPath+=_T("ResourceCap.ini");

		CString strUserResourceCapFile;
		strUserResourceCapFile = m_strUserDataPath;
		strUserResourceCapFile += _T("ResourceCap.ini");
		//ユーザー設定を優先させる。
		if(PathFileExists(strUserResourceCapFile))
		{
			m_strResourceCAPFullPath=strUserResourceCapFile;
		}
		logmsg.Format(_T("ResourceCap.ini [%s]"),m_strResourceCAPFullPath);
		this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_EX,TRUE);
		//ResouceCAPファイル内容の読込
		m_ResourceCAPConf.ReadResourceCAPSetting(m_strResourceCAPFullPath);

		CString strUserThinBridgeBHOINIFile;
		strUserThinBridgeBHOINIFile = m_strUserDataPath;
		strUserThinBridgeBHOINIFile += _T("ThinBridgeBHO.ini");
		//ユーザー設定を優先させる。
		if(PathFileExists(strUserThinBridgeBHOINIFile))
		{
			m_strRedirectFilePath=strUserThinBridgeBHOINIFile;
		}
		logmsg.Format(_T("ThinBridgeBHO.ini [%s]"),m_strRedirectFilePath);
		this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_EX,TRUE);

		logmsg=_T("SetSite Stage:01");
		this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_GE),TRUE;

		if(m_ResourceCAPConf.m_bCPUPriorityCAP)
		{
			//0 _T("高"),
			//1 _T("通常以上"),
			//2 _T("通常"),
			//3_T("通常以下"),
			//4_T("低"),
			if(m_ResourceCAPConf.m_uiCPUPriority==0)
			{
				::SetPriorityClass(::GetCurrentProcess(),HIGH_PRIORITY_CLASS);
				::SetThreadPriority(::GetCurrentThread(),THREAD_PRIORITY_HIGHEST);
			}
			else if(m_ResourceCAPConf.m_uiCPUPriority==1)
			{
				::SetPriorityClass(::GetCurrentProcess(),ABOVE_NORMAL_PRIORITY_CLASS);
				::SetThreadPriority(::GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);
			}
			else if(m_ResourceCAPConf.m_uiCPUPriority==2)
			{
				::SetPriorityClass(::GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
				::SetThreadPriority(::GetCurrentThread(),THREAD_PRIORITY_NORMAL);
			}
			else if(m_ResourceCAPConf.m_uiCPUPriority==3)
			{
				::SetPriorityClass(::GetCurrentProcess(),BELOW_NORMAL_PRIORITY_CLASS);
				::SetThreadPriority(::GetCurrentThread(),THREAD_PRIORITY_BELOW_NORMAL);
			}
			else if(m_ResourceCAPConf.m_uiCPUPriority==4)
			{
				::SetPriorityClass(::GetCurrentProcess(),IDLE_PRIORITY_CLASS);
				::SetThreadPriority(::GetCurrentThread(),THREAD_PRIORITY_IDLE);
			}
		}

		m_IEStatus.m_strProcessName=_AtlModule.m_gSZParentProcessName;
		logmsg=_T("SetSite Stage:02");
		this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_GE,TRUE);

		//Delay load
		m_RedirectList.SetArrayDataDelayLoad(m_strRedirectFilePath);
		//m_RedirectList.SetArrayData(m_strRedirectFilePath);
		m_RedirectList.m_ScMgr.SetDataDelayLoad(m_strRedirect_Script_FilePath);
		logmsg=_T("SetSite Stage:03");
		this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_GE,TRUE);

		try
		{
			// IWebBrowser2 へのポインタをキャッシュします。
			HRESULT hr = pUnkSite->QueryInterface(IID_IWebBrowser2, (void **)&m_spWebBrowser);
			if (SUCCEEDED(hr)&&m_spWebBrowser)
			{
				logmsg=_T("SetSite Stage:04");
				this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_GE,TRUE);

				// DWebBrowserEvents2 からのイベントをシンクに登録します。
				hr = DispEventAdvise(m_spWebBrowser);
				if (SUCCEEDED(hr))
				{
					m_fAdvised = TRUE;
				}
			}
			m_hEventLog = CreateEvent(NULL, FALSE, TRUE, _T("TB_EventCAPChkg"));
			logmsg=_T("SetSite Stage:05 End");
			this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_GE,TRUE);
		}
		catch (...)
		{
			;
		}
    }
	else
    {
		try
		{
			logmsg=_T("UnSetSite");
			this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_GE,TRUE);

			if(m_spWebBrowser)
			{
				logmsg.Format(_T("UnSetSite_WebBrowser[0x%08x]"),m_spWebBrowser);
				this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_GE,TRUE);

				// イベント シンクの登録を抹消します。
				if (m_fAdvised)
				{
					DispEventUnadvise(m_spWebBrowser);
					m_fAdvised = FALSE;
				}
				// ここで、キャッシュしたポインタなどのリソースを解放します。
				m_spWebBrowser.Release();
				m_spWebBrowser=NULL;
			}
			//Global_WindowTabCnt_Decrement((DWORD)m_hWnd);
			if(m_hEventLog)
			{
				CloseHandle(m_hEventLog);
				m_hEventLog=NULL;
			}
		}
		catch (...)
		{
			;
		}
    }
    // 基本クラスの実装を呼び出します。
    return IObjectWithSiteImpl<CBHORedirector>::SetSite(pUnkSite);
}
bool CBHORedirector::IsPageIWebBrowser(IDispatch* pDisp)
{//Knowledge Base Q180366
	if(m_spWebBrowser == NULL)return false;

	CComPtr<IDispatch> spDispatch;
	//IDispatchPtr spDispatch;
	HRESULT hr = m_spWebBrowser->QueryInterface(IID_IDispatch, (void**)&spDispatch);
	if (FAILED(hr))
		return false;
	if (pDisp == spDispatch)// Top-level Window object
		return true;
	else
		return false;
}

void STDMETHODCALLTYPE CBHORedirector::BeforeNavigate(LPDISPATCH pDisp, VARIANT* URL,
		VARIANT* Flags, VARIANT* TargetFrameName,
		VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	CString logmsg;
	CString strURL = V_BSTR(URL);
	CString strURL_FULL = strURL;
	CString strTargetFrameName = V_BSTR(TargetFrameName);

	if(!m_spWebBrowser)
		return;

	if(!SBUtil::IsURL_HTTP(strURL))
		return;

	//HWND hwnd={0};
	//HRESULT hr = m_spWebBrowser->get_HWND((LONG_PTR*)&hwnd);
	//logmsg.Format(_T("CV_WND:0x%08x BeforeNavigate:%s"),hwnd,strURL);
	//this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_URL);
	//hwnd=NULL;

	CString strMsg;
	DWORD dRet=0;

	BOOL bTopPage=FALSE;

	//TOPページ(Frameなし)
	if(IsPageIWebBrowser(pDisp))
	{
		bTopPage=TRUE;
		m_strTopURL=strURL;
	}

	//ESCが押されている場合は、リダイレクトをしない。
	if(::GetKeyState(VK_ESCAPE)<0)
		return;

	if(strTargetFrameName.IsEmpty())
		strTargetFrameName=_T("N/A");
	else
	{
		//google adでものすごく長い(js)を入れてくる作法の悪いものがあるので、最大100文字で切る。
		if(strTargetFrameName.GetLength()>100)
		{
			strTargetFrameName = strTargetFrameName.Mid(0,100);
			strTargetFrameName+=_T("..DIV..");
		}
	}

	logmsg.Format(_T("BeforeNavigate\t%s\tTopPage=%s\tFrameName=%s"),strURL,bTopPage?_T("Yes"):_T("Frame/iFrame"),strTargetFrameName);
	this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_URL);

	strURL = SBUtil::Trim_URLOnly(strURL);

	DWORD dwZone=ZONE_NA;
	dwZone = GetURLZone(strURL);

	//HitしたObjectコレクション格納
	CAtlArray<intptr_t> arr_RedirectBrowserHit;
	CURLRedirectDataClass* pRedirectData=NULL;

	BOOL bResultRedirectURL=FALSE;
	//VOSの中では無い。物理環境
	if(!m_bInVOS)
	{
		bResultRedirectURL = m_RedirectList.IsRedirect(strURL_FULL,bTopPage,dwZone,&arr_RedirectBrowserHit,TRUE);
	}
	else
	{
		bResultRedirectURL = m_RedirectList.IsRedirectInVOS(strURL_FULL,bTopPage,dwZone,&arr_RedirectBrowserHit,TRUE);
	}

	//判定結果 リダイレクトする。
	if(bResultRedirectURL)
	{
		int iColMax=(int)arr_RedirectBrowserHit.GetCount();
		if(iColMax < 1)
			return;

		//ループ防止
		if(!m_strSafeGuardURL.IsEmpty() && m_strSafeGuardURL==m_strTopURL)
		{
			logmsg.Format(_T("LoopBlocker\t%s"),m_strSafeGuardURL);
			this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_URL);
			return;
		}

		if(!IsSafeGuard(strURL_FULL))
		{
			BOOL bJP=TRUE;
			try
			{
				TCHAR szLocalNameBuf[LOCALE_NAME_MAX_LENGTH*2]={0};
				::GetUserDefaultLocaleName(szLocalNameBuf,LOCALE_NAME_MAX_LENGTH);
				TCHAR szNameBuf[3]={0};
				lstrcpyn(szNameBuf,szLocalNameBuf,3);
				CString strLang;
				strLang=szNameBuf;
				strLang.MakeLower();
				if(strLang.Find(_T("ja"))==0)
					bJP=TRUE;
				else
					bJP=FALSE;
			}
			catch (...)
			{
				;
			}
			m_strSafeGuardURL=m_strTopURL;
			CString strSafeGuardMsg;
			strSafeGuardMsg.Format(_T("%s\n%s"),_T("※警告※\n短時間に連続したブラウザー リダイレクトを検出しました。\n\nブラウザー リダイレクト処理を一時的に中断しますか？"),m_strSafeGuardURL);
			if(bJP==FALSE)
				strSafeGuardMsg.Format(_T("%s\n%s"),_T("*WARNING*\nThinBridge detected successive browser redirects in a short time.\n\nWould you like to temporarily suspend browser redirect processing?"),m_strSafeGuardURL);

			if(IDYES!=::MessageBox(NULL,strSafeGuardMsg,_T("ThinBridge Browser Helper"),MB_YESNO|MB_ICONQUESTION))
			{
				m_strSafeGuardURL.Empty();
				m_strSafeGuard_chkURL.Empty();
			}
			return;
		}
		else
		{
			m_strSafeGuardURL.Empty();
		}

		logmsg.Format(_T("#ThinBridge_Redirect(%d)\t%s"),iColMax,m_RedirectList.m_strHitReasonAll);
		this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_DE);

		//TOPページ(Frameなし)
		if(bTopPage)
		{
			//始めにHitした物を使う。
			pRedirectData=(CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(0);
			if(pRedirectData)
			{
				*Cancel = TRUE;
				this->Navigate_RedirectHTM(m_spWebBrowser,
						pRedirectData->m_strExecType,pRedirectData->m_dRedirectPageAction,
						pRedirectData->m_dwCloseTimeout);
				for(int i=0;i<iColMax;i++)
				{
					pRedirectData=(CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(i);
					if(pRedirectData)
					{
						this->OpenAnotherBrowser(pRedirectData,strURL_FULL);
						::Sleep(100);
					}
				}
				return;
			}
		}
		else
		{
			//Frameの場合、小フレーム,iFrame含む
			if(pDisp)
			{
				CComPtr<IWebBrowser2>  spWebBrowserFrame;
				HRESULT hr = pDisp->QueryInterface(IID_IWebBrowser2, (void **)&spWebBrowserFrame);
				if (SUCCEEDED(hr))
				{
					if(spWebBrowserFrame)
					{
						*Cancel = TRUE;
						//始めにHitした物を使う。
						pRedirectData=(CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(0);
						if(pRedirectData)
						{
							*Cancel = TRUE;
							this->Navigate_RedirectHTM(spWebBrowserFrame,
									pRedirectData->m_strExecType,pRedirectData->m_dRedirectPageAction,
									pRedirectData->m_dwCloseTimeout);
							for(int i=0;i<iColMax;i++)
							{
								pRedirectData=(CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(i);
								if(pRedirectData)
								{
									this->OpenAnotherBrowser(pRedirectData,strURL_FULL);
									::Sleep(100);
								}
							}
							return;
						}
						return;
					}
				}
				return;
			}
		}
	}
	return;
}

void STDMETHODCALLTYPE CBHORedirector::NavigateComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	CString logmsg;
	CString strURL = V_BSTR(URL);
	CString strURL_FULL = strURL;

	if(!m_spWebBrowser)
		return;

	if(!SBUtil::IsURL_HTTP(strURL))
		return;

//	HWND hwnd={0};
//	HRESULT hr = m_spWebBrowser->get_HWND((LONG_PTR*)&hwnd);
//	logmsg.Format(_T("CV_WND:0x%08x NavigateComplete:%s"),hwnd,strURL);
//	this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_URL);
//	hwnd=NULL;
	logmsg.Format(_T("NavigateComplete\t%s"),strURL);
	this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_URL);

	CString strMsg;
	DWORD dRet=0;

	CString strExecPath;
	BOOL bTopPage=FALSE;

	//TOPページ(Frameなし)
	if(IsPageIWebBrowser(pDisp))
	{
		bTopPage=TRUE;
		m_strTopURL=strURL;
	}

	//ESCが押されている場合は、リダイレクトをしない。
	if(::GetKeyState(VK_ESCAPE)<0)
		return;

	strURL = SBUtil::Trim_URLOnly(strURL);

	DWORD dwZone=ZONE_NA;
	dwZone = GetURLZone(strURL);

	//HitしたObjectコレクション格納
	CAtlArray<intptr_t> arr_RedirectBrowserHit;
	CURLRedirectDataClass* pRedirectData=NULL;

	BOOL bResultRedirectURL=FALSE;
	//VOSの中では無い。物理環境
	if(!m_bInVOS)
	{
		bResultRedirectURL = m_RedirectList.IsRedirect(strURL_FULL,bTopPage,dwZone,&arr_RedirectBrowserHit,FALSE);
	}
	else
	{
		bResultRedirectURL = m_RedirectList.IsRedirectInVOS(strURL_FULL,bTopPage,dwZone,&arr_RedirectBrowserHit,FALSE);
	}

	//判定結果 リダイレクトする。
	if(bResultRedirectURL)
	{
		int iColMax=(int)arr_RedirectBrowserHit.GetCount();
		if(iColMax < 1)
			return;

		//ループ防止
		if(!m_strSafeGuardURL.IsEmpty() && m_strSafeGuardURL==m_strTopURL)
		{
			logmsg.Format(_T("LoopBlocker\t%s"),m_strSafeGuardURL);
			this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_URL);
			return;
		}
		if(!IsSafeGuard(strURL_FULL))
		{
			m_strSafeGuardURL=m_strTopURL;
			CString strSafeGuardMsg;
			strSafeGuardMsg.Format(_T("%s\n%s"),_T("※警告※\n短時間に連続したブラウザー リダイレクトを検出しました。\n\nブラウザー リダイレクト処理を一時的に中断しますか？"),m_strSafeGuardURL);
			if(IDYES!=::MessageBox(NULL,strSafeGuardMsg,_T("ThinBridge Browser Helper"),MB_YESNO|MB_ICONQUESTION))
			{
				m_strSafeGuardURL.Empty();
				m_strSafeGuard_chkURL.Empty();
			}
			return;
		}
		else
		{
			m_strSafeGuardURL.Empty();
		}

		logmsg.Format(_T("#ThinBridge_Redirect(%d)\t%s"),iColMax,m_RedirectList.m_strHitReasonAll);
		this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_DE);

		//TOPページ(Frameなし)
		if(bTopPage)
		{
			//始めにHitした物を使う。
			pRedirectData=(CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(0);
			if(pRedirectData)
			{
				this->Navigate_RedirectHTM(m_spWebBrowser,
						pRedirectData->m_strExecType,pRedirectData->m_dRedirectPageAction,
						pRedirectData->m_dwCloseTimeout);
				for(int i=0;i<iColMax;i++)
				{
					pRedirectData=(CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(i);
					if(pRedirectData)
					{
						this->OpenAnotherBrowser(pRedirectData,strURL_FULL);
						::Sleep(100);
					}
				}
				return;
			}
		}
		else
		{
			//Frameの場合、小フレーム,iFrame含む
			if(pDisp)
			{
				CComPtr<IWebBrowser2>  spWebBrowserFrame;
				HRESULT hr = pDisp->QueryInterface(IID_IWebBrowser2, (void **)&spWebBrowserFrame);
				if (SUCCEEDED(hr))
				{
					if(spWebBrowserFrame)
					{
						//始めにHitした物を使う。
						pRedirectData=(CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(0);
						if(pRedirectData)
						{
							this->Navigate_RedirectHTM(spWebBrowserFrame,
									pRedirectData->m_strExecType,pRedirectData->m_dRedirectPageAction,
									pRedirectData->m_dwCloseTimeout);
							for(int i=0;i<iColMax;i++)
							{
								pRedirectData=(CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(i);
								if(pRedirectData)
								{
									this->OpenAnotherBrowser(pRedirectData,strURL_FULL);
									::Sleep(100);
								}
							}
							return;
						}
						return;
					}
				}
				return;
			}
		}
	}
	if(bTopPage)
	{
		//Threadに切り離す。
		StartResourceCAPChk();
	}
	return;
}


DWORD WINAPI Thread1(LPVOID ptr){
	HRESULT hr={0};
	__try
	{
		while(TRUE){
			Sleep(200);
			hr = ((CBHORedirector*)ptr)->ResourceCAPChk();
			break;
		}
		ExitThread(TRUE);
	}
	__except( EXCEPTION_EXECUTE_HANDLER)
	{
		return 0;
	}
	return 0;
}

STDMETHODIMP CBHORedirector::StartResourceCAPChk(void)
{
	__try
	{
		if(m_ResourceCAPConf.IsEnableCAP())
			CreateThread(NULL, 0,Thread1,(LPVOID)this, 0, NULL);//Thread1を開始
	}
	__except( EXCEPTION_EXECUTE_HANDLER)
	{
		return S_OK;
	}
	return S_OK;
}

STDMETHODIMP CBHORedirector::ResourceCAPChk(void)
{
	HANDLE hEvent={0};
	hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE,_T("TB_EventCAPChkg"));
	DWORD waitRes=WaitForSingleObject(hEvent,300);
	if(waitRes == WAIT_TIMEOUT)
	{
		;
	}
	else
	{
		CString logmsg;
		CString strCaption;
		strCaption.Format(_T("ThinBridge %s"),m_ResourceCAPConf.m_strReadSettingType);
		//それぞれ警告メッセージは1回しか表示しない。
		for(;;)
		{
			//Tabチェック
			if(m_ResourceCAPConf.m_bTabCntCAP)
			{
				DWORD dwTabCnt=0;
				dwTabCnt = m_IEStatus.GetTabCnt();
				//Tab Limit
				if(dwTabCnt >= m_ResourceCAPConf.m_uiTabCntMAX)
				{
					if(!IsClosedTabWindow())
					{
						//logmsg.Format(_T("現在開いているタブ数(%d)が上限値(%d)に達しています。\n他の不要なタブを閉じてから再度実行して下さい。\n※空白(about:blank)ページを除く"),dwTabCnt,m_ResourceCAPConf.m_uiTabCntMAX);
						logmsg=m_ResourceCAPConf.m_strTab_MAX_Msg;
						this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_EX);
						if(m_ResourceCAPConf.m_uiTab_MAX_ShowTime>0)
							SBUtil::ShowTimeoutMessageBox(logmsg,strCaption,MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL,m_ResourceCAPConf.m_uiTab_MAX_ShowTime*1000);
						CloseTabWindow();
						break;
					}
				}
				else if(dwTabCnt >= m_ResourceCAPConf.m_uiTabCntWARM)
				{
					if(!IsClosedTabWindow())
					{
						//logmsg.Format(_T("警告：現在開いているタブ数(%d)が警告値(%d)に達しています。\n他の不要なタブを閉じて下さい。\n※空白(about:blank)ページを除く"),dwTabCnt,m_ResourceCAPConf.m_uiTabCntWARM);
						logmsg=m_ResourceCAPConf.m_strTab_WARM_Msg;
						this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_EX);
						if(m_ResourceCAPConf.m_uiTab_WARM_ShowTime>0)
							SBUtil::ShowTimeoutMessageBox(logmsg,strCaption,MB_OK|MB_ICONINFORMATION|MB_SYSTEMMODAL,m_ResourceCAPConf.m_uiTab_WARM_ShowTime*1000);
						//break;警告は、続行
					}
				}
			}
			//mem チェック
			if(m_ResourceCAPConf.m_bMemUsageCAP)
			{
				size_t iMem=0;
				iMem = m_IEStatus.GetMemoryUsageSize();
				TCHAR memorysize[24] = _T("");
				TCHAR WarmMemorysize[24] = _T("");
				TCHAR LimitMemorysize[24] = _T("");
				::StrFormatByteSize(iMem, memorysize, 24);
				::StrFormatByteSize(m_ResourceCAPConf.m_uiMemWARM*1024*1024,WarmMemorysize,24);
				::StrFormatByteSize(m_ResourceCAPConf.m_uiMemMAX*1024*1024,LimitMemorysize,24);

				//Mem Limit
				if(iMem >= m_ResourceCAPConf.m_uiMemMAX*1024*1024)
				{
					if(!IsClosedTabWindow())
					{
						//logmsg.Format(_T("現在の使用メモリ(%s)が上限値(%s)に達しています。\n他の不要なタブを閉じてから再度実行して下さい。"),memorysize,LimitMemorysize);
						logmsg=m_ResourceCAPConf.m_strMem_MAX_Msg;
						this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_EX);
						if(m_ResourceCAPConf.m_uiMem_MAX_ShowTime>0)
							SBUtil::ShowTimeoutMessageBox(logmsg,strCaption,MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL,m_ResourceCAPConf.m_uiMem_MAX_ShowTime*1000);
						CloseTabWindow();
						break;
					}
				}
				else if(iMem >= m_ResourceCAPConf.m_uiMemWARM*1024*1024)
				{
					if(!IsClosedTabWindow())
					{
						//logmsg.Format(_T("警告：現在の使用メモリ(%s)が警告値(%s)に達しています。\n他の不要なタブを閉じてください。"),memorysize,WarmMemorysize);
						logmsg=m_ResourceCAPConf.m_strMem_WARM_Msg;
						this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_EX);
						if(m_ResourceCAPConf.m_uiMem_WARM_ShowTime>0)
							SBUtil::ShowTimeoutMessageBox(logmsg,strCaption,MB_OK|MB_ICONINFORMATION|MB_SYSTEMMODAL,m_ResourceCAPConf.m_uiMem_WARM_ShowTime*1000);
						//break;警告は、続行
					}
				}
			}
			break;
		}
		SetEvent(hEvent);
		CloseHandle(hEvent);
		return S_OK;
	}
	CloseHandle(hEvent);
	return S_FALSE;
}


//void STDMETHODCALLTYPE CBHORedirector::TitleChange(BSTR bstrText)
//{
//	return;
//	if(!m_spWebBrowser)
//		return;
//
//	CComPtr<IDispatch> spDispatch;
//	HRESULT hr = m_spWebBrowser->QueryInterface(IID_IDispatch, (void**)&spDispatch);
//	if (FAILED(hr))
//		return;
//
//	CString strMsg;
//	CString logmsg;
//	DWORD dRet=0;
//	//tab
//	if(m_ResourceCAPConf.m_bTabCntCAP)
//	{
//		DWORD dwTabCnt=0;
//		dwTabCnt = m_IEStatus.GetTabCnt();
//		//Tab Limit
//		if(dwTabCnt >= m_ResourceCAPConf.m_uiTabCntMAX)
//		{
//			logmsg.Format(_T("現在開いているタブ数(%d)が上限値(%d)に達しています。\n他の不要なタブを閉じてから再度実行して下さい。\n※空白(about:blank)ページを除く"),dwTabCnt,m_ResourceCAPConf.m_uiTabCntMAX);
//			this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_EX);
//			//::MessageBox(NULL,logmsg,_T("ThinBridge"),MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
//			SBUtil::ShowTimeoutMessageBox(logmsg,_T("ThinBridge"),MB_OK|MB_ICONERROR|MB_SYSTEMMODAL,2000);
//			CloseTabWindow(spDispatch);
//			return;
//		}
//		//else if(dwTabCnt >= m_ResourceCAPConf.m_uiTabCntWARM)
//		//{
//		//	logmsg.Format(_T("警告：現在開いているタブ数(%d)が警告値(%d)に達しています。\n他の不要なタブを閉じて下さい。\n※空白(about:blank)ページを除く"),dwTabCnt,m_ResourceCAPConf.m_uiTabCntWARM);
//		//	this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_EX);
//		//	//::MessageBox(NULL,logmsg,_T("ThinBridge"),MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL);
//		//	SBUtil::ShowTimeoutMessageBox(logmsg,_T("ThinBridge"),MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL,2000);
//		//}
//	}
//
//	if(m_ResourceCAPConf.m_bMemUsageCAP)
//	{
//		UINT iMem=0;
//		iMem = m_IEStatus.GetMemoryUsageSize();
//		TCHAR memorysize[24] = _T("");
//		TCHAR WarmMemorysize[24] = _T("");
//		TCHAR LimitMemorysize[24] = _T("");
//		::StrFormatByteSize(iMem, memorysize, 24);
//		::StrFormatByteSize(m_ResourceCAPConf.m_uiMemWARM*1024*1024,WarmMemorysize,24);
//		::StrFormatByteSize(m_ResourceCAPConf.m_uiMemMAX*1024*1024,LimitMemorysize,24);
//
//		//Mem Limit
//		if(iMem >= m_ResourceCAPConf.m_uiMemMAX*1024*1024)
//		{
//			logmsg.Format(_T("現在の使用メモリ(%s)が上限値(%s)に達しています。\n他の不要なタブを閉じてから再度実行して下さい。"),memorysize,LimitMemorysize);
//			this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_EX);
//			//::MessageBox(NULL,logmsg,_T("ThinBridge"),MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
//			SBUtil::ShowTimeoutMessageBox(logmsg,_T("ThinBridge"),MB_OK|MB_ICONERROR|MB_SYSTEMMODAL,2000);
//			CloseTabWindow(spDispatch);
//			return;
//		}
//		//else if(iMem >= m_ResourceCAPConf.m_uiMemWARM*1024*1024)
//		//{
//		//	logmsg.Format(_T("警告：現在の使用メモリ(%s)が警告値(%s)に達しています。\n他の不要なタブを閉じてください。"),memorysize,WarmMemorysize);
//		//	this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_EX);
//		//	//::MessageBox(NULL,logmsg,_T("ThinBridge"),MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL);
//		//	SBUtil::ShowTimeoutMessageBox(logmsg,_T("ThinBridge"),MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL,2000);
//		//}
//	}
//	return;
//}
//void STDMETHODCALLTYPE CBHORedirector::NewWindow2(IDispatch **ppDisp,VARIANT_BOOL *Cancel)
//{
//	return;
//}
//void STDMETHODCALLTYPE CBHORedirector::NewWindow3(IDispatch **ppDisp,VARIANT_BOOL *Cancel,VARIANT Flags,BSTR bstrUrlContext,BSTR bstrUrl)
//{
//	return;
//	if(!m_spWebBrowser)
//		return;
//
//	CComPtr<IDispatch> spDispatch;
//	HRESULT hr = m_spWebBrowser->QueryInterface(IID_IDispatch, (void**)&spDispatch);
//	if (FAILED(hr))
//		return;
//
//	CString strMsg;
//	CString logmsg;
//	DWORD dRet=0;
//	//tab
//	if(m_ResourceCAPConf.m_bTabCntCAP)
//	{
//		DWORD dwTabCnt=0;
//		dwTabCnt = m_IEStatus.GetTabCnt();
//		//Tab Limit
//		if(dwTabCnt >= m_ResourceCAPConf.m_uiTabCntMAX-1)
//		{
//			logmsg.Format(_T("現在開いているタブ数(%d)が上限値(%d)に達しています。\n他の不要なタブを閉じてから再度実行して下さい。\n※空白(about:blank)ページを除く"),dwTabCnt,m_ResourceCAPConf.m_uiTabCntMAX);
//			this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_EX);
//			//::MessageBox(NULL,logmsg,_T("ThinBridge"),MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
//			SBUtil::ShowTimeoutMessageBox(logmsg,_T("ThinBridge"),MB_OK|MB_ICONERROR|MB_SYSTEMMODAL,2000);
//			*Cancel =VARIANT_TRUE;
//
//			//CloseTabWindow(spDispatch);
//			return;
//		}
//		//else if(dwTabCnt >= m_ResourceCAPConf.m_uiTabCntWARM)
//		//{
//		//	logmsg.Format(_T("警告：現在開いているタブ数(%d)が警告値(%d)に達しています。\n他の不要なタブを閉じて下さい。\n※空白(about:blank)ページを除く"),dwTabCnt,m_ResourceCAPConf.m_uiTabCntWARM);
//		//	this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_EX);
//		//	//::MessageBox(NULL,logmsg,_T("ThinBridge"),MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL);
//		//	SBUtil::ShowTimeoutMessageBox(logmsg,_T("ThinBridge"),MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL,2000);
//		//}
//	}
//
//	if(m_ResourceCAPConf.m_bMemUsageCAP)
//	{
//		UINT iMem=0;
//		iMem = m_IEStatus.GetMemoryUsageSize();
//		TCHAR memorysize[24] = _T("");
//		TCHAR WarmMemorysize[24] = _T("");
//		TCHAR LimitMemorysize[24] = _T("");
//		::StrFormatByteSize(iMem, memorysize, 24);
//		::StrFormatByteSize(m_ResourceCAPConf.m_uiMemWARM*1024*1024,WarmMemorysize,24);
//		::StrFormatByteSize(m_ResourceCAPConf.m_uiMemMAX*1024*1024,LimitMemorysize,24);
//
//		//Mem Limit
//		if(iMem >= m_ResourceCAPConf.m_uiMemMAX*1024*1024)
//		{
//			logmsg.Format(_T("現在の使用メモリ(%s)が上限値(%s)に達しています。\n他の不要なタブを閉じてから再度実行して下さい。"),memorysize,LimitMemorysize);
//			this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_EX);
//			//::MessageBox(NULL,logmsg,_T("ThinBridge"),MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
//			SBUtil::ShowTimeoutMessageBox(logmsg,_T("ThinBridge"),MB_OK|MB_ICONERROR|MB_SYSTEMMODAL,2000);
//			*Cancel =VARIANT_TRUE;
////			CloseTabWindow(spDispatch);
//			return;
//		}
//		//else if(iMem >= m_ResourceCAPConf.m_uiMemWARM*1024*1024)
//		//{
//		//	logmsg.Format(_T("警告：現在の使用メモリ(%s)が警告値(%s)に達しています。\n他の不要なタブを閉じてください。"),memorysize,WarmMemorysize);
//		//	this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_EX);
//		//	//::MessageBox(NULL,logmsg,_T("ThinBridge"),MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL);
//		//	SBUtil::ShowTimeoutMessageBox(logmsg,_T("ThinBridge"),MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL,2000);
//		//}
//	}
//	return;
//}

void CBHORedirector::CloseTabWindow(LPDISPATCH pDisp)
{
	if(m_spWebBrowser == NULL)return;

	CComPtr<IDispatch> spDispatch;
	HRESULT hr = m_spWebBrowser->QueryInterface(IID_IDispatch, (void**)&spDispatch);
	if (FAILED(hr))
		return;
	if (pDisp == spDispatch)
	{
		CloseTabWindow();
	}
	return;
}
BOOL CBHORedirector::IsClosedTabWindow()
{
	if(m_spWebBrowser == NULL)return TRUE;

	intptr_t iVal=0;
	HWND hwnd={0};
	HRESULT hr = m_spWebBrowser->get_HWND((LONG_PTR*)&hwnd);
	if(SBUtil::SafeWnd(hwnd))
	{
		if(m_MapCloseWebBrowserPtrs.Lookup((intptr_t)hwnd,iVal))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	return FALSE;
}
void CBHORedirector::CloseTabWindow()
{
	__try
	{
		if(m_spWebBrowser == NULL)return;
		HWND hwnd={0};
		intptr_t iVal=0;
		HRESULT hr = m_spWebBrowser->get_HWND((LONG_PTR*)&hwnd);
		if(SBUtil::SafeWnd(hwnd))
		{
			if(m_MapCloseWebBrowserPtrs.Lookup((intptr_t)hwnd,iVal))
			{
				//既にClose処理を実行している。2重にCloseすることを回避する。例外で落ちる可能性があるので。
				;
			}
			else
			{
				m_MapCloseWebBrowserPtrs.SetAt((intptr_t)hwnd,(intptr_t)hwnd);
				//if(!(m_IE_M_Ver==8 && m_bInVOS==TRUE))
				//	m_spWebBrowser->Stop();
				m_spWebBrowser->ExecWB(OLECMDID_CLOSE, OLECMDEXECOPT_DONTPROMPTUSER, 0, 0);
				//m_spWebBrowser->put_Visible(VARIANT_FALSE);
				//m_spWebBrowser->Quit();
			}
		}
		else
		{
			if(hwnd)
			{
				if(m_MapCloseWebBrowserPtrs.Lookup((intptr_t)hwnd,iVal))
				{
					m_MapCloseWebBrowserPtrs.RemoveKey((intptr_t)hwnd);
				}
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		;
	}
}
DWORD CBHORedirector::GetURLZone(LPCTSTR pszURL)
{
/*
URLZONE_INTRANET:
URLZONE_TRUSTED:
URLZONE_INTERNET:
URLZONE_UNTRUSTED:

#define ZONE_NA        0x0000
#define INTRANET_ZONE  0x0001
#define TRUSTED_ZONE   0x0010
#define INTERNET_ZONE  0x0100
#define UNTRUSTED_ZONE 0x1000
*/
	DWORD dwZone=0;
	HRESULT hr={0};
	CString strURL;//(pszURL);
	SBUtil::GetDivChar(pszURL,260,strURL);
	
	CComPtr<IInternetSecurityManager>	pIInternetSecurityManager;

	dwZone = URLZONE_INVALID;
	pIInternetSecurityManager.CoCreateInstance(CLSID_InternetSecurityManager,NULL,CLSCTX_INPROC_SERVER);
	if(pIInternetSecurityManager == NULL)
		return	false;

#ifndef _UNICODE
	setlocale( LC_ALL, "ja" );
	size_t     length = strURL.GetLength();
	wchar_t*   wc = new wchar_t[ length+1 ];
	memset(wc,0x00,sizeof(wchar_t)*(length+1));
	length = mbstowcs( wc, strURL, length+1 );
	hr = pIInternetSecurityManager->MapUrlToZone(wc,&dwZone,0);
	if(wc)
		delete [] wc;
#else //_UNICODE
	hr = pIInternetSecurityManager->MapUrlToZone(strURL,&dwZone,0);
#endif //_UNICODE

	//独自の形式に変換する。
	DWORD dwRet=ZONE_NA;
	switch(dwZone)
	{
		case URLZONE_INTRANET:dwRet=INTRANET_ZONE;break;
		case URLZONE_TRUSTED:dwRet=TRUSTED_ZONE;break;
		case URLZONE_INTERNET:dwRet=INTERNET_ZONE;break;
		case URLZONE_UNTRUSTED:dwRet=UNTRUSTED_ZONE;break;
		default:dwRet=ZONE_NA;break;
	}
	return	dwRet;
}

void CBHORedirector::WriteDebugTraceDateTime(LPCTSTR msg,int iLogType,BOOL bForce/*=FALSE*/)
{
	if(!bForce)
	{
		if(!m_gbTraceLog)return;
	}
	if(msg==NULL)return;
	if(_tcslen(msg)==0)return;
	CString strWriteLine;
	CTime time = CTime::GetCurrentTime();
	strWriteLine.Format(_T("ThinBridge:BHO\t%s\t%s\t%s\n"),time.Format(_T("%Y-%m-%d %H:%M:%S")),sDEBUG_LOG_TYPE[iLogType],msg);
	if(strWriteLine.GetLength()>4000)
	{
		strWriteLine = strWriteLine.Mid(0,4000);
		strWriteLine+=_T("..DIV..");
	}
#ifdef _UNICODE
	_wsetlocale(LC_ALL, _T("jpn")); 
#endif
	OutputDebugString(strWriteLine);
	//BOOL bFileWriteFlg=FALSE;
	//switch(m_iAdvancedLogLevel)
	//{
	//	//全てのログを出力
	//	case DEBUG_LOG_LEVEL_OUTPUT_ALL:
	//	{
	//		bFileWriteFlg=TRUE;
	//		break;
	//	}
	//	//ファイル書き込み無し。
	//	case DEBUG_LOG_LEVEL_OUTPUT_NO_FILE:
	//	{
	//		bFileWriteFlg=FALSE;
	//		break;
	//	}
	//	//URL関連のみ
	//	case DEBUG_LOG_LEVEL_OUTPUT_URL:
	//	{
	//		//ログの種類が一般、URLの場合
	//		if(iLogType==DEBUG_LOG_TYPE_GE || iLogType==DEBUG_LOG_TYPE_URL)
	//		{
	//			bFileWriteFlg=TRUE;
	//		}
	//		break;
	//	}
	//	default:
	//		break;
	//}
	//if(bFileWriteFlg)
	//{
	//	FILE *fp=NULL;
	//	if(_wfopen_s(&fp,m_strLogFileFullPath,_T("a"))==0)
	//	{
	//		fputws(strWriteLine,fp);
	//		fclose(fp);
	//	}
	//}
#ifdef _UNICODE
	_wsetlocale(LC_ALL, _T(""));
#endif

}

void CBHORedirector::OpenAnotherBrowser(CURLRedirectDataClass* pRedirectData,CString strURL)
{
	if(pRedirectData==NULL)return;

	if(SBUtil::IsURL_HTTP(strURL))
	{
		CString strExecTypeUpper;
		strExecTypeUpper=pRedirectData->m_strExecType;
		strExecTypeUpper.MakeUpper();
		CString strCommand;
		CString strCommand2;
		CString logmsg;

		if(strExecTypeUpper.CompareNoCase(_T("RDP"))==0
		||strExecTypeUpper.CompareNoCase(_T("VMWARE"))==0
		||strExecTypeUpper.CompareNoCase(_T("CITRIX"))==0
		||strExecTypeUpper.CompareNoCase(_T("FIREFOX"))==0
		||strExecTypeUpper.CompareNoCase(_T("CHROME"))==0
		||strExecTypeUpper.CompareNoCase(_T("EDGE"))==0
		||strExecTypeUpper.CompareNoCase(_T("IE"))==0
		||strExecTypeUpper.CompareNoCase(_T("Default"))==0)
		{
			strCommand.Format(_T("\"%s\" \"%s\" \"%s\""),m_strThinBridge_EXE_FullPath,strURL,pRedirectData->m_strExecType);
			strCommand2.Format(_T("%s \"%s\""),strURL,pRedirectData->m_strExecType);
		}
		//Custom系の場合は、m_strExecExeFullPath
		else if(strExecTypeUpper.Find(_T("CUSTOM"))==0)
		{
			strCommand.Format(_T("\"%s\" \"%s\" \"%s\""),m_strThinBridge_EXE_FullPath,strURL,pRedirectData->m_strExecExeFullPath);
			strCommand2.Format(_T("%s \"%s\""),strURL,pRedirectData->m_strExecExeFullPath);
		}
		else
		{
			strCommand.Format(_T("\"%s\" \"%s\" \"%s\""),m_strThinBridge_EXE_FullPath,strURL,pRedirectData->m_strExecType);
			strCommand2.Format(_T("%s \"%s\""),strURL,pRedirectData->m_strExecType);
		}


		logmsg.Format(_T("#ExecuteThinBridge\t%s"),strCommand);
		WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_DE);

		//::ShellExecute(NULL,NULL,strCommand,NULL,NULL, SW_SHOW);
		STARTUPINFO si={0};
		PROCESS_INFORMATION pi={0};
		si.cb = sizeof( si );
		unsigned long ecode = 0;
		::Sleep(100);
		if(!CreateProcess( NULL,(LPTSTR)(LPCTSTR)strCommand,NULL, NULL, TRUE, 0, NULL,NULL, &si, &pi ))
		{
			::Sleep(100);
			logmsg.Format(_T("ExecuteThinBridge:CreateProcess Retry1 0x%08x"),::GetLastError());
			WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_DE);
			SetLastError(NO_ERROR);
			//Retry
			if(!CreateProcess( m_strThinBridge_EXE_FullPath,(LPTSTR)(LPCTSTR)strCommand2,NULL, NULL, TRUE, 0, NULL,NULL, &si, &pi ))
			{
				::Sleep(100);
				logmsg.Format(_T("ExecuteThinBridge:ShellExecute Retry2 0x%08x"),::GetLastError());
				WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_DE);
				SetLastError(NO_ERROR);
				if(::ShellExecute(NULL,_T("open"),m_strThinBridge_EXE_FullPath,strCommand2,NULL, SW_SHOW) <= HINSTANCE(32))
				{
					::Sleep(100);
					logmsg.Format(_T("ExecuteThinBridge:ShellExecute Retry3 0x%08x"),::GetLastError());
					WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_DE);
					SetLastError(NO_ERROR);
					::ShellExecute(NULL,NULL,strCommand,NULL,NULL, SW_SHOW);
				}
			}
		}
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

void CBHORedirector::Navigate_RedirectHTM(CComPtr<IWebBrowser2>  spWebBrowse,CString OptionParam,DWORD dRedirectPageAction,DWORD dwCloseTimeout)
{
	if(spWebBrowse==NULL)
		return;
	//https://kb.vmware.com/selfservice/microsites/search.do?language=en_US&cmd=displayKC&externalId=2073845
	if(!(m_IE_M_Ver==8 && m_bInVOS==TRUE))
		spWebBrowse->Stop();
	CString strURL;
	strURL.Format(_T("res://%s/205?q=1"),_AtlModule.m_gSZModuleName);

	if(OptionParam.CompareNoCase(_T("Firefox"))==0
	||OptionParam.CompareNoCase(_T("Chrome"))==0
	||OptionParam.CompareNoCase(_T("Edge"))==0
	||OptionParam.CompareNoCase(_T("RDP"))==0
	||OptionParam.CompareNoCase(_T("VMWARE"))==0
	||OptionParam.CompareNoCase(_T("CITRIX"))==0
	)
	{
		strURL+=_T("&type=");
		strURL+=OptionParam.MakeLower();
	}
//	else if(OptionParam.CompareNoCase(_T("Custom"))==0)
//	else if(OptionParam.CompareNoCase(_T("IE"))==0)
//	else

	//res://ThinBridgeBHO.dll/PNG/202 Default
	//res://ThinBridgeBHO.dll/PNG/206 Chrome
	//res://ThinBridgeBHO.dll/PNG/207 Citrix
	//res://ThinBridgeBHO.dll/PNG/208 Edge
	//res://ThinBridgeBHO.dll/PNG/209 Firefox
	//res://ThinBridgeBHO.dll/PNG/210 RDP
	//res://ThinBridgeBHO.dll/PNG/211 VMware

	if(dRedirectPageAction==1)
	{
		strURL+=_T("&hb=1");
	}
	else if(dRedirectPageAction==2)
	{
		strURL+=_T("&hb=2");
	}

	if(dwCloseTimeout!=3)
	{
		CString strCloseTimeout;
		strCloseTimeout.Format(_T("&cto=%d"),dwCloseTimeout);
		strURL+=strCloseTimeout;
	}

	DWORD		dwFlags 			= 0;
	LPCTSTR 	lpszTargetFrameName = NULL;
	LPCTSTR 	lpszHeaders 		= NULL;
	LPVOID		lpvPostData 		= NULL;
	DWORD		dwPostDataLen		= 0;

	CComVariant   vURL;
	CComVariant   vHeaders;
	CComVariant   vTargetFrameName;
	CComVariant   vFlags( (long) dwFlags );
	CComVariant   vPostData;

	vURL = strURL;
	try
	{
		HRESULT hr = spWebBrowse->Navigate2(&vURL, &vFlags, &vTargetFrameName,NULL, &vHeaders);
		//ATLASSERT(hr == S_OK);
	}
	catch (...)
	{
		ATLASSERT(0);
	}
}
//
//typedef int (__stdcall * TMessageBoxTimeout)(HWND, LPCTSTR, LPCTSTR, UINT, WORD, DWORD);
//void CBHORedirector::ShowTimeoutMessageBox(HWND hwnd,LPCTSTR strMsg,int iType,int iTimeOut)
//{
//	BOOL bFreeLibrary = FALSE;
//	HMODULE hModule = {0};
//	hModule = ::GetModuleHandle(_T("user32.dll"));
//	if(!hModule)
//	{
//		hModule = ::LoadLibrary(_T("user32.dll"));
//		if(hModule)
//			bFreeLibrary = TRUE;
//	}
//
//	if(hModule)
//	{
//		TMessageBoxTimeout  MessageBoxTimeout;
//#ifdef _UNICODE
//		MessageBoxTimeout = (TMessageBoxTimeout) GetProcAddress(hModule, "MessageBoxTimeoutW");
//#else
//		MessageBoxTimeout = (TMessageBoxTimeout) GetProcAddress(hModule, "MessageBoxTimeoutA");
//#endif
//		if(MessageBoxTimeout)
//		{
//			MessageBoxTimeout(hwnd, strMsg,
//                       _T("ThinBridge Browser Helper"), iType, LANG_NEUTRAL,iTimeOut);
//		}
//		else
//		{
//			::MessageBox(hwnd,strMsg,_T("ThinBridge Browser Helper"),iType);
//		}
//		if(bFreeLibrary)
//		{
//
//			FreeLibrary(hModule);
//			bFreeLibrary=FALSE;
//			hModule=NULL;
//		}
//
//	}
//}
//

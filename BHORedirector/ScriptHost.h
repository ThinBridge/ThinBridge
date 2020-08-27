#pragma once
#include "resource.h"       // メイン シンボル
#include <shlguid.h> 
#include <exdispid.h> 
#include "BHORedirector_i.h"
#include <atlstr.h>
#include <atlcoll.h>
#include <atlfile.h>
#include "locale.h"
#include "atltime.h"
#include "TBGlobal.h"
#include "SBUtil.h"

class __declspec(uuid("{B54F3741-5B07-11cf-A4B0-00AA004A55E8}")) VBScript;
//class __declspec(uuid("{F414C260-6AC0-11CF-B6D1-00AA00BBBB58}")) JScript;
class __declspec(uuid("{147F5B60-1F9B-4035-A22C-1A87493CF6BD}")) MyScriptSite
	: public CComObjectRoot
	, public CComCoClass<MyScriptSite, &__uuidof(MyScriptSite)>
	, public IActiveScriptSite
	, public IActiveScriptSiteWindow
{
public:

	BEGIN_COM_MAP(MyScriptSite)
		COM_INTERFACE_ENTRY(IActiveScriptSite)
		COM_INTERFACE_ENTRY(IActiveScriptSiteWindow)
	END_COM_MAP( )

	DECLARE_NO_REGISTRY()
	DECLARE_CLASSFACTORY()

	virtual HRESULT STDMETHODCALLTYPE GetLCID( 
		/* [out] */ LCID *plcid) throw()
	{
		//_tprintf(_TEXT("GetLCID\n"));
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetItemInfo( 
		/* [in] */ LPCOLESTR pstrName,
		/* [in] */ DWORD dwReturnMask,
		/* [out] */ IUnknown **ppiunkItem,
		/* [out] */ ITypeInfo **ppti) throw()
	{
		//_tprintf(_TEXT("GetItemInfo\n"));
		return TYPE_E_ELEMENTNOTFOUND;
	}

	virtual HRESULT STDMETHODCALLTYPE GetDocVersionString( 
		/* [out] */ BSTR *pbstrVersion) throw()
	{
		//_tprintf(_TEXT("GetDocVersionString\n"));
		*pbstrVersion = SysAllocString(L"MyScriptSite");
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE OnScriptTerminate( 
		/* [in] */ const VARIANT *pvarResult,
		/* [in] */ const EXCEPINFO *pexcepinfo) throw()
	{
		//_tprintf(_TEXT("OnScriptTerminate\n"));
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE OnStateChange( 
		/* [in] */ SCRIPTSTATE ssScriptState) throw()
	{
		//_tprintf(_TEXT("OnStateChange: %d\n"), ssScriptState);
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE OnScriptError( 
		/* [in] */ IActiveScriptError *pscripterror) throw()
	{
		EXCEPINFO excepinfo = {0};

		pscripterror->GetExceptionInfo(&excepinfo);
		CW2T mes(excepinfo.bstrDescription);
		if(m_gbTraceLog)
		{
			CString strErr;
			strErr.Format(_T("ThinBridge Script Error:\n%s"),static_cast<LPCTSTR>(mes));
			::MessageBox(NULL,strErr,_T("ThinBridge Script Error"),MB_SYSTEMMODAL|MB_ICONERROR);
		}
		//_tprintf(_TEXT("OnScriptError: %s\n"), mes);
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE OnEnterScript(void) throw()
	{
		//_tprintf(_TEXT("OnEnterScript\n"));
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE OnLeaveScript(void) throw()
	{
		//_tprintf(_TEXT("OnLeaveScript\n"));
		return S_OK;
	}

	////

	virtual HRESULT STDMETHODCALLTYPE GetWindow(
		/* [out] */ HWND *phwnd)
	{
		//_tprintf(_TEXT("GetWindow\n"));
		*phwnd = GetDesktopWindow();
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE EnableModeless( 
		/* [in] */ BOOL fEnable)
	{
		//_tprintf(_TEXT("EnableModeless:%d\n"), fEnable);
		return S_OK;
	}
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(MyScriptSite), MyScriptSite);

class CScriptHost
{
public:
	CScriptHost(){}
	~CScriptHost(){}
	BOOL ExecScript(LPCTSTR sURL,BOOL bTopPage,DWORD dwZone,LPCTSTR sUserScript,BOOL bEventBeforeNavigate)
	{
		m_strReturnString.Empty();
		HRESULT hr = pScript_.CoCreateInstance(__uuidof(VBScript));
		//ATLASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			return FALSE;
		}

		// Siteの構築
		CComPtr<IActiveScriptSite> pSite;
		hr = MyScriptSite::CreateInstance(&pSite);
		//ATLASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			return FALSE;
		}

		// スクリプトエンジンにSiteを設定
		hr = pScript_->SetScriptSite(pSite);
		//ATLASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			return FALSE;
		}

		// スクリプト解析インターフェイスの取得
		CComQIPtr<IActiveScriptParse> pParse(pScript_);

		hr = pParse->InitNew(); // 初回のみ、以後Closeするまで呼び出し不可
		//ATLASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			return FALSE;
		}

		CString vbFormatTemp=sURL;
		vbFormatTemp.Replace(_T("\""),_T("\"\""));

		CString vbGlobalURL;
		vbGlobalURL.Format(_T("Const TB_Global_URL=\"%s\"\r\n"),vbFormatTemp);

		CString vbGlobalTopPage;
		vbGlobalTopPage.Format(_T("Const TB_Global_TOP_PAGE=%s\r\n"),bTopPage?_T("True"):_T("False"));

		CString vbGlobalZone;
		if(dwZone==INTRANET_ZONE)
		{
			vbGlobalZone.Format(_T("Const TB_Global_ZONE=\"%s\"\r\n"),_T("LOCAL_INTRANET_ZONE"));
			//_T("ローカルイントラネットゾーン");
		}
		else if(dwZone==INTERNET_ZONE)
		{
			vbGlobalZone.Format(_T("Const TB_Global_ZONE=\"%s\"\r\n"),_T("INTERNET_ZONE"));
			//_T("インターネットゾーン");
		}
		else if(dwZone==TRUSTED_ZONE)
		{
			vbGlobalZone.Format(_T("Const TB_Global_ZONE=\"%s\"\r\n"),_T("TRUSTED_ZONE"));
			//_T("信頼済みサイトゾーン");
		}
		else if(dwZone==UNTRUSTED_ZONE)
		{
			vbGlobalZone.Format(_T("Const TB_Global_ZONE=\"%s\"\r\n"),_T("UNTRUSTED_ZONE"));
			//_T("制限付きサイトゾーン");
		}
		else
		{
			vbGlobalZone=_T("Const TB_Global_ZONE=\"\"\r\n");
			//念のため
		}

		CString vbGlobalEvent;
		if(bEventBeforeNavigate)
		{
			vbGlobalEvent.Format(_T("Const TB_Global_EVENT=\"%s\"\r\n"),_T("BeforeNavigate"));
		}
		else
		{
			vbGlobalEvent.Format(_T("Const TB_Global_EVENT=\"%s\"\r\n"),_T("NavigateComplete"));
		}

		CStringW vbGlobalSettings;
		vbGlobalSettings = vbGlobalURL;
		vbGlobalSettings += vbGlobalTopPage;
		vbGlobalSettings += vbGlobalZone;
		vbGlobalSettings += vbGlobalEvent;
		vbGlobalSettings += _T("\r\n\r\n");

		//vbGlobalSettings += _T("Function CreateObject(str) \r\n msgbox str \r\n End Function\r\n");
		//vbGlobalSettings += _T("\r\n\r\n");

		CStringW vbUserScript=sUserScript;

		CStringW VBSource;
		VBSource+=vbUserScript;
		VBSource += _T("\r\n\r\n");
		VBSource+=vbGlobalSettings;
		LPCWSTR pSource=NULL;
		pSource=VBSource;

		// ParseScriptTextは複数回呼び出し可能
		//LPCWSTR source = L"Function OnRedirect()\r\nMsgBox \"ok2\"\r\n OnRedirect=\"Firefox cccccbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\" \r\n End Function\r\n";

		hr = pParse->ParseScriptText(pSource, NULL, NULL, NULL, 0, 1,
			SCRIPTTEXT_ISPERSISTENT | SCRIPTTEXT_ISVISIBLE, NULL, NULL);
		//ATLASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			return FALSE;
		}

		// スクリプトの開始
		hr = pScript_->SetScriptState(SCRIPTSTATE_CONNECTED);
		//ATLASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			return FALSE;
		}

		// スクリプトのルートコンテキストを取得
		CComPtr<IDispatch> pScriptDisp;
		hr = pScript_->GetScriptDispatch(NULL, &pScriptDisp);
		//ATLASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			return FALSE;
		}

		// スクリプト上に定義されているtestfuncの呼び出し
		LPOLESTR szMember[] = {L"OnRedirect", NULL};
		DISPID dispids[1]={0};
		hr = pScriptDisp->GetIDsOfNames(IID_NULL, &szMember[0], 1,
			LOCALE_SYSTEM_DEFAULT, &dispids[0]);
		//ATLASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			pScriptDisp.Release();
			return FALSE;
		}

		DISPPARAMS param = {NULL, NULL, 0, 0};
		VARIANT   varResult={0};
		VariantInit(&varResult);

		hr = pScriptDisp->Invoke(dispids[0], IID_NULL, LOCALE_SYSTEM_DEFAULT,
			DISPATCH_METHOD, &param, &varResult, NULL, NULL);
		//ATLASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			pScriptDisp.Release();
			return FALSE;
		}
		if(!(varResult.vt==VT_EMPTY || varResult.vt==VT_NULL))
		{
			CString strResult = V_BSTR(&varResult);
			m_strReturnString = strResult;
		}
		VariantClear(&varResult);

		pScriptDisp.Release();

		// スクリプトの解放
		hr = pScript_->SetScriptState(SCRIPTSTATE_CLOSED);
		pScript_->Close();
		return TRUE;
	}
	CString m_strReturnString;

private:
	CComPtr<IActiveScript> pScript_;
};


#pragma comment(lib, "Psapi.lib") 
#define PSAPI_VERSION 1
#include "Psapi.h"
#include <TlHelp32.h>

class CIEStatusHelper
{
	static BOOL CALLBACK EnumWindowsProc_(HWND hwnd,LPARAM lParam)
	{
		if(lParam == NULL)
			return	FALSE;
		return	((CIEStatusHelper*)lParam)->EnumWindowsProc(hwnd);
	}
	static BOOL CALLBACK EnumChildProc_(HWND hwnd,LPARAM lParam)
	{
		if(lParam == NULL)
			return	FALSE;
		return	((CIEStatusHelper*)lParam)->EnumChildProc(hwnd);
	}
public:
	CString m_strProcessName;
	CIEStatusHelper()
	{
		m_IETabCnt=0;
		m_strProcessName=_T("iexplore.exe");
	}
	virtual ~CIEStatusHelper()
	{
	}

	size_t GetMemoryUsageSize()
	{
		if(m_strProcessName.IsEmpty())
			m_strProcessName=_T("iexplore.exe");

		HANDLE snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if(snapshot == INVALID_HANDLE_VALUE)return 0;
		PROCESSENTRY32W entry={0};
		entry.dwSize = sizeof(entry);
		BOOL has_entry = ::Process32FirstW(snapshot, &entry);
		TCHAR	pszName[64]={0};
		size_t iMem=0;
		while(has_entry)
		{
			lstrcpyn(pszName,entry.szExeFile,64);
			//ウインドウクラスの取得
			ATLTRACE(_T("EnumChildProc:%s\n"),pszName);
			if(::_tcsicmp(pszName,m_strProcessName)==0)
			{
				DWORD dwProcessId = entry.th32ProcessID;
				CHandle hProcess;
				hProcess.Attach(::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId));
				//EmptyWorkingSet(hProcess);

				PROCESS_MEMORY_COUNTERS_EX pmcs = { sizeof(PROCESS_MEMORY_COUNTERS_EX) };
				::GetProcessMemoryInfo(hProcess,(PROCESS_MEMORY_COUNTERS*) &pmcs, sizeof(PROCESS_MEMORY_COUNTERS_EX));
				//::CoFreeUnusedLibrariesEx(0,0);
				iMem += pmcs.PrivateUsage;
			}
			has_entry = ::Process32NextW(snapshot, &entry);
		}
		::CloseHandle(snapshot);
		return iMem;
	}


	DWORD m_IETabCnt;
	BOOL EnumChildProc(HWND hwnd)
	{
		//DWORD dwProcessId = 0;
		//::GetWindowThreadProcessId(hwnd, &dwProcessId);
		//if(::GetCurrentProcessId() != dwProcessId)
		//{
		//	return	TRUE;
		//}

		int nRet=0;
		BOOL	bRet=FALSE;
		HRESULT	hr={0};
		TCHAR	pszName[64]={0};
		//ウインドウクラスの取得
		nRet = ::GetClassName(hwnd,pszName,64);
ATLTRACE(_T("EnumChildProc:%s\n"),pszName);
		if(nRet == 0)
			return	TRUE;

		nRet = ::_tcscmp(pszName,_T("Frame Tab"));
		if(nRet != 0)
			return	TRUE;

		if(hwnd == NULL || ::IsWindow(hwnd) == FALSE)
			return	FALSE;

		//空白のタブをカウントしない。
		// TabWindowClass
		HWND hWndTabWindow = {0};
		hWndTabWindow = ::FindWindowEx(hwnd,NULL,_T("TabWindowClass"),NULL);
		if(hWndTabWindow)
		{
			CString strTitle;
			SafeGetWindowText(hWndTabWindow,strTitle);
//::MessageBox(NULL,strTitle,strTitle,MB_SYSTEMMODAL);
			if(strTitle.IsEmpty()
			||strTitle.Find(_T("空白のページ"))==0
			||strTitle.Find(_T("新しいタブ"))==0
			||strTitle.Find(_T("about:blank"))==0
			||strTitle.Find(_T("Blank Page"))==0
			||strTitle.Find(_T("New tab"))==0
			)
				return TRUE;
			else
				m_IETabCnt++;
		}
//		m_IETabCnt++;
		return	TRUE;

	}
	inline HWND SafeWnd(HWND wnd)
	{
		HWND hRetNULL={0};
		if(wnd == NULL)
			return hRetNULL;
		__try
		{
			if(IsWindow(wnd))
			{
				return wnd;
			}
			else
			{
				return hRetNULL;
			}
		}
		__except( EXCEPTION_EXECUTE_HANDLER)
		{
			return hRetNULL;
		}
		return hRetNULL;
	}
	inline void SafeGetWindowText(HWND hWnd,CString& str)
	{
		if(!SafeWnd(hWnd))
			return;
		//TCHAR szTitle[130]={0};
		//::GetWindowText(hWnd,szTitle,128);
		//str=szTitle;return;

		CString strText;
		int nLen = ::GetWindowTextLength(hWnd);
		int nRetLen = ::GetWindowText(hWnd, strText.GetBufferSetLength(nLen+2), nLen + 2);
		strText.ReleaseBuffer();

		if(nRetLen < nLen)
		{
			str.Empty();
			return;
		}
		str=strText;
		return;
	}

	DWORD  GetTabCnt(void)
	{
		m_IETabCnt=0;
		::EnumWindows(EnumWindowsProc_,(LPARAM)this);
		return	m_IETabCnt;
	}

	BOOL EnumWindowsProc(HWND hwnd)
	{
//		DWORD dwProcessId = 0;
//		::GetWindowThreadProcessId(hwnd, &dwProcessId);
//		if(::GetCurrentProcessId() != dwProcessId)
//		{
//			return	TRUE;
//		}

		int nRet=0;
		TCHAR pszName[64]={0};
		//ウインドウクラスの取得
		nRet = ::GetClassName(hwnd,pszName,64);
		if(nRet == 0)
			return	TRUE;
		nRet = ::_tcscmp(pszName,_T("IEFrame"));
ATLTRACE(_T("EnumWindowsProc:%s\n"),pszName);
		if(nRet != 0)
		{
			return TRUE;
		}
		//子ウインドウの列挙方法
		::EnumChildWindows(hwnd,EnumChildProc_,(LPARAM)this);
		return	TRUE;
	}
};


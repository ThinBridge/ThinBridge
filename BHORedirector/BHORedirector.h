// BHORedirector.h : CBHORedirector の宣言

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
#include <activscp.h>
#include <atlfile.h>
#include <string>
#include "TBGlobal.h"
#include "SBUtil.h"
#include "ScriptHost.h"
#include "AtlStdioFile.h"
#include "URLRedirectCore.h"

#pragma comment(lib, "Version.lib")

using namespace ATL;

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "DCOM の完全サポートを含んでいない Windows Mobile プラットフォームのような Windows CE プラットフォームでは、単一スレッド COM オブジェクトは正しくサポートされていません。ATL が単一スレッド COM オブジェクトの作成をサポートすること、およびその単一スレッド COM オブジェクトの実装の使用を許可することを強制するには、_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA を定義してください。ご使用の rgs ファイルのスレッド モデルは 'Free' に設定されており、DCOM Windows CE 以外のプラットフォームでサポートされる唯一のスレッド モデルと設定されていました。"
#endif

// CBHORedirector
class ATL_NO_VTABLE CBHORedirector :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CBHORedirector, &CLSID_BHORedirector>,
	public IObjectWithSiteImpl<CBHORedirector>,
	public IDispatchImpl<IBHORedirector, &IID_IBHORedirector, &LIBID_BHORedirectorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispEventImpl<1, CBHORedirector, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 1>
{
public:
	CBHORedirector()
	{
		m_fAdvised=FALSE;
		m_bInVOS=FALSE;
		m_hEventLog=NULL;
	}
	bool IsPageIWebBrowser(IDispatch* pDisp);
	void CloseTabWindow(LPDISPATCH pDisp);
	void CloseTabWindow();
	BOOL IsClosedTabWindow();

	CString m_strTopURL;

	CString m_strSafeGuardURL;
	CString m_strSafeGuard_chkURL;

	BOOL IsSafeGuard(LPCTSTR sURL,DWORD oTimeout=3)
	{
		BOOL bRet=TRUE;
		DWORD dwNow=::GetTickCount();
		DWORD dwPCon=0;

		ATLTRACE(_T("====================\n"));
		//前回の実行時間がない。現在をセット
		if(g_iPrevMS==0)
			g_iPrevMS=dwNow;
		ATLTRACE(_T("g_iPrevMS:%d\n"),g_iPrevMS);

		dwPCon = dwNow - g_iPrevMS;
		ATLTRACE(_T("dwPCon:%d\n"),dwPCon);
		//前回と今回の時間差を入れる。
		g_iPrevMS_Sum+=dwPCon;
		ATLTRACE(_T("g_iPrevMS_Sum:%d\n"),g_iPrevMS_Sum);

		//起動回数をインクリメント
		InterlockedIncrement(&g_iBreakerCnt);
		ATLTRACE(_T("g_iBreakerCnt:%d\n"),g_iBreakerCnt);


		//差分時間：2秒間に３回起動されている場合は、ループを疑う
		if(g_iPrevMS_Sum<=1000)
		{
			//if(g_iBreakerCnt>=2)
			//{
			//	bRet=FALSE;
			//	g_iPrevMS_Sum=0;
			//	g_iPrevMS=0;
			//	g_iBreakerCnt=0;
			//	return bRet;
			//}
			;
		}
		else if(g_iPrevMS_Sum<=2000)
		{
			if(g_iBreakerCnt>=3)
			{
				bRet=FALSE;
				g_iPrevMS_Sum=0;
				g_iPrevMS=0;
				g_iBreakerCnt=0;
				return bRet;
			}
		}
		else if(g_iPrevMS_Sum<=3000)
		{
			if(g_iBreakerCnt>=4)
			{
				bRet=FALSE;
				g_iPrevMS_Sum=0;
				g_iPrevMS=0;
				g_iBreakerCnt=0;
				return bRet;
			}
		}
		else
		{
			//前回と今回の差分時間が3秒以上の差がある
			if(dwPCon>3000)
			{
				//カウンター類をリセットする。
				g_iPrevMS_Sum=0;
				g_iPrevMS=0;
				g_iBreakerCnt=0;
			}
			if(g_iBreakerCnt>=10)
			{
				//カウンター類をリセットする。
				g_iPrevMS_Sum=0;
				g_iPrevMS=0;
				g_iBreakerCnt=0;
			}
		}

		//同一URLで3秒以内の場合は、ループの疑いあり
		if(m_strSafeGuard_chkURL.Compare(sURL)==0)
		{
			if(dwPCon>0 && dwPCon<oTimeout*1000+1000)
			{
				bRet=FALSE;
				g_iPrevMS_Sum=0;
				g_iPrevMS=0;
				g_iBreakerCnt=0;
				return bRet;
			}
		}
		//抜ける前に前回の時間として今回の時間をセット
		g_iPrevMS=dwNow;
		m_strSafeGuard_chkURL=sURL;
		ATLTRACE(_T("g_iPrevMS:%d\n"),g_iPrevMS);
		ATLTRACE(_T("-------------------\n"));
		return bRet;
	}

	CString m_strThinBridge_EXE_FullPath;
	void Navigate_RedirectHTM(CComPtr<IWebBrowser2>  spWebBrowse,CString OptionParam,DWORD dRedirectPageAction,DWORD dwCloseTimeout);

	//log
	//CString m_strLogFileFullPath;
	//int m_iAdvancedLogLevel;

	BOOL m_bInVOS;

	int m_IE_M_Ver;
	CURLRedirectList m_RedirectList;
	CString m_strRedirectFilePath;
	CString m_strRedirect_Script_FilePath;

	CString m_strResourceCAPFullPath;
	CResourceCAPData m_ResourceCAPConf;
	CIEStatusHelper m_IEStatus;

	CString m_strUserDataPath;


	void WriteDebugTraceDateTime(LPCTSTR msg,int iLogType,BOOL bForce=FALSE);
	void OpenAnotherBrowser(CURLRedirectDataClass* pRedirectData,CString strURL);

	DWORD GetURLZone(LPCTSTR pszURL);

	STDMETHOD(SetSite)(IUnknown *pUnkSite);
DECLARE_REGISTRY_RESOURCEID(IDR_BHOREDIRECTOR1)

DECLARE_NOT_AGGREGATABLE(CBHORedirector)

BEGIN_COM_MAP(CBHORedirector)
	COM_INTERFACE_ENTRY(IBHORedirector)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CBHORedirector)
	IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
	IMPLEMENTED_CATEGORY(CATID_SafeForInitializing)
END_CATEGORY_MAP()

BEGIN_SINK_MAP(CBHORedirector)
//    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete)
	SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, BeforeNavigate)
	SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, NavigateComplete)

//	SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_TITLECHANGE, TitleChange)
//	SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NEWWINDOW2, NewWindow2)
//	SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NEWWINDOW3, NewWindow3)
END_SINK_MAP()
    // DWebBrowserEvents2
//	void STDMETHODCALLTYPE OnDocumentComplete(IDispatch *pDisp, VARIANT *pvarURL);
	void STDMETHODCALLTYPE BeforeNavigate(LPDISPATCH pDisp, VARIANT* URL,
		VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData,
		VARIANT* Headers,   BOOL* Cancel);
	void STDMETHODCALLTYPE NavigateComplete(LPDISPATCH pDisp, VARIANT* URL);

	//void STDMETHODCALLTYPE TitleChange(BSTR bstrText);
	//void STDMETHODCALLTYPE NewWindow2(IDispatch **ppDisp,VARIANT_BOOL *Cancel);
	//void STDMETHODCALLTYPE NewWindow3(IDispatch **ppDisp,VARIANT_BOOL *Cancel,VARIANT Flags,BSTR bstrUrlContext,BSTR bstrUrl);

	HANDLE m_hEventLog;
	STDMETHODIMP CBHORedirector::StartResourceCAPChk(void);
	STDMETHODIMP CBHORedirector::ResourceCAPChk(void);
	CAtlMap<intptr_t, intptr_t> m_MapCloseWebBrowserPtrs;

//	void ShowTimeoutMessageBox(HWND hwnd,LPCTSTR strMsg,int iType,int iTimeOut);
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

//private:
    CComPtr<IWebBrowser2>  m_spWebBrowser;
	BOOL m_fAdvised;
};

OBJECT_ENTRY_AUTO(__uuidof(BHORedirector), CBHORedirector)

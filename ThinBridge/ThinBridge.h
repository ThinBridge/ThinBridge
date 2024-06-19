
// ThinBridge.h
//
//3rd パーティー製ブラウザー拡張
//isolation_writecopy HKEY_CURRENT_USER\Software\Microsoft\Internet Explorer\Main
//  Value=Enable Browser Extensions
//  REG_SZ~no#2300
//isolation_writecopy HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Ext\Settings\{3A56619B-37AC-40DA-833E-410F3BEDCBDC}
//  Value=Flags
//  REG_DWORD=#01#00#00#00
//  Value=Version
//  REG_SZ~*#2300
//isolation_writecopy HKEY_CURRENT_USER\Software\Microsoft\Internet Explorer\Approved Extensions
//  Value={3A56619B-37AC-40DA-833E-410F3BEDCBDC}
//  REG_BINARY=#51#66#7a#6c#4c#1d#3b#1b#8b#7c#46#27#9e#64#b5#0b#98#36#07#4f#39#af#86#c9

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif
#include "afxmt.h"
#include "resource.h"
#include "ProgressDlg.h"
#include "locale.h"
#include <Tlhelp32.h>

#include <shlwapi.h>
#pragma comment(lib, "ShLwApi.Lib")
#pragma comment(lib, "Version.lib")

#define WM_SETTINGSDIALOG_CLOSE				WM_USER+5
#define ID_SETTING_OK 32770

#define MSG_THREAD_START WM_USER+129
#define PROC_RDP	0
#define PROC_VMWARE	1
#define PROC_CITRIX	2
#define PROC_LDefaultBrowser	3
#define PROC_LIE	4
#define PROC_LFirefox	5
#define PROC_LChrome	6
#define PROC_LEdge		7
#define PROC_LCustom	8

#define KEY_COMB_SHIFT		0x00000001
#define KEY_COMB_CTRL		0x00000010
#define KEY_COMB_ALT		0x00000100
#define KEY_COMB_LEFT		0x00001000
#define KEY_COMB_UP			0x00010000
#define KEY_COMB_RIGHT		0x00100000
#define KEY_COMB_DOWN		0x01000000

static TCHAR DEF_URLS[][12]=
{
	_T("http://"),
	_T("https://"),
	_T("ftp://"),
	_T("file://"),
	_T("mailto:"),
	_T("news:"),
	_T("nntp://"),
	_T("prospero://"),
	_T("telnet://"),
	_T("wais://"),
	_T("ms-help://"),
	_T("gopher://"),
	_T("about:"),
	_T("mk:@"),
	_T("\\\\"),
};
static TCHAR DEF_SCRIPT[][11]=
{
	_T("javascript"),
	_T("script"),
	_T("vbscript"),
	_T("jscript"),
};

namespace SBUtil
{
	static inline void Split(CStringArray& strArray,CString strTarget,LPCTSTR strDelimiter)
	{
		UINT intDelimiterLen=0;
		int intStart=0;
		int intEnd=0;
		strTarget += strDelimiter;
		intDelimiterLen = (UINT)_tcslen(strDelimiter);
		intStart = 0;
		while (intEnd = strTarget.Find(strDelimiter, intStart), intEnd >= 0)
		{
			strArray.Add(strTarget.Mid(intStart, intEnd - intStart));
			intStart = intEnd + intDelimiterLen;
		}
		return;
	}
	static inline void Split_TrimBlank(CStringArray& strArray,CString strTarget,LPCTSTR strDelimiter)
	{
		UINT intDelimiterLen=0;
		int intStart=0;
		int intEnd=0;
		strTarget += strDelimiter;
		intDelimiterLen = (UINT)_tcslen(strDelimiter);
		intStart = 0;
		CString strTemp;
		while (intEnd = strTarget.Find(strDelimiter, intStart), intEnd >= 0)
		{
			strTemp=strTarget.Mid(intStart, intEnd - intStart);
			strTemp.TrimLeft();
			strTemp.TrimRight();
			if(!strTemp.IsEmpty())
				strArray.Add(strTemp);
			intStart = intEnd + intDelimiterLen;
		}
		return;
	}
	static inline BOOL IsURL(LPCTSTR str)
	{
		if(str==NULL)
			return FALSE;
		CString strCheckStr = str;
		strCheckStr.MakeLower();
		if(	strCheckStr.Find(DEF_URLS[ 0])==0 ||
			strCheckStr.Find(DEF_URLS[ 1])==0 ||
			strCheckStr.Find(DEF_URLS[ 2])==0 ||
			strCheckStr.Find(DEF_URLS[ 3])==0 ||
			strCheckStr.Find(DEF_URLS[ 4])==0 ||
			strCheckStr.Find(DEF_URLS[ 5])==0 ||
			strCheckStr.Find(DEF_URLS[ 6])==0 ||
			strCheckStr.Find(DEF_URLS[ 7])==0 ||
			strCheckStr.Find(DEF_URLS[ 8])==0 ||
			strCheckStr.Find(DEF_URLS[ 9])==0 ||
			strCheckStr.Find(DEF_URLS[10])==0 ||
			strCheckStr.Find(DEF_URLS[11])==0 ||
			strCheckStr.Find(DEF_URLS[12])==0 ||
			strCheckStr.Find(DEF_URLS[13])==0 ||
			strCheckStr.Find(DEF_URLS[14])==0)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		return FALSE;
	}

	static inline BOOL IsURL_HTTP(LPCTSTR str)
	{
		if(str==NULL)
			return FALSE;
		CString strCheckStr = str;
		if(	strCheckStr.Find(DEF_URLS[ 0])==0 || //http
			strCheckStr.Find(DEF_URLS[ 1])==0 //https
		  )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		return FALSE;
	}
	inline static BOOL IsScript(LPCTSTR strURL)
	{
		if(strURL==NULL)
			return FALSE;
		CString strCheckStr = strURL;
		strCheckStr.MakeLower();
		if(	strCheckStr.Find(DEF_SCRIPT[0])==0 ||
			strCheckStr.Find(DEF_SCRIPT[1])==0 ||
			strCheckStr.Find(DEF_SCRIPT[2])==0 ||
			strCheckStr.Find(DEF_SCRIPT[3])==0 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		return FALSE;
	}
	static void GetDivChar(LPCTSTR str,int size,CString& strRet,BOOL bAppend=TRUE)
	{
		strRet = str;
		CString str1=str;
		int iTabStrLen = size;
		LPCTSTR szEllipsis = _T("...");
		if(strRet.GetLength() > iTabStrLen)
		{
			if(bAppend)
				strRet = str1.Mid(0,iTabStrLen) + szEllipsis;
			else
				strRet = str1.Mid(0,iTabStrLen);
		}
		return;
	}
	static inline CString Trim_URLOnly(LPCTSTR str)
	{
		CString strRet;
		CString strTemp;
		CString strTemp2;
		strTemp = str;
		if(!strTemp.IsEmpty())
		{
			int iQPos=0;
			int iShPos=0;
			iQPos = strTemp.Find(_T("?"));
			if(iQPos > -1)
			{
				strTemp2=strTemp.Mid(0,iQPos);
				strTemp = strTemp2;
			}
			iShPos = strTemp.Find(_T("#"));
			if(iShPos > -1)
			{
				strTemp2=strTemp.Mid(0,iShPos);
				strTemp = strTemp2;
			}
			strRet = strTemp;
		}
		return strRet;
	}
	inline BOOL InThinApp()
	{
		BOOL bRet = FALSE;
		TCHAR szTargetPath[512] = { 0 };
		if (GetEnvironmentVariable(_T("TS_ORIGIN"), szTargetPath, 512))
		{
			if (lstrlen(szTargetPath) > 0)
			{
				//レジストリもチェックする。
				HKEY  hKey = { 0 };
				LONG lResult = 0L;
				lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("FS"), 0, KEY_READ, &hKey);
				if (lResult == ERROR_SUCCESS)
				{
					RegCloseKey(hKey);
					bRet = TRUE;
				}
			}
		}
		return bRet;
	}
};
class CIconHelper
{
private:
	HICON m_hICON;
public:
	CIconHelper(){m_hICON=NULL;}
	~CIconHelper(){Release();}
	void Release(){
		if(m_hICON)
		{
			::DestroyIcon(m_hICON);
			m_hICON=NULL;
		}
	}
	void SetIcon(const HICON ico){
		if(ico)
		{
			Release();
			m_hICON=ico;
		}
	}
	HICON GetIcon(){
		return m_hICON;
	}
	operator HICON() { return GetIcon(); }
	HICON operator=(HICON ico ) {
		SetIcon(ico);
		return GetIcon();
	}
};

#pragma comment(lib, "Psapi.lib") 
#define PSAPI_VERSION 1
#include "Psapi.h"
#include <TlHelp32.h>

class CCRre : public CWinThread
{
	DECLARE_DYNCREATE(CCRre)
private:
	CEvent m_event; // 同期オブジェクト
//	HWND m_hWndParent;
public:
	ProgressDlg* m_pDlgMsg;
	CCRre(HWND hwnd);  // 動的生成に使用されるプロテクト コンストラクタ
	CCRre();           // 動的生成に使用されるプロテクト コンストラクタ
	virtual ~CCRre();
	void Exec();
	CString m_strFilePath;
	BOOL bRun;
	DWORD m_dwThreadId;
	UINT m_iSolutionType;
	CString m_strHorizon_ConnectionServerName;
	CString m_strHorizon_AppName;
	CString m_strCitrix_AppName;
	CString m_strURL;
	CString m_strCustomBrowserPath;

public:
	void IEStart(CString& strURL);
	void FirefoxStart(CString& strURL);
	void ChromeStart(CString& strURL);
	void EdgeStart(CString& strURL);
	void DefaultBrowserStart(CString& strURL);
	void CustomBrowserStart(CString& strPath,CString& strURL);
	CString Get_ViewClient_ProtocolCommand();
	CString Get_Citrix_SelfserviceEXEPath();
	void VMwareViewStart(CString& strCommand);
	void CitrixXenAppStart(CString& strCommand);

public:
	//{{AFX_VIRTUAL(CCRre)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CCRre)
	//}}AFX_MSG
	afx_msg void OnExec(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ZONE_NA        0x0000
#define INTRANET_ZONE  0x0001
#define TRUSTED_ZONE   0x0002
#define INTERNET_ZONE  0x0004
#define UNTRUSTED_ZONE 0x0008
//16
//32

class CURLRedirectDataClass
{
public:
	CURLRedirectDataClass()
	{
		m_bDisabled=FALSE;
		m_bTopPageOnly=FALSE;
		m_dRedirectPageAction=0;
		m_dwCloseTimeout=3;
	}
	virtual ~CURLRedirectDataClass()
	{
		;
	}
	void Clear()
	{
		m_bDisabled=FALSE;
		m_bTopPageOnly=FALSE;
		m_dRedirectPageAction=0;
		m_dwCloseTimeout=3;
		m_strExecType.Empty();
		m_strExecExeFullPath.Empty();
		m_arr_URL.RemoveAll();
		m_arr_URL_EX.RemoveAll();
		m_arr_ExcludeGroup.RemoveAll();
	}
	void Copy(CURLRedirectDataClass* ptr)
	{
		if(ptr==NULL)return;
		m_bDisabled = ptr->m_bDisabled;
		m_bTopPageOnly = ptr->m_bTopPageOnly;
		m_dRedirectPageAction = ptr->m_dRedirectPageAction;
		m_dwCloseTimeout = ptr->m_dwCloseTimeout;
		m_strExecType = ptr->m_strExecType;
		m_strExecExeFullPath = ptr->m_strExecExeFullPath;
		m_arr_URL.Copy(ptr->m_arr_URL);
		m_arr_URL_EX.Copy(ptr->m_arr_URL_EX);
		m_arr_ExcludeGroup.Copy(ptr->m_arr_ExcludeGroup);
	}
public:
	BOOL m_bDisabled;
	BOOL m_bTopPageOnly;
	DWORD m_dRedirectPageAction;
	DWORD m_dwCloseTimeout;

	CString m_strExecType;
	CString m_strExecExeFullPath;
	CStringArray m_arr_URL;
	CStringArray m_arr_URL_EX;
	CStringArray m_arr_ExcludeGroup;

};

class CURLRedirectList
{
public:
	CPtrArray m_arr_RedirectBrowser;
	BOOL m_bURLOnly;
	BOOL m_bTraceLog;
	BOOL m_bQuickRedirect;
	BOOL m_bTopURLOnly;

	CString m_strHorizon_ConnectionServerName;
	CString m_strHorizon_AppName;
	CString m_strCitrix_AppName;

	CString m_strRDP_ServerName;
	BOOL m_bRemoteAppMode;
	CString m_strRemoteAppName;
	CString m_strRemoteAppPath;
	CString m_strRemoteAppCommandLine;
	BOOL m_bRedirect_Clipboard;
	BOOL m_bRedirect_Printer;
	BOOL m_bRedirect_Drive;

	BOOL m_bDisableIE_DDE;

	CURLRedirectDataClass* m_pRDP;
	CURLRedirectDataClass* m_pVMW;
	CURLRedirectDataClass* m_pCTX;
	CURLRedirectDataClass* m_pFirefox;
	CURLRedirectDataClass* m_pChrome;
	CURLRedirectDataClass* m_pEdge;
	CURLRedirectDataClass* m_pIE;
	CURLRedirectDataClass* m_pDefault;

	CURLRedirectDataClass* m_pCustom01;
	CURLRedirectDataClass* m_pCustom02;
	CURLRedirectDataClass* m_pCustom03;
	CURLRedirectDataClass* m_pCustom04;
	CURLRedirectDataClass* m_pCustom05;
	CURLRedirectDataClass* m_pCustom06;
	CURLRedirectDataClass* m_pCustom07;
	CURLRedirectDataClass* m_pCustom08;
	CURLRedirectDataClass* m_pCustom09;
	CURLRedirectDataClass* m_pCustom10;
	CURLRedirectDataClass* m_pCustom11;
	CURLRedirectDataClass* m_pCustom12;
	CURLRedirectDataClass* m_pCustom13;
	CURLRedirectDataClass* m_pCustom14;
	CURLRedirectDataClass* m_pCustom15;
	CURLRedirectDataClass* m_pCustom16;
	CURLRedirectDataClass* m_pCustom17;
	CURLRedirectDataClass* m_pCustom18;
	CURLRedirectDataClass* m_pCustom19;
	CURLRedirectDataClass* m_pCustom20;


	CURLRedirectList()
	{
		m_bURLOnly=FALSE;
		m_bTraceLog=FALSE;
		m_pRDP=NULL;
		m_pVMW=NULL;
		m_pCTX=NULL;
		m_pFirefox=NULL;
		m_pChrome=NULL;
		m_pEdge=NULL;
		m_pIE=NULL;
		m_pDefault=NULL;
		m_pCustom01=NULL;
		m_pCustom02=NULL;
		m_pCustom03=NULL;
		m_pCustom04=NULL;
		m_pCustom05=NULL;
		m_pCustom06=NULL;
		m_pCustom07=NULL;
		m_pCustom08=NULL;
		m_pCustom09=NULL;
		m_pCustom10=NULL;
		m_pCustom11=NULL;
		m_pCustom12=NULL;
		m_pCustom13=NULL;
		m_pCustom14=NULL;
		m_pCustom15=NULL;
		m_pCustom16=NULL;
		m_pCustom17=NULL;
		m_pCustom18=NULL;
		m_pCustom19=NULL;
		m_pCustom20=NULL;
		m_bQuickRedirect = FALSE;
		m_bTopURLOnly=FALSE;
		m_bRemoteAppMode = TRUE;
		m_bRedirect_Clipboard = FALSE;
		m_bRedirect_Printer = FALSE;
		m_bRedirect_Drive = FALSE;
		m_bDisableIE_DDE=FALSE;
	}
	virtual ~CURLRedirectList()
	{
		Clear();
	}
	void Clear()
	{
		m_bURLOnly=FALSE;
		m_bTraceLog=FALSE;
		CURLRedirectDataClass* ptr=NULL;
		int imax = (int)m_arr_RedirectBrowser.GetCount();
		for(int i=0;i<imax;i++)
		{
			ptr=(CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
			if(ptr)
			{
				delete ptr;
				ptr=NULL;
			}
		}
		m_arr_RedirectBrowser.RemoveAll();

		m_pRDP=NULL;
		m_pVMW=NULL;
		m_pCTX=NULL;
		m_pFirefox=NULL;
		m_pChrome=NULL;
		m_pEdge=NULL;
		m_pIE=NULL;
		m_pDefault=NULL;
		m_pCustom01=NULL;
		m_pCustom02=NULL;
		m_pCustom03=NULL;
		m_pCustom04=NULL;
		m_pCustom05=NULL;
		m_pCustom06=NULL;
		m_pCustom07=NULL;
		m_pCustom08=NULL;
		m_pCustom09=NULL;
		m_pCustom10=NULL;
		m_pCustom11=NULL;
		m_pCustom12=NULL;
		m_pCustom13=NULL;
		m_pCustom14=NULL;
		m_pCustom15=NULL;
		m_pCustom16=NULL;
		m_pCustom17=NULL;
		m_pCustom18=NULL;
		m_pCustom19=NULL;
		m_pCustom20=NULL;
		m_bQuickRedirect = FALSE;
		m_bTopURLOnly=FALSE;
		m_strHorizon_ConnectionServerName.Empty();
		m_strHorizon_AppName.Empty();
		m_strCitrix_AppName.Empty();

		m_strRDP_ServerName.Empty();
		m_strRemoteAppName.Empty();
		m_strRemoteAppPath.Empty();
		m_strRemoteAppCommandLine.Empty();
		m_bRemoteAppMode=TRUE;
		m_bRedirect_Clipboard=FALSE;
		m_bRedirect_Printer=FALSE;
		m_bRedirect_Drive=FALSE;
		m_bDisableIE_DDE=FALSE;
	}
	void InitSaveDataAll(BOOL bCitrixCustomEnv =FALSE)
	{
		Clear();
		CURLRedirectDataClass* pRedirectData=NULL;

		//Citrix特殊環境フラグ
		if (bCitrixCustomEnv)
		{
			m_bURLOnly=TRUE;
			m_bQuickRedirect=TRUE;
			m_bTopURLOnly=TRUE;

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled = FALSE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType = _T("Chrome");
			pRedirectData->m_dwCloseTimeout = 3;
			m_pChrome = pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled = FALSE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType = _T("CUSTOM18");
			pRedirectData->m_dwCloseTimeout = 3;
			m_pCustom18 = pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled = FALSE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType = _T("CUSTOM19");
			pRedirectData->m_dwCloseTimeout = 3;
			m_pCustom19 = pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled = FALSE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType = _T("CUSTOM20");
			pRedirectData->m_dwCloseTimeout = 3;
			m_pCustom20 = pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);
		}
		else
		{
			m_bURLOnly = TRUE;
			m_bQuickRedirect = TRUE;
			m_bTopURLOnly = TRUE;


			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled=FALSE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType=_T("RDP");
			pRedirectData->m_dwCloseTimeout=3;
			m_pRDP=pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled=FALSE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType=_T("VMWARE");
			pRedirectData->m_dwCloseTimeout=3;
			m_pVMW=pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled=FALSE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType=_T("CITRIX");
			pRedirectData->m_dwCloseTimeout=3;
			m_pCTX=pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled=FALSE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType=_T("Firefox");
			pRedirectData->m_dwCloseTimeout=3;
			m_pFirefox=pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled = FALSE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType = _T("Chrome");
			pRedirectData->m_dwCloseTimeout = 3;
			m_pChrome = pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled=FALSE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType=_T("Edge");
			pRedirectData->m_dwCloseTimeout=3;
			m_pEdge=pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled = FALSE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType = _T("IE");
			pRedirectData->m_dwCloseTimeout = 3;
			m_pIE = pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled = FALSE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType = _T("Default");
			pRedirectData->m_dwCloseTimeout = 3;
			pRedirectData->m_strExecExeFullPath= _T("IE");
			m_pDefault = pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled=TRUE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType=_T("CUSTOM01");
			pRedirectData->m_dwCloseTimeout=3;
			m_pCustom01=pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled=TRUE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType=_T("CUSTOM02");
			pRedirectData->m_dwCloseTimeout=3;
			m_pCustom02=pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled=TRUE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType=_T("CUSTOM03");
			pRedirectData->m_dwCloseTimeout=3;
			m_pCustom03=pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled=TRUE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType=_T("CUSTOM04");
			pRedirectData->m_dwCloseTimeout=3;
			m_pCustom04=pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled=TRUE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType=_T("CUSTOM05");
			pRedirectData->m_dwCloseTimeout=3;
			m_pCustom05=pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled = FALSE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType = _T("CUSTOM18");
			pRedirectData->m_dwCloseTimeout = 3;
			m_pCustom18 = pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled = TRUE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType = _T("CUSTOM19");
			pRedirectData->m_dwCloseTimeout = 3;
			m_pCustom19 = pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			pRedirectData = NULL;
			pRedirectData = new CURLRedirectDataClass;
			pRedirectData->m_bDisabled = TRUE;
			pRedirectData->m_bTopPageOnly = TRUE;
			pRedirectData->m_strExecType = _T("CUSTOM20");
			pRedirectData->m_dwCloseTimeout = 3;
			m_pCustom20 = pRedirectData;
			m_arr_RedirectBrowser.Add(pRedirectData);

			//pRedirectData = NULL;
			//pRedirectData = new CURLRedirectDataClass;
			//pRedirectData->m_bDisabled=TRUE;
			//pRedirectData->m_strExecType=_T("CUSTOM06");
			//pRedirectData->m_dwCloseTimeout=3;
			//m_pCustom06=pRedirectData;
			//m_arr_RedirectBrowser.Add(pRedirectData);

			//pRedirectData = NULL;
			//pRedirectData = new CURLRedirectDataClass;
			//pRedirectData->m_bDisabled=TRUE;
			//pRedirectData->m_strExecType=_T("CUSTOM07");
			//pRedirectData->m_dwCloseTimeout=3;
			//m_pCustom07=pRedirectData;
			//m_arr_RedirectBrowser.Add(pRedirectData);

			//pRedirectData = NULL;
			//pRedirectData = new CURLRedirectDataClass;
			//pRedirectData->m_bDisabled=TRUE;
			//pRedirectData->m_strExecType=_T("CUSTOM08");
			//pRedirectData->m_dwCloseTimeout=3;
			//m_pCustom08=pRedirectData;
			//m_arr_RedirectBrowser.Add(pRedirectData);

			//pRedirectData = NULL;
			//pRedirectData = new CURLRedirectDataClass;
			//pRedirectData->m_bDisabled=TRUE;
			//pRedirectData->m_strExecType=_T("CUSTOM09");
			//pRedirectData->m_dwCloseTimeout=3;
			//m_pCustom09=pRedirectData;
			//m_arr_RedirectBrowser.Add(pRedirectData);

			//pRedirectData = NULL;
			//pRedirectData = new CURLRedirectDataClass;
			//pRedirectData->m_bDisabled=TRUE;
			//pRedirectData->m_strExecType=_T("CUSTOM10");
			//pRedirectData->m_dwCloseTimeout=3;
			//m_pCustom10=pRedirectData;
			//m_arr_RedirectBrowser.Add(pRedirectData);

			//pRedirectData = NULL;
			//pRedirectData = new CURLRedirectDataClass;
			//pRedirectData->m_bDisabled=TRUE;
			//pRedirectData->m_strExecType=_T("CUSTOM11");
			//pRedirectData->m_dwCloseTimeout=3;

			//m_pCustom11=pRedirectData;
			//m_arr_RedirectBrowser.Add(pRedirectData);

			//pRedirectData = NULL;
			//pRedirectData = new CURLRedirectDataClass;
			//pRedirectData->m_bDisabled=TRUE;
			//pRedirectData->m_strExecType=_T("CUSTOM12");
			//pRedirectData->m_dwCloseTimeout=3;
			//m_pCustom12=pRedirectData;
			//m_arr_RedirectBrowser.Add(pRedirectData);

			//pRedirectData = NULL;
			//pRedirectData = new CURLRedirectDataClass;
			//pRedirectData->m_bDisabled=TRUE;
			//pRedirectData->m_strExecType=_T("CUSTOM13");
			//pRedirectData->m_dwCloseTimeout=3;
			//m_pCustom13=pRedirectData;
			//m_arr_RedirectBrowser.Add(pRedirectData);

			//pRedirectData = NULL;
			//pRedirectData = new CURLRedirectDataClass;
			//pRedirectData->m_bDisabled=TRUE;
			//pRedirectData->m_strExecType=_T("CUSTOM14");
			//pRedirectData->m_dwCloseTimeout=3;
			//m_pCustom14=pRedirectData;
			//m_arr_RedirectBrowser.Add(pRedirectData);

			//pRedirectData = NULL;
			//pRedirectData = new CURLRedirectDataClass;
			//pRedirectData->m_bDisabled=TRUE;
			//pRedirectData->m_strExecType=_T("CUSTOM15");
			//pRedirectData->m_dwCloseTimeout=3;
			//m_pCustom15=pRedirectData;
			//m_arr_RedirectBrowser.Add(pRedirectData);

			//pRedirectData = NULL;
			//pRedirectData = new CURLRedirectDataClass;
			//pRedirectData->m_bDisabled=TRUE;
			//pRedirectData->m_strExecType=_T("CUSTOM16");
			//pRedirectData->m_dwCloseTimeout=3;
			//m_pCustom16=pRedirectData;
			//m_arr_RedirectBrowser.Add(pRedirectData);

			//pRedirectData = NULL;
			//pRedirectData = new CURLRedirectDataClass;
			//pRedirectData->m_bDisabled=TRUE;
			//pRedirectData->m_strExecType=_T("CUSTOM17");
			//pRedirectData->m_dwCloseTimeout=3;
			//m_pCustom17=pRedirectData;
			//m_arr_RedirectBrowser.Add(pRedirectData);
		}
	}


	BOOL SaveData(LPCTSTR lPath,CString& pstrOutPutData)
	{
		if(lPath==NULL)return FALSE;
		BOOL bRet=FALSE;
		CStdioFile out;
		if(out.Open(lPath,CFile::modeWrite|CFile::modeCreate|CFile::shareDenyNone) )
		{
			CURLRedirectDataClass* ptr=NULL;
			CString strWriteLine;
			CString strTemp;
			CString strTempFormat;

			strWriteLine=_T("#####################################################################\n");
			out.WriteString(strWriteLine);
			pstrOutPutData=strWriteLine;

			strWriteLine=_T("## Configuration File for ThinBridge (ver2.0)\n");
			out.WriteString(strWriteLine);
			pstrOutPutData+=strWriteLine;

			CTime time = CTime::GetCurrentTime();
			strWriteLine.Format(_T("## %s\n"),time.Format(_T("%Y-%m-%d %X")));
			out.WriteString(strWriteLine);
			pstrOutPutData+=strWriteLine;

			strWriteLine=_T("#####################################################################\n\n");
			out.WriteString(strWriteLine);
			pstrOutPutData+=strWriteLine;

			strTempFormat=_T("[GLOBAL]\n");
			out.WriteString(strTempFormat);
			pstrOutPutData+=strTempFormat;
			strTempFormat=_T("@DISABLED\n");
			out.WriteString(strTempFormat);
			pstrOutPutData+=strTempFormat;

			if(m_bURLOnly)
			{
				strTempFormat=_T("@TOP_PAGE_ONLY\n");
				out.WriteString(strTempFormat);
				pstrOutPutData+=strTempFormat;

			}
			if(m_bTraceLog)
			{
				strTempFormat=_T("@REDIRECT_PAGE_ACTION:1\n");
				out.WriteString(strTempFormat);
				pstrOutPutData+=strTempFormat;
			}
			if (m_bQuickRedirect)
			{
				strTempFormat = _T("@INTRANET_ZONE\n");
				out.WriteString(strTempFormat);
				pstrOutPutData += strTempFormat;
			}
			//共用URLなどを有効にするためのフラグセット
			{
				strTempFormat = _T("@TRUSTED_ZONE\n");
				out.WriteString(strTempFormat);
				pstrOutPutData += strTempFormat;
			}
			if (m_bTopURLOnly)
			{
				strTempFormat = _T("@UNTRUSTED_ZONE\n");
				out.WriteString(strTempFormat);
				pstrOutPutData += strTempFormat;
			}

			//VMware Horizon
			if (!m_strHorizon_ConnectionServerName.IsEmpty())
			{
				strTempFormat.Format(_T("@VMW_SERVERNAME:%s\n"), m_strHorizon_ConnectionServerName);
				out.WriteString(strTempFormat);
				pstrOutPutData += strTempFormat;
			}
			if (!m_strHorizon_AppName.IsEmpty())
			{
				strTempFormat.Format(_T("@VMW_APPNAME:%s\n"), m_strHorizon_AppName);
				out.WriteString(strTempFormat);
				pstrOutPutData += strTempFormat;
			}

			//Citrix
			if (!m_strCitrix_AppName.IsEmpty())
			{
				strTempFormat.Format(_T("@CTX_APPNAME:%s\n"), m_strCitrix_AppName);
				out.WriteString(strTempFormat);
				pstrOutPutData += strTempFormat;
			}

			//RDP-------------------------------↓
			if (!m_strRDP_ServerName.IsEmpty())
			{
				strTempFormat.Format(_T("@RDP_SERVERNAME:%s\n"), m_strRDP_ServerName);
				out.WriteString(strTempFormat);
				pstrOutPutData += strTempFormat;
			}
			if (!m_strRemoteAppName.IsEmpty())
			{
				strTempFormat.Format(_T("@RDP_APPNAME:%s\n"), m_strRemoteAppName);
				out.WriteString(strTempFormat);
				pstrOutPutData += strTempFormat;
			}
			if (!m_strRemoteAppPath.IsEmpty())
			{
				strTempFormat.Format(_T("@RDP_APPPATH:%s\n"), m_strRemoteAppPath);
				out.WriteString(strTempFormat);
				pstrOutPutData += strTempFormat;
			}
			if (!m_strRemoteAppCommandLine.IsEmpty())
			{
				strTempFormat.Format(_T("@RDP_COMMANDLINE:%s\n"), m_strRemoteAppCommandLine);
				out.WriteString(strTempFormat);
				pstrOutPutData += strTempFormat;
			}

			if(m_bRemoteAppMode)
			{
				strTempFormat=_T("@RDP_APPMODE\n");
				out.WriteString(strTempFormat);
				pstrOutPutData += strTempFormat;
			}
			if(m_bRedirect_Clipboard)
			{
				strTempFormat=_T("@RDP_CLIPBOARD\n");
				out.WriteString(strTempFormat);
				pstrOutPutData += strTempFormat;
			}
			if(m_bRedirect_Printer)
			{
				strTempFormat=_T("@RDP_PRINTER\n");
				out.WriteString(strTempFormat);
				pstrOutPutData += strTempFormat;
			}
			if (m_bRedirect_Drive)
			{
				strTempFormat=_T("@RDP_DRIVE\n");
				out.WriteString(strTempFormat);
				pstrOutPutData += strTempFormat;
			}
			//RDP-------------------------------↑
			if (m_bDisableIE_DDE)
			{
				strTempFormat = _T("@DISABLE_IE_DDE\n");
				out.WriteString(strTempFormat);
				pstrOutPutData += strTempFormat;
			}


			int imax = (int)m_arr_RedirectBrowser.GetCount();
			for(int i=0;i<imax;i++)
			{
				out.WriteString(_T("\n"));
				pstrOutPutData += _T("\n");
				strWriteLine.Empty();
				strTemp.Empty();
				strTempFormat.Empty();
				ptr=(CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
				if(ptr)
				{
					strTemp=ptr->m_strExecType;
					if(strTemp.IsEmpty())
						continue;

					strTempFormat.Format(_T("[%s]\n"),strTemp);
					out.WriteString(strTempFormat);
					pstrOutPutData+=strTempFormat;

					strTemp=ptr->m_strExecExeFullPath;
					strTempFormat.Format(_T("@BROWSER_PATH:%s\n"),strTemp);
					out.WriteString(strTempFormat);
					pstrOutPutData+=strTempFormat;


					if(ptr->m_bDisabled)
					{
						strTempFormat=_T("@DISABLED\n");
						out.WriteString(strTempFormat);
						pstrOutPutData+=strTempFormat;

					}

					if (m_bTopURLOnly)
					{
						strTempFormat = _T("@TOP_PAGE_ONLY\n");
						out.WriteString(strTempFormat);
						pstrOutPutData += strTempFormat;
					}
					strTempFormat.Format(_T("@REDIRECT_PAGE_ACTION:%d\n"),ptr->m_dRedirectPageAction);
					out.WriteString(strTempFormat);
					pstrOutPutData+=strTempFormat;

					strTempFormat.Format(_T("@CLOSE_TIMEOUT:%d\n"),ptr->m_dwCloseTimeout);
					out.WriteString(strTempFormat);
					pstrOutPutData+=strTempFormat;

					int iMaxCnt=0;
					iMaxCnt = ptr->m_arr_ExcludeGroup.GetCount();
					for(int ii=0;ii<iMaxCnt;ii++)
					{
						strWriteLine.Empty();
						strWriteLine = ptr->m_arr_ExcludeGroup.GetAt(ii);
						strTempFormat.Format(_T("@EXCLUDE_GROUP:%s\n"),strWriteLine);
						out.WriteString(strTempFormat);
						pstrOutPutData+=strTempFormat;
					}
					iMaxCnt = ptr->m_arr_URL.GetCount();
					for(int ii=0;ii<iMaxCnt;ii++)
					{
						strWriteLine.Empty();
						strWriteLine = ptr->m_arr_URL.GetAt(ii);
						strTempFormat.Format(_T("%s\n"),strWriteLine);
						out.WriteString(strTempFormat);
						pstrOutPutData+=strTempFormat;
					}
					iMaxCnt = ptr->m_arr_URL_EX.GetCount();
					for(int iii=0;iii<iMaxCnt;iii++)
					{
						strWriteLine.Empty();
						strWriteLine = ptr->m_arr_URL_EX.GetAt(iii);
						strTempFormat.Format(_T("-%s\n"),strWriteLine);
						out.WriteString(strTempFormat);
						pstrOutPutData+=strTempFormat;
					}
				}
			}
			bRet=TRUE;
			out.Close();
		}
		return bRet;
	}
	void SetArrayData(LPCTSTR lPath, BOOL bCitrixCustomEnv = FALSE)
	{
		if(lPath==NULL)return;
		this->Clear();

		InitSaveDataAll(bCitrixCustomEnv);
		CStdioFile in;
		if(in.Open(lPath,CFile::modeRead|CFile::shareDenyNone))
		{
			CURLRedirectDataClass* pRedirectData=NULL;
			CString strTemp;
			CString strTempUpper;
			CString strExTemp;

			CString strExecExeFullPath;
			CString strExecType;
			CStringArray arr_URL;
			CStringArray arr_URL_EX;
			CStringArray arr_ExcludeGroup;
			BOOL bTopPageOnly=FALSE;
			DWORD dRedirectPageAction=0;
			DWORD dwCloseTimeout=3;

			DWORD dwZone=ZONE_NA;
			BOOL bDisabled=FALSE;
			BOOL bFirstSection=FALSE;
			while(in.ReadString(strTemp))
			{
				strTemp.TrimLeft();
				strTemp.TrimRight();
				strTempUpper=strTemp;
				strTempUpper.MakeUpper();
				//空白の場合は、次へ
				if(strTemp.IsEmpty())continue;

				//;だけはNG
				if(strTemp==_T(";"))continue;

				//#だけはNG
				if(strTemp==_T("#"))continue;

				//;-だけはNG
				if(strTemp==_T(";-"))continue;

				//#-だけはNG
				if(strTemp==_T("#-"))continue;

				//;@はNG
				if(strTemp.Find(_T(";@"))==0)continue;

				//#@はNG
				if(strTemp.Find(_T("#@"))==0)continue;

				//;[はNG
				if(strTemp.Find(_T(";["))==0)continue;

				//#[はNG
				if(strTemp.Find(_T("#["))==0)continue;

				if(strTemp.Find(_T("["))==0)
				{
					if(!strExecType.IsEmpty())
					{
						pRedirectData = new CURLRedirectDataClass;
						pRedirectData->m_bDisabled=bDisabled;

						pRedirectData->m_strExecType=strExecType;
						pRedirectData->m_strExecExeFullPath=strExecExeFullPath;
						pRedirectData->m_arr_URL.Copy(arr_URL);
						pRedirectData->m_arr_URL_EX.Copy(arr_URL_EX);
						pRedirectData->m_arr_ExcludeGroup.Copy(arr_ExcludeGroup);
						pRedirectData->m_bTopPageOnly=bTopPageOnly;
						pRedirectData->m_dRedirectPageAction=dRedirectPageAction;
						pRedirectData->m_dwCloseTimeout=dwCloseTimeout;
						
						if(strExecType.CompareNoCase(_T("GLOBAL"))==0)
						{
							m_bURLOnly=bTopPageOnly;
							m_bTraceLog=dRedirectPageAction==1?TRUE:FALSE;
							m_bQuickRedirect = (dwZone&INTRANET_ZONE) == INTRANET_ZONE ? TRUE : FALSE;
							m_bTopURLOnly = (dwZone&UNTRUSTED_ZONE) == UNTRUSTED_ZONE ? TRUE : FALSE;
						}
						else if(strExecType.CompareNoCase(_T("RDP"))==0 && m_pRDP)
							m_pRDP->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("VMWARE"))==0 && m_pVMW)
							m_pVMW->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CITRIX"))==0 && m_pCTX)
							m_pCTX->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("Firefox"))==0 && m_pFirefox)
							m_pFirefox->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("Chrome"))==0 && m_pChrome)
							m_pChrome->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("Edge"))==0 && m_pEdge)
							m_pEdge->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("IE"))==0 && m_pIE)
							m_pIE->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("Default"))==0 && m_pDefault)
							m_pDefault->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM01"))==0 && m_pCustom01)
							m_pCustom01->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM02"))==0 && m_pCustom02)
							m_pCustom02->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM03"))==0 && m_pCustom03)
							m_pCustom03->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM04"))==0 && m_pCustom04)
							m_pCustom04->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM05"))==0 && m_pCustom05)
							m_pCustom05->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM06"))==0 && m_pCustom06)
							m_pCustom06->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM07"))==0 && m_pCustom07)
							m_pCustom07->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM08"))==0 && m_pCustom08)
							m_pCustom08->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM09"))==0 && m_pCustom09)
							m_pCustom09->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM10"))==0 && m_pCustom10)
							m_pCustom10->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM11"))==0 && m_pCustom11)
							m_pCustom11->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM12"))==0 && m_pCustom12)
							m_pCustom12->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM13"))==0 && m_pCustom13)
							m_pCustom13->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM14"))==0 && m_pCustom14)
							m_pCustom14->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM15"))==0 && m_pCustom15)
							m_pCustom15->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM16"))==0 && m_pCustom16)
							m_pCustom16->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM17"))==0 && m_pCustom17)
							m_pCustom17->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM18"))==0 && m_pCustom18)
							m_pCustom18->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM19"))==0 && m_pCustom19)
							m_pCustom19->Copy(pRedirectData);
						else if(strExecType.CompareNoCase(_T("CUSTOM20"))==0 && m_pCustom20)
							m_pCustom20->Copy(pRedirectData);
						else
							;
						delete pRedirectData;
						pRedirectData=NULL;

						bTopPageOnly=FALSE;
						dRedirectPageAction=0;
						dwCloseTimeout=3;
						dwZone=ZONE_NA;
						strExecExeFullPath.Empty();
						strExecType.Empty();
						arr_URL.RemoveAll();
						arr_URL_EX.RemoveAll();
						arr_ExcludeGroup.RemoveAll();
						pRedirectData = NULL;
						bDisabled=FALSE;
					}
					strExecType=strTemp.Mid(1,strTemp.ReverseFind(']')-1);
					bFirstSection=TRUE;
					strTemp.Empty();
				}
				if(!bFirstSection)
					continue;
				if(strTemp.IsEmpty())
					continue;
				if(strTemp.Find(_T("@"))==0)
				{
					if(strTempUpper.Find(_T("@TOP_PAGE_ONLY"))==0)
					{
						bTopPageOnly = TRUE;
					}
					else if(strTempUpper.Find(_T("@REDIRECT_PAGE_ACTION:"))==0)
					{
						int iPosC=0;
						CString strDig;
						strDig = _T("@REDIRECT_PAGE_ACTION:");
						iPosC=strDig.GetLength();
						strDig = strTempUpper.Mid(iPosC);
						dRedirectPageAction = _ttoi(strDig);
						if(dRedirectPageAction < 0)dRedirectPageAction=0;
						else if(dRedirectPageAction > 2)dRedirectPageAction=2;
					}
					else if(strTempUpper.Find(_T("@CLOSE_TIMEOUT:"))==0)
					{
						int iPosC=0;
						CString strDig;
						strDig = _T("@CLOSE_TIMEOUT:");
						iPosC=strDig.GetLength();
						strDig = strTempUpper.Mid(iPosC);
						dwCloseTimeout = _ttoi(strDig);
						if(dwCloseTimeout < 1)dwCloseTimeout=3;
						else if(dwCloseTimeout > 60)dwCloseTimeout=60;
					}
					else if(strTempUpper.Find(_T("@BROWSER_PATH:"))==0)
					{
						int iPosC=0;
						CString strPathDig;
						strPathDig = _T("@BROWSER_PATH:");
						iPosC=strPathDig.GetLength();
						strPathDig = strTemp.Mid(iPosC);
						strPathDig.TrimLeft();
						strPathDig.TrimRight();
						strExecExeFullPath = strPathDig;
					}
					else if(strTempUpper.Find(_T("@DISABLED"))==0)
					{
						bDisabled = TRUE;
					}
					else if(strTempUpper.Find(_T("@INTRANET_ZONE"))==0)
					{
						dwZone |= INTRANET_ZONE;
					}
					else if(strTempUpper.Find(_T("@TRUSTED_ZONE"))==0)
					{
						dwZone |= TRUSTED_ZONE;
					}
					else if(strTempUpper.Find(_T("@INTERNET_ZONE"))==0)
					{
						dwZone |= INTERNET_ZONE;
					}
					else if(strTempUpper.Find(_T("@UNTRUSTED_ZONE"))==0)
					{
						dwZone |= UNTRUSTED_ZONE;
					}

					//VMware Horizon
					else if (strTempUpper.Find(_T("@VMW_SERVERNAME:")) == 0)
					{
						int iPosC = 0;
						CString strPathDig;
						strPathDig = _T("@VMW_SERVERNAME:");
						iPosC = strPathDig.GetLength();
						strPathDig = strTemp.Mid(iPosC);
						strPathDig.TrimLeft();
						strPathDig.TrimRight();
						m_strHorizon_ConnectionServerName = strPathDig;
					}
					else if (strTempUpper.Find(_T("@VMW_APPNAME:")) == 0)
					{
						int iPosC = 0;
						CString strPathDig;
						strPathDig = _T("@VMW_APPNAME:");
						iPosC = strPathDig.GetLength();
						strPathDig = strTemp.Mid(iPosC);
						strPathDig.TrimLeft();
						strPathDig.TrimRight();
						m_strHorizon_AppName = strPathDig;
					}

					//Citrix
					else if (strTempUpper.Find(_T("@CTX_APPNAME:")) == 0)
					{
						int iPosC = 0;
						CString strPathDig;
						strPathDig = _T("@CTX_APPNAME:");
						iPosC = strPathDig.GetLength();
						strPathDig = strTemp.Mid(iPosC);
						strPathDig.TrimLeft();
						strPathDig.TrimRight();
						m_strCitrix_AppName = strPathDig;
					}

					//RDP-------------------------------↓
					else if (strTempUpper.Find(_T("@RDP_SERVERNAME:")) == 0)
					{
						int iPosC = 0;
						CString strPathDig;
						strPathDig = _T("@RDP_SERVERNAME:");
						iPosC = strPathDig.GetLength();
						strPathDig = strTemp.Mid(iPosC);
						strPathDig.TrimLeft();
						strPathDig.TrimRight();
						m_strRDP_ServerName = strPathDig;
					}
					else if (strTempUpper.Find(_T("@RDP_APPNAME:")) == 0)
					{
						int iPosC = 0;
						CString strPathDig;
						strPathDig = _T("@RDP_APPNAME:");
						iPosC = strPathDig.GetLength();
						strPathDig = strTemp.Mid(iPosC);
						strPathDig.TrimLeft();
						strPathDig.TrimRight();
						m_strRemoteAppName = strPathDig;
					}
					else if (strTempUpper.Find(_T("@RDP_APPPATH:")) == 0)
					{
						int iPosC = 0;
						CString strPathDig;
						strPathDig = _T("@RDP_APPPATH:");
						iPosC = strPathDig.GetLength();
						strPathDig = strTemp.Mid(iPosC);
						strPathDig.TrimLeft();
						strPathDig.TrimRight();
						m_strRemoteAppPath = strPathDig;
					}
					else if (strTempUpper.Find(_T("@RDP_COMMANDLINE:")) == 0)
					{
						int iPosC = 0;
						CString strPathDig;
						strPathDig = _T("@RDP_COMMANDLINE:");
						iPosC = strPathDig.GetLength();
						strPathDig = strTemp.Mid(iPosC);
						strPathDig.TrimLeft();
						strPathDig.TrimRight();
						m_strRemoteAppCommandLine = strPathDig;
					}
					else if (strTempUpper.Find(_T("@RDP_APPMODE")) == 0)
					{
						m_bRemoteAppMode = TRUE;
					}
					else if (strTempUpper.Find(_T("@RDP_CLIPBOARD")) == 0)
					{
						m_bRedirect_Clipboard = TRUE;
					}
					else if (strTempUpper.Find(_T("@RDP_PRINTER")) == 0)
					{
						m_bRedirect_Printer = TRUE;
					}
					else if (strTempUpper.Find(_T("@RDP_DRIVE")) == 0)
					{
						m_bRedirect_Drive = TRUE;
					}
					//RDP-------------------------------↑
					else if (strTempUpper.Find(_T("@DISABLE_IE_DDE")) == 0)
					{
						m_bDisableIE_DDE = TRUE;
					}
					else if (strTempUpper.Find(_T("@EXCLUDE_GROUP")) == 0)
					{
						int iPosC = 0;
						CString strName;
						strName = _T("@EXCLUDE_GROUP:");
						iPosC = strName.GetLength();
						strName = strTemp.Mid(iPosC);
						strName.TrimLeft();
						strName.TrimRight();
						arr_ExcludeGroup.Add(strName);
					}
				}
				//;-はEx #-はEx
				else if(strTemp.Find(_T(";-"))==0||strTemp.Find(_T("#-"))==0)
				{
					strExTemp = _T("#");
					strExTemp += strTemp.Mid(2);
					arr_URL_EX.Add(strExTemp);
				}
				else if(strTemp.Find(_T("-"))==0)
				{
					arr_URL_EX.Add(strTemp.Mid(1));
				}
				else
				{
					arr_URL.Add(strTemp);
				}
			}
			in.Close();
			if(!strExecType.IsEmpty())
			{
				if(pRedirectData==NULL)
				{
					pRedirectData = new CURLRedirectDataClass;
					pRedirectData->m_bDisabled=bDisabled;
					pRedirectData->m_strExecType=strExecType;
					pRedirectData->m_strExecExeFullPath=strExecExeFullPath;
					pRedirectData->m_arr_URL.Copy(arr_URL);
					pRedirectData->m_arr_URL_EX.Copy(arr_URL_EX);
					pRedirectData->m_arr_ExcludeGroup.Copy(arr_ExcludeGroup);
					pRedirectData->m_bTopPageOnly=bTopPageOnly;
					pRedirectData->m_dRedirectPageAction=dRedirectPageAction;
					pRedirectData->m_dwCloseTimeout=dwCloseTimeout;
					
					if(strExecType.CompareNoCase(_T("GLOBAL"))==0)
					{
						m_bURLOnly=bTopPageOnly;
						m_bTraceLog=dRedirectPageAction==1?TRUE:FALSE;
						m_bQuickRedirect = (dwZone&INTRANET_ZONE) == INTRANET_ZONE ? TRUE : FALSE;
						m_bTopURLOnly = (dwZone&UNTRUSTED_ZONE) == UNTRUSTED_ZONE ? TRUE : FALSE;
					}
					else if(strExecType.CompareNoCase(_T("RDP"))==0)
						m_pRDP->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("VMWARE"))==0)
						m_pVMW->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CITRIX"))==0)
						m_pCTX->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("Firefox"))==0)
						m_pFirefox->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("Chrome"))==0)
						m_pChrome->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("Edge"))==0)
						m_pEdge->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("IE"))==0)
						m_pIE->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("Default"))==0)
						m_pDefault->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM01"))==0)
						m_pCustom01->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM02"))==0)
						m_pCustom02->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM03"))==0)
						m_pCustom03->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM04"))==0)
						m_pCustom04->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM05"))==0)
						m_pCustom05->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM06"))==0)
						m_pCustom06->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM07"))==0)
						m_pCustom07->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM08"))==0)
						m_pCustom08->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM09"))==0)
						m_pCustom09->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM10"))==0)
						m_pCustom10->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM11"))==0)
						m_pCustom11->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM12"))==0)
						m_pCustom12->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM13"))==0)
						m_pCustom13->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM14"))==0)
						m_pCustom14->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM15"))==0)
						m_pCustom15->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM16"))==0)
						m_pCustom16->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM17"))==0)
						m_pCustom17->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM18"))==0)
						m_pCustom18->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM19"))==0)
						m_pCustom19->Copy(pRedirectData);
					else if(strExecType.CompareNoCase(_T("CUSTOM20"))==0)
						m_pCustom20->Copy(pRedirectData);
					else
						;

					delete pRedirectData;
					pRedirectData=NULL;

					bTopPageOnly=FALSE;
					dRedirectPageAction=0;
					dwCloseTimeout=3;
					dwZone=ZONE_NA;
					strExecType.Empty();
					strExecExeFullPath.Empty();
					arr_URL.RemoveAll();
					arr_URL_EX.RemoveAll();
					arr_ExcludeGroup.RemoveAll();
					pRedirectData = NULL;
					bDisabled=FALSE;
				}
			}
		}
	}

	void SetGlobalData(LPCTSTR lPath)
	{
		if (lPath == NULL)return;
		this->Clear();

		CStdioFile in;
		if (in.Open(lPath, CFile::modeRead | CFile::shareDenyNone))
		{
			CString strTemp;
			CString strTempUpper;
			BOOL bGlobalFlg=FALSE;
			while (in.ReadString(strTemp))
			{
				strTemp.TrimLeft();
				strTemp.TrimRight();
				strTempUpper = strTemp;
				strTempUpper.MakeUpper();
				//空白の場合は、次へ
				if (strTemp.IsEmpty())continue;
				//#コメントはスキップ
				if (strTemp.Find(_T("#")) == 0)continue;

				if(!bGlobalFlg)
				{
					if (strTempUpper.Find(_T("[GLOBAL]")) == 0)
					{
						bGlobalFlg = TRUE;
						continue;
					}
				}
				if(bGlobalFlg)
				{
					if (strTemp.Find(_T("[RDP]")) == 0)
					{
						bGlobalFlg = FALSE;
						break;
					}
					//VMware Horizon
					if (strTempUpper.Find(_T("@VMW_SERVERNAME:")) == 0)
					{
						int iPosC = 0;
						CString strPathDig;
						strPathDig = _T("@VMW_SERVERNAME:");
						iPosC = strPathDig.GetLength();
						strPathDig = strTemp.Mid(iPosC);
						strPathDig.TrimLeft();
						strPathDig.TrimRight();
						m_strHorizon_ConnectionServerName = strPathDig;
					}
					else if (strTempUpper.Find(_T("@VMW_APPNAME:")) == 0)
					{
						int iPosC = 0;
						CString strPathDig;
						strPathDig = _T("@VMW_APPNAME:");
						iPosC = strPathDig.GetLength();
						strPathDig = strTemp.Mid(iPosC);
						strPathDig.TrimLeft();
						strPathDig.TrimRight();
						m_strHorizon_AppName = strPathDig;
					}

					//Citrix
					else if (strTempUpper.Find(_T("@CTX_APPNAME:")) == 0)
					{
						int iPosC = 0;
						CString strPathDig;
						strPathDig = _T("@CTX_APPNAME:");
						iPosC = strPathDig.GetLength();
						strPathDig = strTemp.Mid(iPosC);
						strPathDig.TrimLeft();
						strPathDig.TrimRight();
						m_strCitrix_AppName = strPathDig;
					}

					//RDP-------------------------------↓
					else if (strTempUpper.Find(_T("@RDP_SERVERNAME:")) == 0)
					{
						int iPosC = 0;
						CString strPathDig;
						strPathDig = _T("@RDP_SERVERNAME:");
						iPosC = strPathDig.GetLength();
						strPathDig = strTemp.Mid(iPosC);
						strPathDig.TrimLeft();
						strPathDig.TrimRight();
						m_strRDP_ServerName = strPathDig;
					}
					else if (strTempUpper.Find(_T("@RDP_APPNAME:")) == 0)
					{
						int iPosC = 0;
						CString strPathDig;
						strPathDig = _T("@RDP_APPNAME:");
						iPosC = strPathDig.GetLength();
						strPathDig = strTemp.Mid(iPosC);
						strPathDig.TrimLeft();
						strPathDig.TrimRight();
						m_strRemoteAppName = strPathDig;
					}
					else if (strTempUpper.Find(_T("@RDP_APPPATH:")) == 0)
					{
						int iPosC = 0;
						CString strPathDig;
						strPathDig = _T("@RDP_APPPATH:");
						iPosC = strPathDig.GetLength();
						strPathDig = strTemp.Mid(iPosC);
						strPathDig.TrimLeft();
						strPathDig.TrimRight();
						m_strRemoteAppPath = strPathDig;
					}
					else if (strTempUpper.Find(_T("@RDP_COMMANDLINE:")) == 0)
					{
						int iPosC = 0;
						CString strPathDig;
						strPathDig = _T("@RDP_COMMANDLINE:");
						iPosC = strPathDig.GetLength();
						strPathDig = strTemp.Mid(iPosC);
						strPathDig.TrimLeft();
						strPathDig.TrimRight();
						m_strRemoteAppCommandLine = strPathDig;
					}
					else if (strTempUpper.Find(_T("@RDP_APPMODE")) == 0)
					{
						m_bRemoteAppMode = TRUE;
					}
					else if (strTempUpper.Find(_T("@RDP_CLIPBOARD")) == 0)
					{
						m_bRedirect_Clipboard = TRUE;
					}
					else if (strTempUpper.Find(_T("@RDP_PRINTER")) == 0)
					{
						m_bRedirect_Printer = TRUE;
					}
					else if (strTempUpper.Find(_T("@RDP_DRIVE")) == 0)
					{
						m_bRedirect_Drive = TRUE;
					}
					//RDP-------------------------------↑
					else if (strTempUpper.Find(_T("@DISABLE_IE_DDE")) == 0)
					{
						m_bDisableIE_DDE = TRUE;
					}
				}
			}
			in.Close();
		}
	}
};
class CConfData
{
	public:
	CConfData()
	{
		m_iSolutionType=0;
	}
	~CConfData(){}

	///////////////////////////////////////////////////
	UINT m_iSolutionType;
	CString m_strCustomBrowserPath;
	//////////////////////////////////////////////////

};
#define LB_OK 0
#define LB_NG 1
#define LB_NG_NO_MSG 2
class CEventHelper
{
public:
	CEventHelper()
	{
		m_hEvent=NULL;
	}
	BOOL StartEvent(LPCTSTR lpName,DWORD timeout)
	{
		Clear();
		m_hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, lpName);
		if(m_hEvent)
		{
			DWORD waitRes = WaitForSingleObject(m_hEvent, timeout);
			if (waitRes == WAIT_TIMEOUT)
			{
				return FALSE;
			}
			return TRUE;
		}
		return FALSE;
	}
	virtual ~CEventHelper()
	{
		Clear();
	}
	void Clear()
	{
		if (m_hEvent)
		{
			SetEvent(m_hEvent);
			CloseHandle(m_hEvent);
			m_hEvent = NULL;
		}
	}
	HANDLE m_hEvent;
};
class CLoopBlocker
{
public:
	CLoopBlocker(){}
	virtual ~CLoopBlocker(){}
	CString m_strLogFilePath;
	void SetFilePath(LPCTSTR lp)
	{
		m_strLogFilePath=lp;
	}
	UINT CheckLoop(LPCTSTR lp)
	{
		UINT uRet = LB_NG;
		_wsetlocale(LC_ALL, _T("jpn"));
		CString strCommand;
		strCommand = lp;
		CString strReason;

		//データを読み込む
		CStdioFile in;
		CString strTemp;
		CStringArray strArrayData;
		int iLineCnt=0;


		CEventHelper EventH;

		if(EventH.StartEvent(_T("TB_STP"),1000))
		{
			//1秒待っても駄目な場合は、リダイレクトを続行する。
			//タイムアウトが発生しても、リダイレクト自体は継続させる。
			return LB_OK;
		}

		DWORD dwNow = ::GetTickCount();

		if (in.Open(m_strLogFilePath, CFile::modeRead |CFile::shareDenyWrite))
		{
			while (in.ReadString(strTemp))
			{
				strArrayData.Add(strTemp);
				//先頭行
				if(iLineCnt==0)
				{
					CStringArray strArray;
					SBUtil::Split(strArray, strTemp, _T("\t"));
					if (strArray.GetSize() >= 3)
					{
						//時間の経過をチェック
						DWORD dwPrev = _ttoi(strArray.GetAt(1));
						DWORD dwPCon = 0;
						//前回との差
						dwPCon = dwNow - dwPrev;
						//15秒以上間隔が空いていたらOK
						if (dwPCon >= 15 * 1000)
						{
							uRet = LB_OK;
							strReason = _T("There are more than 15 seconds between them");
							break;
						}
					}
				}
				iLineCnt++;
				//最大50件
				if(iLineCnt>=50-1)
					break;
			}
			in.Close();
		}

		//チェック開始
		if(uRet!= LB_OK)
		{
			//ループ発生?
			int iCntMax = strArrayData.GetSize();
			DWORD iCounter = 0;
			DWORD dwPrev = 0;

			//初期値 OKをセット
			uRet = LB_OK;
			strReason = _T("System checks are clear");

			//最大50件
			for (int i = 0;i<iCntMax;i++)
			{
				CString strLine;
				strLine = strArrayData.GetAt(i);
				CStringArray strArray;
				SBUtil::Split(strArray, strLine, _T("\t"));
				if (strArray.GetSize() >= 3)
				{
					//時間の経過をチェック
					dwPrev = _ttoi(strArray.GetAt(1));
					DWORD dwPCon = 0;
					//前回との差
					dwPCon = dwNow - dwPrev;
					//直前の結果がBLOCKの場合で間隔が3.5秒以内はNG
					if (i == 0)
					{
						if (_T("BLOCK") == strArray.GetAt(2) || _T("BLOCK_NO_MSG") == strArray.GetAt(2))
						{
							//3.5秒以内
							if (dwPCon <= 3.5 * 1000)
							{
								//メッセージ表示なし
								uRet = LB_NG_NO_MSG;
								strReason = _T("Already notified");
								break;
							}
						}
					}
					//15秒以上間隔が空いていたらOK
					if (dwPCon >= 15 * 1000)
					{
						uRet = LB_OK;
						strReason = _T("There are more than 15 seconds");
						break;
					}
					//15秒の間（現在の時間からレコードに記録された時間の間隔）
					else
					{
						//前回の実行コマンドが同じ(同一URL、同一リダイレクト先ブラウザー)
						if (strCommand == strArray.GetAt(0))
						{
							//2回目でNG
							if (iCounter > 1)
							{
								uRet = LB_NG;
								strReason = _T("Calling the same command within 15 seconds");
								break;
							}
							//15秒以内の起動回数
							iCounter++;
						}
					}
				}
			}
		}
		//ログに追記
		CString strLine;
		strLine.Format(_T("%s\t%d\t%s\t%s\t%d\n"), lp, ::GetTickCount(), uRet == LB_OK ? _T("OK") : uRet == LB_NG ? _T("BLOCK") : _T("BLOCK_NO_MSG"), strReason, ::GetCurrentProcessId());
		CStdioFile out;
		//3回リトライしておく。
		for (int i = 0; i < 3; i++)
		{
			TRY
			{
				if (out.Open(m_strLogFilePath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
				{
					//command1
					CString strWriteData;
					//先頭行書込み （直近の値）
					out.WriteString(strLine);
					for (int i = 0; i < strArrayData.GetSize(); i++)
					{
						strWriteData.Format(_T("%s\n"), strArrayData.GetAt(i));
						out.WriteString(strWriteData);
					}
					out.Flush();
					out.Close();
					break;
				}
			}
			CATCH(CFileException, eP) {}
			END_CATCH
			::Sleep(100);
		}
		_wsetlocale(LC_ALL, _T(""));
		return uRet;
	}

};
class CRedirectApp : public CWinApp
{
public:
	CString m_strThisAppName;
	CRedirectApp();
	BOOL m_bCitrixCustomEnv;//Citrix特殊環境フラグ

	CString m_strExeFullPath;
	CString m_strExeFileName;
	CString m_strExeFolderPath;
	CString m_strTemplate_FileFullPath;
	CString m_strTestConnect_FileFullPath;
	CString m_strRDP_FileFullPath;
	CString m_strChromeSwitcherFullPath;
	CString m_strO365ToolFullPath;

	CLoopBlocker m_LoopBlock;
	CString m_strLoopBlockFilePath;

	CString m_strRdp_Template_File_Data;

	CString m_strRdp_ReadOnce_File_Data;
	CString m_strRdp_ReadOnce_FileFullPath;


	CString m_strIE_FullPath;
	CString m_strFirefox_FullPath;
	CString m_strChrome_FullPath;
	CString m_strCustomBrowser_FullPath;

	void SetIE_FullPath();
	void SetFirefox_FullPath();
	void SetChrome_FullPath();

	BOOL IsFirefoxInstalled();
	BOOL IsChromeInstalled();
	BOOL IsCustomInstalled(CString strPath);
	BOOL IsHorizonInstalled();
	BOOL IsCitrixInstalled();

        BOOL IsSettingDisabled();


	CURLRedirectList m_RedirectList;
	CURLRedirectList m_RedirectListSaveData;
	CString m_strRedirectFilePath;
	CConfData SettingConf;

	void Exec_MSTSC(CString& strRDPFile);
	void CreateRDPFile(CString& strCommand);
	void ReadTemplate();
	void ReadRDP_ReadOnceFile();

	void Exec_VMwareHorizon_Start(CString& CommandParam);
	void Exec_VMware_Horizon(CString& strServerHostName,CString& strAppName,CString& strURL);

	void Exec_CitixXenApp_Start(CString& CommandParam);
	void Exec_Citrix_XenApp(CString& strAppName,CString& strURL);


	void Exec_Local_Browser(UINT iSolutionType,CString& strCustomBrowserPath,CString& strURL);
	void Exec_LocalBrowser(CString& strURL);

	BOOL InitBaseFunction();
	CString m_CommandParam;
	CString m_OptionParam;
	CString m_EtcParam;
	void InitCommandParamOptionParam();
	void InitExecCommandParam();
	void InitShowSettingDlg();
	void InitExecLocalBrowser();

	CCRre* m_pReExecThread;
	ProgressDlg* m_pDlgMsg;

	BOOL PumpMessage()
	{
		__try
		{
			return CWinApp::PumpMessage();
		}
		__except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION)
		{
			return FALSE;
		}
	}

	void WriteDebugTraceDateTime(LPCTSTR msg)
	{
		if (msg == NULL)return;
		if (_tcslen(msg) == 0)return;
		_wsetlocale(LC_ALL, _T("jpn"));

		CString strWriteLine;
		CTime time = CTime::GetCurrentTime();
		strWriteLine.Format(_T("ThinBridge:EXE\t%s\t%s\n"), time.Format(_T("%Y-%m-%d %H:%M:%S")), msg);
		if (strWriteLine.GetLength() > 4000)
		{
			strWriteLine = strWriteLine.Mid(0, 4000);
			strWriteLine += _T("..DIV..");
		}
		OutputDebugString(strWriteLine);
	}

	//UTF16をUTF8に変換しURLEncodeした文字列を返却
	CString ConvertUTF8_URLEncode(const CString& src)
	{
		_wsetlocale(LC_ALL, _T("jpn"));
		CString strRet;
		if (src.IsEmpty())return strRet;

		try
		{
			//UTF16->UTF-8変換-----------------------------------↓
			//UTF8格納
			char* pstrData_UTF8 = NULL;
			long UTF8Len = 0;
			long size = src.GetLength() * 2 + 2;
			wchar_t* lpWideString = NULL;
			lpWideString = new wchar_t[size];
			memset(lpWideString, 0x00, size);
			lstrcpyn(lpWideString, src, size);
			long size2 = src.GetLength() * 3 + 2;
			pstrData_UTF8 = new char[size2];
			memset(pstrData_UTF8, 0x00, size2);
			UTF8Len = WideCharToMultiByte(CP_UTF8, 0, lpWideString, -1, pstrData_UTF8, size2, NULL, NULL);
			if (lpWideString)
				delete[] lpWideString;
			//UTF16->UTF-8変換-----------------------------------↑

			//URLEncode-----------------------------------↓
			char* ptrDataURLEncode = NULL;
			long URLEncodeLen = 0;
			const int nLen = min((int)strlen(pstrData_UTF8), UTF8Len);
			size = UTF8Len * 10;
			ptrDataURLEncode = new char[size];
			URLEncodeLen = size;
			memset(ptrDataURLEncode, 0x00, size);
			int iPos = 0;
			BYTE cText = 0;
			for (int i = 0; i < nLen; ++i)
			{
				cText = pstrData_UTF8[i];
				if ((cText >= '0' && cText <= '9')
					|| (cText >= 'a' && cText <= 'z')
					|| (cText >= 'A' && cText <= 'Z')
					|| cText == '-' || cText == '_' || cText == '.' || cText == '!' || cText == '~'
					|| cText == '*' || cText == '\'' || cText == '(' || cText == ')'
					)
				{
					memcpy(ptrDataURLEncode + iPos, &cText, 1);
					iPos++;
				}
				else if (cText == ' ')
				{
					memcpy(ptrDataURLEncode + iPos, "%20", 3);
					iPos += 3;
				}
				else
				{
					char szFmt[4] = { 0 };
					_snprintf_s(szFmt, sizeof(szFmt), "%%%02X", cText & 0xff);
					memcpy(ptrDataURLEncode + iPos, szFmt, 3);
					iPos += 3;
				}
			}
			if (pstrData_UTF8)
				delete[] pstrData_UTF8;

			strRet = ptrDataURLEncode;
			if (ptrDataURLEncode)
				delete[] ptrDataURLEncode;
			//URLEncode-----------------------------------↑

		}
		catch (...)
		{
			ATLASSERT(0);
		}
		return strRet;
	}
	CString GetVersionStr();

	// 親プロセスIDの取得
	BOOL GetParentPID(DWORD& pidResult)
	{
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hSnapShot)
		{
			return FALSE;
		}
		PROCESSENTRY32 procentry={0};
		procentry.dwSize = sizeof(PROCESSENTRY32);
		BOOL bContinue = Process32First(hSnapShot, &procentry);
		DWORD pidParent  = 0;
		DWORD pidCurrent = GetCurrentProcessId();
		while (bContinue)
		{
			if (pidCurrent == procentry.th32ProcessID)
			{
				pidParent = procentry.th32ParentProcessID;
			}
			procentry.dwSize = sizeof(PROCESSENTRY32);
			bContinue = ((0 == pidParent) && Process32Next(hSnapShot, &procentry));
		}
		if (0 == pidParent)
		{
			return FALSE;
		}

		pidResult = pidParent;
		return TRUE;
	}
	CString GetExeFilePath2PID(DWORD dPID)
	{
		CString strRet;
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if(hSnapshot)
		{
			PROCESSENTRY32 pe32={0};
			pe32.dwSize = sizeof(PROCESSENTRY32);
			if(Process32First(hSnapshot, &pe32))
			{
				do
				{
					if(pe32.th32ProcessID==dPID)
					{
						strRet = pe32.szExeFile;
						break;
					}
				}while(Process32Next(hSnapshot, &pe32));
			}
			CloseHandle(hSnapshot);
		}
		return strRet;
	}

	void ShowTimeoutMessageBox(LPCTSTR strMsg,LPCTSTR strCaption,int iType,int iTimeOut);

	void ShowPlusSettingDlgEx();
	inline BOOL IsWnd(CWnd* wnd)
	{
		if(wnd == NULL)
			return FALSE;
		__try
		{
			if(!IsWindow(wnd->m_hWnd))
				return FALSE;
		}
		__except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION)
		{
			return FALSE;
		}
		return TRUE;
	}

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CRedirectApp theApp;

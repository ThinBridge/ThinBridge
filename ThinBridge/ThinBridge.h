
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


//FEATURE
//#define FE_RDP				0x00000001
//#define FE_VMW				0x00000010
//#define FE_CTX				0x00000100
#define FE_PLUS				0x00001000
#define CAP_PLUS			0x00010000

#define FE_ALL FE_RDP|FE_VMW|FE_CTX

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
#define DEBUG_LOG_LEVEL_OUTPUT_ALL 0
#define DEBUG_LOG_LEVEL_OUTPUT_NO_FILE 1
#define DEBUG_LOG_LEVEL_OUTPUT_URL 2 //DEBUG_LOG_TYPE_GE | DEBUG_LOG_TYPE_URL

#define DEBUG_LOG_TYPE_GE	 0 //一般情報
#define DEBUG_LOG_TYPE_DE	 1 //詳細情報
#define DEBUG_LOG_TYPE_URL	 2 //URL情報
#define DEBUG_LOG_TYPE_TR	 3 //トライデント動作情報
#define DEBUG_LOG_TYPE_CL	 4 //Close処理関連情報
#define DEBUG_LOG_TYPE_JS	 5 //Javascript関連情報
#define DEBUG_LOG_TYPE_EX	 6 //例外処理関連情報
#define DEBUG_LOG_TYPE_AC	 7 //操作アクション情報
#define DEBUG_LOG_TYPE_DDE	 8 //DDE情報
static TCHAR sDEBUG_LOG_TYPE[][8]=
{
	_T("GE"),
	_T("DE"),
	_T("URL"),
	_T("TR"),
	_T("CL"),
	_T("JS"),
	_T("EX"),
	_T("AC"),
	_T("DDE"),
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
				strRet = str1.Mid(0,iTabStrLen) + szEllipsis;
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
		PROCESSENTRY32 entry={0};
		entry.dwSize = sizeof(entry);
		BOOL has_entry = ::Process32First(snapshot, &entry);
		TCHAR	pszName[64]={0};
		size_t iMem=0;
		while(has_entry)
		{
			lstrcpyn(pszName,entry.szExeFile,64);
			//ウインドウクラスの取得
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
			has_entry = ::Process32Next(snapshot, &entry);
		}
		::CloseHandle(snapshot);
		return iMem;
	}


	DWORD m_IETabCnt;
	BOOL EnumChildProc(HWND hwnd)
	{
		int nRet=0;
		BOOL	bRet=FALSE;
		HRESULT	hr={0};
		TCHAR	pszName[64]={0};
		//ウインドウクラスの取得
		nRet = ::GetClassName(hwnd,pszName,64);
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
			if(strTitle.IsEmpty()
			||strTitle.Find(_T("空白のページ"))==0
			||strTitle.Find(_T("about:blank"))==0
			||strTitle.Find(_T("Blank Page"))==0)
				return TRUE;
		}


		m_IETabCnt++;
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
		__except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION)
		{
			return hRetNULL;
		}
		return hRetNULL;
	}
	inline void SafeGetWindowText(HWND hWnd,CString& str)
	{
		if(!SafeWnd(hWnd))
			return;

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
		int nRet=0;
		TCHAR pszName[64]={0};
		//ウインドウクラスの取得
		nRet = ::GetClassName(hwnd,pszName,64);
		if(nRet == 0)
			return	TRUE;
		nRet = ::_tcscmp(pszName,_T("IEFrame"));
		if(nRet != 0)
		{
			return TRUE;
		}
		//子ウインドウの列挙方法
		::EnumChildWindows(hwnd,EnumChildProc_,(LPARAM)this);
		return	TRUE;
	}
};

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
		m_dwZone=ZONE_NA;
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
		m_dwZone=ZONE_NA;
		m_dRedirectPageAction=0;
		m_dwCloseTimeout=3;
		m_strExecType.Empty();
		m_strExecExeFullPath.Empty();
		m_arr_URL.RemoveAll();
		m_arr_URL_EX.RemoveAll();
	}
	void Copy(CURLRedirectDataClass* ptr)
	{
		if(ptr==NULL)return;
		m_bDisabled = ptr->m_bDisabled;
		m_bTopPageOnly = ptr->m_bTopPageOnly;
		m_dwZone = ptr->m_dwZone;
		m_dRedirectPageAction = ptr->m_dRedirectPageAction;
		m_dwCloseTimeout = ptr->m_dwCloseTimeout;
		m_strExecType = ptr->m_strExecType;
		m_strExecExeFullPath = ptr->m_strExecExeFullPath;
		m_arr_URL.Copy(ptr->m_arr_URL);
		m_arr_URL_EX.Copy(ptr->m_arr_URL_EX);
	}
public:
	BOOL m_bDisabled;
	BOOL m_bTopPageOnly;
	DWORD m_dwZone;
	DWORD m_dRedirectPageAction;
	DWORD m_dwCloseTimeout;

	CString m_strExecType;
	CString m_strExecExeFullPath;
	CStringArray m_arr_URL;
	CStringArray m_arr_URL_EX;

};
class CURLRedirectList
{
public:
	CPtrArray m_arr_RedirectBrowser;
	BOOL m_bURLOnly;
	BOOL m_bTraceLog;
	BOOL m_bUseScript;
	BOOL m_bQuickRedirect;

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
		m_bUseScript=FALSE;
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
	}
	virtual ~CURLRedirectList()
	{
		Clear();
	}
	void Clear()
	{
		m_bURLOnly=FALSE;
		m_bTraceLog=FALSE;
		m_bUseScript=FALSE;
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
	}
	void InitSaveDataAll()
	{
		Clear();
		CURLRedirectDataClass* pRedirectData=NULL;

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=FALSE;
		pRedirectData->m_strExecType=_T("RDP");
		pRedirectData->m_dwCloseTimeout=3;
		m_pRDP=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=FALSE;
		pRedirectData->m_strExecType=_T("VMWARE");
		pRedirectData->m_dwCloseTimeout=3;
		m_pVMW=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=FALSE;
		pRedirectData->m_strExecType=_T("CITRIX");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCTX=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=FALSE;
		pRedirectData->m_strExecType=_T("Firefox");
		pRedirectData->m_dwCloseTimeout=3;
		m_pFirefox=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=FALSE;
		pRedirectData->m_strExecType=_T("Chrome");
		pRedirectData->m_dwCloseTimeout=3;
		m_pChrome=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=FALSE;
		pRedirectData->m_strExecType=_T("Edge");
		pRedirectData->m_dwCloseTimeout=3;
		m_pEdge=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=FALSE;
		pRedirectData->m_strExecType=_T("IE");
		pRedirectData->m_dwCloseTimeout=3;
		m_pIE=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=FALSE;
		pRedirectData->m_strExecType=_T("Default");
		pRedirectData->m_dwCloseTimeout=3;
		m_pDefault=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);


		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM01");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom01=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM02");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom02=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM03");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom03=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM04");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom04=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM05");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom05=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM06");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom06=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM07");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom07=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM08");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom08=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM09");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom09=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM10");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom10=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM11");
		pRedirectData->m_dwCloseTimeout=3;

		m_pCustom11=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM12");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom12=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM13");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom13=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM14");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom14=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM15");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom15=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM16");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom16=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM17");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom17=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM18");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom18=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM19");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom19=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);

		pRedirectData = NULL;
		pRedirectData = new CURLRedirectDataClass;
		pRedirectData->m_bDisabled=TRUE;
		pRedirectData->m_strExecType=_T("CUSTOM20");
		pRedirectData->m_dwCloseTimeout=3;
		m_pCustom20=pRedirectData;
		m_arr_RedirectBrowser.Add(pRedirectData);
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

			strWriteLine=_T("## Configuration File for ThinBridge\n");
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
			if(m_bUseScript)
			{
				strTempFormat=_T("@CLOSE_TIMEOUT:1\n");
				out.WriteString(strTempFormat);
				pstrOutPutData+=strTempFormat;
			}
			if (m_bQuickRedirect)
			{
				strTempFormat = _T("@INTRANET_ZONE\n");
				out.WriteString(strTempFormat);
				pstrOutPutData += strTempFormat;
			}
			out.WriteString(_T("\n"));
			pstrOutPutData+=_T("\n");

			int imax = (int)m_arr_RedirectBrowser.GetCount();
			for(int i=0;i<imax;i++)
			{
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

					if(ptr->m_bTopPageOnly)
					{
						strTempFormat=_T("@TOP_PAGE_ONLY\n");
						out.WriteString(strTempFormat);
						pstrOutPutData+=strTempFormat;
					}

					strTempFormat.Format(_T("@REDIRECT_PAGE_ACTION:%d\n"),ptr->m_dRedirectPageAction);
					out.WriteString(strTempFormat);
					pstrOutPutData+=strTempFormat;

					strTempFormat.Format(_T("@CLOSE_TIMEOUT:%d\n"),ptr->m_dwCloseTimeout);
					out.WriteString(strTempFormat);
					pstrOutPutData+=strTempFormat;

					if((ptr->m_dwZone&INTRANET_ZONE)==INTRANET_ZONE)
					{
						strTempFormat=_T("@INTRANET_ZONE\n");
						out.WriteString(strTempFormat);
						pstrOutPutData+=strTempFormat;
					}

					if((ptr->m_dwZone&TRUSTED_ZONE)==TRUSTED_ZONE)
					{
						strTempFormat=_T("@TRUSTED_ZONE\n");
						out.WriteString(strTempFormat);
						pstrOutPutData+=strTempFormat;
					}

					if((ptr->m_dwZone&INTERNET_ZONE)==INTERNET_ZONE)
					{
						strTempFormat=_T("@INTERNET_ZONE\n");
						out.WriteString(strTempFormat);
						pstrOutPutData+=strTempFormat;
					}

					if((ptr->m_dwZone&UNTRUSTED_ZONE)==UNTRUSTED_ZONE)
					{
						strTempFormat=_T("@UNTRUSTED_ZONE\n");
						out.WriteString(strTempFormat);
						pstrOutPutData+=strTempFormat;
					}

					int iMaxCnt=0;
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
					out.WriteString(_T("\n"));
					pstrOutPutData+=_T("\n");
				}
			}
			bRet=TRUE;
			out.Close();
		}
		return bRet;
	}
	void SetArrayData(LPCTSTR lPath)
	{
		if(lPath==NULL)return;
		this->Clear();

		InitSaveDataAll();

		CStdioFile in;
		//if(in.Open(lPath,CFile::modeRead|CFile::modeCreate|CFile::modeNoTruncate))
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
						pRedirectData->m_bTopPageOnly=bTopPageOnly;
						pRedirectData->m_dRedirectPageAction=dRedirectPageAction;
						pRedirectData->m_dwCloseTimeout=dwCloseTimeout;
						pRedirectData->m_dwZone=dwZone;
						
						if(strExecType.CompareNoCase(_T("GLOBAL"))==0)
						{
							m_bURLOnly=bTopPageOnly;
							m_bTraceLog=dRedirectPageAction==1?TRUE:FALSE;
							m_bUseScript=dwCloseTimeout==1?TRUE:FALSE;
							m_bQuickRedirect = (dwZone&INTRANET_ZONE) == INTRANET_ZONE ? TRUE : FALSE;
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
						strExecExeFullPath.Empty();
						strExecType.Empty();
						arr_URL.RemoveAll();
						arr_URL_EX.RemoveAll();
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
					pRedirectData->m_bTopPageOnly=bTopPageOnly;
					pRedirectData->m_dRedirectPageAction=dRedirectPageAction;
					pRedirectData->m_dwCloseTimeout=dwCloseTimeout;
					pRedirectData->m_dwZone=dwZone;
					
					if(strExecType.CompareNoCase(_T("GLOBAL"))==0)
					{
						m_bURLOnly=bTopPageOnly;
						m_bTraceLog=dRedirectPageAction==1?TRUE:FALSE;
						m_bUseScript=dwCloseTimeout==1?TRUE:FALSE;
						m_bQuickRedirect = (dwZone&INTRANET_ZONE) == INTRANET_ZONE ? TRUE : FALSE;
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
					pRedirectData = NULL;
					bDisabled=FALSE;
				}
			}
		}
	}
};
class CConfData
{
	public:
	CConfData()
	{
		m_iKeyCombination=0;
		m_iSolutionType=0;
		m_bRemoteAppMode=TRUE;
		m_bRedirect_Clipboard=FALSE;
		m_bRedirect_Printer=FALSE;
		m_bRedirect_Drive=FALSE;
		m_bEnableDebugLog=FALSE;
	}
	~CConfData(){}
	void Copy(CConfData* ptr)
	{
		if(ptr==NULL)return;

		m_iKeyCombination = ptr->m_iKeyCombination;
		m_iSolutionType = ptr->m_iSolutionType;

		m_strCustomBrowserPath = ptr->m_strCustomBrowserPath;
		m_strHorizon_ConnectionServerName = ptr->m_strHorizon_ConnectionServerName;
		m_strHorizon_AppName = ptr->m_strHorizon_AppName;
		m_strCitrix_AppName = ptr->m_strCitrix_AppName;
		m_strServerName = ptr->m_strServerName;

		m_bRemoteAppMode = ptr->m_bRemoteAppMode;

		m_strRemoteAppName = ptr->m_strRemoteAppName;
		m_strRemoteAppPath = ptr->m_strRemoteAppPath;
		m_strRemoteAppCommandLine = ptr->m_strRemoteAppCommandLine;

		m_bRedirect_Clipboard = ptr->m_bRedirect_Clipboard;
		m_bRedirect_Printer = ptr->m_bRedirect_Printer;
		m_bRedirect_Drive = ptr->m_bRedirect_Drive;
		m_bEnableDebugLog = ptr->m_bEnableDebugLog;
	}

	///////////////////////////////////////////////////
	int m_iKeyCombination;
	UINT m_iSolutionType;
	CString m_strCustomBrowserPath;
	CString m_strHorizon_ConnectionServerName;
	CString m_strHorizon_AppName;
	CString m_strCitrix_AppName;
	CString m_strServerName;
	BOOL m_bRemoteAppMode;
	CString m_strRemoteAppName;
	CString m_strRemoteAppPath;
	CString m_strRemoteAppCommandLine;
	BOOL m_bRedirect_Clipboard;
	BOOL m_bRedirect_Printer;
	BOOL m_bRedirect_Drive;
	BOOL m_bEnableDebugLog;

	//////////////////////////////////////////////////

};

class CResourceCAPData
{
	public:
	CResourceCAPData()
	{
		m_bTabCntCAP=FALSE;
		m_uiTabCntWARM=10;
		m_uiTabCntMAX=15;
		m_strTab_WARM_Msg  =_T("【お願い】\n現在開いているタブ・ウインドウ数が警告値に達しました。\n\nお手数ですが他の不要なタブ・ウインドウを閉じて下さい。\n\nコンピュータ資源の節約に、ご協力ください。");
		m_strTab_MAX_Msg =_T("【お知らせ】\n現在開いているタブ・ウインドウ数が上限値に達しました。\nこれ以上タブ・ウインドウを追加できません。\n\nお手数ですが他の不要なタブ・ウインドウを閉じてから再度実行して下さい。\n\nコンピュータ資源の節約に、ご協力ください。");
		m_uiTab_WARM_ShowTime=5;
		m_uiTab_MAX_ShowTime=5;

		m_bMemUsageCAP=FALSE;
		m_uiMemWARM=600;
		m_uiMemMAX=1024;
		m_strMem_WARM_Msg  =_T("【お願い】\n現在の使用メモリが警告値に達しました。\n\nお手数ですが他の不要なタブ・ウインドウを閉じて下さい。\n\nコンピュータ資源の節約に、ご協力ください。");
		m_strMem_MAX_Msg =_T("【お知らせ】\n現在の使用メモリが上限値に達しました。\n\nお手数ですが他の不要なタブ・ウインドウを閉じてから再度実行して下さい。\n\nコンピュータ資源の節約に、ご協力ください。");
		m_uiMem_WARM_ShowTime=5;
		m_uiMem_MAX_ShowTime=5;

		m_bCPUPriorityCAP=FALSE;
		m_uiCPUPriority=2;
		m_strReadSettingType=_T("(N/A Default)");
	}
	~CResourceCAPData(){}
	void Copy(CResourceCAPData* ptr)
	{
		if(ptr==NULL)return;

		m_bTabCntCAP = ptr->m_bTabCntCAP;
		m_uiTabCntWARM = ptr->m_uiTabCntWARM;
		m_uiTabCntMAX = ptr->m_uiTabCntMAX;
		m_strTab_WARM_Msg = ptr->m_strTab_WARM_Msg;
		m_strTab_MAX_Msg = ptr->m_strTab_MAX_Msg;
		m_uiTab_WARM_ShowTime = ptr->m_uiTab_WARM_ShowTime;
		m_uiTab_MAX_ShowTime = ptr->m_uiTab_MAX_ShowTime;

		m_bMemUsageCAP = ptr->m_bMemUsageCAP;
		m_uiMemWARM = ptr->m_uiMemWARM;
		m_uiMemMAX = ptr->m_uiMemMAX;
		m_strMem_WARM_Msg = ptr->m_strMem_WARM_Msg;
		m_strMem_MAX_Msg = ptr->m_strMem_MAX_Msg;
		m_uiMem_WARM_ShowTime = ptr->m_uiMem_WARM_ShowTime;
		m_uiMem_MAX_ShowTime = ptr->m_uiMem_MAX_ShowTime;

		m_bCPUPriorityCAP = ptr->m_bCPUPriorityCAP;
		m_uiCPUPriority = ptr->m_uiCPUPriority;
		m_strReadSettingType=ptr->m_strReadSettingType;

	}

	///////////////////////////////////////////////////
	BOOL m_bTabCntCAP;
	UINT m_uiTabCntWARM;
	UINT m_uiTabCntMAX;
	CString m_strTab_WARM_Msg;
	CString m_strTab_MAX_Msg;
	UINT m_uiTab_WARM_ShowTime;
	UINT m_uiTab_MAX_ShowTime;

	BOOL m_bMemUsageCAP;
	UINT m_uiMemWARM;
	UINT m_uiMemMAX;
	CString m_strMem_WARM_Msg;
	CString m_strMem_MAX_Msg;
	UINT m_uiMem_WARM_ShowTime;
	UINT m_uiMem_MAX_ShowTime;

	BOOL m_bCPUPriorityCAP;
	UINT m_uiCPUPriority;

	CString m_strReadSettingType;
	//////////////////////////////////////////////////
};
#define LB_OK 0
#define LB_NG 1
#define LB_NG_NO_MSG 2
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
		if (in.Open(m_strLogFilePath, CFile::modeRead | CFile::shareDenyWrite))
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
						dwPCon = ::GetTickCount() - dwPrev;
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
			DWORD dwNow = ::GetTickCount();
			int iCntMax = strArrayData.GetSize() - 1;
			DWORD dwBase = 0;
			dwBase = dwNow;
			DWORD iCounter = 0;
			DWORD iCounter4sec = 0;
			DWORD dwPrev = 0;
			DWORD dwPrevVal = 0;
			DWORD dwPrevVal4 = 0;

			uRet = LB_OK;
			strReason = _T("System checks are clear");

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
					dwPCon = dwBase - dwPrev;
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

					//2秒以内に同じパラメータはNG
					if (dwPCon <= 2 * 1000)
					{
						//前回の実行コマンドが同じ(同一URL、同一リダイレクト先ブラウザー)
						if (strCommand == strArray.GetAt(0))
						{
							dwBase = dwPrev;
							//2回目でNG
							if (iCounter > 1)
							{
								uRet = LB_NG;
								strReason = _T("Calling the same command within 2 seconds");
								break;
							}
						}
						//2秒以内の起動回数
						iCounter++;
						dwPrevVal = dwPrev;
					}
					//4秒以内
					else if (dwPCon <= 4 * 1000)
					{
						dwBase = dwPrev;
						iCounter4sec++;
						dwPrevVal4 = dwPrev;
					}
					else
						break;
				}
			}
			//2秒以内の呼び出しが5回あったらNG
			if (iCounter >= 5)
			{
				//12秒間で1秒以下の呼び出しが5回
				DWORD dwPCon = 0;
				dwPCon = dwNow - dwPrevVal;
				if (dwPCon <= 12 * 1000)
				{
					uRet = LB_NG;
					strReason = _T("Due to 5 calls of 2 seconds or less in 12 seconds");
				}
			}
			//4秒以内の呼び出しが3回あったらNG
			else if (iCounter4sec >= 3)
			{
				//12秒間で4秒以下の呼び出しが3回
				DWORD dwPCon = 0;
				dwPCon = dwNow - dwPrevVal4;
				if (dwPCon <= 12 * 1000)
				{
					uRet = LB_NG;
					strReason = _T("Due to 3 calls of 4 seconds or less in 12 seconds");
				}
			}
		}
		//ログに追記
		CString strLine;
		strLine.Format(_T("%s\t%d\t%s\t%s\n"), lp, ::GetTickCount(), uRet == LB_OK ? _T("OK") : uRet == LB_NG ? _T("BLOCK") : _T("BLOCK_NO_MSG"), strReason);
		CStdioFile out;
		if (out.Open(m_strLogFilePath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
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
			out.Close();
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

	CString m_strProfileFullPath;
	int m_iFeatureType;

	CString m_strExeFullPath;
	CString m_strExeFileName;
	CString m_strSetting_FileFullPath;
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


	CURLRedirectList m_RedirectList;
	CURLRedirectList m_RedirectListSaveData;
	CString m_strRedirectFilePath;
	CString m_strRedirect_Script_FilePath;
	CConfData SettingConf;
	CConfData SettingConfMod;


	void Exec_MSTSC(CString& strRDPFile);
	void CreateRDPFile(CString& strCommand);
	void ReadSetting();
	void ReadTemplate();
	void ReadRDP_ReadOnceFile();

	//ResourceCAP
	CString m_strResourceCAPFullPath;
	CResourceCAPData ResourceCAPConf;
	CResourceCAPData ResourceCAPConfMod;
	void ReadResourceCAPSetting();
	BOOL ReadResourceCAPSettingFromReg();

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

	//log
	CString m_strLogFileDir;
	CString m_strLogFileFullPath;
	int m_iAdvancedLogLevel;

	BOOL PumpMessage()
	{
		__try
		{
			return CWinApp::PumpMessage();
		}
		__except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION)
		{
			WriteDebugTraceDateTime(_T("PumpMessage_EXCEPTION_ACCESS_VIOLATION"),DEBUG_LOG_TYPE_EX);
			return FALSE;
		}
	}


	void LogRotate(LPCTSTR LogFolderPath)
	{
		//12ヶ月分のログを保持する。ファイル名は、ThinBridgeYYYY-MM.log
		int iMaxLogCnt=12;
		CTime time = CTime::GetCurrentTime();
		CString strLogFileNow;
		strLogFileNow.Format(_T("%s%s.log"),m_strThisAppName,time.Format(_T("%Y-%m")));

		int NowY=0;
		int NowM=0;
		NowY=time.GetYear();
		NowM=time.GetMonth();
		CString strTemp;
		//面倒なので、用意してしまう。
		CStringArray strASaveFileList;
		for(int ii=0;ii<iMaxLogCnt;ii++)
		{
			if(NowM-ii <= 0)
			{
				strTemp.Format(_T("%s%04d-%02d.log"),m_strThisAppName,NowY-1,12+NowM-ii);
				strASaveFileList.Add(strTemp);
			}
			else
			{
				strTemp.Format(_T("%s%04d-%02d.log"),m_strThisAppName,NowY,NowM-ii);
				strASaveFileList.Add(strTemp);
			}
		}

		//ログファイルを確認する。
		//基準
		CString strFindPath = LogFolderPath;
		strFindPath += m_strThisAppName;
		strFindPath += _T("*.log");
		WIN32_FIND_DATA wfd={0};
		HANDLE h = ::FindFirstFile(strFindPath, &wfd);
		CString strFileNameTemp;
		CString strDelFilePath;
		//削除対象List
		CStringArray strADeleteFileList;
		if (h == INVALID_HANDLE_VALUE)
		{
			return;
		}
		do
		{
			BOOL bDirectory = wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
			if (bDirectory)
			{
				if (::lstrcmp(wfd.cFileName, _T(".")) == 0 || ::lstrcmp(wfd.cFileName, _T("..")) == 0)
					continue;
			}
			else
			{
				BOOL bHit=FALSE;
				strFileNameTemp=wfd.cFileName;
				for(int iii=0;iii<iMaxLogCnt;iii++)
				{
					//Listに一致した場合は抜ける
					if(strFileNameTemp.CompareNoCase(strASaveFileList.GetAt(iii))==0)
					{
						bHit=TRUE;
						break;
					}
				}
				//Listに見つからない。
				if(!bHit)
				{
					strDelFilePath=LogFolderPath;
					strDelFilePath+=strFileNameTemp;
					strADeleteFileList.Add(strDelFilePath);
				}
			}

		} while (::FindNextFile(h, &wfd));
		::FindClose(h);

		for(int iiii=0;iiii<strADeleteFileList.GetCount() ;iiii++)
		{
			strDelFilePath=strADeleteFileList.GetAt(iiii);
			::DeleteFile(strDelFilePath);
		}
		SetLastError(NO_ERROR);
	}

	void WriteDebugTraceDateTime(LPCTSTR msg,int iLogType)
	{
		if(msg==NULL)return;
		if(_tcslen(msg)==0)return;
		if(!SettingConf.m_bEnableDebugLog)return;

		CString strWriteLine;
		CString strDebugLine;
		CTime time = CTime::GetCurrentTime();
		strWriteLine.Format(_T("%s\t%s\t%s\n"),time.Format(_T("%Y-%m-%d %H:%M:%S")),sDEBUG_LOG_TYPE[iLogType],msg);
		strDebugLine.Format(_T("ThinBridge:EXE\t%s"),strWriteLine);

		_wsetlocale(LC_ALL, _T("jpn")); 

		OutputDebugString(strDebugLine);

		CStdioFile stdFile;

		BOOL bFileWriteFlg=FALSE;
		switch(m_iAdvancedLogLevel)
		{
			//全てのログを出力
			case DEBUG_LOG_LEVEL_OUTPUT_ALL:
			{
				bFileWriteFlg=TRUE;
				break;
			}
			//ファイル書き込み無し。
			case DEBUG_LOG_LEVEL_OUTPUT_NO_FILE:
			{
				bFileWriteFlg=FALSE;
				break;
			}
			//URL関連のみ
			case DEBUG_LOG_LEVEL_OUTPUT_URL:
			{
				//ログの種類が一般、URLの場合
				//if(iLogType==DEBUG_LOG_TYPE_GE || iLogType==DEBUG_LOG_TYPE_URL)
				//{
				//	bFileWriteFlg=TRUE;
				//}
				if(iLogType==DEBUG_LOG_TYPE_URL)
				{
					bFileWriteFlg=TRUE;
				}
				break;
			}
			default:
				break;
		}
		if(bFileWriteFlg)
		{
			if(stdFile.Open(m_strLogFileFullPath,CFile::modeWrite|CFile::shareDenyNone|CFile::modeCreate|CFile::modeNoTruncate))
			{
				TRY
				{
					stdFile.SeekToEnd();
					stdFile.WriteString(strWriteLine);
				}
				CATCH (CFileException, eP) {}
				END_CATCH
				stdFile.Close();
			}
		}
		_wsetlocale(LC_ALL, _T(""));

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

	DWORD GetKeyCombi()
	{
		//#define KEY_COMB_SHIFT	0x00000001
		//#define KEY_COMB_CTRL		0x00000010
		//#define KEY_COMB_ALT		0x00000100
		//#define KEY_COMB_LEFT		0x00001000
		//#define KEY_COMB_UP		0x00010000
		//#define KEY_COMB_RIGHT	0x00100000
		//#define KEY_COMB_DOWN		0x01000000

		DWORD dRet=0;
		dRet = dRet | (::GetKeyState(VK_SHIFT)<0?KEY_COMB_SHIFT:0);
		dRet = dRet | (::GetKeyState(VK_CONTROL)<0?KEY_COMB_CTRL:0);
		dRet = dRet | (::GetKeyState(VK_MENU)<0?KEY_COMB_ALT:0);

		dRet = dRet | (::GetKeyState(VK_LEFT)<0?KEY_COMB_LEFT:0);
		dRet = dRet | (::GetKeyState(VK_UP)<0?KEY_COMB_UP:0);
		dRet = dRet | (::GetKeyState(VK_RIGHT)<0?KEY_COMB_RIGHT:0);
		dRet = dRet | (::GetKeyState(VK_DOWN)<0?KEY_COMB_DOWN:0);

		return dRet;
	}
	BOOL bValidKeyCombi()
	{
		BOOL bRet=FALSE;
		int iKeyCombination=0;
		iKeyCombination=SettingConf.m_iKeyCombination;
		if(iKeyCombination==0)
		{
			return TRUE;
		}
		DWORD dwKC = GetKeyCombi();
		if(iKeyCombination==dwKC)
			bRet=TRUE;
		return bRet;
	}
	void ShowTimeoutMessageBox(LPCTSTR strMsg,LPCTSTR strCaption,int iType,int iTimeOut);

	void InitFeatureSetting()
	{
		m_iFeatureType = FE_PLUS;
		CString strFeatureType;

		strFeatureType = GetProfileString(_T("Common"), _T("FeatureType"));
		//ThinBridgePlus フル機能
		if(strFeatureType.CompareNoCase(_T("FULL_PLUS"))==0)
		{
			m_iFeatureType = FE_PLUS;
		}
		//ThinBridgePlusリソースキャップ
		else if(strFeatureType.CompareNoCase(_T("CAP_PLUS"))==0)
		{
			m_iFeatureType = CAP_PLUS;
		}
		else
		{
			m_iFeatureType = FE_PLUS;
		}
	}
	BOOL IsResourceCAP_IE();
	void ShowPlusSettingDlgEx();
	void ShowPlusSettingDlgCAP();
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
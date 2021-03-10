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

namespace SBUtil
{
	inline void Split(CAtlArray<CAtlString>& strArray,CString strTarget,LPCTSTR strDelimiter)
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
	static inline void Split_TrimBlank(CAtlArray<CAtlString>& strArray,CString strTarget,LPCTSTR strDelimiter)
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

	inline BOOL IsURL(LPCTSTR str)
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

	inline BOOL IsURL_HTTP(LPCTSTR str)
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
	void GetDivChar(LPCTSTR str,int size,CString& strRet,BOOL bAppend=TRUE)
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
	inline void Trim_URLOnly(LPCTSTR str,CString& ptrURL)
	{
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
			ptrURL = strTemp;
		}
		return;
	}
	inline CString Trim_URLOnly(LPCTSTR str)
	{
		CString strRet;
		Trim_URLOnly(str,strRet);
		return strRet;
	}
	//2021-02-26 MS-EdgeのIEMode判定用
	inline BOOL InEdgeIEMode()
	{
		CString strCommandLine;
		strCommandLine = GetCommandLine();
		if(strCommandLine.IsEmpty())
			return FALSE;

		//参考：
		//MS-Edge IEMode
		//"C:\Program Files (x86)\Internet Explorer\IEXPLORE.EXE" SCODEF:15064 CREDAT:75010 APPID:MSEdge.UserData.Default /prefetch:2
		//IE
		//"C:\Program Files (x86)\Internet Explorer\IEXPLORE.EXE" SCODEF:30748 CREDAT:9500 /prefetch:2

		//コマンドラインにAPPID:MSEdge.がある場合はMS-Edge IEMode
		if(strCommandLine.Find(_T("APPID:MSEdge")) >= 0)
		{
			return TRUE;
		}
		return FALSE;
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
	inline CString GetVirtIELabelStr()
	{
		TCHAR lpKey[] = _T("SOFTWARE\\ThinBridge");
		TCHAR lpRegSub[] = _T("VirtIELabel");
		CString strVal;
		HKEY  hKey = { 0 };
		LONG lResult = 0L;
		DWORD dwType = 0;
		DWORD dwOption = 0;
		dwOption = KEY_READ;
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpKey,
			0, dwOption, &hKey);
		if (lResult == ERROR_SUCCESS)
		{
			DWORD iSize = 0;
			TCHAR* pVal = NULL;

			RegQueryValueEx(hKey, lpRegSub, NULL, &dwType, NULL, &iSize);
			if (iSize > 0)
			{
				iSize += 1;//+1=null
				pVal = new TCHAR[iSize];
				memset(pVal, 0x00, sizeof(TCHAR)*iSize);
				RegQueryValueEx(hKey, lpRegSub, NULL, &dwType, (LPBYTE)pVal, &iSize);
				strVal = pVal;
				delete[] pVal;
			}
			RegCloseKey(hKey);
		}
		//wow6432node
		if (strVal.IsEmpty())
		{
			TCHAR lpKeyWow6432[] = _T("SOFTWARE\\WOW6432Node\\ThinBridge");
			lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpKeyWow6432,
				0, dwOption, &hKey);
			if (lResult == ERROR_SUCCESS)
			{
				DWORD iSize = 0;
				TCHAR* pVal = NULL;

				RegQueryValueEx(hKey, lpRegSub, NULL, &dwType, NULL, &iSize);
				if (iSize > 0)
				{
					iSize += 1;//+1=null
					pVal = new TCHAR[iSize];
					memset(pVal, 0x00, sizeof(TCHAR)*iSize);
					RegQueryValueEx(hKey, lpRegSub, NULL, &dwType, (LPBYTE)pVal, &iSize);
					strVal = pVal;
					delete[] pVal;
				}
				RegCloseKey(hKey);
			}
		}
		//x64OS
		if (strVal.IsEmpty())
		{
			dwOption = dwOption | KEY_WOW64_64KEY;
			lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpKey,
				0, dwOption, &hKey);
			if (lResult == ERROR_SUCCESS)
			{
				DWORD iSize = 0;
				TCHAR* pVal = NULL;

				RegQueryValueEx(hKey, lpRegSub, NULL, &dwType, NULL, &iSize);
				if (iSize > 0)
				{
					iSize += 1;//+1=null
					pVal = new TCHAR[iSize];
					memset(pVal, 0x00, sizeof(TCHAR)*iSize);
					RegQueryValueEx(hKey, lpRegSub, NULL, &dwType, (LPBYTE)pVal, &iSize);
					strVal = pVal;
					delete[] pVal;
				}
				RegCloseKey(hKey);
			}
		}
		return strVal;
	}

	inline int GetTridentVersion()
	{
		int iTridentVersion=10;
		DWORD dwDummy=0;
		DWORD dwSize=0;
		DWORD dwMajar=0;
		DWORD dwMinor=0;
		DWORD dwBuild=0;
		DWORD dwPrivate=0;
		dwSize = ::GetFileVersionInfoSize(_T("MSHTML.DLL"), &dwDummy);
		if(dwSize>0)
		{
			PBYTE pData = new BYTE[dwSize];
			memset(pData,0x00,dwSize);
			if(::GetFileVersionInfo(_T("MSHTML.DLL"), 0, dwSize, pData))
			{ 
				VS_FIXEDFILEINFO* pFileInfo={0};
				UINT nLen=0;
				if(::VerQueryValue(pData, _T("\\"),(PVOID*)&pFileInfo, &nLen))
				{
					dwMajar = HIWORD(pFileInfo->dwFileVersionMS);
					dwMinor = LOWORD(pFileInfo->dwFileVersionMS);
					dwBuild = HIWORD(pFileInfo->dwFileVersionLS);
					dwPrivate = LOWORD(pFileInfo->dwFileVersionLS);
					iTridentVersion=dwMajar;
				}
			}
			delete[] pData;
		}
		return iTridentVersion;
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

	typedef int (__stdcall * TMessageBoxTimeout)(HWND, LPCTSTR, LPCTSTR, UINT, WORD, DWORD);
	void ShowTimeoutMessageBox(LPCTSTR strMsg,LPCTSTR strCaption,int iType,int iTimeOut)
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
	static inline BOOL IsWindowsServerRDS()
	{
		OSVERSIONINFOEX ovi = {0};
		ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		GetVersionEx((OSVERSIONINFO*)&ovi);
		if(ovi.wProductType==VER_NT_WORKSTATION)
			return FALSE;
		else if(ovi.wProductType==VER_NT_DOMAIN_CONTROLLER||ovi.wProductType==VER_NT_SERVER)
		{
			if((ovi.wSuiteMask&VER_SUITE_TERMINAL)==VER_SUITE_TERMINAL)
			{
				if(!((ovi.wSuiteMask&VER_SUITE_SINGLEUSERTS)==VER_SUITE_SINGLEUSERTS))
					return TRUE;
			}
		}
		return FALSE;
	}

};

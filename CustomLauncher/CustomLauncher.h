
// CustomLauncher.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル
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

// CCustomLauncherApp:
// このクラスの実装については、CustomLauncher.cpp を参照してください。
//
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

class CCustomLauncherApp : public CWinApp
{
public:
	CCustomLauncherApp();
	CString m_strExeNameNoExt;//拡張子なしのファイル名
	CString m_strExeFullPath;
	CString m_strSetting_FileFullPath;
	CString m_CommandParam;
	CString m_OptionParam;

	CString m_strCommand1;
	CString m_strCommand2;
	int m_iKeyCombination;
	void CustomBrowserStart(CString strCommand1,CString strCommand2,CString strURL);
	void ReadSetting();
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
		iKeyCombination=m_iKeyCombination;
		if(iKeyCombination==0)
		{
			return TRUE;
		}
		DWORD dwKC = GetKeyCombi();
		if(iKeyCombination==dwKC)
			bRet=TRUE;
		return bRet;
	}

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CCustomLauncherApp theApp;
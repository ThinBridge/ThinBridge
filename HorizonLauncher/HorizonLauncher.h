
// HorizonLauncher.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル
#include "locale.h"
#define KEY_COMB_SHIFT		0x00000001
#define KEY_COMB_CTRL		0x00000010
#define KEY_COMB_ALT		0x00000100
#define KEY_COMB_LEFT		0x00001000
#define KEY_COMB_UP			0x00010000
#define KEY_COMB_RIGHT		0x00100000
#define KEY_COMB_DOWN		0x01000000


// CHorizonLauncherApp:
// このクラスの実装については、HorizonLauncher.cpp を参照してください。
//

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

class CHorizonLauncherApp : public CWinApp
{
public:
	CHorizonLauncherApp();
	CString m_strExeNameNoExt;//拡張子なしのファイル名
	CString m_strExeFullPath;
	CString m_strSetting_FileFullPath;
	CString m_CommandParam;
	CString m_OptionParam;

	CString m_strServerName;
	CString m_strAppName;
	int m_iKeyCombination;
	BOOL IsHorizonInstalled();
	void CustomBrowserStart(const CString& strServerName,const CString& strAppName,const CString& strURL);
	CString Get_ViewClient_ProtocolCommand();

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
			char* pstrData_UTF8=NULL;
			long UTF8Len=0;
			long size = src.GetLength() * 2 + 2;
			wchar_t* lpWideString = NULL;
			lpWideString = new wchar_t[size];
			memset(lpWideString, 0x00, size);
			lstrcpyn(lpWideString, src, size);
			long size2 = src.GetLength() * 3 + 2;
			pstrData_UTF8 = new char[size2];
			memset(pstrData_UTF8, 0x00, size2);
			UTF8Len = WideCharToMultiByte(CP_UTF8, 0, lpWideString, -1, pstrData_UTF8, size2, NULL, NULL);
			if(lpWideString)
				delete[] lpWideString;
			//UTF16->UTF-8変換-----------------------------------↑

			//URLEncode-----------------------------------↓
			char* ptrDataURLEncode=NULL;
			long URLEncodeLen=0;
			const int nLen = min((int)strlen(pstrData_UTF8), UTF8Len);
			size= UTF8Len *10;
			ptrDataURLEncode=new char[size];
			URLEncodeLen = size;
			memset(ptrDataURLEncode, 0x00, size);
			int iPos=0;
			BYTE cText=0;
			for (int i = 0; i < nLen; ++i)
			{
				cText = pstrData_UTF8[i];
				if((cText >= '0' && cText <= '9')
					||(cText >= 'a' && cText <= 'z')
					||(cText >= 'A' && cText <= 'Z')
					||	cText == '-' || cText == '_'  || cText == '.' || cText == '!' || cText == '~'
					|| cText == '*' || cText == '\'' || cText == '(' || cText == ')'
				)
				{
					memcpy(ptrDataURLEncode+iPos,&cText,1);
					iPos++;
				}
				else if (cText == ' ')
				{
					memcpy(ptrDataURLEncode + iPos,"%20", 3);
					iPos+=3;
				}
				else
				{
					char szFmt[4]={0};
					_snprintf_s(szFmt, sizeof(szFmt), "%%%02X", cText & 0xff);
					memcpy(ptrDataURLEncode + iPos, szFmt, 3);
					iPos+=3;
				}
			}
			if (pstrData_UTF8)
				delete[] pstrData_UTF8;

			strRet=ptrDataURLEncode;
			if(ptrDataURLEncode)
				delete[] ptrDataURLEncode;
			//URLEncode-----------------------------------↑

		}
		catch (...)
		{
			ATLASSERT(0);
		}
		return strRet;
	}


// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CHorizonLauncherApp theApp;
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// メイン シンボル
#include "URLRedirectCore.h"

class CTBRedirectorApp : public CWinApp
{
public:
	CTBRedirectorApp();
	CString m_strExeNameNoExt;//拡張子なしのファイル名
	CString m_strExeFullPath;
	CString m_CommandParam;
	CString m_OptionParam;

	CString m_strThinBridge_EXE_FullPath;
	CURLRedirectList m_RedirectList;
	CString m_strRedirectFilePath;
	CString m_strUserDataPath;


	void WriteDebugTraceDateTime(LPCTSTR msg);
	void ExecRedirect(LPCTSTR lpURL);
	void OpenAnotherBrowser(CURLRedirectDataClass* pRedirectData, const CString& strURL);
// オーバーライド
public:
	virtual BOOL InitInstance();
	BOOL IsDefaultBrowser();
	CString GetDefaultBrowser();
	CString GetDefaultHTMLBrowser();
// 実装

	DECLARE_MESSAGE_MAP()
};

extern CTBRedirectorApp theApp;

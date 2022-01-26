
// ThinBridgeRuleUpdater.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif
#include <sddl.h>

#include "resource.h"		// メイン シンボル
#define KEY_COMB_SHIFT		0x00000001
#define KEY_COMB_CTRL		0x00000010
#define KEY_COMB_ALT		0x00000100
#define KEY_COMB_LEFT		0x00001000
#define KEY_COMB_UP			0x00010000
#define KEY_COMB_RIGHT		0x00100000
#define KEY_COMB_DOWN		0x01000000

#define SECTION_DMZ	"CUSTOM18"
#define SECTION_CHROME_SWITCHER	"CUSTOM19"
#define SECTION_O365	"CUSTOM20"

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

// CThinBridgeRuleUpdaterApp:
// このクラスの実装については、ThinBridgeRuleUpdater.cpp を参照してください。
//
namespace SBUtil
{
	static inline void SplitEx(CStringArray& strArray,CString strTarget,LPCTSTR strDelimiter)
	{
		UINT intDelimiterLen=0;
		int intStart=0;
		int intEnd=0;
		//strTarget += strDelimiter;
		intDelimiterLen = (UINT)_tcslen(strDelimiter);
		intStart = 0;
		intEnd = strTarget.Find(strDelimiter, intStart);
		if(intEnd >= 0)
		{
			strArray.Add(strTarget.Mid(intStart, intEnd - intStart));
			intStart = intEnd + intDelimiterLen;
			strArray.Add(strTarget.Mid(intStart));
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
#include "locale.h"

class CConfData
{
public:
	CConfData()
	{
		m_strConfigServerURL1 = _T("");
		m_IsGPOSetting=FALSE;
		m_iInterval=10;
		m_iServerConnectionRule =0;
	}
	~CConfData() {}
	void Copy(CConfData* ptr)
	{
		if (ptr == NULL)return;
		m_strConfigServerURL1 = ptr->m_strConfigServerURL1;
		m_strConfigServerURL2 = ptr->m_strConfigServerURL2;
		m_strConfigServerURL3 = ptr->m_strConfigServerURL3;
		m_strConfigServerURL4 = ptr->m_strConfigServerURL4;
		m_strConfigServerURL5 = ptr->m_strConfigServerURL5;
		m_strConfigServerURL6 = ptr->m_strConfigServerURL6;
		m_iInterval = ptr->m_iInterval;
		m_iServerConnectionRule = ptr->m_iServerConnectionRule;
	}

	///////////////////////////////////////////////////
	CString m_strConfigServerURL1;
	CString m_strConfigServerURL2;
	CString m_strConfigServerURL3;
	CString m_strConfigServerURL4;
	CString m_strConfigServerURL5;
	CString m_strConfigServerURL6;

	CString m_strConfigServerURLOK;

	int m_iInterval;
	int m_iServerConnectionRule;

	//////////////////////////////////////////////////
	BOOL m_IsGPOSetting;
	BOOL IsGPO(){return m_IsGPOSetting;}
	BOOL ReadSettingGPO()
	{
		TCHAR REG_SETTING_KEY[] = _T("SOFTWARE\\Policies\\ThinBridge\\Rule");
		BOOL bRet = FALSE;
		DWORD Val = 0;
		HKEY  hKey = { 0 };
		LONG lResult = 0L;
		DWORD dwType = 0;
		DWORD dwSize = 0;
		DWORD iSize = 0;
		CString strVal;
		TCHAR* pVal = NULL;

		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_SETTING_KEY, 0, KEY_READ, &hKey);
		if (lResult == ERROR_SUCCESS)
		{
			Val = 0;
			iSize = 0;
			strVal.Empty();
			pVal = NULL;
			RegQueryValueEx(hKey, _T("ConfigServerURL"), NULL, &dwType, NULL, &iSize);
			if (iSize > 0)
			{
				iSize += 1;//+1=null
				pVal = new TCHAR[iSize];
				memset(pVal, 0x00, sizeof(TCHAR)*iSize);
				RegQueryValueEx(hKey, _T("ConfigServerURL"), NULL, &dwType, (LPBYTE)pVal, &iSize);
				strVal = pVal;
				delete[] pVal;
				strVal.TrimRight();
				strVal.TrimLeft();
				if (!strVal.IsEmpty())
				{
					m_strConfigServerURL1 = strVal;
					bRet=TRUE;
					m_IsGPOSetting = TRUE;
				}
			}
			RegQueryValueEx(hKey, _T("ConfigServerURL2"), NULL, &dwType, NULL, &iSize);
			if (iSize > 0)
			{
				iSize += 1;//+1=null
				pVal = new TCHAR[iSize];
				memset(pVal, 0x00, sizeof(TCHAR)*iSize);
				RegQueryValueEx(hKey, _T("ConfigServerURL2"), NULL, &dwType, (LPBYTE)pVal, &iSize);
				strVal = pVal;
				delete[] pVal;
				strVal.TrimRight();
				strVal.TrimLeft();
				if (!strVal.IsEmpty())
				{
					m_strConfigServerURL2 = strVal;
					bRet = TRUE;
					m_IsGPOSetting = TRUE;
				}
			}
			RegQueryValueEx(hKey, _T("ConfigServerURL3"), NULL, &dwType, NULL, &iSize);
			if (iSize > 0)
			{
				iSize += 1;//+1=null
				pVal = new TCHAR[iSize];
				memset(pVal, 0x00, sizeof(TCHAR)*iSize);
				RegQueryValueEx(hKey, _T("ConfigServerURL3"), NULL, &dwType, (LPBYTE)pVal, &iSize);
				strVal = pVal;
				delete[] pVal;
				strVal.TrimRight();
				strVal.TrimLeft();
				if (!strVal.IsEmpty())
				{
					m_strConfigServerURL3 = strVal;
					bRet = TRUE;
					m_IsGPOSetting = TRUE;
				}
			}
			RegQueryValueEx(hKey, _T("ConfigServerURL4"), NULL, &dwType, NULL, &iSize);
			if (iSize > 0)
			{
				iSize += 1;//+1=null
				pVal = new TCHAR[iSize];
				memset(pVal, 0x00, sizeof(TCHAR)*iSize);
				RegQueryValueEx(hKey, _T("ConfigServerURL4"), NULL, &dwType, (LPBYTE)pVal, &iSize);
				strVal = pVal;
				delete[] pVal;
				strVal.TrimRight();
				strVal.TrimLeft();
				if (!strVal.IsEmpty())
				{
					m_strConfigServerURL4 = strVal;
					bRet = TRUE;
					m_IsGPOSetting = TRUE;
				}
			}
			RegQueryValueEx(hKey, _T("ConfigServerURL5"), NULL, &dwType, NULL, &iSize);
			if (iSize > 0)
			{
				iSize += 1;//+1=null
				pVal = new TCHAR[iSize];
				memset(pVal, 0x00, sizeof(TCHAR)*iSize);
				RegQueryValueEx(hKey, _T("ConfigServerURL5"), NULL, &dwType, (LPBYTE)pVal, &iSize);
				strVal = pVal;
				delete[] pVal;
				strVal.TrimRight();
				strVal.TrimLeft();
				if (!strVal.IsEmpty())
				{
					m_strConfigServerURL5 = strVal;
					bRet = TRUE;
					m_IsGPOSetting = TRUE;
				}
			}
			RegQueryValueEx(hKey, _T("ConfigServerURL6"), NULL, &dwType, NULL, &iSize);
			if (iSize > 0)
			{
				iSize += 1;//+1=null
				pVal = new TCHAR[iSize];
				memset(pVal, 0x00, sizeof(TCHAR)*iSize);
				RegQueryValueEx(hKey, _T("ConfigServerURL6"), NULL, &dwType, (LPBYTE)pVal, &iSize);
				strVal = pVal;
				delete[] pVal;
				strVal.TrimRight();
				strVal.TrimLeft();
				if (!strVal.IsEmpty())
				{
					m_strConfigServerURL6 = strVal;
					bRet = TRUE;
					m_IsGPOSetting = TRUE;
				}
			}
			Val = 0;
			lResult = RegQueryValueEx(hKey, _T("Interval"), NULL, &dwType, NULL, &dwSize);
			if (lResult == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey, _T("Interval"), NULL, &dwType, (LPBYTE)&Val, &dwSize);
				int iW = Val;
				if (iW < 0 || iW>60)
					iW=0;
				m_iInterval = iW;
				bRet = TRUE;
				m_IsGPOSetting = TRUE;
			}
			Val = 0;
			lResult = RegQueryValueEx(hKey, _T("ServerConnectionRule"), NULL, &dwType, NULL, &dwSize);
			if (lResult == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey, _T("ServerConnectionRule"), NULL, &dwType, (LPBYTE)&Val, &dwSize);
				int iW = Val;
				m_iServerConnectionRule = iW;
				bRet = TRUE;
				m_IsGPOSetting = TRUE;
			}
			RegCloseKey(hKey);
		}
		return bRet;
	}
	void ReadSetting(LPCTSTR lpFilePath)
	{
		_wsetlocale(LC_ALL, _T("jpn"));
		CStdioFile in;
		CString strTemp;
		CString strLogMsgLine;

		if (in.Open(lpFilePath, CFile::modeRead | CFile::shareDenyWrite))
		{
			CString strTemp2;
			CString strTemp3;
			CStringArray strArray;
			while (in.ReadString(strTemp))
			{
				strTemp.TrimLeft();
				strTemp.TrimRight();
				strArray.RemoveAll();
				SBUtil::SplitEx(strArray, strTemp, _T("="));
				strLogMsgLine += strTemp;
				strLogMsgLine += "\n";

				if (strArray.GetSize() >= 2)
				{
					strTemp2 = strArray.GetAt(0);
					strTemp2.TrimLeft();
					strTemp2.TrimRight();

					strTemp3 = strArray.GetAt(1);
					strTemp3.TrimLeft();
					strTemp3.TrimRight();

					if (strTemp2.Find(_T(";")) == 0)
					{
						;
					}
					else if (strTemp2.Find(_T("#")) == 0)
					{
						;
					}
					else
					{
						if (strTemp2.CompareNoCase(_T("ConfigServerURL")) == 0)
						{
							m_strConfigServerURL1 = strTemp3;
						}
						else if (strTemp2.CompareNoCase(_T("ConfigServerURL2")) == 0)
						{
							m_strConfigServerURL2 = strTemp3;
						}
						else if (strTemp2.CompareNoCase(_T("ConfigServerURL3")) == 0)
						{
							m_strConfigServerURL3 = strTemp3;
						}
						else if (strTemp2.CompareNoCase(_T("ConfigServerURL4")) == 0)
						{
							m_strConfigServerURL4 = strTemp3;
						}
						else if (strTemp2.CompareNoCase(_T("ConfigServerURL5")) == 0)
						{
							m_strConfigServerURL5 = strTemp3;
						}
						else if (strTemp2.CompareNoCase(_T("ConfigServerURL6")) == 0)
						{
							m_strConfigServerURL6 = strTemp3;
						}
						else if (strTemp2.CompareNoCase(_T("Interval")) == 0)
						{
							if (!strTemp3.IsEmpty())
							{
								m_iInterval = _ttoi(strTemp3);
								if (m_iInterval < 0 || m_iInterval>60)
									m_iInterval = 0;
							}
							else
								m_iInterval = 10;
						}
						else if (strTemp2.CompareNoCase(_T("ServerConnectionRule")) == 0)
						{
							if (!strTemp3.IsEmpty())
								m_iServerConnectionRule = _ttoi(strTemp3);
							else
								m_iServerConnectionRule = 0;
						}
					}
				}
			}
			in.Close();
		}
	}
	BOOL WriteSetting(LPCTSTR lpFilePath)
	{
		_wsetlocale(LC_ALL, _T("jpn"));
		CString strConfFile;
		strConfFile = lpFilePath;
		//file backup
		::CopyFile(strConfFile + ".bak4", strConfFile + ".bak5", FALSE);
		::CopyFile(strConfFile + ".bak3", strConfFile + ".bak4", FALSE);
		::CopyFile(strConfFile + ".bak2", strConfFile + ".bak3", FALSE);
		::CopyFile(strConfFile + ".bak1", strConfFile + ".bak2", FALSE);
		::CopyFile(strConfFile, strConfFile + ".bak1", FALSE);
		SetLastError(NO_ERROR);

		CStdioFile out;
		if (out.Open(lpFilePath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
		{
			//command1
			CString strWriteData;
			strWriteData.Format(_T("ConfigServerURL=%s\n"), m_strConfigServerURL1);
			out.WriteString(strWriteData);
			strWriteData.Format(_T("ConfigServerURL2=%s\n"), m_strConfigServerURL2);
			out.WriteString(strWriteData);
			strWriteData.Format(_T("ConfigServerURL3=%s\n"), m_strConfigServerURL3);
			out.WriteString(strWriteData);
			strWriteData.Format(_T("ConfigServerURL4=%s\n"), m_strConfigServerURL4);
			out.WriteString(strWriteData);
			strWriteData.Format(_T("ConfigServerURL5=%s\n"), m_strConfigServerURL5);
			out.WriteString(strWriteData);
			strWriteData.Format(_T("ConfigServerURL6=%s\n"), m_strConfigServerURL6);
			out.WriteString(strWriteData);

			strWriteData.Format(_T("Interval=%d\n"), m_iInterval);
			out.WriteString(strWriteData);
			strWriteData.Format(_T("ServerConnectionRule=%d\n"), m_iServerConnectionRule);
			out.WriteString(strWriteData);

			out.WriteString(strWriteData);
			out.Close();
			return TRUE;
		}
		return FALSE;
	}
};

#define ERR_THINBRIDGE_BHO_INI_NOT_FOUND 1
#define ERR_THINBRIDGE_BHO_INI_FILE_OPEN 2
#define ERR_THINBRIDGE_BHO_INI_FILE_WRITE 3
#define ERR_CONFIG_SERVER 4
#define ERR_THINBRIDGE_BHO_INI_INVALID 5
#define INFO_CONFIG_SERVER_NOT_SETTING 6
#define SUCCESS_ALL 200
#define SUCCESS_NO_FILE_WRITE 201
#define SUCCESS_SAME_DATA 202

#define SECURITY_WIN32
#include "sspi.h"
#include "secext.h"
#pragma comment( lib, "secur32.lib" )

class CThinBridgeRuleUpdaterApp : public CWinApp
{
public:
	CThinBridgeRuleUpdaterApp();
	virtual ~CThinBridgeRuleUpdaterApp()
	{
		if (m_hMutex)
		{
			::CloseHandle(m_hMutex);
			m_hMutex = NULL;
		}
	}


	CString m_strExeNameNoExt;//拡張子なしのファイル名
	CString m_strExeFullPath;
	CString m_strExeFolderPath;

	CString m_strSetting_FileFullPath;
	CString m_strThinBridgeBHOFileFullPath;
	CString m_strExecLogFileDir;
	CString m_strExecLogFileFullPath;

	CString m_CommandParam;
	CConfData SettingConf;

	HANDLE m_hMutex;

	void ReadSetting();
	CString GetErrorMsg()
	{
		int iRet = m_iWriteThinBridgeBHO_ResultCode;
		CString strServerErrMsg;
		if (iRet == ERR_THINBRIDGE_BHO_INI_NOT_FOUND)
			strServerErrMsg = _T("エラー：ThinBridgeBHO.iniファイルが見つかりません。ERR_THINBRIDGE_BHO_INI_NOT_FOUND");
		else if (iRet == ERR_THINBRIDGE_BHO_INI_FILE_OPEN)
			strServerErrMsg = _T("エラー：ThinBridgeBHO.iniファイル オープンエラー ERR_THINBRIDGE_BHO_INI_FILE_OPEN");
		else if (iRet == ERR_THINBRIDGE_BHO_INI_FILE_WRITE)
			strServerErrMsg = _T("エラー：ThinBridgeBHO.iniファイルが書込みエラー ERR_THINBRIDGE_BHO_INI_FILE_WRITE");
		else if (iRet == ERR_CONFIG_SERVER)
			strServerErrMsg = _T("エラー：リダイレクト定義ファイルをサーバーから取得できませんでした。ERR_CONFIG_SERVER");
		else if(iRet == INFO_CONFIG_SERVER_NOT_SETTING)
			strServerErrMsg = _T("情報：リダイレクト定義ファイル取得先サーバーが設定されていません。INFO_CONFIG_SERVER_NOT_SETTING");
		else if (iRet == ERR_THINBRIDGE_BHO_INI_INVALID)
			strServerErrMsg = _T("エラー：リダイレクト定義ファイルの内容が不正です。ERR_THINBRIDGE_BHO_INI_INVALID");
		else if (iRet == SUCCESS_ALL)
			strServerErrMsg = _T("成功：全て処理に成功しました 差分あり SUCCESS_ALL");
		else if (iRet == SUCCESS_NO_FILE_WRITE)
			strServerErrMsg = _T("成功：ThinBridgeBHO.ini書込み無し-テスト実行成功 SUCCESS_NO_FILE_WRITE ");
		else if (iRet == SUCCESS_SAME_DATA)
			strServerErrMsg = _T("成功：全ての処理に成功しました 差分なし SUCCESS_SAME_DATA");
		else if (iRet == 0)
			strServerErrMsg = _T("N/A");
		else
			strServerErrMsg = _T("UNKNOWN_CODE");
		return strServerErrMsg;
	}

	void LogRotate(LPCTSTR LogFolderPath)
	{
		CTime time = CTime::GetCurrentTime();
		long number = 0;
		CString strTemp;
		//面倒なので、用意してしまう。
		CMapStringToString strASaveFileList;
		for (number = 31; number > 0; number--)
		{
			CTimeSpan span;
			span = CTimeSpan(number, 0, 0, 0);
			CTime timeStart = CTime::GetCurrentTime() - span;
			strTemp.Format(_T("ThinBridgeRuleUpdater%s.log"), timeStart.Format(_T("%Y-%m-%d")));
			strTemp.MakeUpper();
			strASaveFileList.SetAt(strTemp,strTemp);
		}
		//念の為　未来日に関しても31日分は消さない。
		for (number = 0; number <= 31; number++)
		{
			CTimeSpan span;
			span = CTimeSpan(number, 0, 0, 0);
			CTime timeStart = CTime::GetCurrentTime() + span;
			strTemp.Format(_T("ThinBridgeRuleUpdater%s.log"), timeStart.Format(_T("%Y-%m-%d")));
			strTemp.MakeUpper();
			strASaveFileList.SetAt(strTemp, strTemp);
		}

		//ログファイルを確認する。
		//基準
		CString strFindPath = LogFolderPath;
		strFindPath += _T("ThinBridgeRuleUpdater????-??-??.log");
		WIN32_FIND_DATA wfd = { 0 };
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
				BOOL bHit = FALSE;
				strFileNameTemp = wfd.cFileName;
				strFileNameTemp.MakeUpper();
				if (strASaveFileList.Lookup(strFileNameTemp, strFileNameTemp))
				{
					bHit = TRUE;
				}
				else
				{
					strDelFilePath = LogFolderPath;
					strDelFilePath += strFileNameTemp;
					strADeleteFileList.Add(strDelFilePath);
					bHit=FALSE;
				}
			}

		} while (::FindNextFile(h, &wfd));
		::FindClose(h);

		for (int iiii = 0; iiii < strADeleteFileList.GetCount(); iiii++)
		{
			strDelFilePath = strADeleteFileList.GetAt(iiii);
			::DeleteFile(strDelFilePath);
		}
		SetLastError(NO_ERROR);
	}
	int m_iWriteThinBridgeBHO_ResultCode;
	CString WriteThinBridgeBHO(CConfData* pSettingConf,BOOL bWriteBHO_ini,BOOL bFromGUI);
	CStringArray m_strArrayBHODataOrg;
	void WaitSec(DWORD dwWaitSec)
	{
		DWORD	dwTime = GetTickCount();
		while (GetTickCount() - dwTime < dwWaitSec)
		{
			MSG	msg = { 0 };
			if (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE | PM_QS_PAINT))
			{
				AfxGetThread()->PumpMessage();
			}
			::Sleep(100);
		}
	}
	BOOL InCtrixVDI()
	{
		BOOL bRet=FALSE;
		CString strTB;
		strTB= m_strExeFolderPath;
		strTB += _T("ThinBridgeCTXServer.exe");
		//ThinBridgeCTXServer.exeがあるか。
		if (PathFileExists(strTB))
		{
			//wfapi.dllがあるか。
			HMODULE hHandle={0};
			hHandle = ::LoadLibrary(_T("wfapi.dll"));
			if (hHandle)
			{
				bRet = TRUE;
				::FreeLibrary(hHandle);
			}
		}
		return bRet;
	}
	void WriteThinBridgeEnvironmentFlg(LPCTSTR lp)
	{
		TCHAR REG_TB_KEY[] = _T("SOFTWARE\\ThinBridge");
		CString strVal;
		strVal = lp;
		HKEY  hKey = { 0 };
		DWORD dwDisposition = 0;
		LONG lResult = 0L;
		CString strRegKey;
		strRegKey.Format(_T("%s"), REG_TB_KEY);

		//2020-09-29
		//何とかHKEY_CUに設定しようとしていたが無理があるのでHKEY_LMにセットする。
		//Chrome Extension NativeMessageingHost側でHKEY_LMをチェックするので問題なし（VDIの場合）
		//HKEY_LMに書き込むのでsystemアカウントでの実行の場合
		//systemアカウントのSIDの場合に実行。
		if (m_strCurrentSID == _T("S-1-5-18"))
		{
			lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, strRegKey,
				0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition);
			//if (m_strCurrentSID != m_strSID)
			//{
			//	CString strRegKeyV;
			//	strRegKeyV.Format(_T("%s\\%s"), m_strSID,strRegKey);
			//	lResult = RegCreateKeyEx(HKEY_USERS, strRegKeyV,
			//		0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition);
			//}
			//else
			//{
			//	lResult = RegCreateKeyEx(HKEY_CURRENT_USER, strRegKey,
			//		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
			//}

			if (lResult == ERROR_SUCCESS)
			{
				int iSize = 0;
				iSize = strVal.GetLength();
				TCHAR* pVal = NULL;
				iSize += 1;//+1=null
				pVal = new TCHAR[iSize];
				memset(pVal, 0x00, sizeof(TCHAR)*iSize);
				lstrcpyn(pVal, strVal, iSize);
				RegSetValueEx(hKey, _T("ClientType"), 0, REG_SZ, (LPBYTE)pVal, sizeof(TCHAR)*iSize);
				RegCloseKey(hKey);
				delete[] pVal;
			}
		}
	}
	void WriteThinBridgeFlg(LPCTSTR key,LPCTSTR lp)
	{
		TCHAR REG_TB_KEY[] = _T("SOFTWARE\\ThinBridge");
		CString strVal;
		strVal = lp;
		HKEY  hKey = { 0 };
		DWORD dwDisposition = 0;
		LONG lResult = 0L;
		CString strRegKey;
		strRegKey.Format(_T("%s"), REG_TB_KEY);
		lResult = RegCreateKeyEx(HKEY_CURRENT_USER, strRegKey,
			0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition);
		if (lResult == ERROR_SUCCESS)
		{
			int iSize = 0;
			iSize = strVal.GetLength();
			TCHAR* pVal = NULL;
			iSize += 1;//+1=null
			pVal = new TCHAR[iSize];
			memset(pVal, 0x00, sizeof(TCHAR)*iSize);
			lstrcpyn(pVal, strVal, iSize);
			RegSetValueEx(hKey, key, 0, REG_SZ, (LPBYTE)pVal, sizeof(TCHAR)*iSize);
			RegCloseKey(hKey);
			delete[] pVal;
		}
	}
	CString LogFmt(LPCTSTR lpMsg)
	{
		_wsetlocale(LC_ALL, _T("jpn"));
		CString strWriteLine;
		CString strDebugLine;
		CTime time = CTime::GetCurrentTime();
		strWriteLine.Format(_T("%s\t%s\r\n"), time.Format(_T("%Y-%m-%d %H:%M:%S")), lpMsg);
		strDebugLine.Format(_T("ThinBridgeRuleUpdater\t%s"), strWriteLine);
		_wsetlocale(LC_ALL, _T(""));
		return strDebugLine;
	}
	void LogRotateETC(LPCTSTR LogFolderPath, LPCTSTR lpPattern, LPCTSTR lpext)
	{
		CTime time = CTime::GetCurrentTime();
		long number = 0;
		CString strTemp;
		//面倒なので、用意してしまう。1年分保持。
		CMapStringToString strASaveFileList;
		for (number = 366; number > 0; number--)
		{
			CTimeSpan span;
			span = CTimeSpan(number, 0, 0, 0);
			CTime timeStart = CTime::GetCurrentTime() - span;
			strTemp.Format(_T("%s%s%s"), lpPattern, timeStart.Format(_T("%Y-%m-%d")), lpext);
			strTemp.MakeUpper();
			strASaveFileList.SetAt(strTemp, strTemp);
		}
		//念の為　未来日に関しても366日分は消さない。
		for (number = 0; number <= 366; number++)
		{
			CTimeSpan span;
			span = CTimeSpan(number, 0, 0, 0);
			CTime timeStart = CTime::GetCurrentTime() + span;
			strTemp.Format(_T("%s%s%s"), lpPattern, timeStart.Format(_T("%Y-%m-%d")), lpext);
			strTemp.MakeUpper();
			strASaveFileList.SetAt(strTemp, strTemp);
		}

		//ログファイルを確認する。
		//基準
		CString strFindPath = LogFolderPath;
		strFindPath += lpPattern;
		strFindPath += _T("????-??-??-??????");
		strFindPath += lpext;
		WIN32_FIND_DATA wfd = { 0 };
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
				BOOL bHit = FALSE;
				strFileNameTemp = wfd.cFileName;
				strFileNameTemp.MakeUpper();
				CString strFileTmp;
				int ipos = 0;
				ipos = strFileNameTemp.ReverseFind('-');
				strFileTmp = strFileNameTemp.Mid(0, ipos);
				strFileTmp += lpext;
				strFileTmp.MakeUpper();
				if (strASaveFileList.Lookup(strFileTmp, strFileTmp))
				{
					bHit = TRUE;
				}
				else
				{
					strDelFilePath = LogFolderPath;
					strDelFilePath += strFileNameTemp;
					strADeleteFileList.Add(strDelFilePath);
					bHit = FALSE;
				}
			}

		} while (::FindNextFile(h, &wfd));
		::FindClose(h);

		for (int iiii = 0; iiii < strADeleteFileList.GetCount(); iiii++)
		{
			strDelFilePath = strADeleteFileList.GetAt(iiii);
			::DeleteFile(strDelFilePath);
		}
		SetLastError(NO_ERROR);
	}

	void CompareFiles(LPCTSTR lpszFirstFile, LPCTSTR lpszSecondFile, LPCTSTR lpszOutputFile)
	{
		_wsetlocale(LC_ALL, _T("jpn"));
		CString strRet;
		#define MAX_LINE_LENGTH		4096
		int iCurrentLine1 = 0;
		int iCurrentLine2 = 0;
		int iTotalLines1 = 0;
		int iTotalLines2 = 0;
		int off1 = -1;
		int off2 = -1;
		TCHAR* lpszT1 = NULL;
		TCHAR* lpszT2 = NULL;
		TCHAR lpszText1[MAX_LINE_LENGTH] = { 0 };
		TCHAR lpszText2[MAX_LINE_LENGTH] = { 0 };
		TCHAR lpszText3[MAX_LINE_LENGTH] = { 0 };
		TCHAR lpszText1_Temp[MAX_LINE_LENGTH] = { 0 };
		TCHAR lpszText2_Temp[MAX_LINE_LENGTH] = { 0 };
		TCHAR* lpszT1_Temp = NULL;
		TCHAR* lpszT2_Temp = NULL;

		// Check for valid file names
		if ((lpszFirstFile != NULL) && (lpszSecondFile != NULL))
		{
			if (!PathFileExists(lpszFirstFile))
				return;
			if (!PathFileExists(lpszSecondFile))
				return;
			// Try to open files
			FILE* f1 = { 0 };
			_tfopen_s(&f1, lpszFirstFile, _T("r"));
			FILE* f2 = { 0 };
			_tfopen_s(&f2, lpszSecondFile, _T("r"));

			// Open log file
			FILE* f3 = NULL;
			_tfopen_s(&f3, lpszOutputFile, _T("w"));
			_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("Comparing files...\n"));
			_fputts(lpszText3, f3);
			_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("[File1]: %s\n"), lpszFirstFile);
			_fputts(lpszText3, f3);
			_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("[File2]: %s\n\n"), lpszSecondFile);
			_fputts(lpszText3, f3);

			// Check for valid files
			if ((f1 != NULL) && (f2 != NULL))
			{
				do
				{
					// Increment line counters
					iCurrentLine1++;
					iCurrentLine2++;

					// Read single line of text from files
					lpszT1 = _fgetts(lpszText1, MAX_LINE_LENGTH, f1);
					lpszT2 = _fgetts(lpszText2, MAX_LINE_LENGTH, f2);

					// Compare lines of text
					if (_tcscmp(lpszText1, lpszText2) != 0)
					{
						// Scan through second file looking for equal lines
						iTotalLines2 = 0;
						off2 = ftell(f2);
						FILE* f2t = { 0 };
						_tfopen_s(&f2t, lpszSecondFile, _T("r"));
						fseek(f2t, off2, SEEK_SET);
						do
						{
							// Skip different lines
							iTotalLines2++;
							lpszT2_Temp = _fgetts(lpszText2_Temp, MAX_LINE_LENGTH, f2t);
						} while ((lpszT2_Temp != NULL) && ((_tcscmp(lpszText1,
							lpszText2_Temp) != 0)));
						fclose(f2t);

						// Scan through first file looking for equal lines
						iTotalLines1 = 0;
						off1 = ftell(f1);
						FILE* f1t = { 0 };
						_tfopen_s(&f1t, lpszFirstFile, _T("r"));
						fseek(f1t, off1, SEEK_SET);
						do
						{
							// Skip different lines
							iTotalLines1++;
							lpszT1_Temp = _fgetts(lpszText1_Temp, MAX_LINE_LENGTH, f1t);
						} while ((lpszT1_Temp != NULL) && ((_tcscmp(lpszText2,
							lpszText1_Temp) != 0)));
						fclose(f1t);

						// Compare lines passed (find minimum)
						if ((lpszT1_Temp != NULL) || (lpszT2_Temp != NULL))
						{
							if (iTotalLines2 < iTotalLines1)
							{
								_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("\n******************************\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("*       StartOfSection       *\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("******************************\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("[File2, %d]: %s"),
									iCurrentLine2, lpszText2);
								_fputts(lpszText3, f3);
								off2 = ftell(f2);
								FILE* f2t = { 0 };
								_tfopen_s(&f2t, lpszSecondFile, _T("r"));
								fseek(f2t, off2, SEEK_SET);
								for (int i = 0; i < iTotalLines2 - 1; i++)
								{
									_fgetts(lpszText2_Temp, MAX_LINE_LENGTH, f2t);
									_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("[File2, %d]: %s"),
										iCurrentLine2 + i + 1, lpszText2_Temp);
									_fputts(lpszText3, f3);
								}
								_fgetts(lpszText2_Temp, MAX_LINE_LENGTH, f2t);
								off2 = ftell(f2t);
								fseek(f2, off2, SEEK_SET);
								fclose(f2t);
								iCurrentLine2 += iTotalLines2;
								_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("******************************\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("*        EndOfSection        *\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("******************************\n\n"));
								_fputts(lpszText3, f3);
							}
							else
							{
								_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("\n******************************\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("*       StartOfSection       *\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("******************************\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("[File1, %d]: %s"), iCurrentLine1,
									lpszText1);
								_fputts(lpszText3, f3);
								off1 = ftell(f1);
								FILE* f1t = { 0 };
								_tfopen_s(&f1t, lpszFirstFile, _T("r"));
								fseek(f1t, off1, SEEK_SET);
								for (int i = 0; i < iTotalLines1 - 1; i++)
								{
									_fgetts(lpszText1_Temp, MAX_LINE_LENGTH, f1t);
									_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("[File1, %d]: %s"),
										iCurrentLine1 + i + 1, lpszText1_Temp);
									_fputts(lpszText3, f3);
								}
								_fgetts(lpszText1_Temp, MAX_LINE_LENGTH, f1t);
								off1 = ftell(f1t);
								fseek(f1, off1, SEEK_SET);
								fclose(f1t);
								iCurrentLine1 += iTotalLines1;
								_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("******************************\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("*        EndOfSection        *\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("******************************\n\n"));
								_fputts(lpszText3, f3);
							}
						}
						else
						{
							_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("\n******************************\n"));
							_fputts(lpszText3, f3);
							_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("*       StartOfSection       *\n"));
							_fputts(lpszText3, f3);
							_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("******************************\n"));
							_fputts(lpszText3, f3);
							_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("[File1, %d]: %s"), iCurrentLine1,
								lpszText1);
							_fputts(lpszText3, f3);
							_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("[File2, %d]: %s"), iCurrentLine2,
								lpszText2);
							_fputts(lpszText3, f3);
							_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("******************************\n"));
							_fputts(lpszText3, f3);
							_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("*        EndOfSection        *\n"));
							_fputts(lpszText3, f3);
							_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("******************************\n\n"));
							_fputts(lpszText3, f3);
						}
					}
				} while ((lpszT1 != NULL) && (lpszT2 != NULL));

				_fputts(_T("\nSuccess: The files compared.\n"), f3);
			}
			else
			{
				// The first file can not be found
				if (f1 == NULL)
				{
					_fputts(_T("Error: The first file can not be found.\n"), f3);
				}

				// The second file can not be found
				if (f2 == NULL)
				{
					_fputts(_T("Error: The second file can not be found.\n"), f3);
				}
			}
			// Close files
			if (f1)
				fclose(f1);
			if (f2)
				fclose(f2);

			// Close log file
			if (f3)
				fclose(f3);
		}
	}


// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()


////////////
public:
	CString m_strExecUserName;
	CString m_strDomainUserName;
	CString m_strSID;
	CString m_strCurrentSID;
	CString GetUserNameData()
	{
		if (m_strDomainUserName.IsEmpty())
		{
			TCHAR szBuffer[512] = { 0 };
			ULONG iSize = 512;
			::GetUserNameEx(NameSamCompatible, szBuffer, &iSize);
			CString strRet;
			strRet = szBuffer;
			strRet.Replace(_T("\\"), _T("/"));
			strRet.TrimLeft();
			strRet.TrimRight();
			m_strExecUserName= strRet;
			m_strDomainUserName = strRet;
		}
		return m_strDomainUserName;
	}

	void SetActiveUserNameSID();
	int ExecActiveSession();

	CString MyConvertSidToStringSid(PSID pSid)
	{
		LPTSTR p = NULL;
		BOOL br = ::ConvertSidToStringSid(pSid, &p);
		if (!br)
		{
			return _T("");
		}
		CString strResult = p;
		::LocalFree(p);
		return strResult;
	}
	CString GetTokenStringSid(HANDLE hToken)
	{
		DWORD dwTokenUserLength = 0;
		::GetTokenInformation(hToken, TokenUser, 0, 0, &dwTokenUserLength);
		TOKEN_USER* pTokenUser = (TOKEN_USER*)::LocalAlloc(LMEM_FIXED, dwTokenUserLength);
		if (pTokenUser == NULL)
		{
			return _T("");
		}
		BOOL br = ::GetTokenInformation(hToken, TokenUser, pTokenUser, dwTokenUserLength, &dwTokenUserLength);
		if (!br)
		{
			::LocalFree(pTokenUser);
			return _T("");
		}
		CString strResult = MyConvertSidToStringSid(pTokenUser->User.Sid);
		::LocalFree(pTokenUser);
		return strResult;
	}
	CString GetProcessStringSid(HANDLE hProcess)
	{
		HANDLE hToken = NULL;
		BOOL br = ::OpenProcessToken(hProcess, TOKEN_QUERY, &hToken);
		if (!br)
		{
			return _T("");
		}
		CString strResult = GetTokenStringSid(hToken);
		::CloseHandle(hToken);
		return strResult;
	}
	// 現在の(プロセスの)ユーザのSIDの取得
	CString GetCurrentProcessStringSid()
	{
		HANDLE hProcess = ::GetCurrentProcess();
		return GetProcessStringSid(hProcess);
	}

};

extern CThinBridgeRuleUpdaterApp theApp;

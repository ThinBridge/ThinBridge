
// TBo365URLSync.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
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

// CTBo365URLSyncApp:
// このクラスの実装については、TBo365URLSync.cpp を参照してください。
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
	}
	~CConfData() {}
	void SetDefaultData()
	{
		//DefaultValue
		m_strO365URL = _T("https://endpoints.office.com/endpoints/worldwide?clientrequestid=ab1560fd-5da2-428d-c2ca-a14bcd34436c");
		//m_strFilter_ServiceArea;
		//m_strFilter_Category = _T("Optimize|Allow");
		m_strFilter_Required=_T("TRUE");
		m_strFilter_TcpPort = _T("443|80,443");

	}
	void Copy(CConfData* ptr)
	{
		if (ptr == NULL)return;
		m_strO365URL = ptr->m_strO365URL;
		m_strFilter_ServiceArea = ptr->m_strFilter_ServiceArea;
		m_strFilter_Category = ptr->m_strFilter_Category;
		m_strFilter_Required = ptr->m_strFilter_Required;
		m_strFilter_TcpPort = ptr->m_strFilter_TcpPort;
		ptr->m_arr_URL.Copy(m_arr_URL);
		ptr->m_arr_URL_EX.Copy(m_arr_URL_EX);
	}

	///////////////////////////////////////////////////
	CString m_strO365URL;
	CString m_strFilter_ServiceArea;
	CString m_strFilter_Category;
	CString m_strFilter_Required;
	CString m_strFilter_TcpPort;
	CStringArray m_arr_URL;
	CStringArray m_arr_URL_EX;

	//////////////////////////////////////////////////
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
						if (strTemp2.CompareNoCase(_T("O365URL")) == 0)
						{
							m_strO365URL = strTemp3;
						}
						else if (strTemp2.CompareNoCase(_T("Filter_ServiceArea")) == 0)
						{
							m_strFilter_ServiceArea = strTemp3;
						}
						else if (strTemp2.CompareNoCase(_T("Filter_Category")) == 0)
						{
							m_strFilter_Category = strTemp3;
						}
						else if (strTemp2.CompareNoCase(_T("Filter_Required")) == 0)
						{
							m_strFilter_Required = strTemp3;
						}
						else if (strTemp2.CompareNoCase(_T("Filter_TcpPort")) == 0)
						{
							m_strFilter_TcpPort = strTemp3;
						}

					}
				}
				else
				{
					if (strTemp.IsEmpty())
						continue;
					if (strTemp.Find(_T("-")) == 0)
					{
						m_arr_URL_EX.Add(strTemp.Mid(1));
					}
					else
					{
						m_arr_URL.Add(strTemp);
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
			strWriteData.Format(_T("O365URL=%s\n"), m_strO365URL);
			out.WriteString(strWriteData);

			strWriteData.Format(_T("Filter_ServiceArea=%s\n"), m_strFilter_ServiceArea);
			out.WriteString(strWriteData);
			strWriteData.Format(_T("Filter_Category=%s\n"), m_strFilter_Category);
			out.WriteString(strWriteData);
			strWriteData.Format(_T("Filter_Required=%s\n"), m_strFilter_Required);
			out.WriteString(strWriteData);
			strWriteData.Format(_T("Filter_TcpPort=%s\n"), m_strFilter_TcpPort);
			out.WriteString(strWriteData);
			for (int i = 0; i < m_arr_URL.GetCount(); i++)
			{
				strWriteData.Format(_T("%s\n"), m_arr_URL.GetAt(i));
				out.WriteString(strWriteData);
			}
			for (int i = 0; i < m_arr_URL_EX.GetCount(); i++)
			{
				strWriteData.Format(_T("-%s\n"), m_arr_URL_EX.GetAt(i));
				out.WriteString(strWriteData);
			}

			out.Close();
			return TRUE;
		}
		return FALSE;
	}
};

#define ERR_THINBRIDGE_BHO_INI_NOT_FOUND 1
#define ERR_THINBRIDGE_BHO_INI_FILE_OPEN 2
#define ERR_THINBRIDGE_BHO_INI_FILE_WRITE 3
#define ERR_O365_API 4
#define ERR_O365_JSON 5
#define ERR_O365_TB_RULE 6
#define SUCCESS_ALL 200
#define SUCCESS_NO_FILE_WRITE 201
#define SUCCESS_SAME_DATA 202

#define SECURITY_WIN32
#include "sspi.h"
#include "secext.h"
#pragma comment( lib, "secur32.lib" )
#include <sddl.h>

class CTBo365URLSyncApp : public CWinApp
{
public:
	CTBo365URLSyncApp();
	virtual ~CTBo365URLSyncApp()
	{
	}
	CString m_strExeNameNoExt;//拡張子なしのファイル名
	CString m_strExeFullPath;
	CString m_strSetting_FileFullPath;
	CString m_strThinBridgeBHOFileFullPath;
	CString m_strExecLogFileDir;
	CString m_strExecLogFileFullPath;

	CString m_strExecDateTime;
	CString m_strO365EndpointJsonLogFileDir;
	CString m_strO365EndpointJsonLogFileFullPath;
	CString m_strO365EndpointCSVLogFileDir;
	CString m_strO365EndpointCSVLogFileFullPath;
	CString m_strO365EndpointTBRuleLogFileDir;
	CString m_strO365EndpointTBRuleLogFileFullPath;
	CString m_strO365EndpointTBRuleDiffLogFileDir;
	CString m_strO365EndpointTBRuleDiffLogFileFullPath;

	CString m_strO365EndpointTBRuleDiffLogFileDirINI;
	CString m_strO365EndpointTBRuleDiffLogFileFullPathINI;
	CString m_strO365EndpointTBRuleDiffLogFileDirNOW;
	CString m_strO365EndpointTBRuleDiffLogFileFullPathNOW;


	CString m_strTBRuleHistoryLogFileDir;
	CString m_strTBRuleHistoryLogFileFullPath_Before;
	CString m_strTBRuleHistoryLogFileFullPath_After;
	CString m_strTBRuleHistoryLogFileFullPath_Diff;


	CString m_CommandParam;
	CConfData SettingConf;

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
		else if (iRet == ERR_O365_JSON)
			strServerErrMsg = _T("エラー：JSONデータが取得できませんでした ERR_O365_JSON");
		else if (iRet == ERR_O365_TB_RULE)
			strServerErrMsg = _T("エラー：ThinBridgeルールが生成できませんでした ERR_O365_TB_RULE");
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
		for (number = 180; number > 0; number--)
		{
			CTimeSpan span;
			span = CTimeSpan(number, 0, 0, 0);
			CTime timeStart = CTime::GetCurrentTime() - span;
			strTemp.Format(_T("%s%s.log"), _T("TBo365URLSync"), timeStart.Format(_T("%Y-%m-%d")));
			strTemp.MakeUpper();
			strASaveFileList.SetAt(strTemp,strTemp);
		}
		//念の為　未来日に関しても180日分は消さない。
		for (number = 0; number <= 180; number++)
		{
			CTimeSpan span;
			span = CTimeSpan(number, 0, 0, 0);
			CTime timeStart = CTime::GetCurrentTime() + span;
			strTemp.Format(_T("%s%s.log"), _T("TBo365URLSync"), timeStart.Format(_T("%Y-%m-%d")));
			strTemp.MakeUpper();
			strASaveFileList.SetAt(strTemp, strTemp);
		}

		//ログファイルを確認する。
		//基準
		CString strFindPath = LogFolderPath;
		strFindPath += _T("TBo365URLSync");
		strFindPath += _T("????-??-??.log");
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
	void LogRotateETC(LPCTSTR LogFolderPath,LPCTSTR lpPattern,LPCTSTR lpext)
	{
		CTime time = CTime::GetCurrentTime();
		long number = 0;
		CString strTemp;
		//面倒なので、用意してしまう。
		CMapStringToString strASaveFileList;
		for (number = 180; number > 0; number--)
		{
			CTimeSpan span;
			span = CTimeSpan(number, 0, 0, 0);
			CTime timeStart = CTime::GetCurrentTime() - span;
			strTemp.Format(_T("%s%s%s"), lpPattern, timeStart.Format(_T("%Y-%m-%d")), lpext);
			strTemp.MakeUpper();
			strASaveFileList.SetAt(strTemp, strTemp);
		}
		//念の為　未来日に関しても180日分は消さない。
		for (number = 0; number <= 180; number++)
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
				int ipos=0;
				ipos = strFileNameTemp.ReverseFind('-');
				strFileTmp = strFileNameTemp.Mid(0,ipos);
				strFileTmp+=lpext;
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
	void CompareFiles(LPCTSTR lpszFirstFile,LPCTSTR lpszSecondFile, LPCTSTR lpszOutputFile)
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
		TCHAR lpszText1[MAX_LINE_LENGTH]={0};
		TCHAR lpszText2[MAX_LINE_LENGTH]={0};
		TCHAR lpszText3[MAX_LINE_LENGTH]={0};
		TCHAR lpszText1_Temp[MAX_LINE_LENGTH]={0};
		TCHAR lpszText2_Temp[MAX_LINE_LENGTH]={0};
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
			FILE* f1 = {0};
			_tfopen_s(&f1,lpszFirstFile, _T("r"));
			FILE* f2 = {0};
			_tfopen_s(&f2,lpszSecondFile, _T("r"));

			// Open log file
			FILE* f3 = NULL;
			_tfopen_s(&f3,lpszOutputFile, _T("w"));
			_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("Comparing files...\n"));
			_fputts(lpszText3, f3);
			_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("[File1]: %s\n"), lpszFirstFile);
			_fputts(lpszText3, f3);
			_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("[File2]: %s\n\n"), lpszSecondFile);
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
						FILE* f2t = {0};
						_tfopen_s(&f2t,lpszSecondFile, _T("r"));
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
						FILE* f1t = {0};
						_tfopen_s(&f1t,lpszFirstFile, _T("r"));
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
								_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("\n******************************\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("*       StartOfSection       *\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T( "******************************\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T( "[File2, %d]: %s"),
									iCurrentLine2, lpszText2);
								_fputts(lpszText3, f3);
								off2 = ftell(f2);
								FILE* f2t = {0};
								_tfopen_s(&f2t,lpszSecondFile, _T("r"));
								fseek(f2t, off2, SEEK_SET);
								for (int i = 0; i < iTotalLines2 - 1; i++)
								{
									_fgetts(lpszText2_Temp, MAX_LINE_LENGTH, f2t);
									_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("[File2, %d]: %s"),
										iCurrentLine2 + i + 1, lpszText2_Temp);
									_fputts(lpszText3, f3);
								}
								_fgetts(lpszText2_Temp, MAX_LINE_LENGTH, f2t);
								off2 = ftell(f2t);
								fseek(f2, off2, SEEK_SET);
								fclose(f2t);
								iCurrentLine2 += iTotalLines2;
								_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("******************************\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("*        EndOfSection        *\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("******************************\n\n"));
								_fputts(lpszText3, f3);
							}
							else
							{
								_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("\n******************************\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("*       StartOfSection       *\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("******************************\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("[File1, %d]: %s"), iCurrentLine1,
									lpszText1);
								_fputts(lpszText3, f3);
								off1 = ftell(f1);
								FILE* f1t = {0};
								_tfopen_s(&f1t,lpszFirstFile, _T("r"));
								fseek(f1t, off1, SEEK_SET);
								for (int i = 0; i < iTotalLines1 - 1; i++)
								{
									_fgetts(lpszText1_Temp, MAX_LINE_LENGTH, f1t);
									_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("[File1, %d]: %s"),
										iCurrentLine1 + i + 1, lpszText1_Temp);
									_fputts(lpszText3, f3);
								}
								_fgetts(lpszText1_Temp, MAX_LINE_LENGTH, f1t);
								off1 = ftell(f1t);
								fseek(f1, off1, SEEK_SET);
								fclose(f1t);
								iCurrentLine1 += iTotalLines1;
								_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("******************************\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("*        EndOfSection        *\n"));
								_fputts(lpszText3, f3);
								_stprintf_s(lpszText3, MAX_LINE_LENGTH, _T("******************************\n\n"));
								_fputts(lpszText3, f3);
							}
						}
						else
						{
							_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("\n******************************\n"));
							_fputts(lpszText3, f3);
							_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("*       StartOfSection       *\n"));
							_fputts(lpszText3, f3);
							_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("******************************\n"));
							_fputts(lpszText3, f3);
							_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("[File1, %d]: %s"), iCurrentLine1,
								lpszText1);
							_fputts(lpszText3, f3);
							_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("[File2, %d]: %s"), iCurrentLine2,
								lpszText2);
							_fputts(lpszText3, f3);
							_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("******************************\n"));
							_fputts(lpszText3, f3);
							_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("*        EndOfSection        *\n"));
							_fputts(lpszText3, f3);
							_stprintf_s(lpszText3, MAX_LINE_LENGTH,_T("******************************\n\n"));
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
	int m_iWriteThinBridgeBHO_ResultCode;
	CString WriteThinBridgeBHO(CConfData* pSettingConf,BOOL bWriteBHO_ini,BOOL bLogAppend);
	CStringArray m_strArrayBHODataOrg;

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
			m_strExecUserName = strRet;
			m_strDomainUserName = strRet;
		}
		return m_strDomainUserName;
	}

	void SetActiveUserNameSID();
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
	void WriteThinBridgeFlg(LPCTSTR key, LPCTSTR lp)
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
};
class CIconHelper
{
private:
	HICON m_hICON;
public:
	CIconHelper() { m_hICON = NULL; }
	~CIconHelper() { Release(); }
	void Release() {
		if (m_hICON)
		{
			::DestroyIcon(m_hICON);
			m_hICON = NULL;
		}
	}
	void SetIcon(const HICON ico) {
		if (ico)
		{
			Release();
			m_hICON = ico;
		}
	}
	HICON GetIcon() {
		return m_hICON;
	}
	operator HICON() { return GetIcon(); }
	HICON operator=(HICON ico) {
		SetIcon(ico);
		return GetIcon();
	}
};

class CDlgMsgBox : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMsgBox)

public:
	CDlgMsgBox(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CDlgMsgBox();
	CString m_strMsg;
	CString m_strTitle;
	// ダイアログ データ
	enum { IDD = IDD_DLG_MSGBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	CEdit m_Edit;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

// CDlgRDEdit ダイアログ
class CDlgRDEdit : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRDEdit)

public:
	CDlgRDEdit(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CDlgRDEdit();
	CString m_strURL;

	// ダイアログ データ
	enum { IDD = IDD_DLG_RD_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
class CDlgEditURLML : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgEditURLML)

public:
	CDlgEditURLML(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CDlgEditURLML();

	// ダイアログ データ
	enum { IDD = IDD_DLG_EDIT_URL_ML };
	CString m_strEditData;
	CString m_strTitle;
	CStringArray m_strArrayResult;
	BOOL bValidData();

protected:
	BOOL bFirstFlg;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Edit;
	int m_SelPosS;
	int m_SelPosE;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnPaint();
};


extern CTBo365URLSyncApp theApp;
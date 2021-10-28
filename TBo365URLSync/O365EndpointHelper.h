#pragma once
#include "stdafx.h"
#include "TBo365URLSync.h"
#include "locale.h"

namespace SBUtil2
{
	static inline void Split(CStringArray* pstrArray, LPCTSTR szTarget, LPCTSTR szDelimiter)
	{
		if (!szTarget)
			return;
		if (!szDelimiter)
			return;
		if (!pstrArray)
			return;

		pstrArray->RemoveAll();

		CString strTarget(szTarget);
		size_t intDelimiterLen = 0;
		size_t intStart = 0;
		int intEnd = 0;

		strTarget += szDelimiter;
		intDelimiterLen = _tcslen(szDelimiter);
		intStart = 0;
		while (intEnd = strTarget.Find(szDelimiter, intStart), intEnd >= 0)
		{
			pstrArray->Add(strTarget.Mid(intStart, intEnd - intStart));
			//::OutputDebugString(strTarget.Mid(intStart, intEnd - intStart));
			intStart = intEnd + intDelimiterLen;
		}
		return;
	}
};
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
//////////////////////////////////////////////////////////
#define URLBUFFER_SIZE 4096
//thinforie SG Server Status
#define ERR_SERVER_SETTINGS_NOT_FOUND 1
#define ERR_SERVER_SESSION 2
#define ERR_SERVER_SETTINGS_INVALID 3
#define ERR_SERVER_CONNECT	4
#define ERR_SERVER_REQUEST	5
#define ERR_SERVER_SEND_REQUEST	6
#define ERR_SERVER_RECEIVE_RESPONSE	7
#define ERR_SERVER_QUERY_HEADERS	8
#define ERR_SERVER_HTTP_STATUS	9
#define ERR_SERVER_RESPONSE_EMPTY	10
#define ERR_SERVER_RESPONSE_INVALID 11
#define ERR_SERVER_RESPONSE_STOP	12
#define ERR_SERVER_429				13
#define ERR_SERVER_ETC	999
#define OK_SERVER 200

class WinHttpHelper
{
public:
	WinHttpHelper()
	{
		m_hConnect = { 0 };
		m_hRequest = { 0 };
		m_hSession = { 0 };
		ZeroMemory(&m_URLComponents, sizeof(URL_COMPONENTS));
		m_URLComponents.dwStructSize = sizeof(URL_COMPONENTS);
		ZeroMemory(szHostName, sizeof(szHostName));
		ZeroMemory(szUrlPath, sizeof(szUrlPath));

	}
	~WinHttpHelper() { Cleanup(); }
	void Cleanup()
	{
		if (m_hRequest)
		{
			WinHttpCloseHandle(m_hRequest);
			m_hRequest = { 0 };
		}
		if (m_hConnect)
		{
			WinHttpCloseHandle(m_hConnect);
			m_hConnect = { 0 };
		}
		if (m_hSession)
		{
			WinHttpCloseHandle(m_hSession);
			m_hSession = { 0 };
		}
		ZeroMemory(&m_URLComponents, sizeof(URL_COMPONENTS));
		m_URLComponents.dwStructSize = sizeof(URL_COMPONENTS);
		ZeroMemory(szHostName, sizeof(szHostName));
		ZeroMemory(szUrlPath, sizeof(szUrlPath));
	}
	// セッションオブジェクト
	HINTERNET m_hConnect;
	HINTERNET m_hRequest;
	HINTERNET m_hSession;
	//URL
	URL_COMPONENTS m_URLComponents;
	TCHAR szHostName[URLBUFFER_SIZE];
	TCHAR szUrlPath[URLBUFFER_SIZE];
	CString m_strVerb;

	BOOL InitUrl(LPCTSTR lpURL, LPCTSTR lpVerb /*GET | POST*/)
	{
		Cleanup();
		CString strURL(lpURL);
		// URL解析
		ZeroMemory(&m_URLComponents, sizeof(URL_COMPONENTS));
		m_URLComponents.dwStructSize = sizeof(URL_COMPONENTS);

		m_URLComponents.lpszHostName = szHostName;
		m_URLComponents.lpszUrlPath = szUrlPath;
		m_URLComponents.dwHostNameLength = URLBUFFER_SIZE;
		m_URLComponents.dwUrlPathLength = URLBUFFER_SIZE;

		if (!WinHttpCrackUrl(strURL, 0, 0, &m_URLComponents))
		{
			// URLの解析に失敗
			return FALSE;
		}
		m_strVerb = lpVerb;
		return TRUE;
	}
	BOOL InitSession()
	{
		WINHTTP_CURRENT_USER_IE_PROXY_CONFIG ieProxyConfig = { 0 };
		ZeroMemory(&ieProxyConfig, sizeof(WINHTTP_CURRENT_USER_IE_PROXY_CONFIG));
		WinHttpGetIEProxyConfigForCurrentUser(&ieProxyConfig);
		if (ieProxyConfig.lpszProxy)
		{
			// InternetExplorerのプロキシ設定でセッションオブジェクト作成
			m_hSession = WinHttpOpen(L"ThinBridge",	// UserAgent名
				WINHTTP_ACCESS_TYPE_NAMED_PROXY,	// プロキシ設定
				ieProxyConfig.lpszProxy,	// プロキシ名
				ieProxyConfig.lpszProxyBypass ? ieProxyConfig.lpszProxyBypass : WINHTTP_NO_PROXY_BYPASS,	// プロキシバイパス
				WINHTTP_FLAG_ASYNC);		// 振る舞いオプション。現状WINHTTP_FLAG_ASYNCのみ指定可能。
		}
		else
		{
			m_hSession = WinHttpOpen(L"ThinBridge",	// UserAgent名 
				WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,	// プロキシ設定
				WINHTTP_NO_PROXY_NAME,	// プロキシ名
				WINHTTP_NO_PROXY_BYPASS,	// プロキシバイパス
				WINHTTP_FLAG_ASYNC);		// 振る舞いオプション。現状WINHTTP_FLAG_ASYNCのみ指定可能。
		}
		if (!m_hSession)
		{
			return FALSE;
		}

		WinHttpSetTimeouts(m_hSession,
			10000, //dwResolveTimeout
			10000,//dwConnectTimeout
			10000,//dwSendTimeout
			10000//dwReceiveTimeout
		);
		return TRUE;
	}

	BOOL InitConnect()
	{
		// 変数
		CString strServer;
		INTERNET_PORT nPort = 0;
		strServer = m_URLComponents.lpszHostName;
		nPort = m_URLComponents.nPort;

		// HTTP接続
		m_hConnect = WinHttpConnect(m_hSession, strServer, nPort, 0);
		if (NULL == m_hConnect)
		{
			return FALSE;
		}
		return TRUE;
	}

	BOOL InitRequest()
	{
		CString strObjectName;
		// HTTPかHTTPSかそれ以外か
		DWORD dwOpenRequestFlag = (INTERNET_SCHEME_HTTPS == m_URLComponents.nScheme) ? WINHTTP_FLAG_SECURE : 0;
		dwOpenRequestFlag = dwOpenRequestFlag | WINHTTP_FLAG_REFRESH;
		strObjectName = m_URLComponents.lpszUrlPath;
		// HTTP接続を開く
		m_hRequest = WinHttpOpenRequest(m_hConnect,
			m_strVerb,
			strObjectName,
			NULL,
			WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			dwOpenRequestFlag);
		if (NULL == m_hRequest)
		{
			return FALSE;
		}
		//自己証明書対応 2020-12-07 Proxyがらみで自己証明書を通過する可能性を考慮
		if (INTERNET_SCHEME_HTTPS == m_URLComponents.nScheme)
		{
			DWORD dwSSLFlags =
				SECURITY_FLAG_IGNORE_UNKNOWN_CA |
				SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE |
				SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
				SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;

			if (!WinHttpSetOption(
				m_hRequest,
				WINHTTP_OPTION_SECURITY_FLAGS,
				&dwSSLFlags,
				sizeof(dwSSLFlags)))
			{
				return FALSE;
			}
		}

		return TRUE;
	}
};
//#include <xmllite.h>
//#pragma comment(lib, "xmllite.lib")

class CO365EndpointData
{
public:
	CO365EndpointData() {}
	~CO365EndpointData() {}
	CString id;
	CString serviceArea;
	CString serviceAreaDisplayName;
	CStringArray urls;
	CStringArray ips;
	CString tcpPorts;
	CString udpPorts;
	CString expressRoute;
	CString category;
	CString required;
	CString	notes;

	void Clear()
	{
		id.Empty();
		serviceArea.Empty();
		serviceAreaDisplayName.Empty();
		urls.RemoveAll();
		ips.RemoveAll();
		tcpPorts.Empty();
		udpPorts.Empty();
		expressRoute.Empty();
		category.Empty();
		required.Empty();
		notes.Empty();
	}
	void Copy(const CO365EndpointData& dat)
	{
		id = dat.id;
		serviceArea=dat.serviceArea;
		serviceAreaDisplayName=dat.serviceAreaDisplayName;
		urls.Copy(dat.urls);
		ips.Copy(dat.ips);
		tcpPorts=dat.tcpPorts;
		udpPorts=dat.udpPorts;
		expressRoute=dat.expressRoute;
		category=dat.category;
		required=dat.required;
		notes=dat.notes;
	}
	CString GetCSVData()
	{
		_wsetlocale(LC_ALL, _T("jpn"));
		CString strURLS;
		CString strRet;

		for (int i = 0; i < urls.GetCount(); i++)
		{
			CString strLine;
			strLine.Format(_T("\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\r\n")
				, id
				, serviceArea
				, serviceAreaDisplayName
				, category
				, required
				, tcpPorts
				, urls.GetAt(i)
				, notes);
			strRet += strLine;
		}
		_wsetlocale(LC_ALL, _T(""));
		return strRet;
	}
	CString GetThinBridgeRules()
	{
		_wsetlocale(LC_ALL, _T("jpn"));
		CString strURLS;
		CString strRet;
		for (int i = 0; i < urls.GetCount(); i++)
		{
			CString strLine;
			if(tcpPorts==_T("443"))
			{
				strLine.Format(_T("https://%s/*\r\n"),urls.GetAt(i));
			}
			else if (tcpPorts == _T("80"))
			{
				strLine.Format(_T("http://%s/*\r\n"), urls.GetAt(i));
			}
			else if (tcpPorts == _T("80,443"))
			{
				strLine.Format(_T("*://%s/*\r\n"), urls.GetAt(i));
			}
			else if (tcpPorts == _T("443,80"))
			{
				strLine.Format(_T("*://%s/*\r\n"), urls.GetAt(i));
			}
			else
				strLine.Format(_T("*://%s/*\r\n"), urls.GetAt(i));
			strRet += strLine;
		}
		_wsetlocale(LC_ALL, _T(""));
		return strRet;
	}
};
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>

class CO365EndpointJSONHelper
{
public:
	CO365EndpointJSONHelper() {}
	~CO365EndpointJSONHelper() { Clear(); }
	CString m_strJsonData;
	CString GetJSONData()
	{
		return m_strJsonData;
	}
	void Clear()
	{
		CO365EndpointData* pResData = NULL;
		for (int i = 0; i < m_ptrData.GetSize(); i++)
		{
			pResData = (CO365EndpointData*)m_ptrData.GetAt(i);
			if (pResData)
			{
				delete pResData;
				pResData = NULL;
			}
		}
		m_ptrData.RemoveAll();
		m_strJsonData.Empty();
	}
	CPtrArray m_ptrData;
	CString GetCSVData()
	{
		_wsetlocale(LC_ALL, _T("jpn"));
		CString strRet;
		CString strLine;
		CO365EndpointData* pResData = NULL;
		for (int i = 0; i < m_ptrData.GetSize(); i++)
		{
			//header
			if (i == 0)
			{
				strRet = _T("\"id\",\"serviceArea\",\"serviceAreaDisplayName\",\"category\",\"required\",\"tcpPorts\",\"urls\",\"notes\"\r\n");
			}
			pResData = (CO365EndpointData*)m_ptrData.GetAt(i);
			if (pResData)
			{
				strLine = pResData->GetCSVData();
				if(!strLine.IsEmpty())
					strRet += strLine;
			}
		}
		_wsetlocale(LC_ALL, _T(""));
		return strRet;
	}
	CString GetThinBridgeRules()
	{
		_wsetlocale(LC_ALL, _T("jpn"));
		CString strRet;
		CString strLine;
		CO365EndpointData* pResData = NULL;
		CMapStringToString strMapDupChk;
		for (int i = 0; i < m_ptrData.GetSize(); i++)
		{
			pResData = (CO365EndpointData*)m_ptrData.GetAt(i);
			if (pResData)
			{
				strLine = pResData->GetThinBridgeRules();
				strRet += strLine;
			}
		}

		CStringArray strA;
		SBUtil2::Split(&strA,strRet,_T("\r\n"));
		strRet.Empty();
		for(int i=0;i<strA.GetSize();i++)
		{
			strLine = strA.GetAt(i);
			if (strLine.IsEmpty())continue;

			//重複チェック
			if (strMapDupChk.Lookup(strLine, pResData->id))
			{
				//Lookup出来た場合は、重複している。
				CString err;
				err.Format(_T("Duplicate %d %s\n"), i + 1, strLine);
				::OutputDebugString(err);
			}
			//重複無し。
			else
			{
				strMapDupChk.SetAt(strLine, pResData->id);
				strRet += strLine;
				strRet += _T("\r\n");
			}
		}

		_wsetlocale(LC_ALL, _T(""));
		return strRet;
	}
	int GetCount()
	{
		return m_ptrData.GetCount();
	}
	BOOL IsFilterHit(LPCTSTR filter,LPCTSTR src)
	{
		CString strSrc;
		CString strFilter;
		strSrc=src;
		strFilter= filter;
		//フィルターが空はOK
		if(strFilter.IsEmpty())
			return TRUE;

		//ターゲットが空はNG
		if (strSrc.IsEmpty())
			return FALSE;

		strSrc.MakeUpper();
		strFilter.MakeUpper();

		BOOL bRet=FALSE;
		CStringArray strA;
		SBUtil2::Split(&strA, strFilter, _T("|"));
		UINT iSize = strA.GetSize();
		for(UINT i=0;i<iSize;i++)
		{
			CString strTemp;
			strTemp = strA.GetAt(i);
			if (strTemp == strSrc)
			{
				bRet=TRUE;
				break;
			}
		}
		return bRet;
	}

	CString ReadJSONData(LPCSTR lp, LPCTSTR filterServiceArea, LPCTSTR filterCategory, LPCTSTR filterRequired, LPCTSTR filterTcpPort)
	{
		_wsetlocale(LC_ALL, _T("jpn"));
		Clear();
		CString strRet;
		m_strJsonData=lp;

		rapidjson::Document jsonDoc;
		jsonDoc.Parse(lp);
		if (jsonDoc.HasParseError()) {
			strRet.Format(_T("Json Parse error:%d"), jsonDoc.GetParseError());
			return strRet;
		}
		rapidjson::SizeType iSize= jsonDoc.Size();
		// JSON 配列
		for (rapidjson::SizeType i = 0; i < iSize; i++)
		{
			CO365EndpointData ResDataTmp;
			ResDataTmp.Clear();
			if(jsonDoc[i].IsObject())
			{
				if (jsonDoc[i].HasMember("id"))
				{
					if(jsonDoc[i]["id"].IsInt())
					{
						int iID=0;
						iID = jsonDoc[i]["id"].GetInt();
						ResDataTmp.id.Format(_T("%d"),iID);
					}
				}
				if(jsonDoc[i].HasMember("serviceArea"))
				{
					if (jsonDoc[i]["serviceArea"].IsString())
					{
						CString strTemp;
						strTemp = jsonDoc[i]["serviceArea"].GetString();
						if(IsFilterHit(filterServiceArea, strTemp))
						{
							ResDataTmp.serviceArea = strTemp;
						}
						else
							continue;
					}
				}
				if (jsonDoc[i].HasMember("serviceAreaDisplayName"))
				{
					if (jsonDoc[i]["serviceAreaDisplayName"].IsString())
					{
						ResDataTmp.serviceAreaDisplayName = jsonDoc[i]["serviceAreaDisplayName"].GetString();
					}
				}
				if (jsonDoc[i].HasMember("urls"))
				{
					if (jsonDoc[i]["urls"].IsArray())
					{
						for (rapidjson::Value::ConstValueIterator itr = jsonDoc[i]["urls"].Begin(); itr != jsonDoc[i]["urls"].End(); ++itr)
						{
							if (itr->IsString())
							{
								CString strTemp;
								strTemp = itr->GetString();
								ResDataTmp.urls.Add(strTemp);
							}

						}
					}
				}
				if (jsonDoc[i].HasMember("ips"))
				{
					if (jsonDoc[i]["ips"].IsArray())
					{
						for (rapidjson::Value::ConstValueIterator itr = jsonDoc[i]["ips"].Begin(); itr != jsonDoc[i]["ips"].End(); ++itr)
						{
							if (itr->IsString())
							{
								CString strTemp;
								strTemp = itr->GetString();
								ResDataTmp.ips.Add(strTemp);
							}
						}
					}
				}
				if (jsonDoc[i].HasMember("tcpPorts"))
				{
					if (jsonDoc[i]["tcpPorts"].IsString())
					{
						CString strTemp;
						strTemp = jsonDoc[i]["tcpPorts"].GetString();
						if (IsFilterHit(filterTcpPort, strTemp))
						{
							ResDataTmp.tcpPorts = strTemp;
						}
						else
							continue;

					}
				}
				if (jsonDoc[i].HasMember("udpPorts"))
				{
					if (jsonDoc[i]["udpPorts"].IsString())
					{
						ResDataTmp.udpPorts = jsonDoc[i]["udpPorts"].GetString();
					}
				}
				if (jsonDoc[i].HasMember("expressRoute"))
				{
					if (jsonDoc[i]["expressRoute"].IsBool())
					{
						ResDataTmp.expressRoute = jsonDoc[i]["expressRoute"].GetBool()?_T("TRUE"):_T("FALSE");
					}
				}
				if (jsonDoc[i].HasMember("category"))
				{
					if (jsonDoc[i]["category"].IsString())
					{
						CString strTemp;
						strTemp = jsonDoc[i]["category"].GetString();
						if (IsFilterHit(filterCategory, strTemp))
						{
							ResDataTmp.category = strTemp;
						}
						else
							continue;
					}
				}
				if (jsonDoc[i].HasMember("required"))
				{
					if (jsonDoc[i]["required"].IsBool())
					{
						CString strTemp;
						strTemp= filterRequired;
						strTemp.TrimLeft();
						strTemp.TrimRight();
						if(strTemp.IsEmpty())
							ResDataTmp.required = jsonDoc[i]["required"].GetBool() ? _T("TRUE") : _T("FALSE");
						else
						{
							if (strTemp.CompareNoCase(_T("TRUE"))==0)
							{
								if(jsonDoc[i]["required"].GetBool())
									ResDataTmp.required = jsonDoc[i]["required"].GetBool() ? _T("TRUE") : _T("FALSE");
								else
									continue;
							}
							else if (strTemp.CompareNoCase(_T("FALSE"))==0)
							{
								if (!jsonDoc[i]["required"].GetBool())
									ResDataTmp.required = jsonDoc[i]["required"].GetBool() ? _T("TRUE") : _T("FALSE");
								else
									continue;
							}
						}
					}
				}
				if (jsonDoc[i].HasMember("notes"))
				{
					if (jsonDoc[i]["notes"].IsString())
					{
						CString strTemp;
						strTemp = jsonDoc[i]["notes"].GetString();
						if(!strTemp.IsEmpty())
							ResDataTmp.notes = strTemp;
					}
				}
				if(ResDataTmp.urls.GetSize()>0)
				{
					CO365EndpointData* pResData = NULL;
					pResData = new CO365EndpointData;
					pResData->Copy(ResDataTmp);
					m_ptrData.Add(pResData);
				}
				ResDataTmp.Clear();
			}
		}
		_wsetlocale(LC_ALL, _T(""));
		return strRet;
	}
};
class CByteBuffer
{
public:
	CByteBuffer()
	{
		m_pByte = NULL;
		m_nTotalSize = 0;
	}
	~CByteBuffer()
	{
		Cleanup();
	}
	BOOL IsEmpty()
	{
		return m_ptrArray.GetSize() == 0 ? TRUE : FALSE;
	}
	void Cleanup()
	{
		BYTE* pPtr = NULL;
		for (int i = 0; i < m_ptrArray.GetSize(); i++)
		{
			pPtr = NULL;
			pPtr = (BYTE*)m_ptrArray.GetAt(i);
			if (pPtr)
			{
				delete[] pPtr;
			}
		}
		m_ptrArray.RemoveAll();
		m_ptrArraySize.RemoveAll();
		if (m_pByte)
		{
			delete[]m_pByte;
			m_pByte = NULL;
		}
	}
	void Add(BYTE* lpData, UINT iSize)
	{
		if (iSize <= 0)return;
		if (!lpData)return;

		BYTE* pPtr = NULL;
		pPtr = new BYTE[iSize];
		memset(pPtr, 0x00, iSize);
		m_nTotalSize += iSize;
		memcpy(pPtr, lpData, iSize);
		m_ptrArray.Add(pPtr);
		m_ptrArraySize.Add(iSize);
	}
	BYTE* GetData(UINT* iSize)
	{
		if (m_nTotalSize <= 0)return NULL;

		if (m_pByte)
		{
			delete[] m_pByte;
			m_pByte = NULL;
		}
		m_pByte = new BYTE[m_nTotalSize];
		memset(m_pByte, 0x00, m_nTotalSize);
		BYTE* pPtr = NULL;
		UINT iIndex = 0;
		for (int i = 0; i < min(m_ptrArray.GetSize(), m_ptrArraySize.GetSize()); i++)
		{
			pPtr = NULL;
			pPtr = (BYTE*)m_ptrArray.GetAt(i);
			if (pPtr)
			{
				UINT iSize = 0;
				iSize = m_ptrArraySize.GetAt(i);
				memcpy_s(m_pByte + iIndex, m_nTotalSize, pPtr, iSize);
				iIndex += iSize;
			}
		}
		*iSize = m_nTotalSize;
		return m_pByte;
	}
	CHAR* GetStringData()
	{
		if (m_nTotalSize <= 0)return NULL;

		if (m_pByte)
		{
			delete[] m_pByte;
			m_pByte = NULL;
		}
		m_pByte = new BYTE[m_nTotalSize+2];
		memset(m_pByte, 0x00, m_nTotalSize+2);
		BYTE* pPtr = NULL;
		UINT iIndex = 0;
		for (int i = 0; i < min(m_ptrArray.GetSize(), m_ptrArraySize.GetSize()); i++)
		{
			pPtr = NULL;
			pPtr = (BYTE*)m_ptrArray.GetAt(i);
			if (pPtr)
			{
				UINT iSize = 0;
				iSize = m_ptrArraySize.GetAt(i);
				memcpy_s(m_pByte + iIndex, m_nTotalSize, pPtr, iSize);
				iIndex += iSize;
			}
		}
		return (CHAR*)m_pByte;

	}
	BYTE* m_pByte;
	CPtrArray m_ptrArray;
	CUIntArray m_ptrArraySize;
	UINT m_nTotalSize;
};
class O365EndpointHelper
{
public:
	O365EndpointHelper() {}
	~O365EndpointHelper() {}
	CString m_strHostURL;
	CString m_strLog;
	CO365EndpointJSONHelper m_ResData;
	void PutLog(LPCTSTR lpMsg)
	{
		_wsetlocale(LC_ALL, _T("jpn"));
		//::OutputDebugString(lpMsg);
		if (lpMsg == NULL)return;
		if (_tcslen(lpMsg) == 0)return;
		CString strWriteLine;
		CString strDebugLine;
		CTime time = CTime::GetCurrentTime();
		strWriteLine.Format(_T("%s\t%s\r\n"), time.Format(_T("%Y-%m-%d %H:%M:%S")),lpMsg);
		strDebugLine.Format(_T("TBo365URLSync\t%s"), strWriteLine);
		m_strLog+= strDebugLine;
		_wsetlocale(LC_ALL, _T(""));
	}
	LPCTSTR GetLog()
	{
		return m_strLog;
	}
	CString GetErrorMsg(int iRet)
	{
		CString strServerErrMsg;
		if (iRet == OK_SERVER)
			strServerErrMsg = _T("OK_SERVER");
		else if (iRet == ERR_SERVER_SETTINGS_NOT_FOUND)
			strServerErrMsg = _T("ERR_SERVER_SETTINGS_NOT_FOUND");
		else if (iRet == ERR_SERVER_SESSION)
			strServerErrMsg = _T("ERR_SERVER_SESSION サーバーへのセッションが確立できません。インターネット接続を確認してください。");
		else if (iRet == ERR_SERVER_SETTINGS_INVALID)
			strServerErrMsg = _T("ERR_SERVER_SETTINGS_INVALID");
		else if (iRet == ERR_SERVER_CONNECT)
			strServerErrMsg = _T("ERR_SERVER_CONNECT サーバーへの接続に失敗しました。インターネット接続を確認してください。");
		else if (iRet == ERR_SERVER_REQUEST)
			strServerErrMsg = _T("ERR_SERVER_REQUEST");
		else if (iRet == ERR_SERVER_SEND_REQUEST)
			strServerErrMsg = _T("ERR_SERVER_SEND_REQUEST サーバーへのリクエスト送信で問題が発生しています。URLを確認してください。");
		else if (iRet == ERR_SERVER_RECEIVE_RESPONSE)
			strServerErrMsg = _T("ERR_SERVER_RECEIVE_RESPONSE サーバーからのレスポンス取得で問題が発生しています。URLを確認してください。");
		else if (iRet == ERR_SERVER_QUERY_HEADERS)
			strServerErrMsg = _T("ERR_SERVER_QUERY_HEADERS");
		else if (iRet == ERR_SERVER_HTTP_STATUS)
			strServerErrMsg = _T("ERR_SERVER_HTTP_STATUS");
		else if (iRet == ERR_SERVER_RESPONSE_EMPTY)
			strServerErrMsg = _T("ERR_SERVER_RESPONSE_EMPTY");
		else if (iRet == ERR_SERVER_RESPONSE_INVALID)
			strServerErrMsg = _T("ERR_SERVER_RESPONSE_INVALID");
		else if (iRet == ERR_SERVER_RESPONSE_STOP)
			strServerErrMsg = _T("ERR_SERVER_RESPONSE_STOP");
		else if (iRet == ERR_SERVER_ETC)
			strServerErrMsg = _T("ERR_SERVER_ETC");
		else if (iRet == ERR_SERVER_429)
			strServerErrMsg = _T("ERR_SERVER_429 Too Many Requests 一定時間に大量のAPIリクエストを行った結果 サーバーに拒否されました。");
		else if (iRet == ERROR_WINHTTP_CANNOT_CONNECT)
			strServerErrMsg = _T("サーバーに接続できません。ERROR_WINHTTP_CANNOT_CONNECT");
		else if (iRet == ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED)
			strServerErrMsg = _T("サーバーに接続できません。ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED");
		else if (iRet == ERROR_WINHTTP_CONNECTION_ERROR)
			strServerErrMsg = _T("サーバーに接続できません。ERROR_WINHTTP_CONNECTION_ERROR");
		else if (iRet == ERROR_WINHTTP_INTERNAL_ERROR)
			strServerErrMsg = _T("サーバーに接続できません。ERROR_WINHTTP_INTERNAL_ERROR");
		else if (iRet == ERROR_WINHTTP_INVALID_URL)
			strServerErrMsg = _T("URLが正しくありません。ERROR_WINHTTP_INVALID_URL");
		else if (iRet == ERROR_WINHTTP_LOGIN_FAILURE)
			strServerErrMsg = _T("サーバーに接続できません。ERROR_WINHTTP_LOGIN_FAILURE");
		else if (iRet == ERROR_WINHTTP_NAME_NOT_RESOLVED)
			strServerErrMsg = _T("サーバーに接続できません。（名前解決エラー）ERROR_WINHTTP_NAME_NOT_RESOLVED");
		else if (iRet == ERROR_WINHTTP_OPERATION_CANCELLED)
			strServerErrMsg = _T("サーバーに接続できません。ERROR_WINHTTP_OPERATION_CANCELLED");
		else if (iRet == ERROR_WINHTTP_TIMEOUT)
			strServerErrMsg = _T("リクエストタイムアウトが発生しました。ERROR_WINHTTP_TIMEOUT");
		else
			strServerErrMsg.Format(_T("UNKNOWN_CODE %d"), iRet);
		return strServerErrMsg;
	}
	void SetBaseURL(LPCTSTR lpStr)
	{
		m_strHostURL = lpStr;
	}
	int ExecMain(LPCTSTR lpStr, LPCTSTR filterServiceArea, LPCTSTR filterCategory, LPCTSTR filterRequired, LPCTSTR filterTcpPort)
	{
		_wsetlocale(LC_ALL, _T("jpn"));
		m_strLog.Empty();
		CString logmsg;
		logmsg.Format(_T(">>>ExecMain"));
		PutLog(logmsg);
		SetBaseURL(lpStr);
		int iRet = 0;
		while (1)
		{
			iRet = GetEndpointData(m_strHostURL, filterServiceArea,  filterCategory,  filterRequired,  filterTcpPort);
			if (iRet != OK_SERVER)
				break;
			break;
		}
//		CString strTemp;
//		strTemp = m_ResData.GetCSVData();
//		logmsg.Format(_T("\r\nCSV>>>>>>>>>>\r\n%s<<<<<<<<<<CSV"), strTemp);
//		PutLog(logmsg);

//		strTemp = m_ResData.GetThinBridgeRules();
//		logmsg.Format(_T("\r\nThinBridgeRule>>>>>>>>>>\r\n%s<<<<<<<<<<ThinBridgeRule"), strTemp);
//		PutLog(logmsg);

		_wsetlocale(LC_ALL, _T(""));
		logmsg.Format(_T(">>>ExecMain"));
		PutLog(logmsg);
		return iRet;
	}
	int GetEndpointData(LPCTSTR lpURL,LPCTSTR filterServiceArea, LPCTSTR filterCategory, LPCTSTR filterRequired, LPCTSTR filterTcpPort)
	{
		_wsetlocale(LC_ALL, _T("jpn"));
		CString logmsg;
		CString strURL(lpURL);
		logmsg.Format(_T(">>>GetEndpointData %s"), strURL);
		PutLog(logmsg);
		logmsg.Format(_T("Filter ServiceArea=%s,Category=%s,Required=%s,TcpPort=%s"), filterServiceArea, filterCategory, filterRequired, filterTcpPort);
		PutLog(logmsg);

		DWORD iRet = 0;
		WinHttpHelper WinHttpHelper;

		//1回で抜ける gotoの変わり
		while (1)
		{
			if (!WinHttpHelper.InitUrl(strURL, _T("GET")))
			{
				iRet = ERR_SERVER_SETTINGS_INVALID;
				break;
			}
			if (!WinHttpHelper.InitSession())
			{
				iRet = ERR_SERVER_SESSION;
				break;
			}
			if (!WinHttpHelper.InitConnect())
			{
				iRet = ERR_SERVER_CONNECT;
				break;
			}
			if (!WinHttpHelper.InitRequest())
			{
				iRet = ERR_SERVER_REQUEST;
				break;
			}
			CString strHeaders;
			SetLastError(0);

			//GET
			if (!WinHttpSendRequest(WinHttpHelper.m_hRequest,
				strHeaders,
				strHeaders.GetLength(),
				WINHTTP_NO_REQUEST_DATA,
				0,
				0,
				0))
			{
				iRet = ERR_SERVER_SEND_REQUEST;
				DWORD iErrNo = 0;
				iErrNo = GetLastError();
				if (iErrNo >= WINHTTP_ERROR_BASE)
					iRet = iErrNo;
				break;
			}
			// レスポンスの到着の待機
			if (!WinHttpReceiveResponse(WinHttpHelper.m_hRequest, NULL))
			{
				iRet = ERR_SERVER_RECEIVE_RESPONSE;
				DWORD iErrNo = 0;
				iErrNo = GetLastError();
				if (iErrNo >= WINHTTP_ERROR_BASE)
					iRet = iErrNo;
				break;
			}
			// ステータスコードの取得
			DWORD dwStatusCode = 0;
			DWORD dwStatusCodeSize = sizeof(DWORD);
			if (!WinHttpQueryHeaders(WinHttpHelper.m_hRequest,
				WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,	// ステータスコードをDWORD型で取得する。
				WINHTTP_HEADER_NAME_BY_INDEX,	// ヘッダー名のポインタ
				&dwStatusCode,				// バッファー
				&dwStatusCodeSize,			// バッファーサイズ
				WINHTTP_NO_HEADER_INDEX))		// 最初に発生したヘッダーのみ取り出す
			{
				iRet = ERR_SERVER_QUERY_HEADERS;
				break;
			}

			logmsg.Format(_T("GetEndpointData dwStatusCode:%d"), dwStatusCode);
			PutLog(logmsg);
			if (HTTP_STATUS_OK != dwStatusCode)
			{
				if(dwStatusCode==429)
					iRet = ERR_SERVER_429;
				else
					iRet = ERR_SERVER_HTTP_STATUS;
				break;
			}
			// レスポンスデータ読み込み
			CByteBuffer ResultData;
			BYTE* pszBuffer = NULL;
			BOOL bFirstGetData = TRUE;
			while (1)
			{
				// レスポンスデータ問い合わせ
				DWORD dwSize = 0;
				if (!WinHttpQueryDataAvailable(WinHttpHelper.m_hRequest, &dwSize))
				{
					break;
				}
				if (0 == dwSize)
				{
					break;
				}

				// レスポンスデータ読み込み
				DWORD dwLength = dwSize + 1;
				pszBuffer = NULL;
				pszBuffer = (BYTE*)malloc(dwLength * sizeof(BYTE));
				ZeroMemory(pszBuffer, dwLength * sizeof(BYTE));

				if (!WinHttpReadData(WinHttpHelper.m_hRequest,
					pszBuffer,		// バッファー
					dwSize,			// 読み込むバイト数
					NULL))
				{
					if (pszBuffer)
					{
						free(pszBuffer);
						pszBuffer = NULL;
					}
					break;
				}
				pszBuffer[dwSize] = '\0';
				//UTF-8 BOM chk 0xEF 0xBB 0xBF
				if (bFirstGetData)
				{
					bFirstGetData = FALSE;
					//BOM + \0 = 4
					if (dwSize >= 4)
					{
						BYTE byBOM[3] = { 0xEF,0xBB,0xBF };
						if (memcmp(pszBuffer, &byBOM, 3) == 0)
						{
							//半角スペースで埋めてしまう。あとでTrimしている
							memset(pszBuffer, 0x20, 3);
						}
					}
				}
				ResultData.Add(pszBuffer, dwSize);
				if (pszBuffer)
				{
					free(pszBuffer);
					pszBuffer = NULL;
				}
			}
			if (!ResultData.IsEmpty())
			{
				CHAR* lp = NULL;
				lp = ResultData.GetStringData();
				CStringA strA(lp);
				CString strL;
				strL=strA;
//				logmsg.Format(_T("\r\nJSON>>>>>>>>>>\r\n%s\r\n<<<<<<<<<<JSON"), strL);
//				PutLog(logmsg);
				strL = m_ResData.ReadJSONData(lp, filterServiceArea, filterCategory, filterRequired, filterTcpPort);
				PutLog(strL);
				iRet = OK_SERVER;
			}
			break;
		}
		CString strServerErrMsg;
		strServerErrMsg = GetErrorMsg(iRet);
		logmsg.Format(_T("<<<GetEndpointData %s"), strServerErrMsg);
		PutLog(logmsg);
		_wsetlocale(LC_ALL, _T(""));
		return iRet;
	}
};


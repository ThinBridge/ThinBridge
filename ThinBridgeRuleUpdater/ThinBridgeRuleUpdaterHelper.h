#pragma once
#include "stdafx.h"
#include "ThinBridgeRuleUpdater.h"
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
#define ERR_SERVER_401				14
#define ERR_SERVER_404				15
#define ERR_SERVER_FORMAT_INVALID	16

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

	CString GetComputerNameData()
	{
		TCHAR szBuffer[512] = { 0 };
		ULONG iSize = 512;
		::GetComputerName(szBuffer, &iSize);
		CString strRet;
		strRet = szBuffer;
		strRet.TrimLeft();
		strRet.TrimRight();
		return strRet;
	}

	// �Z�b�V�����I�u�W�F�N�g
	HINTERNET m_hConnect;
	HINTERNET m_hRequest;
	HINTERNET m_hSession;
	//URL
	URL_COMPONENTS m_URLComponents;
	TCHAR szHostName[URLBUFFER_SIZE];
	TCHAR szUrlPath[URLBUFFER_SIZE];
	CString m_strVerb;
	CString m_strUserAgent;

	BOOL InitUrl(LPCTSTR lpURL, LPCTSTR lpVerb /*GET | POST*/)
	{
		Cleanup();
		CString strURL(lpURL);
		// URL���
		ZeroMemory(&m_URLComponents, sizeof(URL_COMPONENTS));
		m_URLComponents.dwStructSize = sizeof(URL_COMPONENTS);

		m_URLComponents.lpszHostName = szHostName;
		m_URLComponents.lpszUrlPath = szUrlPath;
		m_URLComponents.dwHostNameLength = URLBUFFER_SIZE;
		m_URLComponents.dwUrlPathLength = URLBUFFER_SIZE;

		if (!WinHttpCrackUrl(strURL, 0, 0, &m_URLComponents))
		{
			// URL�̉�͂Ɏ��s
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
		m_strUserAgent.Format(_T("ThinBridge PC:%s ID:%s"), GetComputerNameData(), theApp.GetUserNameData());
		if (ieProxyConfig.lpszProxy)
		{
			// InternetExplorer�̃v���L�V�ݒ�ŃZ�b�V�����I�u�W�F�N�g�쐬
			m_hSession = WinHttpOpen(m_strUserAgent,	// UserAgent��
				WINHTTP_ACCESS_TYPE_NAMED_PROXY,	// �v���L�V�ݒ�
				ieProxyConfig.lpszProxy,	// �v���L�V��
				ieProxyConfig.lpszProxyBypass ? ieProxyConfig.lpszProxyBypass : WINHTTP_NO_PROXY_BYPASS,	// �v���L�V�o�C�p�X
				WINHTTP_FLAG_ASYNC);		// �U�镑���I�v�V�����B����WINHTTP_FLAG_ASYNC�̂ݎw��\�B
		}
		else
		{
			m_hSession = WinHttpOpen(m_strUserAgent,	// UserAgent�� 
				WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,	// �v���L�V�ݒ�
				WINHTTP_NO_PROXY_NAME,	// �v���L�V��
				WINHTTP_NO_PROXY_BYPASS,	// �v���L�V�o�C�p�X
				WINHTTP_FLAG_ASYNC);		// �U�镑���I�v�V�����B����WINHTTP_FLAG_ASYNC�̂ݎw��\�B
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
		// �ϐ�
		CString strServer;
		INTERNET_PORT nPort = 0;
		strServer = m_URLComponents.lpszHostName;
		nPort = m_URLComponents.nPort;

		// HTTP�ڑ�
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
		// HTTP��HTTPS������ȊO��
		DWORD dwOpenRequestFlag = (INTERNET_SCHEME_HTTPS == m_URLComponents.nScheme) ? WINHTTP_FLAG_SECURE : 0;
		dwOpenRequestFlag = dwOpenRequestFlag | WINHTTP_FLAG_REFRESH;
		strObjectName = m_URLComponents.lpszUrlPath;
		// HTTP�ڑ����J��
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

		//2020-12-21 ����ς莩�ȏؖ�����NG�ɂ���B
		////���ȏؖ����Ή� 2020-12-07 �C���g���ł������p���Ȃ��V�X�e���Ȃ̂Ŏ��ȏؖ������f�t�H���g�ŋ�����B
		//if (INTERNET_SCHEME_HTTPS == m_URLComponents.nScheme)
		//{
		//	DWORD dwSSLFlags =
		//		SECURITY_FLAG_IGNORE_UNKNOWN_CA |
		//		SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE |
		//		SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
		//		SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;

		//	if (!WinHttpSetOption(
		//		m_hRequest,
		//		WINHTTP_OPTION_SECURITY_FLAGS,
		//		&dwSSLFlags,
		//		sizeof(dwSSLFlags)))
		//	{
		//		return FALSE;
		//	}
		//}

		return TRUE;
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

class ThinBridgeRuleUpdaterHelper
{
public:
	ThinBridgeRuleUpdaterHelper() {}
	~ThinBridgeRuleUpdaterHelper() {}
	CString m_strHostURL;
	CString m_strLog;
	CString m_strGetData;

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
		strDebugLine.Format(_T("ThinBridgeRuleUpdater\t%s"), strWriteLine);
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
			strServerErrMsg = _T("ERR_SERVER_SESSION �T�[�o�[�ւ̃Z�b�V�������m���ł��܂���B�C���^�[�l�b�g�ڑ����m�F���Ă��������B");
		else if (iRet == ERR_SERVER_SETTINGS_INVALID)
			strServerErrMsg = _T("ERR_SERVER_SETTINGS_INVALID");
		else if (iRet == ERR_SERVER_CONNECT)
			strServerErrMsg = _T("ERR_SERVER_CONNECT �T�[�o�[�ւ̐ڑ��Ɏ��s���܂����B�C���^�[�l�b�g�ڑ����m�F���Ă��������B");
		else if (iRet == ERR_SERVER_REQUEST)
			strServerErrMsg = _T("ERR_SERVER_REQUEST");
		else if (iRet == ERR_SERVER_SEND_REQUEST)
			strServerErrMsg = _T("ERR_SERVER_SEND_REQUEST �T�[�o�[�ւ̃��N�G�X�g���M�Ŗ�肪�������Ă��܂��BURL���m�F���Ă��������B");
		else if (iRet == ERR_SERVER_RECEIVE_RESPONSE)
			strServerErrMsg = _T("ERR_SERVER_RECEIVE_RESPONSE �T�[�o�[����̃��X�|���X�擾�Ŗ�肪�������Ă��܂��BURL���m�F���Ă��������B");
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
			strServerErrMsg = _T("ERR_SERVER_429 Too Many Requests ��莞�Ԃɑ�ʂ�API���N�G�X�g���s�������� �T�[�o�[�ɋ��ۂ���܂����B");
		else if (iRet == ERR_SERVER_401)
			strServerErrMsg = _T("ERR_SERVER_401 �A�N�Z�X�����ۂ���܂����B");
		else if(iRet == ERR_SERVER_404)
			strServerErrMsg = _T("ERR_SERVER_404 �t�@�C����������܂���B");
		else if (iRet == ERROR_WINHTTP_CANNOT_CONNECT)
			strServerErrMsg = _T("�T�[�o�[�ɐڑ��ł��܂���BERROR_WINHTTP_CANNOT_CONNECT");
		else if (iRet == ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED)
			strServerErrMsg = _T("�T�[�o�[�ɐڑ��ł��܂���BERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED");
		else if (iRet == ERROR_WINHTTP_CONNECTION_ERROR)
			strServerErrMsg = _T("�T�[�o�[�ɐڑ��ł��܂���BERROR_WINHTTP_CONNECTION_ERROR");
		else if (iRet == ERROR_WINHTTP_INTERNAL_ERROR)
			strServerErrMsg = _T("�T�[�o�[�ɐڑ��ł��܂���BERROR_WINHTTP_INTERNAL_ERROR");
		else if (iRet == ERROR_WINHTTP_INVALID_URL)
			strServerErrMsg = _T("URL������������܂���BERROR_WINHTTP_INVALID_URL");
		else if (iRet == ERROR_WINHTTP_LOGIN_FAILURE)
			strServerErrMsg = _T("�T�[�o�[�ɐڑ��ł��܂���BERROR_WINHTTP_LOGIN_FAILURE");
		else if (iRet == ERROR_WINHTTP_NAME_NOT_RESOLVED)
			strServerErrMsg = _T("�T�[�o�[�ɐڑ��ł��܂���B�i���O�����G���[�jERROR_WINHTTP_NAME_NOT_RESOLVED");
		else if (iRet == ERROR_WINHTTP_OPERATION_CANCELLED)
			strServerErrMsg = _T("�T�[�o�[�ɐڑ��ł��܂���BERROR_WINHTTP_OPERATION_CANCELLED");
		else if (iRet == ERROR_WINHTTP_TIMEOUT)
			strServerErrMsg = _T("���N�G�X�g�^�C���A�E�g���������܂����BERROR_WINHTTP_TIMEOUT");
		else if (iRet == ERR_SERVER_FORMAT_INVALID)
			strServerErrMsg = _T("�T�[�o�[����擾����ThinBridge�ݒ�f�[�^���s���ȏ�Ԃł��BERR_SERVER_FORMAT_INVALID");

		else
			strServerErrMsg.Format(_T("UNKNOWN_CODE %d"), iRet);
		return strServerErrMsg;
	}
	void SetBaseURL(LPCTSTR lpStr)
	{
		m_strHostURL = lpStr;
	}
	int ExecMain(LPCTSTR lpStr)
	{
		_wsetlocale(LC_ALL, _T("jpn"));
		m_strLog.Empty();
		CString logmsg;
		logmsg.Format(_T(">>>ExecMain"));
		PutLog(logmsg);
		SetBaseURL(lpStr);
		int iRet = 0;
		if(SBUtil::IsURL_HTTP(m_strHostURL))
			iRet = GetThinBridgeRedirectRule(m_strHostURL);
		else
			iRet = GetThinBridgeRedirectRuleCIFS(m_strHostURL);
		CString strTemp;

		//�t�@�C���̓��e�𐸍�����B
		if (iRet == OK_SERVER)
		{
			CStringArray strArrayBHOData;
			SBUtil2::Split(&strArrayBHOData, m_strGetData, _T("\r\n"));
			//�T�[�o�[����擾�t�@�C���̓��e�`�F�b�N
			int iValidCnt = 0;
			for (int j = 0; j < strArrayBHOData.GetSize(); j++)
			{
				strTemp = strArrayBHOData.GetAt(j);
				if (!strTemp.IsEmpty())
				{
					if (strTemp == _T("[GLOBAL]"))
						iValidCnt++;
					else if (strTemp == _T("[RDP]"))
						iValidCnt++;
					else if (strTemp == _T("[VMWARE]"))
						iValidCnt++;
					else if (strTemp == _T("[CITRIX]"))
						iValidCnt++;
					else if (strTemp == _T("[Firefox]"))
						iValidCnt++;
					else if (strTemp == _T("[Chrome]"))
						iValidCnt++;
					else if (strTemp == _T("[Edge]"))
						iValidCnt++;
					else if (strTemp == _T("[IE]"))
						iValidCnt++;
					else if (strTemp == _T("[Default]"))
						iValidCnt++;
					else if (strTemp == _T("[CUSTOM01]"))
						iValidCnt++;
					else if (strTemp == _T("[CUSTOM02]"))
						iValidCnt++;
					else if (strTemp == _T("[CUSTOM03]"))
						iValidCnt++;
					else if (strTemp == _T("[CUSTOM04]"))
						iValidCnt++;
					else if (strTemp == _T("[CUSTOM05]"))
						iValidCnt++;
					else if (strTemp == _T("[CUSTOM18]"))
						iValidCnt++;
					else if (strTemp == _T("[CUSTOM19]"))
						iValidCnt++;
					else if (strTemp == _T("[CUSTOM20]"))
						iValidCnt++;
				}
			}
			if (iValidCnt < 4)
			{
				iRet = ERR_SERVER_FORMAT_INVALID;
			}
		}

		logmsg.Format(_T(">>>ExecMain"));
		PutLog(logmsg);
		_wsetlocale(LC_ALL, _T(""));
		return iRet;
	}

	int GetThinBridgeRedirectRule(LPCTSTR lpURL)
	{
		_wsetlocale(LC_ALL, _T("jpn"));
		CString logmsg;
		CString strURL(lpURL);
		logmsg.Format(_T(">>>GetThinBridgeRedirectRule %s"), strURL);
		PutLog(logmsg);

		DWORD iRet = 0;
		WinHttpHelper WinHttpHelper;

		//1��Ŕ����� goto�̕ς��
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
			logmsg.Format(_T(">>>UserAgent %s"), WinHttpHelper.m_strUserAgent);
			PutLog(logmsg);
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
				strHeaders.GetString(),
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
			// ���X�|���X�̓����̑ҋ@
			if (!WinHttpReceiveResponse(WinHttpHelper.m_hRequest, NULL))
			{
				iRet = ERR_SERVER_RECEIVE_RESPONSE;
				DWORD iErrNo = 0;
				iErrNo = GetLastError();
				if (iErrNo >= WINHTTP_ERROR_BASE)
					iRet = iErrNo;
				break;
			}
			// �X�e�[�^�X�R�[�h�̎擾
			DWORD dwStatusCode = 0;
			DWORD dwStatusCodeSize = sizeof(DWORD);
			if (!WinHttpQueryHeaders(WinHttpHelper.m_hRequest,
				WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,	// �X�e�[�^�X�R�[�h��DWORD�^�Ŏ擾����B
				WINHTTP_HEADER_NAME_BY_INDEX,	// �w�b�_�[���̃|�C���^
				&dwStatusCode,				// �o�b�t�@�[
				&dwStatusCodeSize,			// �o�b�t�@�[�T�C�Y
				WINHTTP_NO_HEADER_INDEX))		// �ŏ��ɔ��������w�b�_�[�̂ݎ��o��
			{
				iRet = ERR_SERVER_QUERY_HEADERS;
				break;
			}

			logmsg.Format(_T("GetThinBridgeRedirectRule dwStatusCode:%d"), dwStatusCode);
			PutLog(logmsg);
			if (HTTP_STATUS_OK != dwStatusCode)
			{
				if(dwStatusCode==429)
					iRet = ERR_SERVER_429;
				else if (dwStatusCode == 401)
					iRet = ERR_SERVER_401;
				else if (dwStatusCode == 404)
					iRet = ERR_SERVER_404;
				else
					iRet = ERR_SERVER_HTTP_STATUS;
				break;
			}
			// ���X�|���X�f�[�^�ǂݍ���
			CByteBuffer ResultData;
			BYTE* pszBuffer = NULL;
			BOOL bFirstGetData = TRUE;
			while (1)
			{
				// ���X�|���X�f�[�^�₢���킹
				DWORD dwSize = 0;
				if (!WinHttpQueryDataAvailable(WinHttpHelper.m_hRequest, &dwSize))
				{
					break;
				}
				if (0 == dwSize)
				{
					break;
				}

				// ���X�|���X�f�[�^�ǂݍ���
				DWORD dwLength = dwSize + 1;
				pszBuffer = NULL;
				pszBuffer = (BYTE*)malloc(dwLength * sizeof(BYTE));
				ZeroMemory(pszBuffer, dwLength * sizeof(BYTE));

				if (!WinHttpReadData(WinHttpHelper.m_hRequest,
					pszBuffer,		// �o�b�t�@�[
					dwSize,			// �ǂݍ��ރo�C�g��
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
							//���p�X�y�[�X�Ŗ��߂Ă��܂��B���Ƃ�Trim���Ă���
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
				m_strGetData=strA;
				logmsg.Format(_T("\r\n>>>>>>>>>>\r\n%s\r\n<<<<<<<<<<"), m_strGetData);
				PutLog(logmsg);
				iRet = OK_SERVER;
			}
			break;
		}
		CString strServerErrMsg;
		strServerErrMsg = GetErrorMsg(iRet);
		logmsg.Format(_T("<<<GetThinBridgeRedirectRule %s"), strServerErrMsg);
		PutLog(logmsg);
		_wsetlocale(LC_ALL, _T(""));
		return iRet;
	}
	int GetThinBridgeRedirectRuleCIFS(LPCTSTR lpURL)
	{
		_wsetlocale(LC_ALL, _T("jpn"));
		CString logmsg;
		CString strURL(lpURL);
		logmsg.Format(_T(">>>GetThinBridgeRedirectRuleCIFS %s"), strURL);
		PutLog(logmsg);

		DWORD iRet = 0;
		//1��Ŕ����� goto�̕ς��
		while (1)
		{
			CString strFromPath;
			strFromPath = lpURL;
			if (strFromPath.Find(_T("..\\")) >= 0
				|| strFromPath.Find(_T("../")) >= 0
				|| strFromPath.Find(_T(".\\")) >= 0
				|| strFromPath.Find(_T("./")) >= 0
				|| strFromPath.Find(_T("/")) >= 0
				|| strFromPath.Find(_T("\\.\\")) >= 0
				|| strFromPath.Find(_T("/./")) >= 0
				|| strFromPath.Find(_T("\\..\\")) >= 0
				|| strFromPath.Find(_T("/../")) >= 0
				)
			{
				iRet = ERR_SERVER_CONNECT;
				break;
			}
			if (strFromPath.Find(_T("%")) >= 0)
			{
				TCHAR szExpPath[MAX_PATH * 2] = { 0 };
				ExpandEnvironmentStrings(strFromPath, szExpPath, MAX_PATH * 2);
				strFromPath = szExpPath;
			}

			if (strFromPath.Find(_T("\\\\?\\")) != 0)
			{
				CString strSecurePathTemp(strFromPath);
				if (::PathIsUNC(strSecurePathTemp))
				{
					strFromPath = _T("\\\\?\\UNC\\");
					strSecurePathTemp.TrimLeft('\\');
					strFromPath += strSecurePathTemp;
				}
				else
				{
					strFromPath = _T("\\\\?\\");
					strFromPath += strSecurePathTemp;
				}
			}
			CFile in;
			CByteBuffer ResultData;
			BYTE* pszBuffer = NULL;
			if(in.Open(strFromPath, CFile::modeRead| CFile::shareDenyNone))
			{
				UINT uiFileLen = (UINT)in.GetLength();
				DWORD dwLength = uiFileLen + 1;
				pszBuffer = NULL;
				pszBuffer = (BYTE*)malloc(dwLength * sizeof(BYTE));
				ZeroMemory(pszBuffer, dwLength * sizeof(BYTE));
				in.Read(pszBuffer, uiFileLen);
				pszBuffer[uiFileLen] = '\0';
				ResultData.Add(pszBuffer, uiFileLen);
				if (pszBuffer)
				{
					free(pszBuffer);
					pszBuffer = NULL;
				}
				in.Close();
			}
			else
			{
				iRet = ERR_SERVER_404;
				break;

			}
			if (!ResultData.IsEmpty())
			{
				CHAR* lp = NULL;
				lp = ResultData.GetStringData();
				CStringA strA(lp);
				m_strGetData = strA;
				logmsg.Format(_T("\r\n>>>>>>>>>>\r\n%s\r\n<<<<<<<<<<"), m_strGetData);
				PutLog(logmsg);
				iRet = OK_SERVER;
			}
			break;
		}
		CString strServerErrMsg;
		strServerErrMsg = GetErrorMsg(iRet);
		logmsg.Format(_T("<<<GetThinBridgeRedirectRuleCIFS %s"), strServerErrMsg);
		PutLog(logmsg);
		_wsetlocale(LC_ALL, _T(""));
		return iRet;
	}
};


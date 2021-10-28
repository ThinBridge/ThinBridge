
// TBo365URLSync.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
//

#include "stdafx.h"
#include "TBo365URLSync.h"
#include "TBo365URLSyncDlg.h"
#include "O365EndpointHelper.h"
#include "locale.h"
#include "psapi.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTBo365URLSyncApp

BEGIN_MESSAGE_MAP(CTBo365URLSyncApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTBo365URLSyncApp �R���X�g���N�V����

CTBo365URLSyncApp::CTBo365URLSyncApp()
{
	m_iWriteThinBridgeBHO_ResultCode=0;
}


// �B��� CTBo365URLSyncApp �I�u�W�F�N�g�ł��B

CTBo365URLSyncApp theApp;


// CTBo365URLSyncApp ������
#include "wtsapi32.h"
#pragma comment(lib, "wtsapi32.lib")
#include <userenv.h>
#pragma comment (lib, "userenv.lib")
void CTBo365URLSyncApp::SetActiveUserNameSID()
{
	m_strCurrentSID = GetCurrentProcessStringSid();
	m_strSID = m_strCurrentSID;
	GetUserNameData();
	PWTS_SESSION_INFOW pStSesInf = NULL;
	DWORD dwCount = 0;
	DWORD dwSesId = 0;
	DWORD dwSize = 0;
	LPTSTR lpNameTmp = NULL;
	// ���O�I�����[�U�̃Z�b�V�������X�g���擾����
	if (::WTSEnumerateSessionsW(NULL,
		0,                    // �\��i�K��0���w��j
		1,                    // �񋓗v���̃o�[�W����(�K��1���w��)
		&pStSesInf,           // �\���̂̔z��ւ̃|�C���^������
		&dwCount) == FALSE) // �\���̂̐��i�[�p(�Z�b�V�������X�g��)
	{
		// �擾���s
		return;
	}
	// �Z�b�V�������X�g�����J��Ԃ�
	for (DWORD dwLoop = 0; dwLoop < dwCount; dwLoop++)
	{
		// �Z�b�V�������X�g���珇���Z�b�V���������擾����
		if (::WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE,      // �^�[�~�i���T�[�o�n���h��
			pStSesInf[dwLoop].SessionId,  // �Z�b�V�����̎��ʎq
			WTSUserName,                    // �Z�b�V�����Ɋ֘A�t����ꂽ���[�U�[�����w��
			&lpNameTmp,                     // ���[�U���|�C���^�i�[�p
			&dwSize) == TRUE)             // ���̃f�[�^�T�C�Y
		{
			CString strUserName;
			strUserName = lpNameTmp;
			// ���݃A�N�e�B�u�ȃZ�b�V������ID���擾
			dwSesId = ::WTSGetActiveConsoleSessionId();
			// �Z�b�V����ID���r���A�N�e�B�u�ȃZ�b�V����ID�Ɠ���Ȃ烆�[�U����\������
			if (pStSesInf[dwLoop].SessionId == dwSesId)
			{
				if (!strUserName.IsEmpty())
				{
					LPTSTR lpDomainTmp = NULL;
					if (::WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE,      // �^�[�~�i���T�[�o�n���h��
						pStSesInf[dwLoop].SessionId,  // �Z�b�V�����̎��ʎq
						WTSDomainName,                    // �Z�b�V�����Ɋ֘A�t����ꂽ���[�U�[�����w��
						&lpDomainTmp,                     // ���[�U���|�C���^�i�[�p
						&dwSize) == TRUE)             // ���̃f�[�^�T�C�Y
					{
						CString strDomainName;
						strDomainName = lpDomainTmp;
						if (lpDomainTmp != NULL)
						{
							// ���
							WTSFreeMemory(lpDomainTmp);
							lpDomainTmp = NULL;
						}
						HANDLE hToken = { 0 };
						WTSQueryUserToken(dwSesId, &hToken);
						if (hToken)
						{
							m_strSID = GetTokenStringSid(hToken);
							WriteThinBridgeFlg(_T("O365_LOGON_SID"), m_strSID);
							WriteThinBridgeFlg(_T("O365_CURRENT_SID"), m_strCurrentSID);
							WriteThinBridgeFlg(_T("O365_EXEC_USER"), GetUserNameData());
							CloseHandle(hToken);
							//LPVOID              lpEnvironment={0};
							//STARTUPINFO         startupInfo={0};
							//PROCESS_INFORMATION processInformation={0};
							//CreateEnvironmentBlock(&lpEnvironment, hToken, TRUE);

							//ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
							//startupInfo.cb = sizeof(STARTUPINFO);
							//const WCHAR szD[]= _T("winsta0\\default");
							//startupInfo.lpDesktop = (LPWSTR)szD;
							//CreateProcessAsUser(hToken, L"C:\\Windows\\notepad.exe", NULL, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT, lpEnvironment, NULL, &startupInfo, &processInformation);
							//DestroyEnvironmentBlock(lpEnvironment);
							//CloseHandle(hToken);
							//CloseHandle(processInformation.hThread);
							//CloseHandle(processInformation.hProcess);
						}
						m_strDomainUserName = strDomainName + _T("/") + strUserName;
						WriteThinBridgeFlg(_T("O365_LOGON_USER"), m_strDomainUserName);
					}
				}
			}
		}
		else
		{
			// �擾���s
			return;
		}
	}
	if (lpNameTmp != NULL)
	{
		// ���
		WTSFreeMemory(lpNameTmp);
		lpNameTmp = NULL;
	}
	if (pStSesInf != NULL)
	{
		// ���
		WTSFreeMemory(pStSesInf);
		pStSesInf = NULL;
	}
	return;
}


BOOL CTBo365URLSyncApp::InitInstance()
{
	_wsetlocale(LC_ALL, _T("jpn"));

	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	::OleInitialize(NULL);
	if (!AfxOleInit())
	{
		return FALSE;
	}
	AfxOleGetMessageFilter()->EnableNotRespondingDialog(FALSE);
	AfxOleGetMessageFilter()->EnableBusyDialog(FALSE);

	INITCOMMONCONTROLSEX InitCtrls={0};
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	AfxEnableControlContainer();

	TCHAR szPath[MAX_PATH+1]={0};
	TCHAR szDrive[_MAX_DRIVE]={0};
	TCHAR szDir[_MAX_DIR]={0};
	TCHAR szFileName[_MAX_FNAME]={0};
	TCHAR szExt[_MAX_EXT]={0};
	GetModuleFileName(AfxGetInstanceHandle(),szPath,MAX_PATH);
	_tsplitpath_s(szPath, szDrive,_MAX_DRIVE, szDir,_MAX_DIR, szFileName,_MAX_FNAME, szExt,_MAX_EXT); 

	m_strExeFullPath=szPath;
	m_strExeNameNoExt = szFileName;

	//�ݒ�f�[�^�̃��[�h
	m_strSetting_FileFullPath = szDrive;
	m_strSetting_FileFullPath += szDir;
	m_strSetting_FileFullPath += _T("TBo365URLSync");
	m_strSetting_FileFullPath += _T(".conf");
	this->ReadSetting();

	m_strThinBridgeBHOFileFullPath = szDrive;
	m_strThinBridgeBHOFileFullPath += szDir;
	m_strThinBridgeBHOFileFullPath += _T("ThinBridgeBHO.ini");

	m_strExecLogFileFullPath = szDrive;
	m_strExecLogFileFullPath += szDir;
	m_strExecLogFileFullPath += _T("TBo365Log\\");
	::CreateDirectory(m_strExecLogFileFullPath, NULL);
	m_strExecLogFileDir= m_strExecLogFileFullPath;
	CTime time = CTime::GetCurrentTime();
	CString strLogFileNow;
	strLogFileNow.Format(_T("%s%s.log"), _T("TBo365URLSync"), time.Format(_T("%Y-%m-%d")));
	m_strExecLogFileFullPath += strLogFileNow;
	this->LogRotate(m_strExecLogFileDir);



	m_strExecDateTime = time.Format(_T("%Y-%m-%d %H:%M:%S"));

	//JSON log
	m_strO365EndpointJsonLogFileDir = m_strExecLogFileDir;
	m_strO365EndpointJsonLogFileDir += _T("json\\");
	::CreateDirectory(m_strO365EndpointJsonLogFileDir, NULL);
	m_strO365EndpointJsonLogFileFullPath = m_strO365EndpointJsonLogFileDir;
	strLogFileNow.Format(_T("%s%s.log"), _T("o365json"), time.Format(_T("%Y-%m-%d-%H%M%S")));
	m_strO365EndpointJsonLogFileFullPath+= strLogFileNow;
	this->LogRotateETC(m_strO365EndpointJsonLogFileDir, _T("o365json"), _T(".log"));

	//CSV log
	m_strO365EndpointCSVLogFileDir = m_strExecLogFileDir;
	m_strO365EndpointCSVLogFileDir += _T("csv\\");
	::CreateDirectory(m_strO365EndpointCSVLogFileDir, NULL);
	m_strO365EndpointCSVLogFileFullPath = m_strO365EndpointCSVLogFileDir;
	strLogFileNow.Format(_T("%s%s.log"), _T("o365csv"), time.Format(_T("%Y-%m-%d-%H%M%S")));
	m_strO365EndpointCSVLogFileFullPath += strLogFileNow;
	this->LogRotateETC(m_strO365EndpointCSVLogFileDir, _T("o365csv"), _T(".log"));

	//ThinBridge Rule log
	m_strO365EndpointTBRuleLogFileDir = m_strExecLogFileDir;
	m_strO365EndpointTBRuleLogFileDir += _T("tbr\\");
	::CreateDirectory(m_strO365EndpointTBRuleLogFileDir, NULL);
	m_strO365EndpointTBRuleLogFileFullPath = m_strO365EndpointTBRuleLogFileDir;
	strLogFileNow.Format(_T("%s%s.log"), _T("TBRule"), time.Format(_T("%Y-%m-%d-%H%M%S")));
	m_strO365EndpointTBRuleLogFileFullPath += strLogFileNow;
	this->LogRotateETC(m_strO365EndpointTBRuleLogFileDir, _T("TBRule"), _T(".log"));


	//ThinBridge Rule Diff log
	m_strO365EndpointTBRuleDiffLogFileDir = m_strExecLogFileDir;
	m_strO365EndpointTBRuleDiffLogFileDir += _T("diff\\");
	::CreateDirectory(m_strO365EndpointTBRuleDiffLogFileDir, NULL);
	m_strO365EndpointTBRuleDiffLogFileFullPath = m_strO365EndpointTBRuleDiffLogFileDir;
	strLogFileNow.Format(_T("%s%s.log"), _T("TBRuleDiff"), time.Format(_T("%Y-%m-%d-%H%M%S")));
	m_strO365EndpointTBRuleDiffLogFileFullPath += strLogFileNow;
	this->LogRotateETC(m_strO365EndpointTBRuleDiffLogFileDir, _T("TBRuleDiff"), _T(".log"));

	m_strO365EndpointTBRuleDiffLogFileDirINI = m_strO365EndpointTBRuleDiffLogFileDir;
	m_strO365EndpointTBRuleDiffLogFileDirINI += _T("INI\\");
	::CreateDirectory(m_strO365EndpointTBRuleDiffLogFileDirINI, NULL);
	m_strO365EndpointTBRuleDiffLogFileFullPathINI = m_strO365EndpointTBRuleDiffLogFileDirINI;
	strLogFileNow.Format(_T("%s%s.log"), _T("TBRuleDiff_INI"), time.Format(_T("%Y-%m-%d-%H%M%S")));
	m_strO365EndpointTBRuleDiffLogFileFullPathINI += strLogFileNow;
	this->LogRotateETC(m_strO365EndpointTBRuleDiffLogFileDirINI, _T("TBRuleDiff_INI"), _T(".log"));

	m_strO365EndpointTBRuleDiffLogFileDirNOW = m_strO365EndpointTBRuleDiffLogFileDir;
	m_strO365EndpointTBRuleDiffLogFileDirNOW += _T("EXEC\\");
	::CreateDirectory(m_strO365EndpointTBRuleDiffLogFileDirNOW, NULL);
	m_strO365EndpointTBRuleDiffLogFileFullPathNOW = m_strO365EndpointTBRuleDiffLogFileDirNOW;
	strLogFileNow.Format(_T("%s%s.log"), _T("TBRuleDiff_EXEC"), time.Format(_T("%Y-%m-%d-%H%M%S")));
	m_strO365EndpointTBRuleDiffLogFileFullPathNOW += strLogFileNow;
	this->LogRotateETC(m_strO365EndpointTBRuleDiffLogFileDirNOW, _T("TBRuleDiff_EXEC"), _T(".log"));

	//history
	m_strTBRuleHistoryLogFileDir = m_strExecLogFileDir;
	m_strTBRuleHistoryLogFileDir += _T("History\\");
	::CreateDirectory(m_strTBRuleHistoryLogFileDir, NULL);
	m_strTBRuleHistoryLogFileFullPath_Before = m_strTBRuleHistoryLogFileDir;
	strLogFileNow.Format(_T("%s_%s_Before.ini"), _T("ThinBridgeBHO"), time.Format(_T("%Y-%m-%d-%H%M%S")));
	m_strTBRuleHistoryLogFileFullPath_Before += strLogFileNow;
	this->LogRotateETC(m_strTBRuleHistoryLogFileDir, _T("ThinBridgeBHO_"), _T("_Before.ini"));

	m_strTBRuleHistoryLogFileFullPath_After = m_strTBRuleHistoryLogFileDir;
	strLogFileNow.Format(_T("%s_%s_After.ini"), _T("ThinBridgeBHO"), time.Format(_T("%Y-%m-%d-%H%M%S")));
	m_strTBRuleHistoryLogFileFullPath_After += strLogFileNow;
	this->LogRotateETC(m_strTBRuleHistoryLogFileDir, _T("ThinBridgeBHO_"), _T("_After.ini"));

	m_strTBRuleHistoryLogFileFullPath_Diff = m_strTBRuleHistoryLogFileDir;
	strLogFileNow.Format(_T("%s_%s_Diff.ini"), _T("ThinBridgeBHO"), time.Format(_T("%Y-%m-%d-%H%M%S")));
	m_strTBRuleHistoryLogFileFullPath_Diff += strLogFileNow;
	this->LogRotateETC(m_strTBRuleHistoryLogFileDir, _T("ThinBridgeBHO_"), _T("_Diff.ini"));



	SetActiveUserNameSID();

	CString strMsg;
	CString Command1;
	CString Command2;
	//�R�}���h���C������B
	if(m_lpCmdLine[0] != '\0')
	{
		//�p�����[�^��1�ȏ�
		if(__argc >=2)
		{
			Command1 = CString(__wargv[1]);
			Command1.Replace(_T("\""),_T(""));
			Command1.TrimLeft();
			Command1.TrimRight();
		}
		//�����X�V�A�X�V��ɏI��
		if (Command1.CompareNoCase(_T("/Config")) == 0)
		{
			CTBo365URLSyncDlg dlg;
			m_pMainWnd = &dlg;
			dlg.DoModal();
			return FALSE;
		}
	}
	else
	{
		if (m_strExeNameNoExt.CompareNoCase(_T("TBo365URLSyncSetting")) == 0)
		{
			CTBo365URLSyncDlg dlg;
			m_pMainWnd = &dlg;
			dlg.DoModal();
			return FALSE;
		}
		////�^�X�N�X�P�W���[������̋N���̏ꍇ
		////�^�X�N�X�P�W���[����1���Ԃ�1�� 00���Ɏ��s����邽�ߕ��ׂ𕪎U���邽�߂ɁA�����_����Wait����B0-10���͈̔�
		//int nRand = 0;
		//CString strWaitTime;
		//CTime time = CTime::GetCurrentTime();
		////system�A�J�E���g��SID
		//if (m_strCurrentSID ==_T("S-1-5-18"))
		//{
		//	srand(GetTickCount());
		//	nRand = rand() % 10;
		//	DWORD dWaitTime = 0;
		//	dWaitTime = nRand * 1000 * 60;
		//	DWORD	dwTime = GetTickCount();
		//	EmptyWorkingSet(GetCurrentProcess());
		//	while (GetTickCount() - dwTime < dWaitTime)
		//	{
		//		MSG	msg = { 0 };
		//		if (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE | PM_QS_PAINT))
		//		{
		//			AfxGetThread()->PumpMessage();
		//		}
		//		::Sleep(100);
		//	}
		//}
		//strWaitTime.Format(_T(" �ҋ@(%d��)"), nRand);
		CString strResult;
		strResult = WriteThinBridgeBHO(&SettingConf, TRUE, TRUE);
		CStdioFile out;
		if (out.Open(m_strExecLogFileFullPath, CFile::modeWrite | CFile::shareDenyNone | CFile::modeCreate | CFile::modeNoTruncate))
		{
			TRY
			{
				out.SeekToEnd();
				out.WriteString(_T("\n�����ݒ�//////////////////////////////////////////////////////////////////////////////////////////////////\n"));
				out.WriteString(time.Format(_T("%Y-%m-%d %H:%M:%S")));
				//out.WriteString(strWaitTime);
				out.WriteString(_T("\n�������ʁF"));
				out.WriteString(GetErrorMsg());
				out.WriteString(_T("\n"));
				out.WriteString(_T("ExecUser:["));
				out.WriteString(m_strExecUserName);
				out.WriteString(_T("]"));
				out.WriteString(_T("LogonUser:["));
				out.WriteString(m_strDomainUserName);
				out.WriteString(_T("]"));
				out.WriteString(_T("LogonUserSID:["));
				out.WriteString(m_strSID);
				out.WriteString(_T("]"));
				out.WriteString(_T("CurrentSID:["));
				out.WriteString(m_strCurrentSID);
				out.WriteString(_T("]\n"));
				strResult.Replace(_T("\r\n"), _T("\n"));
				out.WriteString(strResult);
				out.Close();
			}
				CATCH(CFileException, eP) {}
			END_CATCH
		}
	}
	return FALSE;
}

void CTBo365URLSyncApp::ReadSetting()
{
	SettingConf.SetDefaultData();
	SettingConf.ReadSetting(m_strSetting_FileFullPath);
}
CString CTBo365URLSyncApp::WriteThinBridgeBHO(CConfData* pSettingConf,BOOL bWriteBHO_ini,BOOL bLogAppend)
{
	m_iWriteThinBridgeBHO_ResultCode=0;
	_wsetlocale(LC_ALL, _T("jpn"));

	CString strRet;
	if (!PathFileExists(m_strThinBridgeBHOFileFullPath))
	{
		strRet.Format( _T("�G���[�FThinBridgeBHO.ini �t�@�C����������܂���B\r\n%s\r\n"), m_strThinBridgeBHOFileFullPath);
		m_iWriteThinBridgeBHO_ResultCode= ERR_THINBRIDGE_BHO_INI_NOT_FOUND;
		return strRet;
	}
	if (pSettingConf == NULL)
	{
		strRet.Format(_T("�G���[�F�ݒ�f�[�^���s���ȏ�Ԃł��B\r\n%s\r\n"), m_strSetting_FileFullPath);
		m_iWriteThinBridgeBHO_ResultCode = ERR_THINBRIDGE_BHO_INI_NOT_FOUND;
		return strRet;
	}

	O365EndpointHelper o365;
//	if (o365.ExecMain(pSettingConf->m_strO365URL
//		, pSettingConf->m_strFilter_ServiceArea
//		, pSettingConf->m_strFilter_Category
//		, pSettingConf->m_strFilter_Required
//		, pSettingConf->m_strFilter_TcpPort) != OK_SERVER)
//	{
//		strRet = _T("�G���[�F��肪�������܂����B\r\n");
//		strRet += o365.GetLog();
//		m_iWriteThinBridgeBHO_ResultCode = ERR_O365_API;
//		return strRet;
//	}

	int iRet = 0;
	for (int i = 0; i < 5; i++)
	{
		iRet = o365.ExecMain(pSettingConf->m_strO365URL
			, pSettingConf->m_strFilter_ServiceArea
			, pSettingConf->m_strFilter_Category
			, pSettingConf->m_strFilter_Required
			, pSettingConf->m_strFilter_TcpPort);
		if (iRet == OK_SERVER)
		{
			break;
		}
		else
		{
			if (bLogAppend)
			{
				strRet += _T("Retry:\r\n");
				strRet += o365.GetLog();
				strRet += _T("\r\n");
			}
		}
		//retry
		::Sleep(5000);
	}

	if (iRet != OK_SERVER)
	{
		strRet += _T("�G���[�F��肪�������܂����B\r\n");
		strRet += o365.GetLog();
		m_iWriteThinBridgeBHO_ResultCode = ERR_O365_API;
		return strRet;
	}

	strRet = o365.GetLog();
	CString strJSON;
	strJSON = o365.m_ResData.GetJSONData();
	//�擾����JSON�f�[�^�����O�o��
	CStdioFile out;
	if (out.Open(m_strO365EndpointJsonLogFileFullPath, CFile::modeWrite | CFile::shareDenyNone | CFile::modeCreate))
	{
		TRY
		{
			out.WriteString(_T("�f�[�^�擾�����F"));
			out.WriteString(m_strExecDateTime);
			out.WriteString(_T("\n"));
			strJSON.Replace(_T("\r\n"), _T("\n"));
			out.WriteString(strJSON);
			out.Close();
		}
		CATCH(CFileException, eP) {}
		END_CATCH
	}
	CString strCSV;
	strCSV = o365.m_ResData.GetCSVData();
	//�擾����CSV�f�[�^�����O�o��
	if (out.Open(m_strO365EndpointCSVLogFileFullPath, CFile::modeWrite | CFile::shareDenyNone | CFile::modeCreate))
	{
		TRY
		{
			out.WriteString(_T("�f�[�^�擾�����F"));
			out.WriteString(m_strExecDateTime);
			out.WriteString(_T("\n"));
			strCSV.Replace(_T("\r\n"), _T("\n"));
			out.WriteString(strCSV);
			out.Close();
		}
			CATCH(CFileException, eP) {}
		END_CATCH
	}

	CString strThinBridgeRule;
	strThinBridgeRule= o365.m_ResData.GetThinBridgeRules();

	if (o365.m_ResData.GetCount() == 0)
	{
		strRet = _T("�G���[�FOffice365 �f�[�^�擾�ɖ�肪�������Ă��܂��B������0��\r\n");
		strRet += o365.GetLog();
		m_iWriteThinBridgeBHO_ResultCode = ERR_O365_JSON;
		return strRet;
	}
	if (strThinBridgeRule.IsEmpty())
	{
		strRet = _T("�G���[�FOffice365 �f�[�^����ThinBridge���[���ɕϊ��������ʂ�0���ł��B\r\n");
		strRet += o365.GetLog();
		m_iWriteThinBridgeBHO_ResultCode = ERR_O365_TB_RULE;
		return strRet;
	}

	CStringArray strArrayO356Rules;

	CStringArray strA;
	SBUtil2::Split(&strA, strThinBridgeRule, _T("\r\n"));

	CString strLine;
	CMapStringToString strMapDupChk;
	for (int i = 0; i < strA.GetSize(); i++)
	{
		strLine = strA.GetAt(i);
		if(strLine.IsEmpty())continue;
		//�d���`�F�b�N
		if (strMapDupChk.Lookup(strLine, strLine))
		{
			//Lookup�o�����ꍇ�́A�d�����Ă���B
			CString err;
			err.Format(_T("Duplicate %d %s\r\n"), i + 1, strLine);
			//���O�o�͂���
			if (bLogAppend)
				strRet += err;
		}
		//�d�������B
		else
		{
			strMapDupChk.SetAt(strLine, strLine);
			strArrayO356Rules.Add(strLine);
		}
	}

	int iMaxCnt = 0;
	//�Ώ�List
	iMaxCnt = pSettingConf->m_arr_URL.GetCount();
	for (int i = 0; i < iMaxCnt; i++)
	{
		strLine = pSettingConf->m_arr_URL.GetAt(i);
		//�d���`�F�b�N
		if (strMapDupChk.Lookup(strLine, strLine))
		{
			//Lookup�o�����ꍇ�́A�d�����Ă���B
			CString err;
			err.Format(_T("Duplicate %d %s\r\n"), i + 1, strLine);
			//���O�o�͂���
			if (bLogAppend)
				strRet += err;

		}
		//�d�������B
		else
		{
			strMapDupChk.SetAt(strLine, strLine);
			strArrayO356Rules.Add(strLine);
		}
	}

	//���O�Ώ�List
	iMaxCnt = pSettingConf->m_arr_URL_EX.GetCount();
	for (int i = 0; i < iMaxCnt; i++)
	{
		strLine.Format(_T("-%s"), pSettingConf->m_arr_URL_EX.GetAt(i));
		strArrayO356Rules.Add(strLine);
	}


	int iRuleCnt=0;
	iRuleCnt = strArrayO356Rules.GetCount();
	if(iRuleCnt>0)
	{
		//�擾����ThinBridgeRule�����O�o��
		CStdioFile out;
		CString strTBRule;
		if (out.Open(m_strO365EndpointTBRuleLogFileFullPath, CFile::modeWrite | CFile::shareDenyNone | CFile::modeCreate))
		{
			TRY
			{
				out.WriteString(_T("�f�[�^�擾�����F"));
				out.WriteString(m_strExecDateTime);
				out.WriteString(_T("\n"));
				strTBRule.Format(_T("URL���[�������F%d\n"),iRuleCnt);
				out.WriteString(strTBRule);
				strTBRule.Empty();
				for(int i=0;i<iRuleCnt;i++)
				{
					strTBRule=strArrayO356Rules.GetAt(i);
					out.WriteString(strTBRule);
					out.WriteString(_T("\n"));
				}
				out.Close();
			}
			CATCH(CFileException, eP) {}
			END_CATCH
		}
	}

	//ThinBridgeBHO�̃f�[�^��S�ēǂݍ���
	CStdioFile in;
	CString strTemp;
	CString strLogMsgLine;

	if (!in.Open(m_strThinBridgeBHOFileFullPath, CFile::modeRead | CFile::shareDenyWrite))
	{
		//���O�o�͂���
		if (bLogAppend)
		{
			CString strTemp1;
			strTemp1 = strRet;
			strRet = _T("�G���[�FThinBridgeBHO.ini �t�@�C���I�[�v�����ł��܂���B\r\n");
			strRet += strTemp1;
			strTemp.Format(_T("#Error:ThinBridgeBHO.ini FileOpenError %s\r\n"), m_strThinBridgeBHOFileFullPath);
			strRet += strTemp;
		}
		m_iWriteThinBridgeBHO_ResultCode = ERR_THINBRIDGE_BHO_INI_FILE_OPEN;
		return strRet;
	}

	CStringArray strArrayBHOData;
	CStringArray strArrayBHODataOrgCustom20;
	BOOL bCustom20Section=FALSE;
	m_strArrayBHODataOrg.RemoveAll();
	while (in.ReadString(strTemp))
	{
		m_strArrayBHODataOrg.Add(strTemp);
		//[CUSTOM20]�ȍ~�͕s�v
		if (strTemp == _T("[CUSTOM20]"))
		{
			strArrayBHOData.Add(strTemp);
			bCustom20Section=TRUE;
			continue;
		}
		if(bCustom20Section)
		{
			if(strTemp.Find(_T("@BROWSER_PATH:"))==0)
			{
				strArrayBHOData.Add(strTemp);
				continue;
			}
			else if (strTemp == _T("@DISABLED"))
			{
				strArrayBHOData.Add(strTemp);
				continue;
			}
			else if (strTemp == _T("@TOP_PAGE_ONLY"))
			{
				strArrayBHOData.Add(strTemp);
				continue;
			}
			else if (strTemp == _T("@INTRANET_ZONE"))
				continue;
			else if (strTemp == _T("@TRUSTED_ZONE"))
				continue;
			else if (strTemp == _T("@INTERNET_ZONE"))
				continue;
			else if (strTemp == _T("@UNTRUSTED_ZONE"))
				continue;
			else if(strTemp==_T("@TOP_PAGE_ONLY"))
			{
				strArrayBHOData.Add(strTemp);
				continue;
			}
			else if (strTemp.Find(_T("@REDIRECT_PAGE_ACTION:")) == 0)
			{
				strArrayBHOData.Add(strTemp);
				continue;
			}
			else if (strTemp.Find(_T("@CLOSE_TIMEOUT:")) == 0)
			{
				strArrayBHOData.Add(strTemp);
				continue;
			}
			else
			{
				strArrayBHODataOrgCustom20.Add(strTemp);
				continue;
			}
		}
		strArrayBHOData.Add(strTemp);
	}
	in.Close();

	//Diff
	BOOL bDiff=FALSE;
	CStringArray strArrayBHODataNew;
	for (int i = 0; i < strArrayBHOData.GetSize(); i++)
	{
		strArrayBHODataNew.Add(strArrayBHOData.GetAt(i));
	}
	for (int i = 0; i < strArrayO356Rules.GetSize(); i++)
	{
		strArrayBHODataNew.Add(strArrayO356Rules.GetAt(i));
	}

	CString strDiffLine;
	CString strDiffLine2;
	if(m_strArrayBHODataOrg.GetSize()!= strArrayBHODataNew.GetSize())
		bDiff = TRUE;
	else
	{
		for (int j = 0; j < m_strArrayBHODataOrg.GetSize(); j++)
		{
			strDiffLine = m_strArrayBHODataOrg.GetAt(j);
			strDiffLine2 = strArrayBHODataNew.GetAt(j);
			if (strDiffLine != strDiffLine2)
			{
				bDiff=TRUE;
				break;
			}
		}
	}


	//�ύX����
	if(bDiff)
	{
		//Diff
		iRuleCnt = 0;
		iRuleCnt = strArrayO356Rules.GetCount();
		//�擾����ThinBridgeRule�����O�o��
		CString strTBRule;
		if (out.Open(m_strO365EndpointTBRuleDiffLogFileFullPathNOW, CFile::modeWrite | CFile::shareDenyNone | CFile::modeCreate))
		{
			TRY
			{
				for (int i = 0; i < iRuleCnt; i++)
				{
					strTBRule = strArrayO356Rules.GetAt(i);
					out.WriteString(strTBRule);
					out.WriteString(_T("\n"));
				}
				out.Close();
			}
				CATCH(CFileException, eP) {}
			END_CATCH
		}
		if (out.Open(m_strO365EndpointTBRuleDiffLogFileFullPathINI, CFile::modeWrite | CFile::shareDenyNone | CFile::modeCreate))
		{
			TRY
			{
				for (int j = 0; j < strArrayBHODataOrgCustom20.GetSize(); j++)
				{
					strTBRule = strArrayBHODataOrgCustom20.GetAt(j);
					out.WriteString(strTBRule);
					out.WriteString(_T("\n"));
				}
				out.Close();
			}
				CATCH(CFileException, eP) {}
			END_CATCH
		}
		////////////////////////////////////////////////////////////
		//Diff fc.exe
		CString strCommand;
//		strCommand.Format(_T("/c fc.exe \"%s\" \"%s\" /N > %s"), m_strO365EndpointTBRuleDiffLogFileFullPathINI, m_strO365EndpointTBRuleDiffLogFileFullPathNOW, m_strO365EndpointTBRuleDiffLogFileFullPath);
		CompareFiles(m_strO365EndpointTBRuleDiffLogFileFullPathINI, m_strO365EndpointTBRuleDiffLogFileFullPathNOW, m_strO365EndpointTBRuleDiffLogFileFullPath);

		//if (m_strCurrentSID ==_T("S-1-5-18"))
		//{
		//	STARTUPINFO si = { 0 };
		//	PROCESS_INFORMATION pi = { 0 };
		//	si.cb = sizeof(si);
		//	CString strCommandFull;
		//	strCommandFull.Format(_T("cmd.exe %s"), strCommand);
		//	if (CreateProcess(NULL, (LPTSTR)(LPCTSTR)strCommandFull, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
		//	{
		//		if (pi.hThread)
		//		{
		//			CloseHandle(pi.hThread);  // �X���b�h�̃n���h���͎g��Ȃ��̂ł����j��
		//			pi.hThread = 0;
		//		}
		//		if (pi.hProcess)
		//		{
		//			CloseHandle(pi.hProcess); // �����v���Z�X�̃n���h���͎g��Ȃ��̂Ŕj��
		//			pi.hProcess = 0;
		//		}
		//	}
		//}
		//else
		//	::ShellExecute(NULL, _T("open"), _T("cmd.exe"), strCommand, _T(""), SW_HIDE);
		//DWORD dWaitTime = 0;
		//dWaitTime = 1000 * 10;
		//DWORD	dwTime = GetTickCount();
		//while (GetTickCount() - dwTime < dWaitTime)
		//{
		//	MSG	msg = { 0 };
		//	if (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE | PM_QS_PAINT))
		//	{
		//		AfxGetThread()->PumpMessage();
		//	}
		//	::Sleep(100);
		//}

		CString strFCResult;
		if (in.Open(m_strO365EndpointTBRuleDiffLogFileFullPath, CFile::modeRead | CFile::shareDenyWrite))
		{
			while (in.ReadString(strTemp))
			{
				strFCResult += strTemp;
				strFCResult += _T("\r\n");
			}
			in.Close();
		}

		//BHO�̏�������
		if(bWriteBHO_ini)
		{
			//file backup
			::CopyFile(m_strThinBridgeBHOFileFullPath + ".o365_bak4", m_strThinBridgeBHOFileFullPath + ".o365_bak5", FALSE);
			::CopyFile(m_strThinBridgeBHOFileFullPath + ".o365_bak3", m_strThinBridgeBHOFileFullPath + ".o365_bak4", FALSE);
			::CopyFile(m_strThinBridgeBHOFileFullPath + ".o365_bak2", m_strThinBridgeBHOFileFullPath + ".o365_bak3", FALSE);
			::CopyFile(m_strThinBridgeBHOFileFullPath + ".o365_bak1", m_strThinBridgeBHOFileFullPath + ".o365_bak2", FALSE);
			::CopyFile(m_strThinBridgeBHOFileFullPath, m_strThinBridgeBHOFileFullPath + ".o365_bak1", FALSE);
			SetLastError(NO_ERROR);

			//�ǂݎ���p�̃`�F�b�N���O���B
			DWORD dwAttributes = 0;
			SetFileAttributes(m_strThinBridgeBHOFileFullPath, dwAttributes);

			CStdioFile out;
			if (out.Open(m_strThinBridgeBHOFileFullPath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
			{
				//command1
				CString strWriteData;
				for(int i=0; i<strArrayBHOData.GetSize();i++)
				{
					strWriteData.Format(_T("%s\n"), strArrayBHOData.GetAt(i));
					out.WriteString(strWriteData);
					//strRet += strArrayBHOData.GetAt(i);
					//strRet += _T("\r\n");
				}
				for (int i = 0; i < strArrayO356Rules.GetSize(); i++)
				{
					strWriteData.Format(_T("%s\n"), strArrayO356Rules.GetAt(i));
					out.WriteString(strWriteData);
					//strRet+= strArrayO356Rules.GetAt(i);
					//strRet+=_T("\r\n");
				}
				out.Close();
				//FC�R�}���h�̌��ʂ�����B
				if (!strFCResult.IsEmpty())
				{
					strRet += _T("=====�������=====\r\n");
					strRet += strFCResult;
				}
				//���O�o�͂���
				if (bLogAppend)
				{
					CString strTemp1;
					strTemp1 = strRet;
					strRet = _T("�y�����z\r\n�ݒ�t�@�C���̕ۑ��ɐ������܂����B\r\n");
					strRet +=strTemp1;
				}
				m_iWriteThinBridgeBHO_ResultCode = SUCCESS_ALL;

				//history��
				if (out.Open(m_strTBRuleHistoryLogFileFullPath_Before, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
				{
					//command1
					CString strWriteData;
					for (int i = 0; i < m_strArrayBHODataOrg.GetSize(); i++)
					{
						strWriteData.Format(_T("%s\n"), m_strArrayBHODataOrg.GetAt(i));
						out.WriteString(strWriteData);
					}
					out.Close();
				}
				if (out.Open(m_strTBRuleHistoryLogFileFullPath_After, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
				{
					//command1
					CString strWriteData;
					for (int i = 0; i < strArrayBHOData.GetSize(); i++)
					{
						strWriteData.Format(_T("%s\n"), strArrayBHOData.GetAt(i));
						out.WriteString(strWriteData);
					}
					for (int i = 0; i < strArrayO356Rules.GetSize(); i++)
					{
						strWriteData.Format(_T("%s\n"), strArrayO356Rules.GetAt(i));
						out.WriteString(strWriteData);
					}
					out.Close();
				}
//				strCommand.Format(_T("/c fc.exe \"%s\" \"%s\" /N > %s"), m_strTBRuleHistoryLogFileFullPath_Before, m_strTBRuleHistoryLogFileFullPath_After, m_strTBRuleHistoryLogFileFullPath_Diff);
				CompareFiles(m_strTBRuleHistoryLogFileFullPath_Before, m_strTBRuleHistoryLogFileFullPath_After, m_strTBRuleHistoryLogFileFullPath_Diff);
				//				if (m_strCurrentSID == _T("S-1-5-18"))
//				{
//				}
//				else
//					::ShellExecute(NULL, _T("open"), _T("cmd.exe"), strCommand, _T(""), SW_HIDE);

			}
			else
			{
				//���O�o�͂���
				if (bLogAppend)
				{
					CString strTemp1;
					strTemp1 = strRet;
					strRet = _T("�G���[�FThinBridgeBHO.ini �t�@�C���ۑ��Ŗ�肪�������܂����B\r\n");
					strRet += strTemp1;
					strTemp.Format(_T("#Error2:ThinBridgeBHO.ini FileOpenError %s\r\n"), m_strThinBridgeBHOFileFullPath);
					strRet += strTemp;
				}
				m_iWriteThinBridgeBHO_ResultCode = ERR_THINBRIDGE_BHO_INI_FILE_WRITE;
			}
//			//�ǂݎ���p|�B���t�@�C���ɂ���B
//			dwAttributes = FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN;
//			SetFileAttributes(m_strThinBridgeBHOFileFullPath, dwAttributes);
		}
		else
		{
			//���O�o�͂���
			if (bLogAppend)
			{
				CString strTemp1;
				strTemp1 = strRet;
				strRet = _T("�y�����z\r\n�ݒ�f�[�^�擾�A�����`�F�b�N�ɐ������܂����B\r\n");
				strRet += strTemp1;
			}
			m_iWriteThinBridgeBHO_ResultCode = SUCCESS_NO_FILE_WRITE;
		}
	}
	else
	{
		//���O�o�͂���
		if (bLogAppend)
		{
			CString strTemp1;
			strTemp1 = strRet;
			strRet =_T("�y������e �ύX�Ȃ��z\r\nOffice365�̍X�V���e�ɍ���������܂���ł����B\r\n");
			strRet += strTemp1;
		}
		else
		{
			strRet=_T("SAME_DATA");
		}
		m_iWriteThinBridgeBHO_ResultCode=SUCCESS_SAME_DATA;
	}
	return strRet;
}

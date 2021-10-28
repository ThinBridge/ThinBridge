
// ThinBridgeRuleUpdater.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
//

#include "stdafx.h"
#include "ThinBridgeRuleUpdater.h"
#include "ThinBridgeRuleUpdaterDlg.h"
#include "ThinBridgeRuleUpdaterHelper.h"
#include "locale.h"
#include "psapi.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CThinBridgeRuleUpdaterApp

BEGIN_MESSAGE_MAP(CThinBridgeRuleUpdaterApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CThinBridgeRuleUpdaterApp �R���X�g���N�V����

CThinBridgeRuleUpdaterApp::CThinBridgeRuleUpdaterApp()
{
	m_iWriteThinBridgeBHO_ResultCode=0;
}


// �B��� CThinBridgeRuleUpdaterApp �I�u�W�F�N�g�ł��B

CThinBridgeRuleUpdaterApp theApp;


// CThinBridgeRuleUpdaterApp ������
#include "wtsapi32.h"
#pragma comment(lib, "wtsapi32.lib")
#include <userenv.h>
#pragma comment (lib, "userenv.lib")
void CThinBridgeRuleUpdaterApp::SetActiveUserNameSID()
{
	m_strCurrentSID= GetCurrentProcessStringSid();
	m_strSID= m_strCurrentSID;
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
				if(!strUserName.IsEmpty())
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
						HANDLE hToken={0};
						WTSQueryUserToken(dwSesId, &hToken);
						if(hToken)
						{
							m_strSID =GetTokenStringSid(hToken);
							WriteThinBridgeFlg(_T("LOGON_SID"),m_strSID);
							WriteThinBridgeFlg(_T("CURRENT_SID"), m_strCurrentSID);
							WriteThinBridgeFlg(_T("EXEC_USER"),GetUserNameData());
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
						WriteThinBridgeFlg(_T("LOGON_USER"), m_strDomainUserName);
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

int CThinBridgeRuleUpdaterApp::ExecActiveSession()
{
	//�^�X�N�X�P�W���[������̎��s�̏ꍇ�ɍi��B
	if(m_strCurrentSID != _T("S-1-5-18"))
		return 1;

	PWTS_SESSION_INFOW pStSesInf = NULL;
	DWORD dwCount = 0;
	DWORD dwSesId = 0;
	DWORD dwSize = 0;
	// ���O�I�����[�U�̃Z�b�V�������X�g���擾����
	if (::WTSEnumerateSessionsW(NULL,
		0,                    // �\��i�K��0���w��j
		1,                    // �񋓗v���̃o�[�W����(�K��1���w��)
		&pStSesInf,           // �\���̂̔z��ւ̃|�C���^������
		&dwCount) == FALSE) // �\���̂̐��i�[�p(�Z�b�V�������X�g��)
	{
		// �擾���s
		return 2;
	}
	int iRet=0;
	// �Z�b�V�������X�g�����J��Ԃ�
	for (DWORD dwLoop = 0; dwLoop < dwCount; dwLoop++)
	{
		iRet=3;
		// ���݃A�N�e�B�u�ȃZ�b�V������ID���擾
		dwSesId = ::WTSGetActiveConsoleSessionId();
		// �Z�b�V����ID���r���A�N�e�B�u�ȃZ�b�V����ID�Ɠ���Ȃ烆�[�U����\������
		if (pStSesInf[dwLoop].SessionId == dwSesId)
		{
			iRet = 4;
			HANDLE hToken = { 0 };
			WTSQueryUserToken(pStSesInf[dwLoop].SessionId, &hToken);
			if (hToken)
			{
				iRet = 5;
				CString strSID = GetTokenStringSid(hToken);
				//system�A�J�E���g��SID�ł͖����B���O�C���Z�b�V����������B
				if (strSID != _T("S-1-5-18"))
				{
					//���O�C�����[�U�[�Z�b�V�����ł��̃v���O�������N������B
					//�������݁iCIFS�Ȃǂւ̃A�N�Z�X�j
					LPVOID              lpEnvironment={0};
					STARTUPINFO         startupInfo={0};
					PROCESS_INFORMATION processInformation={0};
					CreateEnvironmentBlock(&lpEnvironment, hToken, TRUE);
					ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
					startupInfo.cb = sizeof(STARTUPINFO);
					const WCHAR szD[]= _T("winsta0\\default");
					startupInfo.lpDesktop = (LPWSTR)szD;
					CreateProcessAsUser(hToken, m_strExeFullPath, NULL, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT, lpEnvironment, NULL, &startupInfo, &processInformation);
					DestroyEnvironmentBlock(lpEnvironment);
					CloseHandle(processInformation.hThread);
					CloseHandle(processInformation.hProcess);
					iRet=200;	
					CloseHandle(hToken);
					break;
				}
				CloseHandle(hToken);
			}
		}
	}
	//RDP�Ή�
	//Console Session�����Ȃ��ꍇ�́A�͂��߂̃��[�U�[�Ŏ��s���Ă݂�B
	if(iRet!=200)
	{
		// �Z�b�V�������X�g�����J��Ԃ�
		for (DWORD dwLoop = 0; dwLoop < dwCount; dwLoop++)
		{
			iRet = 6;
			HANDLE hToken = { 0 };
			WTSQueryUserToken(pStSesInf[dwLoop].SessionId, &hToken);
			if (hToken)
			{
				iRet = 7;
				CString strSID = GetTokenStringSid(hToken);
				//system�A�J�E���g��SID�ł͖����B���O�C���Z�b�V����������B
				if (strSID != _T("S-1-5-18"))
				{
					//���O�C�����[�U�[�Z�b�V�����ł��̃v���O�������N������B
					//�������݁iCIFS�Ȃǂւ̃A�N�Z�X�j
					LPVOID              lpEnvironment = { 0 };
					STARTUPINFO         startupInfo = { 0 };
					PROCESS_INFORMATION processInformation = { 0 };
					CreateEnvironmentBlock(&lpEnvironment, hToken, TRUE);
					ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
					startupInfo.cb = sizeof(STARTUPINFO);
					const WCHAR szD[] = _T("winsta0\\default");
					startupInfo.lpDesktop = (LPWSTR)szD;
					CreateProcessAsUser(hToken, m_strExeFullPath, NULL, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT, lpEnvironment, NULL, &startupInfo, &processInformation);
					DestroyEnvironmentBlock(lpEnvironment);
					CloseHandle(processInformation.hThread);
					CloseHandle(processInformation.hProcess);
					iRet = 200;
					CloseHandle(hToken);
					break;
				}
				CloseHandle(hToken);
			}
		}
	}
	if (pStSesInf != NULL)
	{
		// ���
		WTSFreeMemory(pStSesInf);
		pStSesInf = NULL;
	}
	return iRet;
}

BOOL CThinBridgeRuleUpdaterApp::InitInstance()
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

	m_strExeFolderPath=szDrive;
	m_strExeFolderPath+= szDir;

	//�ݒ�f�[�^�̃��[�h
	m_strSetting_FileFullPath = szDrive;
	m_strSetting_FileFullPath += szDir;
	m_strSetting_FileFullPath += _T("ThinBridgeRuleUpdater.conf");
	this->ReadSetting();

	m_strThinBridgeBHOFileFullPath = szDrive;
	m_strThinBridgeBHOFileFullPath += szDir;
	m_strThinBridgeBHOFileFullPath += _T("ThinBridgeBHO.ini");

	m_strExecLogFileFullPath = szDrive;
	m_strExecLogFileFullPath += szDir;
	m_strExecLogFileFullPath += _T("TBUpdateLog\\");
	::CreateDirectory(m_strExecLogFileFullPath, NULL);
	m_strExecLogFileDir= m_strExecLogFileFullPath;
	CTime time = CTime::GetCurrentTime();
	CString strLogFileNow;
	strLogFileNow.Format(_T("ThinBridgeRuleUpdater%s.log"), time.Format(_T("%Y-%m-%d")));
	m_strExecLogFileFullPath += strLogFileNow;
	this->LogRotate(m_strExecLogFileDir);

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
		if (Command1.CompareNoCase(_T("/Config")) == 0)
		{
			CThinBridgeRuleUpdaterDlg dlg;
			m_pMainWnd = &dlg;
			dlg.DoModal();
			return FALSE;
		}
	}
	else
	{
		if (m_strExeNameNoExt.CompareNoCase(_T("ThinBridgeRuleUpdaterSetting")) == 0)
		{
			CThinBridgeRuleUpdaterDlg dlg;
			m_pMainWnd = &dlg;
			dlg.DoModal();
			return FALSE;
		}

		//�^�X�N�X�P�W���[������̋N���̏ꍇ
		//�^�X�N�X�P�W���[����1���Ԃ�1�� ���s����邽�ߕ��ׂ𕪎U���邽�߂ɁA�����_����Wait����B0-10���͈̔�
		int nRand = 0;
		CString strWaitTime;
		CTime time = CTime::GetCurrentTime();

		//system�A�J�E���g��SID
		if (m_strCurrentSID == _T("S-1-5-18"))
		{
			CTime cTime = CTime::GetCurrentTime();
			int iMin=0;
			iMin = cTime.GetMinute();
			BOOL bWait=FALSE;
			srand(GetTickCount());

			//���O�C�����ɂ��^�X�N�X�P�W���[���o�R�ł��ꂪ�����B
			//�^�X�N�X�P�W���[���o�R�Ŏ��s���Ă���̂Ŕ��f���ł��Ȃ����߁B
			//30���̊Ԃ́A�X�P�W���[����30���N���Ȃ̂�0-10��Wait����B���ו��U(VDI)
			if (InCtrixVDI())
			{
				if (iMin == 30)
				{
					//VDI��30���̋N���Ń^�X�N�X�P�W���[������̏ꍇ�@���O�I���^�X�N�̏ꍇ�ƒ�����s���d�Ȃ�ꍇ������B
					//VDI���́A�h���C�u�����t���b�V�������̂�ThinBridgeBHO.ini���c��Ȃ��B
					//�t�@�C�����Ȃ��ꍇ�́AWait�����ɂ����Ɏ��s����B
					if (PathFileExists(m_strThinBridgeBHOFileFullPath))
					{
						bWait = TRUE;
						//1��(60�b)�̒��Ń����_���l��Wait������B�����ł����ׂ𕪎U�������B
						nRand = rand() % 60;
						DWORD dWaitTime = 0;
						dWaitTime = nRand * 1000;//�b�P��
						WaitSec(dWaitTime);
					}
					else
					{
						bWait = FALSE;
					}
				}
			}
			//00���̊Ԃ́A�X�P�W���[����00���N���Ȃ̂�0-10��Wait����B���ו��U(LTSC)
			else
			{
				//�[�����́A���O�C���Ɋւ��ăX�^�[�g�A�b�v�̃V���[�g�J�b�g�Ŏ��s�����̂�
				//�^�X�N���s�͒�����s�̂݁B������x�x��������\�����l����0-1���̊Ԃ̏ꍇ��Wait����B
				if (iMin == 0|| iMin == 1)
				{
					//�t�@�C�����Ȃ��ꍇ�́AWait�����ɂ����Ɏ��s����B
					if (PathFileExists(m_strThinBridgeBHOFileFullPath))
					{
						bWait = TRUE;
						//1��(60�b)�̒��Ń����_���l��Wait������B�����ł����ׂ𕪎U�������B
						nRand = rand() % 60;
						DWORD dWaitTime = 0;
						dWaitTime = nRand * 1000;//�b�P��
						WaitSec(dWaitTime);
					}
					else
					{
						bWait = FALSE;
					}
				}
			}
			if(bWait)
			{
				EmptyWorkingSet(GetCurrentProcess());
				if(SettingConf.m_iInterval!=0)
				{
					nRand = rand() % SettingConf.m_iInterval;
					DWORD dWaitTime = 0;
					dWaitTime = nRand * 1000*60;//���P��
					WaitSec(dWaitTime);
				}
			}
			else
			{
				//���O�C�����g���K�[�ɂȂ��Ă���\���������̂�Wait�Ȃ��B
				nRand=0;
			}

			int iRet = 0;
			iRet = ExecActiveSession();
			//���s�Ɏ��s�����ꍇ�́ASystem�����Ŏ��s����
			//200 OK�̏ꍇ�́A���O�C���Z�b�V�����ł���EXE�����s�����̂ŁA�����ŏI��
			//���s���Ă���ꍇ�́A���̂܂ܗ����B
			if (iRet==200)
			{
				return FALSE;
			}
		}
		//2�d�N������
		HANDLE hMutexCtx = NULL;
		hMutexCtx = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("Local\\TBRuleUpdateFlg"));
		if (hMutexCtx != NULL)
		{
			//���ɋN�����Ă���̂ŁA�N�����Ă���v���Z�X�ɔC����
			return FALSE;
		}
		//Mutex�쐬
		if (!m_hMutex)
		{
			m_hMutex = ::CreateMutex(NULL, FALSE, _T("Local\\TBRuleUpdateFlg"));
		}
		//strWaitTime.Format(_T(" �ҋ@(%d��)"), nRand);
		CString strResult;
		strResult = WriteThinBridgeBHO(&SettingConf, TRUE, FALSE);
		CStdioFile out;
		if (out.Open(m_strExecLogFileFullPath, CFile::modeWrite | CFile::shareDenyNone | CFile::modeCreate | CFile::modeNoTruncate))
		{
			TRY
			{
				out.SeekToEnd();
				out.WriteString(_T("\n�����X�V//////////////////////////////////////////////////////////////////////////////////////////////////\n"));
				out.WriteString(time.Format(_T("%Y-%m-%d %H:%M:%S")));
				out.WriteString(strWaitTime);
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
				out.WriteString(_T("SettingType:"));
				out.WriteString(SettingConf.IsGPO()?_T("GPO\n"):_T("File\n"));
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

void CThinBridgeRuleUpdaterApp::ReadSetting()
{
	//GPO��D�悳����
	if(!SettingConf.ReadSettingGPO())
		SettingConf.ReadSetting(m_strSetting_FileFullPath);
}
CString CThinBridgeRuleUpdaterApp::WriteThinBridgeBHO(CConfData* pSettingConf,BOOL bWriteBHO_ini,BOOL bFromGUI)
{
	m_iWriteThinBridgeBHO_ResultCode=0;
	_wsetlocale(LC_ALL, _T("jpn"));

	CString strRet;
	if (pSettingConf == NULL)
	{
		strRet.Format(_T("�G���[�F�ݒ�f�[�^���s���ȏ�Ԃł��B\r\n%s\r\n"), m_strSetting_FileFullPath);
		m_iWriteThinBridgeBHO_ResultCode = ERR_THINBRIDGE_BHO_INI_NOT_FOUND;
		return strRet;
	}

	if (pSettingConf->m_strConfigServerURL1.IsEmpty())
	{
		m_iWriteThinBridgeBHO_ResultCode = INFO_CONFIG_SERVER_NOT_SETTING;
		return strRet;
	}

	CString strConfigServerURL;
	ThinBridgeRuleUpdaterHelper TBRuleUpdate;

	CString strServerIndex;
	strConfigServerURL = pSettingConf->m_strConfigServerURL1;
	int iRet = 0;
	CString strLineMsg;
	CString strLogTry;
	CStringArray strConfigServerArry;
	srand(GetTickCount());

	strConfigServerArry.Add(pSettingConf->m_strConfigServerURL1);
	if (!pSettingConf->m_strConfigServerURL2.IsEmpty())
		strConfigServerArry.Add(pSettingConf->m_strConfigServerURL2);
	if (!pSettingConf->m_strConfigServerURL3.IsEmpty())
		strConfigServerArry.Add(pSettingConf->m_strConfigServerURL3);
	if (!pSettingConf->m_strConfigServerURL4.IsEmpty())
		strConfigServerArry.Add(pSettingConf->m_strConfigServerURL4);
	if (!pSettingConf->m_strConfigServerURL5.IsEmpty())
		strConfigServerArry.Add(pSettingConf->m_strConfigServerURL5);
	if (!pSettingConf->m_strConfigServerURL6.IsEmpty())
		strConfigServerArry.Add(pSettingConf->m_strConfigServerURL6);

	CStringArray strConfigServerArryExec;
	int iSetServerCnt = 0;
	iSetServerCnt = strConfigServerArry.GetSize();

	int nRandServer = 0;
	//1�̂݁B
	if (iSetServerCnt == 1)
	{
		strConfigServerArryExec.Add(pSettingConf->m_strConfigServerURL1);
	}
	else
	{
		//�����_��
		if(pSettingConf->m_iServerConnectionRule==0)
		{
			CString strSvrTemp;
			CStringArray strConfigServerArryWork;
			for (int ii = 0; ii < 20; ii++)
			{
				strSvrTemp.Empty();
				int iSetServerCntAll = strConfigServerArry.GetSize();
				for (int j = iSetServerCntAll; j > 0; j--)
				{
					nRandServer = rand() % iSetServerCntAll;
					strSvrTemp = strConfigServerArry.GetAt(nRandServer);
					if (!strSvrTemp.IsEmpty())
					{
						strConfigServerArryExec.Add(strSvrTemp);
						strConfigServerArry.SetAt(nRandServer, _T(""));
					}
				}
				strConfigServerArryWork.RemoveAll();
				for (int i = 0; i < strConfigServerArry.GetSize(); i++)
				{
					strSvrTemp = strConfigServerArry.GetAt(i);
					if (!strSvrTemp.IsEmpty())
						strConfigServerArryWork.Add(strSvrTemp);
				}
				if (strConfigServerArryWork.GetSize() > 0)
				{
					strConfigServerArry.RemoveAll();
					strConfigServerArry.Copy(strConfigServerArryWork);
				}
				int iSetServerCntNow = strConfigServerArryExec.GetSize();
				if (iSetServerCntNow == iSetServerCnt)
					break;
			}
			if (strConfigServerArryExec.GetSize() != iSetServerCnt)
			{
				for (int i = 0; i < strConfigServerArry.GetSize(); i++)
				{
					strSvrTemp.Empty();
					strSvrTemp = strConfigServerArry.GetAt(i);
					if (!strSvrTemp.IsEmpty())
					{
						strConfigServerArryExec.Add(strSvrTemp);
						strConfigServerArry.SetAt(nRandServer, _T(""));
					}
				}
			}
		}
		//���X�g��
		else
		{
			strConfigServerArryExec.Copy(strConfigServerArry);
		}
		CString strMsgTemp;
		for (int i = 0; i < strConfigServerArryExec.GetSize(); i++)
		{
			strMsgTemp.Format(_T("�擾��%d�F%s\r\n"), i + 1, strConfigServerArryExec.GetAt(i));
			strServerIndex += strMsgTemp;
		}
	}

	//GUI����̏ꍇ
	if(bFromGUI)
	{
		//�����T�[�o�[�璷��
		for (int j = 0; j < strConfigServerArryExec.GetSize(); j++)
		{
			strConfigServerURL = strConfigServerArryExec.GetAt(j);
			iRet = TBRuleUpdate.ExecMain(strConfigServerURL);
			if (iRet == OK_SERVER)
			{
				strLineMsg.Format(_T("%d:OK %d %s %s"), j + 1, iRet, TBRuleUpdate.GetErrorMsg(iRet), strConfigServerURL);
				pSettingConf->m_strConfigServerURLOK = strConfigServerURL;
				strLogTry += LogFmt(strLineMsg);
				strRet += strLogTry;
				break;
			}
			strLineMsg.Format(_T("%d:Err %d %s %s"), j + 1, iRet, TBRuleUpdate.GetErrorMsg(iRet), strConfigServerURL);
			strLogTry += LogFmt(strLineMsg);
		}
		if (OK_SERVER != iRet)
		{
			strRet = _T("�G���[�F��肪�������܂����B\r\n");
			strRet += strServerIndex;
			strRet += strLogTry;
			strRet += TBRuleUpdate.GetLog();
			m_iWriteThinBridgeBHO_ResultCode = ERR_CONFIG_SERVER;
			return strRet;
		}
	}
	else//�^�X�N�X�P�W���[���A�X�^�[�g�A�b�v�A���ڋN���̏ꍇ�̓��g���C������B
	{
		//�����T�[�o�[�璷��
		for(int j=0;j< strConfigServerArryExec.GetSize();j++)
		{
			strConfigServerURL = strConfigServerArryExec.GetAt(j);
			iRet = TBRuleUpdate.ExecMain(strConfigServerURL);
			if (iRet == OK_SERVER)
			{
				strLineMsg.Format(_T("%d:OK %d %s %s"), j + 1, iRet, TBRuleUpdate.GetErrorMsg(iRet), strConfigServerURL);
				pSettingConf->m_strConfigServerURLOK = strConfigServerURL;
				strLogTry += LogFmt(strLineMsg);
				strRet += strLogTry;
				break;
			}
			strLineMsg.Format(_T("%d:Err %d %s %s"), j + 1, iRet, TBRuleUpdate.GetErrorMsg(iRet), strConfigServerURL);
			strLogTry += LogFmt(strLineMsg);

			////10�b�̊Ԃő҂��Ă݂�
			//int nRand = rand() % 10;
			//DWORD dWaitTime = 0;
			//dWaitTime = nRand * 1000;//�b�P��
			//WaitSec(dWaitTime);
		}

		//retry 2
		if (OK_SERVER != iRet)
		{
			//10�b�҂��Ă݂�
			WaitSec(1000*10);

			int nRand = rand() % 10;
			DWORD dWaitTime = 0;
			dWaitTime = nRand * 1000;//�b�P��
			//+10�b���炢�̊Ԃő҂��Ă݂�
			WaitSec(dWaitTime);

			for (int j = 0; j < strConfigServerArryExec.GetSize(); j++)
			{
				strConfigServerURL = strConfigServerArryExec.GetAt(j);
				iRet = TBRuleUpdate.ExecMain(strConfigServerURL);
				if (iRet == OK_SERVER)
				{
					strLineMsg.Format(_T("%d:OK %d %s %s"), j + 1, iRet, TBRuleUpdate.GetErrorMsg(iRet), strConfigServerURL);
					pSettingConf->m_strConfigServerURLOK = strConfigServerURL;
					strLogTry += LogFmt(strLineMsg);
					strRet += strLogTry;
					break;
				}
				strLineMsg.Format(_T("%d:Err %d %s %s"), j + 1, iRet, TBRuleUpdate.GetErrorMsg(iRet), strConfigServerURL);
				strLogTry += LogFmt(strLineMsg);

				////10�b�̊Ԃő҂��Ă݂�
				//int nRand = rand() % 10;
				//DWORD dWaitTime = 0;
				//dWaitTime = nRand * 1000;//�b�P��
				//WaitSec(dWaitTime);
			}
		}

		//retry 3
		if (OK_SERVER != iRet)
		{
			//10�b�҂��Ă݂�
			WaitSec(1000 * 10);

			int nRand = rand() % 10;
			DWORD dWaitTime = 0;
			dWaitTime = nRand * 1000;//�b�P��
			//+10�b���炢�̊Ԃő҂��Ă݂�
			WaitSec(dWaitTime);

			for (int j = 0; j < strConfigServerArryExec.GetSize(); j++)
			{
				strConfigServerURL = strConfigServerArryExec.GetAt(j);
				iRet = TBRuleUpdate.ExecMain(strConfigServerURL);
				if (iRet == OK_SERVER)
				{
					strLineMsg.Format(_T("%d:OK %d %s %s"), j + 1, iRet, TBRuleUpdate.GetErrorMsg(iRet), strConfigServerURL);
					pSettingConf->m_strConfigServerURLOK = strConfigServerURL;
					strLogTry += LogFmt(strLineMsg);
					strRet += strLogTry;
					break;
				}
				strLineMsg.Format(_T("%d:Err %d %s %s"), j + 1, iRet, TBRuleUpdate.GetErrorMsg(iRet), strConfigServerURL);
				strLogTry += LogFmt(strLineMsg);

				////10�b�̊Ԃő҂��Ă݂�
				//int nRand = rand() % 10;
				//DWORD dWaitTime = 0;
				//dWaitTime = nRand * 1000;//�b�P��
				//WaitSec(dWaitTime);
			}
		}
		if (OK_SERVER != iRet)
		{
			strRet = _T("�G���[�F��肪�������܂����B\r\n");
			strRet += strServerIndex;
			strRet += strLogTry;
			strRet += TBRuleUpdate.GetLog();
			m_iWriteThinBridgeBHO_ResultCode = ERR_CONFIG_SERVER;
			return strRet;
		}
	}
	strRet += TBRuleUpdate.GetLog();

	//ThinBridgeBHO�̃f�[�^��S�ēǂݍ���
	CStdioFile in;
	CString strTemp;
	CString strLogMsgLine;

	m_strArrayBHODataOrg.RemoveAll();
	if(in.Open(m_strThinBridgeBHOFileFullPath, CFile::modeRead | CFile::shareDenyWrite))
	{
		CStringArray strArrayBHOData;
		while (in.ReadString(strTemp))
		{
			m_strArrayBHODataOrg.Add(strTemp);
		}
		in.Close();
	}
	//Diff
	BOOL bDiff=FALSE;
	CStringArray strArrayBHODataNew;
	SBUtil2::Split(&strArrayBHODataNew, TBRuleUpdate.m_strGetData,_T("\r\n"));

	//�T�[�o�[����擾������Ƀ`�F�b�N���Ă���̂ł����ł͂��Ȃ��B
	////�T�[�o�[����擾�t�@�C���̓��e�`�F�b�N
	//int iValidCnt=0;
	//for (int j = 0; j < strArrayBHODataNew.GetSize(); j++)
	//{
	//	strTemp= strArrayBHODataNew.GetAt(j);
	//	if (!strTemp.IsEmpty())
	//	{
	//		if(strTemp==_T("[GLOBAL]"))
	//			iValidCnt++;
	//		else if(strTemp == _T("[Chrome]"))
	//			iValidCnt++;
	//		else if (strTemp == _T("[CUSTOM18]"))
	//			iValidCnt++;
	//		else if (strTemp == _T("[CUSTOM19]"))
	//			iValidCnt++;
	//		else if (strTemp == _T("[CUSTOM20]"))
	//			iValidCnt++;
	//	}
	//}
	//if(iValidCnt < 3)
	//{
	//	strRet.Format(_T("�G���[�F�T�[�o�[����擾����ThinBridge�ݒ�f�[�^���s���ȏ�Ԃł��B\r\n"));
	//	strRet += TBRuleUpdate.GetLog();
	//	m_iWriteThinBridgeBHO_ResultCode = ERR_THINBRIDGE_BHO_INI_INVALID;
	//	return strRet;
	//}

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
		//BHO�̏�������
		if(bWriteBHO_ini)
		{
			//file backup
			::CopyFile(m_strThinBridgeBHOFileFullPath + ".update_bak4", m_strThinBridgeBHOFileFullPath + ".update_bak5", FALSE);
			::CopyFile(m_strThinBridgeBHOFileFullPath + ".update_bak3", m_strThinBridgeBHOFileFullPath + ".update_bak4", FALSE);
			::CopyFile(m_strThinBridgeBHOFileFullPath + ".update_bak2", m_strThinBridgeBHOFileFullPath + ".update_bak3", FALSE);
			::CopyFile(m_strThinBridgeBHOFileFullPath + ".update_bak1", m_strThinBridgeBHOFileFullPath + ".update_bak2", FALSE);
			::CopyFile(m_strThinBridgeBHOFileFullPath, m_strThinBridgeBHOFileFullPath + ".update_bak1", FALSE);
			SetLastError(NO_ERROR);

			//�ǂݎ���p�̃`�F�b�N���O���B
			DWORD dwAttributes=0;
			SetFileAttributes(m_strThinBridgeBHOFileFullPath, dwAttributes);

			CStdioFile out;
			if (out.Open(m_strThinBridgeBHOFileFullPath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
			{
				//command1
				CString strWriteData;
				for(int i=0; i< strArrayBHODataNew.GetSize();i++)
				{
					strWriteData.Format(_T("%s\n"), strArrayBHODataNew.GetAt(i));
					out.WriteString(strWriteData);
					strRet += strArrayBHODataNew.GetAt(i);
					strRet += _T("\r\n");
				}
				out.Close();
				CString strTemp1;
				strTemp1 = strRet;
				strRet = _T("�y�����z\r\n�ݒ�t�@�C���̕ۑ��ɐ������܂����B\r\n");
				strRet += strServerIndex;
				strRet +=strTemp1;
				m_iWriteThinBridgeBHO_ResultCode = SUCCESS_ALL;
				////////////////////////////////////////////////////////////////////////////////
				//�������o�͂���B
				CTime time = CTime::GetCurrentTime();
				CString strLogFileNow;
				CString strHistoryDir;

				strHistoryDir = m_strExecLogFileDir;
				strHistoryDir += _T("History\\");
				::CreateDirectory(strHistoryDir, NULL);

				CString strBefore;
				strBefore = strHistoryDir;
				strLogFileNow.Format(_T("%s_%s_Before.ini"), _T("ThinBridgeBHO"), time.Format(_T("%Y-%m-%d-%H%M%S")));
				strBefore += strLogFileNow;
				::CopyFile(m_strThinBridgeBHOFileFullPath + ".update_bak1", strBefore, FALSE);
				SetFileAttributes(strBefore, 0);
				this->LogRotateETC(strHistoryDir, _T("ThinBridgeBHO_"), _T("_Before.ini"));

				CString strAfter;
				strAfter = strHistoryDir;
				strLogFileNow.Format(_T("%s_%s_After.ini"), _T("ThinBridgeBHO"), time.Format(_T("%Y-%m-%d-%H%M%S")));
				strAfter += strLogFileNow;
				::CopyFile(m_strThinBridgeBHOFileFullPath, strAfter, FALSE);
				SetFileAttributes(strAfter, 0);
				this->LogRotateETC(strHistoryDir, _T("ThinBridgeBHO_"), _T("_After.ini"));


				CString strDiffFile;
				strDiffFile = strHistoryDir;
				strLogFileNow.Format(_T("%s_%s_Diff.ini"), _T("ThinBridgeBHO"), time.Format(_T("%Y-%m-%d-%H%M%S")));
				strDiffFile += strLogFileNow;
				this->LogRotateETC(strHistoryDir, _T("ThinBridgeBHO_"), _T("_Diff.ini"));

				CompareFiles(strBefore, strAfter, strDiffFile);

			}
			else
			{
				CString strTemp1;
				strTemp1 = strRet;
				strRet = _T("�G���[�FThinBridgeBHO.ini �t�@�C���ۑ��Ŗ�肪�������܂����B\r\n");
				strRet += strServerIndex;
				strRet += strTemp1;
				strTemp.Format(_T("#Error2:ThinBridgeBHO.ini FileOpenError %s\r\n"), m_strThinBridgeBHOFileFullPath);
				strRet += strTemp;
				m_iWriteThinBridgeBHO_ResultCode = ERR_THINBRIDGE_BHO_INI_FILE_WRITE;
			}
			//�ǂݎ���p|�B���t�@�C���ɂ���B
			dwAttributes = FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN;
			SetFileAttributes(m_strThinBridgeBHOFileFullPath, dwAttributes);

		}
		else
		{
			CString strTemp1;
			strTemp1 = strRet;
			strRet = _T("�y�����z\r\n�ݒ�f�[�^�擾�A�����`�F�b�N�ɐ������܂����B\r\n");
			CString strTemp2;
			strTemp2.Format(_T("�擾���F%s\r\n�X�V��F%s\r\n"), strConfigServerURL, m_strThinBridgeBHOFileFullPath);
			strRet += strServerIndex;
			strRet += strTemp2;
			strRet += strTemp1;
			m_iWriteThinBridgeBHO_ResultCode = SUCCESS_NO_FILE_WRITE;
		}
	}
	else
	{
		CString strTemp1;
		strTemp1 = strRet;
		strRet =_T("�y������e �ύX�Ȃ��z\r\n�X�V���e�ɍ���������܂���ł����B\r\n");
		strTemp1.Format(_T("�擾���F%s\r\n�X�V��F%s\r\n"), strConfigServerURL, m_strThinBridgeBHOFileFullPath);
		strRet += strServerIndex;
		strRet += strTemp1;
		m_iWriteThinBridgeBHO_ResultCode=SUCCESS_SAME_DATA;
	}
	return strRet;
}


// ThinBridgeChecker.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
//

#include "stdafx.h"
#include "ThinBridgeChecker.h"
#include "ThinBridgeCheckerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CThinBridgeCheckerApp

BEGIN_MESSAGE_MAP(CThinBridgeCheckerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CThinBridgeCheckerApp �R���X�g���N�V����

CThinBridgeCheckerApp::CThinBridgeCheckerApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}


// �B��� CThinBridgeCheckerApp �I�u�W�F�N�g�ł��B

CThinBridgeCheckerApp theApp;


// CThinBridgeCheckerApp ������

BOOL CThinBridgeCheckerApp::InitInstance()
{
	// �A�v���P�[�V���� �}�j�t�F�X�g�� visual �X�^�C����L���ɂ��邽�߂ɁA
	// ComCtl32.dll Version 6 �ȍ~�̎g�p���w�肷��ꍇ�́A
	// Windows XP �� InitCommonControlsEx() ���K�v�ł��B�����Ȃ���΁A�E�B���h�E�쐬�͂��ׂĎ��s���܂��B
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// �A�v���P�[�V�����Ŏg�p���邷�ׂẴR���� �R���g���[�� �N���X���܂߂�ɂ́A
	// �����ݒ肵�܂��B
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
    if (!AfxOleInit())
    {
        return FALSE;
    }
	AfxOleGetMessageFilter()->EnableNotRespondingDialog(FALSE);
	AfxOleGetMessageFilter()->EnableBusyDialog(FALSE);


	AfxEnableControlContainer();

	TCHAR szPath[MAX_PATH+1]={0};
	GetModuleFileName(AfxGetInstanceHandle(),szPath,MAX_PATH);
	CString strZonePath;
	strZonePath = szPath;
	strZonePath += _T(":Zone.Identifier");
	::DeleteFile(strZonePath);
	SetLastError(NO_ERROR);


	// �_�C�A���O�ɃV�F�� �c���[ �r���[�܂��̓V�F�� ���X�g �r���[ �R���g���[����
	// �܂܂�Ă���ꍇ�ɃV�F�� �}�l�[�W���[���쐬���܂��B
	//CShellManager *pShellManager = new CShellManager;

	// �W��������
	// �����̋@�\���g�킸�ɍŏI�I�Ȏ��s�\�t�@�C����
	// �T�C�Y���k���������ꍇ�́A�ȉ�����s�v�ȏ�����
	// ���[�`�����폜���Ă��������B
	// �ݒ肪�i�[����Ă��郌�W�X�g�� �L�[��ύX���܂��B
	// TODO: ��Ж��܂��͑g�D���Ȃǂ̓K�؂ȕ������
	// ���̕������ύX���Ă��������B
	//SetRegistryKey(_T("�A�v���P�[�V���� �E�B�U�[�h�Ő������ꂽ���[�J�� �A�v���P�[�V����"));

	CThinBridgeCheckerDlg dlg;
	m_pMainWnd = &dlg;
	//�R�}���h���C������B
	if (m_lpCmdLine[0] != '\0')
	{
		CString Command1;
		//�p�����[�^��1�ȏ�
		if (__argc >= 2)
		{
			Command1 = CString(__wargv[1]);
			Command1.Replace(_T("\""), _T(""));
			Command1.TrimLeft();
			Command1.TrimRight();
		}
		if (Command1.CompareNoCase(_T("/log")) == 0)
		{
			TCHAR szPath[MAX_PATH + 1] = { 0 };
			TCHAR szDrive[_MAX_DRIVE] = { 0 };
			TCHAR szDir[_MAX_DIR] = { 0 };
			TCHAR szFileName[_MAX_FNAME] = { 0 };
			TCHAR szExt[_MAX_EXT] = { 0 };
			GetModuleFileName(AfxGetInstanceHandle(), szPath, MAX_PATH);
			_tsplitpath_s(szPath, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFileName, _MAX_FNAME, szExt, _MAX_EXT);

			CString strExecLogFileFullPath;
			CString strExecLogFileDir;
			strExecLogFileFullPath = szDrive;
			strExecLogFileFullPath += szDir;
			//strExecLogFileFullPath += _T("TBUpdateLog\\");
			//::CreateDirectory(m_strExecLogFileFullPath, NULL);
			strExecLogFileDir = strExecLogFileFullPath;
			strExecLogFileFullPath += _T("ThinBridgeInstall.log");
			::CopyFile(strExecLogFileFullPath + ".bak4", strExecLogFileFullPath + ".bak5", FALSE);
			::CopyFile(strExecLogFileFullPath + ".bak3", strExecLogFileFullPath + ".bak4", FALSE);
			::CopyFile(strExecLogFileFullPath + ".bak2", strExecLogFileFullPath + ".bak3", FALSE);
			::CopyFile(strExecLogFileFullPath + ".bak1", strExecLogFileFullPath + ".bak2", FALSE);
			::CopyFile(strExecLogFileFullPath, strExecLogFileFullPath + ".bak1", FALSE);
			SetLastError(NO_ERROR);

			CString strLog;
			strLog = dlg.ExecChk();
			CStdioFile out;
			if (out.Open(strExecLogFileFullPath, CFile::modeWrite | CFile::shareDenyNone | CFile::modeCreate))
			{
				TRY
				{
					strLog.Replace(_T("\r\n"), _T("\n"));
					out.WriteString(strLog);
					out.Close();
				}
				CATCH(CFileException, eP) {}
				END_CATCH
			}
			return FALSE;
		}
	}
	else
	{
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: �_�C�A���O�� <OK> �ŏ����ꂽ���̃R�[�h��
			//  �L�q���Ă��������B
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: �_�C�A���O�� <�L�����Z��> �ŏ����ꂽ���̃R�[�h��
			//  �L�q���Ă��������B
		}
	}
	// ��ō쐬���ꂽ�V�F�� �}�l�[�W���[���폜���܂��B
	//if (pShellManager != NULL)
	//{
	//	delete pShellManager;
	//}

	// �_�C�A���O�͕����܂����B�A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n���Ȃ���
	//  �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
	return FALSE;
}


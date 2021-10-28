
// ThinBridgeRuleUpdaterDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "ThinBridgeRuleUpdater.h"
#include "ThinBridgeRuleUpdaterDlg.h"
#include "afxdialogex.h"
#include "locale.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CThinBridgeRuleUpdaterDlg �_�C�A���O
CThinBridgeRuleUpdaterDlg::CThinBridgeRuleUpdaterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CThinBridgeRuleUpdaterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CThinBridgeRuleUpdaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CThinBridgeRuleUpdaterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CThinBridgeRuleUpdaterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CThinBridgeRuleUpdaterDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CThinBridgeRuleUpdaterDlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_WRITEINI, &CThinBridgeRuleUpdaterDlg::OnBnClickedButtonWriteini)
	ON_BN_CLICKED(IDC_BUTTON_WRITEINI_NOW, &CThinBridgeRuleUpdaterDlg::OnBnClickedButtonWriteiniNow)
	ON_BN_CLICKED(IDC_BUTTON1, &CThinBridgeRuleUpdaterDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CThinBridgeRuleUpdaterDlg ���b�Z�[�W �n���h���[

BOOL CThinBridgeRuleUpdaterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	SetDlgItemText(IDC_EDIT1,theApp.SettingConf.m_strConfigServerURL1);

	//GPO�Őݒ肳��Ă���ꍇ�́A�\���ƃe�X�g���s�̂݁B
	if (theApp.SettingConf.IsGPO())
	{
		GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_WRITEINI)->EnableWindow(FALSE);
		SetDlgItemText(IDC_STATIC_MSG,_T("���Ǘ��҂ɂ��ݒ肳��Ă��邽�ߕύX�s��"));

	}

	return FALSE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

void CThinBridgeRuleUpdaterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CThinBridgeRuleUpdaterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CThinBridgeRuleUpdaterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CThinBridgeRuleUpdaterDlg::OnBnClickedButtonTest()
{
	CConfData SettingConfData;

	SettingConfData.Copy(&theApp.SettingConf);
	CString strCommand1;
	GetDlgItemText(IDC_EDIT1, strCommand1);

	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	SettingConfData.m_strConfigServerURL1 = strCommand1;

	CWaitCursor cur;
	CString strOutPutDataString;
	strOutPutDataString = theApp.WriteThinBridgeBHO(&SettingConfData,FALSE,TRUE);
	CDlgMsgBox MsgBox;
	MsgBox.m_strMsg = strOutPutDataString;
	MsgBox.m_strTitle = _T("�ڑ����擾�f�[�^�e�X�g����");
	MsgBox.DoModal();
	return;
}

void CThinBridgeRuleUpdaterDlg::OnBnClickedOk()
{
	int iRet=0;
	iRet = ::MessageBox(this->m_hWnd,_T("�ݒ�݂̂�ۑ����܂����H"),theApp.m_strExeNameNoExt,MB_ICONQUESTION | MB_OKCANCEL);
	if(iRet == IDCANCEL)
		return;

	CString strCommand1;

	GetDlgItemText(IDC_EDIT1,strCommand1);

	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	theApp.SettingConf.m_strConfigServerURL1 =strCommand1;

	if(!theApp.SettingConf.WriteSetting(theApp.m_strSetting_FileFullPath))
	{
		CString strSaveMsg;
		strSaveMsg.Format(_T("�ݒ�t�@�C���̕ۑ��Ɏ��s���܂����B\n\n�ʂ̃v���O�������t�@�C�����J���Ă��邩�A�����������s�����Ă��܂��B����������ł��܂���B�t�@�C������Ă���Ď��s���Ă��������B\n\n%s"),theApp.m_strSetting_FileFullPath);
		::MessageBox(this->m_hWnd,strSaveMsg,theApp.m_strExeNameNoExt,MB_OK|MB_ICONERROR );
	}
}

void CThinBridgeRuleUpdaterDlg::OnBnClickedButtonWriteini()
{
	int iRet = 0;
	iRet = ::MessageBox(this->m_hWnd, _T("ThinBridgeBHO.ini�̍X�V�Ɛݒ�ۑ����s���܂����H"), theApp.m_strExeNameNoExt, MB_ICONQUESTION | MB_OKCANCEL);
	if (iRet == IDCANCEL)
		return;
	CWaitCursor cur;

	CString strCommand1;

	GetDlgItemText(IDC_EDIT1, strCommand1);

	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	theApp.SettingConf.m_strConfigServerURL1 = strCommand1;

	if (!theApp.SettingConf.WriteSetting(theApp.m_strSetting_FileFullPath))
	{
		CString strSaveMsg;
		strSaveMsg.Format(_T("�ݒ�t�@�C���̕ۑ��Ɏ��s���܂����B\n\n�ʂ̃v���O�������t�@�C�����J���Ă��邩�A�����������s�����Ă��܂��B����������ł��܂���B�t�@�C������Ă���Ď��s���Ă��������B\n\n%s"), theApp.m_strSetting_FileFullPath);
		::MessageBox(this->m_hWnd, strSaveMsg, theApp.m_strExeNameNoExt, MB_OK | MB_ICONERROR);
		return;
	}

	CString strOutPutDataString;
	strOutPutDataString= theApp.WriteThinBridgeBHO(&theApp.SettingConf,TRUE,TRUE);
	if (strOutPutDataString.IsEmpty())
	{
		CString strSaveMsg;
		strSaveMsg.Format(_T("ThinBridgeBHO.ini�t�@�C���̕ۑ��Ɏ��s���܂����B\n\n�ʂ̃v���O�������t�@�C�����J���Ă��邩�A�����������s�����Ă��܂��B����������ł��܂���B�t�@�C������Ă���Ď��s���Ă��������B\n\n%s"), theApp.m_strThinBridgeBHOFileFullPath);
		::MessageBox(this->m_hWnd, strSaveMsg, theApp.m_strExeNameNoExt, MB_OK | MB_ICONERROR);
		return;
	}
	else
	{
		CDlgMsgBox MsgBox;
		CString strSaveMsg;
		if (strOutPutDataString.Find(_T("�y������e �ύX�Ȃ��z")) == 0)
		{
			CString strOutData;
			for (int j = 0; j < theApp.m_strArrayBHODataOrg.GetSize(); j++)
			{
				strOutData += theApp.m_strArrayBHODataOrg.GetAt(j);
				strOutData += _T("\r\n");
			}
			strSaveMsg.Format(_T("%s\r\n%s"), strOutPutDataString, strOutData);
			MsgBox.m_strMsg = strSaveMsg;
			MsgBox.m_strTitle = _T("�ݒ�t�@�C���ۑ� ������e");
		}
		else if (strOutPutDataString.Find(_T("�G���[�F")) == 0)
		{
			strSaveMsg = strOutPutDataString;
			MsgBox.m_strMsg = strSaveMsg;
			MsgBox.m_strTitle = _T("�ݒ�t�@�C���ۑ� �G���[");
		}
		else
		{
			strSaveMsg = strOutPutDataString;
			MsgBox.m_strMsg = strSaveMsg;
			MsgBox.m_strTitle = _T("�ݒ�t�@�C���ۑ� ����");
		}
		CStdioFile out;
		CTime time = CTime::GetCurrentTime();
		if (out.Open(theApp.m_strExecLogFileFullPath, CFile::modeWrite | CFile::shareDenyNone | CFile::modeCreate | CFile::modeNoTruncate))
		{
			TRY
			{
				out.SeekToEnd();
				out.WriteString(_T("\n�蓮�X�V//////////////////////////////////////////////////////////////////////////////////////////////////\n"));
				out.WriteString(time.Format(_T("%Y-%m-%d %H:%M:%S")));
				out.WriteString(_T("\n�������ʁF"));
				out.WriteString(theApp.GetErrorMsg());
				out.WriteString(_T("\n"));
				strSaveMsg.Replace(_T("\r\n"), _T("\n"));
				out.WriteString(strSaveMsg);
				out.Close();
			}
				CATCH(CFileException, eP) {}
			END_CATCH
		}
		MsgBox.DoModal();
	}
}

void CThinBridgeRuleUpdaterDlg::OnBnClickedButtonWriteiniNow()
{
	int iRet = 0;
	iRet = ::MessageBox(this->m_hWnd, _T("ThinBridgeBHO.ini�̍X�V���s���܂����H"), theApp.m_strExeNameNoExt, MB_ICONQUESTION | MB_OKCANCEL);
	if (iRet == IDCANCEL)
		return;
	CWaitCursor cur;

	CString strCommand1;

	GetDlgItemText(IDC_EDIT1, strCommand1);

	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	theApp.SettingConf.m_strConfigServerURL1 = strCommand1;

	CString strOutPutDataString;
	strOutPutDataString = theApp.WriteThinBridgeBHO(&theApp.SettingConf, TRUE, TRUE);
	if (strOutPutDataString.IsEmpty())
	{
		CString strSaveMsg;
		strSaveMsg.Format(_T("ThinBridgeBHO.ini�t�@�C���̕ۑ��Ɏ��s���܂����B\n\n�ʂ̃v���O�������t�@�C�����J���Ă��邩�A�����������s�����Ă��܂��B����������ł��܂���B�t�@�C������Ă���Ď��s���Ă��������B\n\n%s"), theApp.m_strThinBridgeBHOFileFullPath);
		::MessageBox(this->m_hWnd, strSaveMsg, theApp.m_strExeNameNoExt, MB_OK | MB_ICONERROR);
		return;
	}
	else
	{
		CDlgMsgBox MsgBox;
		CString strSaveMsg;
		if (strOutPutDataString.Find(_T("�y������e �ύX�Ȃ��z")) == 0)
		{
			CString strOutData;
			for (int j = 0; j < theApp.m_strArrayBHODataOrg.GetSize(); j++)
			{
				strOutData += theApp.m_strArrayBHODataOrg.GetAt(j);
				strOutData += _T("\r\n");
			}
			strSaveMsg.Format(_T("%s\r\n%s"), strOutPutDataString, strOutData);
			MsgBox.m_strMsg = strSaveMsg;
			MsgBox.m_strTitle = _T("�ݒ�t�@�C���ۑ� ������e");
		}
		else if (strOutPutDataString.Find(_T("�G���[�F")) == 0)
		{
			strSaveMsg= strOutPutDataString;
			MsgBox.m_strMsg = strSaveMsg;
			MsgBox.m_strTitle = _T("�ݒ�t�@�C���ۑ� �G���[");
		}
		else
		{
			strSaveMsg = strOutPutDataString;
			MsgBox.m_strMsg = strSaveMsg;
			MsgBox.m_strTitle = _T("�ݒ�t�@�C���ۑ� ����");
		}
		CStdioFile out;
		CTime time = CTime::GetCurrentTime();
		if (out.Open(theApp.m_strExecLogFileFullPath, CFile::modeWrite | CFile::shareDenyNone | CFile::modeCreate | CFile::modeNoTruncate))
		{
			TRY
			{
				out.SeekToEnd();
				out.WriteString(_T("\n�蓮�X�V//////////////////////////////////////////////////////////////////////////////////////////////////\n"));
				out.WriteString(time.Format(_T("%Y-%m-%d %H:%M:%S")));
				out.WriteString(_T("\n�������ʁF"));
				out.WriteString(theApp.GetErrorMsg());
				out.WriteString(_T("\n"));
				strSaveMsg.Replace(_T("\r\n"), _T("\n"));
				out.WriteString(strSaveMsg);
				out.Close();
			}
				CATCH(CFileException, eP) {}
			END_CATCH
		}
		MsgBox.DoModal();
	}
}

void CThinBridgeRuleUpdaterDlg::OnBnClickedButton1()
{
	CDlgSrv Dlg;
	Dlg.m_strConfigServerURL2 = theApp.SettingConf.m_strConfigServerURL2;
	Dlg.m_strConfigServerURL3 = theApp.SettingConf.m_strConfigServerURL3;
	Dlg.m_strConfigServerURL4 = theApp.SettingConf.m_strConfigServerURL4;
	Dlg.m_strConfigServerURL5 = theApp.SettingConf.m_strConfigServerURL5;
	Dlg.m_strConfigServerURL6 = theApp.SettingConf.m_strConfigServerURL6;

	Dlg.m_iInterval = theApp.SettingConf.m_iInterval;
	Dlg.m_iServerConnectionRule = theApp.SettingConf.m_iServerConnectionRule;

	if (IDOK == Dlg.DoModal())
	{
		theApp.SettingConf.m_strConfigServerURL2 = Dlg.m_strConfigServerURL2;
		theApp.SettingConf.m_strConfigServerURL3 = Dlg.m_strConfigServerURL3;
		theApp.SettingConf.m_strConfigServerURL4 = Dlg.m_strConfigServerURL4;
		theApp.SettingConf.m_strConfigServerURL5 = Dlg.m_strConfigServerURL5;
		theApp.SettingConf.m_strConfigServerURL6 = Dlg.m_strConfigServerURL6;

		theApp.SettingConf.m_iInterval= Dlg.m_iInterval;
		theApp.SettingConf.m_iServerConnectionRule = Dlg.m_iServerConnectionRule;
	}
}


void CThinBridgeRuleUpdaterDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

// CDlgMsgBox �_�C�A���O
IMPLEMENT_DYNAMIC(CDlgMsgBox, CDialogEx)

CDlgMsgBox::CDlgMsgBox(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgMsgBox::IDD, pParent)
{

}

CDlgMsgBox::~CDlgMsgBox()
{
}

void CDlgMsgBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
}


BEGIN_MESSAGE_MAP(CDlgMsgBox, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgMsgBox::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgMsgBox::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgMsgBox ���b�Z�[�W �n���h���[
void CDlgMsgBox::OnBnClickedOk()
{
	CDialogEx::OnOK();
}


void CDlgMsgBox::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


BOOL CDlgMsgBox::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	if (!m_strTitle.IsEmpty())
	{
		this->SetWindowText(m_strTitle);
	}
	m_Edit.LimitText(1024 * 1024 * 5);
	m_Edit.SetWindowText(m_strMsg);
	m_Edit.SetFocus();
	int SelPosS = 0;
	int SelPosE = 0;
	m_Edit.PostMessage(EM_SETSEL, (WPARAM)SelPosS, (LPARAM)SelPosE);
	//SendMessage(�iHWND)IDC_EDIT, EM_LIMITTEXT, 0, (LPARAM)1024*1024*5);
	return FALSE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}


BOOL CDlgMsgBox::PreTranslateMessage(MSG* pMsg)
{
	ASSERT(pMsg != NULL);
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);
	CString strLine;

	if (pMsg->hwnd == m_Edit.m_hWnd)
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			if (pMsg->wParam == 'A')
			{
				if (::GetKeyState(VK_CONTROL) < 0)
				{
					m_Edit.PostMessage(EM_SETSEL, (WPARAM)0, (LPARAM)-1);
					return TRUE;
				}
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}




// CDlgSrv �_�C�A���O

IMPLEMENT_DYNAMIC(CDlgSrv, CDialogEx)

CDlgSrv::CDlgSrv(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CDlgSrv::~CDlgSrv()
{
}

void CDlgSrv::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSrv, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgSrv::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSrv ���b�Z�[�W �n���h���[


BOOL CDlgSrv::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetDlgItemText(IDC_EDIT2, m_strConfigServerURL2);
	SetDlgItemText(IDC_EDIT3, m_strConfigServerURL3);
	SetDlgItemText(IDC_EDIT4, m_strConfigServerURL4);
	SetDlgItemText(IDC_EDIT5, m_strConfigServerURL5);
	SetDlgItemText(IDC_EDIT6, m_strConfigServerURL6);

	if(m_iServerConnectionRule==0)
	{
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(1);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(1);
	}
	SetDlgItemInt(IDC_EDIT_INT, m_iInterval);

	//GPO�Őݒ肳��Ă���ꍇ�́A�\���̂݁B
	if (theApp.SettingConf.IsGPO())
	{
		GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT3)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT4)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT5)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT6)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_INT)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
	return FALSE;
}


void CDlgSrv::OnOK()
{
	//GPO�Őݒ肳��Ă���ꍇ�́A�\���̂݁B
	if(!theApp.SettingConf.IsGPO())
	{
		GetDlgItemText(IDC_EDIT2, m_strConfigServerURL2);
		GetDlgItemText(IDC_EDIT3, m_strConfigServerURL3);
		GetDlgItemText(IDC_EDIT4, m_strConfigServerURL4);
		GetDlgItemText(IDC_EDIT5, m_strConfigServerURL5);
		GetDlgItemText(IDC_EDIT6, m_strConfigServerURL6);
		if (((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck() == 1)
			m_iServerConnectionRule=0;
		else if (((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck() == 1)
			m_iServerConnectionRule = 1;
		else
			m_iServerConnectionRule = 0;

		m_iInterval=GetDlgItemInt(IDC_EDIT_INT);
		if(m_iInterval<0|| m_iInterval>60)
			m_iInterval=0;
		
	}
	CDialogEx::OnOK();
}


void CDlgSrv::OnBnClickedOk()
{
	OnOK();
}

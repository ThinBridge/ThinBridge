
// TBo365URLSyncDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "TBo365URLSync.h"
#include "TBo365URLSyncDlg.h"
#include "O365EndpointHelper.h"
#include "afxdialogex.h"
#include "locale.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTBo365URLSyncDlg �_�C�A���O




CTBo365URLSyncDlg::CTBo365URLSyncDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTBo365URLSyncDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTBo365URLSyncDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_LIST2, m_List2);

}

BEGIN_MESSAGE_MAP(CTBo365URLSyncDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CTBo365URLSyncDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTBo365URLSyncDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CTBo365URLSyncDlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_INS, &CTBo365URLSyncDlg::OnBnClickedButtonIns)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CTBo365URLSyncDlg::OnBnClickedButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CTBo365URLSyncDlg::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_EDITALL, &CTBo365URLSyncDlg::OnBnClickedButtonEditall)
	ON_BN_CLICKED(IDC_BUTTON_UP, &CTBo365URLSyncDlg::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &CTBo365URLSyncDlg::OnBnClickedButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_INS2, &CTBo365URLSyncDlg::OnBnClickedButtonIns2)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE2, &CTBo365URLSyncDlg::OnBnClickedButtonUpdate2)
	ON_BN_CLICKED(IDC_BUTTON_DEL2, &CTBo365URLSyncDlg::OnBnClickedButtonDel2)
	ON_BN_CLICKED(IDC_BUTTON_EDITALL2, &CTBo365URLSyncDlg::OnBnClickedButtonEditall2)
	ON_BN_CLICKED(IDC_BUTTON_UP2, &CTBo365URLSyncDlg::OnBnClickedButtonUp2)
	ON_BN_CLICKED(IDC_BUTTON_DOWN2, &CTBo365URLSyncDlg::OnBnClickedButtonDown2)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CTBo365URLSyncDlg::OnDblclkList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, &CTBo365URLSyncDlg::OnDblclkList2)


	ON_BN_CLICKED(IDC_BUTTON_WRITEINI, &CTBo365URLSyncDlg::OnBnClickedButtonWriteini)
END_MESSAGE_MAP()


// CTBo365URLSyncDlg ���b�Z�[�W �n���h���[

BOOL CTBo365URLSyncDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	SetDlgItemText(IDC_EDIT1,theApp.SettingConf.m_strO365URL);

	SetDlgItemText(IDC_EDIT_AREA, theApp.SettingConf.m_strFilter_ServiceArea);
	SetDlgItemText(IDC_EDIT_CATEGORY, theApp.SettingConf.m_strFilter_Category);
	SetDlgItemText(IDC_EDIT_REQUIRED, theApp.SettingConf.m_strFilter_Required);
	SetDlgItemText(IDC_EDIT_TCPPORT, theApp.SettingConf.m_strFilter_TcpPort);

	
	m_List.InsertColumn(URL, _T("�^�[�Q�b�gURL ���[��"), LVCFMT_LEFT, 800);
	ListView_SetExtendedListViewStyle(m_List.m_hWnd, LVS_EX_FULLROWSELECT);
	m_List2.InsertColumn(URL, _T("���OURL ���[��"), LVCFMT_LEFT, 800);
	ListView_SetExtendedListViewStyle(m_List2.m_hWnd, LVS_EX_FULLROWSELECT);

	int iMaxCnt = 0;
	CString strLineData;

	//�Ώ�List
	iMaxCnt = theApp.SettingConf.m_arr_URL.GetCount();
	for (int i = 0; i < iMaxCnt; i++)
	{
		strLineData = theApp.SettingConf.m_arr_URL.GetAt(i);
		int index = this->m_List.GetItemCount();
		int iItem = this->m_List.InsertItem(index, _T(""));
		this->m_List.SetItemText(iItem, URL, strLineData);
	}

	//���O�Ώ�List
	iMaxCnt = theApp.SettingConf.m_arr_URL_EX.GetCount();
	for (int i = 0; i < iMaxCnt; i++)
	{
		strLineData = theApp.SettingConf.m_arr_URL_EX.GetAt(i);
		int index = this->m_List2.GetItemCount();
		int iItem = this->m_List2.InsertItem(index, _T(""));
		this->m_List2.SetItemText(iItem, URL, strLineData);
	}


	return FALSE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

void CTBo365URLSyncDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CTBo365URLSyncDlg::OnPaint()
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
HCURSOR CTBo365URLSyncDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTBo365URLSyncDlg::OnBnClickedButtonTest()
{
	CConfData SettingConfData;
	CString strCommand1;
	GetDlgItemText(IDC_EDIT1, strCommand1);

	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	SettingConfData.m_strO365URL = strCommand1;

	GetDlgItemText(IDC_EDIT_AREA, strCommand1);
	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	SettingConfData.m_strFilter_ServiceArea = strCommand1;

	GetDlgItemText(IDC_EDIT_CATEGORY, strCommand1);
	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	SettingConfData.m_strFilter_Category = strCommand1;

	GetDlgItemText(IDC_EDIT_REQUIRED, strCommand1);
	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	SettingConfData.m_strFilter_Required = strCommand1;

	GetDlgItemText(IDC_EDIT_TCPPORT, strCommand1);
	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	SettingConfData.m_strFilter_TcpPort = strCommand1;
	CWaitCursor cur;
	CStringArray strURLs;
	CString strURL;
	int iSelCount = -1;
	while ((iSelCount = m_List.GetNextItem(iSelCount, LVNI_ALL)) != -1)
	{
		strURL.Empty();
		strURL = m_List.GetItemText(iSelCount, URL);
		strURL.TrimLeft();
		strURL.TrimRight();
		if (!strURL.IsEmpty())
		{
			strURLs.Add(strURL);
		}
	}
	SettingConfData.m_arr_URL.RemoveAll();
	SettingConfData.m_arr_URL.Copy(strURLs);
	strURLs.RemoveAll();
	//���O�Ώ�List
	iSelCount = -1;
	while ((iSelCount = m_List2.GetNextItem(iSelCount, LVNI_ALL)) != -1)
	{
		strURL.Empty();
		strURL = m_List2.GetItemText(iSelCount, URL);
		strURL.TrimLeft();
		strURL.TrimRight();
		if (!strURL.IsEmpty())
		{
			strURLs.Add(strURL);
		}
	}
	SettingConfData.m_arr_URL_EX.RemoveAll();
	SettingConfData.m_arr_URL_EX.Copy(strURLs);
	strURLs.RemoveAll();
	if (!PathFileExists(theApp.m_strThinBridgeBHOFileFullPath))
	{
		CString strSaveMsg;
		strSaveMsg.Format(_T("ThinBridgeBHO.ini�t�@�C����������܂���B\n%s"), theApp.m_strThinBridgeBHOFileFullPath);
		::MessageBox(this->m_hWnd, strSaveMsg, theApp.m_strExeNameNoExt, MB_OK | MB_ICONERROR);
		return;
	}

	CString strOutPutDataString;
	strOutPutDataString = theApp.WriteThinBridgeBHO(&SettingConfData,FALSE,TRUE);
	CDlgMsgBox MsgBox;
	MsgBox.m_strMsg = strOutPutDataString;
	MsgBox.m_strTitle = _T("�ڑ����擾�f�[�^�e�X�g����");
	MsgBox.DoModal();
	return;
}

void CTBo365URLSyncDlg::OnBnClickedOk()
{
	int iRet=0;
	iRet = ::MessageBox(this->m_hWnd,_T("�ݒ�݂̂�ۑ����܂����H"),theApp.m_strExeNameNoExt,MB_ICONQUESTION | MB_OKCANCEL);
	if(iRet == IDCANCEL)
		return;

	CString strCommand1;

	GetDlgItemText(IDC_EDIT1,strCommand1);

	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	theApp.SettingConf.m_strO365URL =strCommand1;

	GetDlgItemText(IDC_EDIT_AREA, strCommand1);
	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	theApp.SettingConf.m_strFilter_ServiceArea= strCommand1;

	GetDlgItemText(IDC_EDIT_CATEGORY, strCommand1);
	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	theApp.SettingConf.m_strFilter_Category=strCommand1;

	GetDlgItemText(IDC_EDIT_REQUIRED, strCommand1);
	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	theApp.SettingConf.m_strFilter_Required=strCommand1;

	GetDlgItemText(IDC_EDIT_TCPPORT,strCommand1);
	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	theApp.SettingConf.m_strFilter_TcpPort=strCommand1;

	CStringArray strURLs;
	CString strURL;
	int iSelCount=-1;
	while ((iSelCount = m_List.GetNextItem(iSelCount, LVNI_ALL)) != -1)
	{
		strURL.Empty();
		strURL = m_List.GetItemText(iSelCount, URL);
		strURL.TrimLeft();
		strURL.TrimRight();
		if (!strURL.IsEmpty())
		{
			strURLs.Add(strURL);
		}
	}
	theApp.SettingConf.m_arr_URL.RemoveAll();
	theApp.SettingConf.m_arr_URL.Copy(strURLs);
	strURLs.RemoveAll();
	//���O�Ώ�List
	iSelCount = -1;
	while ((iSelCount = m_List2.GetNextItem(iSelCount, LVNI_ALL)) != -1)
	{
		strURL.Empty();
		strURL = m_List2.GetItemText(iSelCount, URL);
		strURL.TrimLeft();
		strURL.TrimRight();
		if (!strURL.IsEmpty())
		{
			strURLs.Add(strURL);
		}
	}
	theApp.SettingConf.m_arr_URL_EX.RemoveAll();
	theApp.SettingConf.m_arr_URL_EX.Copy(strURLs);
	strURLs.RemoveAll();


	if(!theApp.SettingConf.WriteSetting(theApp.m_strSetting_FileFullPath))
	{
		CString strSaveMsg;
		strSaveMsg.Format(_T("�ݒ�t�@�C���̕ۑ��Ɏ��s���܂����B\n\n�ʂ̃v���O�������t�@�C�����J���Ă��邩�A�����������s�����Ă��܂��B����������ł��܂���B�t�@�C������Ă���Ď��s���Ă��������B\n\n%s"),theApp.m_strSetting_FileFullPath);
		::MessageBox(this->m_hWnd,strSaveMsg,theApp.m_strExeNameNoExt,MB_OK|MB_ICONERROR );
	}
	
//	CDialogEx::OnOK();
}

void CTBo365URLSyncDlg::OnBnClickedButtonWriteini()
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
	theApp.SettingConf.m_strO365URL = strCommand1;

	GetDlgItemText(IDC_EDIT_AREA, strCommand1);
	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	theApp.SettingConf.m_strFilter_ServiceArea = strCommand1;

	GetDlgItemText(IDC_EDIT_CATEGORY, strCommand1);
	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	theApp.SettingConf.m_strFilter_Category = strCommand1;

	GetDlgItemText(IDC_EDIT_REQUIRED, strCommand1);
	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	theApp.SettingConf.m_strFilter_Required = strCommand1;

	GetDlgItemText(IDC_EDIT_TCPPORT, strCommand1);
	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	theApp.SettingConf.m_strFilter_TcpPort = strCommand1;

	CStringArray strURLs;
	CString strURL;
	int iSelCount = -1;
	while ((iSelCount = m_List.GetNextItem(iSelCount, LVNI_ALL)) != -1)
	{
		strURL.Empty();
		strURL = m_List.GetItemText(iSelCount, URL);
		strURL.TrimLeft();
		strURL.TrimRight();
		if (!strURL.IsEmpty())
		{
			strURLs.Add(strURL);
		}
	}
	theApp.SettingConf.m_arr_URL.RemoveAll();
	theApp.SettingConf.m_arr_URL.Copy(strURLs);
	strURLs.RemoveAll();
	//���O�Ώ�List
	iSelCount = -1;
	while ((iSelCount = m_List2.GetNextItem(iSelCount, LVNI_ALL)) != -1)
	{
		strURL.Empty();
		strURL = m_List2.GetItemText(iSelCount, URL);
		strURL.TrimLeft();
		strURL.TrimRight();
		if (!strURL.IsEmpty())
		{
			strURLs.Add(strURL);
		}
	}
	theApp.SettingConf.m_arr_URL_EX.RemoveAll();
	theApp.SettingConf.m_arr_URL_EX.Copy(strURLs);
	strURLs.RemoveAll();
	if (!theApp.SettingConf.WriteSetting(theApp.m_strSetting_FileFullPath))
	{
		CString strSaveMsg;
		strSaveMsg.Format(_T("�ݒ�t�@�C���̕ۑ��Ɏ��s���܂����B\n\n�ʂ̃v���O�������t�@�C�����J���Ă��邩�A�����������s�����Ă��܂��B����������ł��܂���B�t�@�C������Ă���Ď��s���Ă��������B\n\n%s"), theApp.m_strSetting_FileFullPath);
		::MessageBox(this->m_hWnd, strSaveMsg, theApp.m_strExeNameNoExt, MB_OK | MB_ICONERROR);
		return;
	}
	if(!PathFileExists(theApp.m_strThinBridgeBHOFileFullPath))
	{
		CString strSaveMsg;
		strSaveMsg.Format(_T("ThinBridgeBHO.ini�t�@�C����������܂���B\n%s"), theApp.m_strThinBridgeBHOFileFullPath);
		::MessageBox(this->m_hWnd, strSaveMsg, theApp.m_strExeNameNoExt, MB_OK | MB_ICONERROR);
		return;
	}

	CString strOutPutDataString;
	strOutPutDataString= theApp.WriteThinBridgeBHO(&theApp.SettingConf,TRUE,FALSE);
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
		if (strOutPutDataString == _T("SAME_DATA"))
		{
			CString strOutData;
			for (int j = 0; j < theApp.m_strArrayBHODataOrg.GetSize(); j++)
			{
				strOutData += theApp.m_strArrayBHODataOrg.GetAt(j);
				strOutData+=_T("\r\n");
			}

			strSaveMsg.Format(_T("�y������e �ύX�Ȃ��z\r\nOffice365�̍X�V���e�ɍ���������܂���ł����B\r\n%s\r\n%s"), theApp.m_strThinBridgeBHOFileFullPath, strOutData);
			MsgBox.m_strMsg = strSaveMsg;
			MsgBox.m_strTitle = _T("�ݒ�t�@�C���ۑ� ������e");
		}
		else if (strOutPutDataString.Find(_T("�G���[�F"))==0)
		{
			strSaveMsg.Format(_T("�y�G���[�����z\r\nOffice365�̍X�V�ŃG���[���������Ă��܂��B\r\n%s\r\n%s"), theApp.m_strThinBridgeBHOFileFullPath, strOutPutDataString);
			MsgBox.m_strMsg = strSaveMsg;
			MsgBox.m_strTitle = _T("�ݒ�t�@�C���ۑ� �G���[");
		}
		else
		{
			strSaveMsg.Format(_T("�y�����z\r\n�ݒ�t�@�C���̕ۑ��ɐ������܂����B\r\n\r\n%s\r\n%s"), theApp.m_strThinBridgeBHOFileFullPath, strOutPutDataString);
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
				out.WriteString(_T("\n�蓮�ݒ�//////////////////////////////////////////////////////////////////////////////////////////////////\n"));
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

int CTBo365URLSyncDlg::DuplicateChk(CListCtrl* ptrList, LPCTSTR sURL)
{
	int iRet = -1; //�d���Ȃ��́A-1��Ԃ��B
	CString strURL;
	int iSelCount = -1;
	while ((iSelCount = ptrList->GetNextItem(iSelCount, LVNI_ALL)) != -1)
	{
		strURL.Empty();
		strURL = ptrList->GetItemText(iSelCount, URL);
		if (strURL == sURL)
		{
			iRet = iSelCount;
			break;
		}
	}
	return iRet;
}

void CTBo365URLSyncDlg::UpdateListCounter(CListCtrl* ptrList)
{
	CString str;
	str.Format(_T("%d��"), ptrList->GetItemCount());
	if (ptrList == &this->m_List)
	{
		SetDlgItemText(IDC_STATIC_CNT, str);
	}
	else
	{
		SetDlgItemText(IDC_STATIC_CNT2, str);
	}
}

void CTBo365URLSyncDlg::PopIns(CListCtrl* ptrList)
{
	CDlgRDEdit Dlg(this);
	if (Dlg.DoModal() == IDOK)
	{
		int iRet = DuplicateChk(ptrList, Dlg.m_strURL);
		//�d���Ȃ��B
		if (iRet == -1)
		{
			int index = ptrList->GetItemCount();
			int iItem = ptrList->InsertItem(index, _T(""));
			ptrList->SetItemText(iItem, URL, Dlg.m_strURL);
			ptrList->SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);
		}
		else
		{
			ptrList->SetFocus();
			ptrList->SetItemState(iRet, LVIS_SELECTED, LVIS_SELECTED);
			CString strErrMsg;
			strErrMsg.Format(_T("����URL�́A���ɓo�^����Ă��܂��B\n%d�s��\n%s\n"), iRet + 1, Dlg.m_strURL);
			::MessageBox(this->m_hWnd, strErrMsg, theApp.m_strExeNameNoExt, MB_OK | MB_ICONWARNING);
		}
	}
	UpdateListCounter(ptrList);
}

void CTBo365URLSyncDlg::PopDel(CListCtrl* ptrList)
{
	int iSelCount = 0;
	iSelCount = ptrList->GetSelectedCount();
	if (iSelCount == 0)
		return;

	int nItemCount = ptrList->GetItemCount();
	while (nItemCount--)
	{
		if (ptrList->GetItemState(nItemCount, LVIS_SELECTED) == LVIS_SELECTED)
		{
			ptrList->DeleteItem(nItemCount);
			{
				int nItemNowCount = ptrList->GetItemCount();
				if (nItemNowCount == nItemCount)
					ptrList->SetItemState(nItemCount - 1, LVIS_SELECTED, LVIS_SELECTED);
				else if (nItemNowCount > 0)
					ptrList->SetItemState(nItemCount, LVIS_SELECTED, LVIS_SELECTED);
				break;
			}
		}
	}
	UpdateListCounter(ptrList);
}

void CTBo365URLSyncDlg::PopUpdate(CListCtrl* ptrList)
{
	int iSelCount = 0;
	iSelCount = ptrList->GetSelectedCount();
	if (iSelCount != 1)
		return;
	iSelCount = ptrList->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (iSelCount != -1)
	{
		CDlgRDEdit Dlg(this);
		BOOL bEnable = FALSE;
		CString strURL;
		strURL = ptrList->GetItemText(iSelCount, URL);
		Dlg.m_strURL = strURL;
		if (Dlg.DoModal() == IDOK)
		{
			int iRet = DuplicateChk(&m_List, Dlg.m_strURL);
			//�d���Ȃ��B
			if (iRet == -1 || iRet == iSelCount)
			{
				CString strTemp;
				ptrList->SetItemText(iSelCount, URL, Dlg.m_strURL);
			}
			else
			{
				ptrList->SetFocus();
				ptrList->SetItemState(iRet, LVIS_SELECTED, LVIS_SELECTED);
				CString strErrMsg;
				strErrMsg.Format(_T("����URL�́A���ɓo�^����Ă��܂��B\n%d�s��\n%s\n"), iRet + 1, Dlg.m_strURL);
				::MessageBox(this->m_hWnd, strErrMsg, theApp.m_strExeNameNoExt, MB_OK | MB_ICONWARNING);
			}
		}
	}
	UpdateListCounter(ptrList);
}

void CTBo365URLSyncDlg::PopEditAll(CListCtrl* ptrList)
{
	CString strData;

	CString strURL;
	CString strTemp;
	int iSelCount = -1;
	while ((iSelCount = ptrList->GetNextItem(iSelCount, LVNI_ALL)) != -1)
	{
		strURL.Empty();
		strTemp.Empty();
		strURL = ptrList->GetItemText(iSelCount, URL);
		strData += strURL;
		strData += _T("\r\n");
	}

	CDlgEditURLML Dlg;
	if (ptrList == &m_List)
		Dlg.m_strTitle = _T("�y�^�[�Q�b�gURL ���[���z �ꊇ�o�^�E�ҏW");
	else
		Dlg.m_strTitle = _T("�y���OURL ���[���z �ꊇ�o�^�E�ҏW");

	Dlg.m_strEditData = strData;
	if (IDOK == Dlg.DoModal())
	{
		//��U�S�������B
		ptrList->DeleteAllItems();
		int iMaxCnt = 0;
		iMaxCnt = Dlg.m_strArrayResult.GetCount();
		CString strLineData;
		for (int i = 0; i < iMaxCnt; i++)
		{
			strLineData = Dlg.m_strArrayResult.GetAt(i);
			int index = ptrList->GetItemCount();
			int iItem = ptrList->InsertItem(index, _T(""));
			ptrList->SetItemText(iItem, URL, strLineData);
		}
	}
	UpdateListCounter(ptrList);
}

void CTBo365URLSyncDlg::PopUp(CListCtrl* ptrList)
{
	int iSelCount = 0;
	int iAfterPos = 0;
	iSelCount = ptrList->GetSelectedCount();
	if (iSelCount != 1)
		return;
	iSelCount = ptrList->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (iSelCount == -1)
		return;

	CString strURL;
	strURL = ptrList->GetItemText(iSelCount, URL);
	iAfterPos = iSelCount - 1;
	if (iAfterPos < 0)
	{
		ptrList->SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		ptrList->SetFocus();
		return;
	}
	ptrList->DeleteItem(iSelCount);
	int iItem = ptrList->InsertItem(iAfterPos, _T(""));
	ptrList->SetItemText(iItem, URL, strURL);
	ptrList->SetItemState(iAfterPos, LVIS_SELECTED, LVIS_SELECTED);
	ptrList->SetFocus();
}

void CTBo365URLSyncDlg::PopDown(CListCtrl* ptrList)
{
	int iSelCount = 0;
	int iAfterPos = 0;
	iSelCount = ptrList->GetSelectedCount();
	if (iSelCount != 1)
		return;
	iSelCount = ptrList->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (iSelCount == -1)
		return;

	CString strURL;
	strURL = ptrList->GetItemText(iSelCount, URL);
	iAfterPos = iSelCount + 1;
	if (iAfterPos >= ptrList->GetItemCount())
	{
		ptrList->SetItemState(iSelCount, LVIS_SELECTED, LVIS_SELECTED);
		ptrList->SetFocus();
		return;
	}
	ptrList->DeleteItem(iSelCount);
	int iItem = ptrList->InsertItem(iAfterPos, _T(""));
	ptrList->SetItemText(iItem, URL, strURL);
	ptrList->SetItemState(iAfterPos, LVIS_SELECTED, LVIS_SELECTED);
	ptrList->SetFocus();
}

void CTBo365URLSyncDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}




void CTBo365URLSyncDlg::OnBnClickedButtonIns()
{
	PopIns(&m_List);
}


void CTBo365URLSyncDlg::OnBnClickedButtonUpdate()
{
	PopUpdate(&m_List);
}


void CTBo365URLSyncDlg::OnBnClickedButtonDel()
{
	PopDel(&m_List);
}


void CTBo365URLSyncDlg::OnBnClickedButtonEditall()
{
	PopEditAll(&m_List);
}


void CTBo365URLSyncDlg::OnBnClickedButtonUp()
{
	PopUp(&m_List);
}


void CTBo365URLSyncDlg::OnBnClickedButtonDown()
{
	PopDown(&m_List);
}


void CTBo365URLSyncDlg::OnBnClickedButtonIns2()
{
	PopIns(&m_List2);
}


void CTBo365URLSyncDlg::OnBnClickedButtonUpdate2()
{
	PopUpdate(&m_List2);
}


void CTBo365URLSyncDlg::OnBnClickedButtonDel2()
{
	PopDel(&m_List2);
}


void CTBo365URLSyncDlg::OnBnClickedButtonEditall2()
{
	PopEditAll(&m_List2);
}


void CTBo365URLSyncDlg::OnBnClickedButtonUp2()
{
	PopUp(&m_List2);
}


void CTBo365URLSyncDlg::OnBnClickedButtonDown2()
{
	PopDown(&m_List2);
}
void CTBo365URLSyncDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	PopUpdate(&m_List);
	*pResult = 0;
}
void CTBo365URLSyncDlg::OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult)
{
	PopUpdate(&m_List2);
	*pResult = 0;
}

///////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDlgRDEdit, CDialogEx)

CDlgRDEdit::CDlgRDEdit(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgRDEdit::IDD, pParent)
{
}

CDlgRDEdit::~CDlgRDEdit()
{
}

void CDlgRDEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRDEdit, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgRDEdit::OnBnClickedOk)
END_MESSAGE_MAP()

// CDlgRDEdit ���b�Z�[�W �n���h���[
void CDlgRDEdit::OnBnClickedOk()
{
	//�N�����̋����p�����[�^���m�F
	CString strURL;
	GetDlgItemText(IDC_EDIT1, strURL);

	//���`
	strURL.TrimLeft();
	strURL.TrimRight();

	//�擪��-�́A�֎~�B
	if (strURL.Find(_T("-")) == 0)
	{
		//strURL=strURL.Mid(1);
		GetDlgItem(IDC_EDIT1)->SetFocus();
		::MessageBox(this->m_hWnd, _T("�擪���n�C�t���L��'-'����n�܂�URL�͐ݒ�ł��܂���B"), theApp.m_strExeNameNoExt, MB_OK | MB_ICONINFORMATION);
		return;
	}

	//�擪��# ; �̓R�����g�Ȃ̂Ŗ�����
	else if (strURL.Find(_T("#")) == 0)
	{
		GetDlgItem(IDC_EDIT1)->SetFocus();
		::MessageBox(this->m_hWnd, _T("�擪���V���[�v�L��'#'����n�܂�URL�͐ݒ�ł��܂���B"), theApp.m_strExeNameNoExt, MB_OK | MB_ICONINFORMATION);
		return;
	}
	else if (strURL.Find(_T(";")) == 0)
	{
		GetDlgItem(IDC_EDIT1)->SetFocus();
		::MessageBox(this->m_hWnd, _T("�擪���Z�~�R�����L��';'����n�܂�URL�͐ݒ�ł��܂���B"), theApp.m_strExeNameNoExt, MB_OK | MB_ICONINFORMATION);
		return;
	}
	else
	{
		if (strURL.IsEmpty())
		{
			GetDlgItem(IDC_EDIT1)->SetFocus();
			::MessageBox(this->m_hWnd, _T("URL����͂��ĉ������B"), theApp.m_strExeNameNoExt, MB_OK | MB_ICONINFORMATION);
			return;
		}
		m_strURL = strURL;
	}
	CDialogEx::OnOK();
}

BOOL CDlgRDEdit::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetDlgItemText(IDC_EDIT1, m_strURL);
	SHAutoComplete(GetDlgItem(IDC_EDIT1)->m_hWnd, SHACF_URLALL | SHACF_AUTOSUGGEST_FORCE_ON | SHACF_AUTOAPPEND_FORCE_ON);
	CIconHelper ICoHelper;
	ICoHelper = theApp.LoadIcon(IDR_MAINFRAME);
	SetIcon(ICoHelper, TRUE);
	SetIcon(ICoHelper, FALSE);
	return FALSE;
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

///////////////////////////////////////////////////////
// CDlgEditURLML �_�C�A���O

IMPLEMENT_DYNAMIC(CDlgEditURLML, CDialogEx)

CDlgEditURLML::CDlgEditURLML(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgEditURLML::IDD, pParent)
{

}

CDlgEditURLML::~CDlgEditURLML()
{
}

void CDlgEditURLML::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
}


BEGIN_MESSAGE_MAP(CDlgEditURLML, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgEditURLML::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgEditURLML::OnBnClickedOk)
	ON_WM_ACTIVATE()
	ON_WM_NCACTIVATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDlgEditURLML ���b�Z�[�W �n���h���[
void CDlgEditURLML::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

BOOL CDlgEditURLML::bValidData()
{
	BOOL bRet = FALSE;
	CString strText;
	CString strValidData;
	m_Edit.GetWindowText(strText);

	m_strArrayResult.RemoveAll();

	CStringArray strArray;
	CMapStringToPtr strMapDupChk;
	int iCnt = 0;
	int iCntMax = 0;
	void* dwpLineIndex = { 0 };
	DWORD dwLine = 0;
	CString strTempLine;
	CString strTempLineOrg;

	CString strErrMsg;
	CString strErrMsgLine;
	int iErrCnt = 0;

	SBUtil::Split_TrimBlank(strArray, strText, _T("\r\n"));
	strText.Empty();

	//�d���Ƌ֑������`�F�b�N
	iCntMax = strArray.GetCount();
	BOOL bEnable = TRUE;

	for (iCnt = 0; iCnt < iCntMax; iCnt++)
	{
		bEnable = TRUE;
		strTempLine.Empty();
		strTempLineOrg.Empty();
		strTempLine = strArray.GetAt(iCnt);
		strTempLineOrg = strTempLine;
		strText += strTempLine;
		strText += "\r\n";

		//�擪�̃n�C�t��-�`�F�b�N
		if (strTempLine.Find(_T("-")) == 0)
		{
			strErrMsgLine.Format(_T("�֑��F%d�s�ڂ̃f�[�^[%s]�́A�擪���n�C�t���L��'-'����n�܂邽�ߐݒ�ł��܂���B\r\n"), iCnt + 1, strTempLine);
			strErrMsg += strErrMsgLine;
			iErrCnt++;
			continue;
		}
		//�擪��#�`�F�b�N
		if (strTempLine.Find(_T("#")) == 0)
		{
			//#�����Ȃ��ꍇ�́A���߁B
			if (strTempLine.GetLength() == 1)
			{
				strErrMsgLine.Format(_T("�֑��F%d�s�ڂ̃f�[�^[%s]�́A#�L���ׁ݂̂̈A�ݒ�ł��܂���B\r\n"), iCnt + 1, strTempLine);
				strErrMsg += strErrMsgLine;
				iErrCnt++;
				continue;
			}
			else
				bEnable = FALSE;
		}
		//�擪��;�`�F�b�N
		if (strTempLine.Find(_T(";")) == 0)
		{
			//#�����Ȃ��ꍇ�́A���߁B
			if (strTempLine.GetLength() == 1)
			{
				strErrMsgLine.Format(_T("�֑��F%d�s�ڂ̃f�[�^[%s]�́A;�L���ׁ݂̂̈A�ݒ�ł��܂���B\r\n"), iCnt + 1, strTempLine);
				strErrMsg += strErrMsgLine;
				iErrCnt++;
				continue;
			}
			else
				bEnable = FALSE;
		}

		//comment���폜���Ēǉ�����B
		if (!bEnable)
		{
			strTempLine = strTempLine.Mid(1);
		}

		//�d���`�F�b�N
		if (strMapDupChk.Lookup(strTempLine, dwpLineIndex))
		{
			dwLine = 0;
			dwLine = (DWORD)dwpLineIndex;
			//Lookup�o�����ꍇ�́A�d�����Ă���B
			strErrMsgLine.Format(_T("�d���F%d�s�ڂ̃f�[�^[%s]�́A%d�s�ڂɊ��ɓo�^����Ă��܂��B\r\n"), iCnt + 1, strTempLine, dwLine + 1);
			strErrMsg += strErrMsgLine;
			iErrCnt++;
		}
		//�d�������B
		else
		{
			dwLine = iCnt;
			dwpLineIndex = (void*)dwLine;
			strMapDupChk.SetAt(strTempLine, dwpLineIndex);
			m_strArrayResult.Add(strTempLine);
			strValidData += strTempLineOrg;
			strValidData += _T("\r\n");
		}
	}
	if (!strErrMsg.IsEmpty())
	{
		CString strErrMsgResult;
		strErrMsgResult.Format(_T("�y�f�[�^�G���[�z\r\n��%d�̃G���[������܂��B\r\n�X�V�Ɏ��s���܂����B�f�[�^���C�����ĉ������B\r\n=====================================================================\r\n%s====================================================================="), iErrCnt, strErrMsg);
		CDlgMsgBox MsgBox;
		MsgBox.m_strMsg = strErrMsgResult;
		if (IDOK == MsgBox.DoModal())
		{
			int iRet = 0;
			iRet = ::MessageBox(this->m_hWnd, _T("�f�[�^�G���[�������I�ɏC�����܂����H\n\n���u�͂��v��I������ƃG���[�̔������Ă���s���폜���܂��B\n���ɖ߂����Ƃ͂ł��܂���B"), theApp.m_strExeNameNoExt, MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);
			if (iRet == IDYES)
			{
				//�O�ׂ̈ɂ�����x�_�������B
				iRet = 0;
				iRet = ::MessageBox(this->m_hWnd, _T("�f�[�^�G���[�̎����C�������s���܂��B"), theApp.m_strExeNameNoExt, MB_ICONASTERISK | MB_YESNO | MB_DEFBUTTON2);
				if (iRet == IDYES)
					strText = strValidData;
			}
		}
		m_SelPosS = 0;
		m_SelPosE = 0;
		m_Edit.SetFocus();
		m_Edit.SetWindowText(strText);
		m_Edit.PostMessage(EM_SETSEL, (WPARAM)m_SelPosS, (LPARAM)m_SelPosE);
		return FALSE;
	}
	else
	{
		m_strEditData = strText;
		bRet = TRUE;
	}
	return bRet;
}

void CDlgEditURLML::OnBnClickedOk()
{
	int iRet = 0;
	iRet = ::MessageBox(this->m_hWnd, _T("�ݒ���X�V���܂����H\n���󔒍s�́A�����I�ɐ؂�l�߂��܂��B"), theApp.m_strExeNameNoExt, MB_ICONQUESTION | MB_OKCANCEL);
	if (iRet == IDCANCEL)
		return;

	//�d���Ƌ֑������`�F�b�N
	if (bValidData())
	{
		CDialogEx::OnOK();
	}
}

BOOL CDlgEditURLML::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	bFirstFlg = TRUE;
	m_SelPosS = 0;
	m_SelPosE = 0;
	m_Edit.LimitText(1024 * 1024 * 5);
	m_Edit.SetFocus();
	m_Edit.SetWindowText(m_strEditData);
	if (!m_strTitle.IsEmpty())
		SetWindowText(m_strTitle);
	return FALSE;
}
void CDlgEditURLML::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
}


BOOL CDlgEditURLML::PreTranslateMessage(MSG* pMsg)
{
	ASSERT(pMsg != NULL);
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);
	CString strLine;

	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			//OnCancel();
			return TRUE;
		}
	}
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
		if (pMsg->message == WM_MOUSEMOVE
			|| pMsg->message == WM_LBUTTONDOWN
			|| pMsg->message == WM_KEYDOWN
			|| pMsg->message == WM_KEYUP
			|| pMsg->message == WM_LBUTTONDOWN
			)
		{
			DWORD dwStart = 0, dwEnd = 0;
			DWORD dwCursorPos = 0;
			DWORD dwLineIndex = 0;
			CPoint   pt;
			LRESULT lr = { 0 };
			dwCursorPos = m_Edit.SendMessage(EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd);
			if (pMsg->message == WM_MOUSEMOVE && dwStart == dwEnd)
				return lr;
			lr = CDialogEx::PreTranslateMessage(pMsg);

			dwLineIndex = m_Edit.SendMessage(EM_LINEFROMCHAR, -1, 0);
			strLine.Format(_T("%d�s"), dwLineIndex + 1);
			SetDlgItemText(IDC_STATIC_LINE, strLine);
			return lr;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CDlgEditURLML::OnNcActivate(BOOL bActive)
{
	if (bActive)
	{
		if (bFirstFlg)
		{
			bFirstFlg = FALSE;
		}
		else
			m_Edit.PostMessage(EM_SETSEL, (WPARAM)m_SelPosS, (LPARAM)m_SelPosE);
	}
	else
	{
		m_Edit.GetSel(m_SelPosS, m_SelPosE);
	}
	return CDialogEx::OnNcActivate(bActive);
}


void CDlgEditURLML::OnPaint()
{
	CPaintDC dc(this);
}


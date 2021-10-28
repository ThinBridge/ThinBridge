// DlgURL.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "TBo365URLSync.h"
#include "DlgURL.h"
#include "afxdialogex.h"


// CDlgURL �_�C�A���O

IMPLEMENT_DYNAMIC(CDlgURL, CDialogEx)

CDlgURL::CDlgURL(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgURL::IDD, pParent)
{

}

CDlgURL::~CDlgURL()
{
}

void CDlgURL::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgURL, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgURL::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgURL::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgURL ���b�Z�[�W �n���h���[


void CDlgURL::OnBnClickedOk()
{
	GetDlgItemText(IDC_EDIT1,m_strURL);
	m_strURL.TrimLeft();
	m_strURL.TrimRight();
	CDialogEx::OnOK();
}


void CDlgURL::OnBnClickedCancel()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	CDialogEx::OnCancel();
}


BOOL CDlgURL::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SHAutoComplete(GetDlgItem(IDC_EDIT1)->m_hWnd,SHACF_URLALL|SHACF_AUTOSUGGEST_FORCE_ON|SHACF_AUTOAPPEND_FORCE_ON);
	return FALSE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

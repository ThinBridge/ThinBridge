#include "stdafx.h"
#include "ThinBridge.h"
#include "URLInputDlg.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CURLInputDlg, CDialogEx)

CURLInputDlg::CURLInputDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CURLInputDlg::IDD, pParent)
{

}

CURLInputDlg::~CURLInputDlg()
{
}

void CURLInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CURLInputDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CURLInputDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT1, &CURLInputDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


void CURLInputDlg::OnBnClickedOk()
{
	GetDlgItemText(IDC_EDIT1,m_strURL);
	CDialogEx::OnOK();
}


BOOL CURLInputDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SHAutoComplete(GetDlgItem(IDC_EDIT1)->m_hWnd,SHACF_URLALL|SHACF_AUTOSUGGEST_FORCE_ON|SHACF_AUTOAPPEND_FORCE_ON);
	SetDlgItemText(IDC_EDIT1,m_strURL);
	return FALSE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CURLInputDlg::OnEnChangeEdit1()
{
}

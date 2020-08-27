#include "stdafx.h"
#include "ThinBridge.h"
#include "DlgRDEdit.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CDlgRDEdit, CDialogEx)

CDlgRDEdit::CDlgRDEdit(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgRDEdit::IDD, pParent)
{
	m_bEnable=TRUE;
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

void CDlgRDEdit::OnBnClickedOk()
{
	//起動時の強制パラメータを確認
	CString strURL;
	GetDlgItemText(IDC_EDIT1,strURL);

	//整形
	strURL.TrimLeft();
	strURL.TrimRight();
	//strURL.Replace(_T("\""),_T(""));

	if(((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck() == 1)
		m_bEnable=TRUE;
	else
		m_bEnable=FALSE;

	//先頭の-は、禁止。
	if(strURL.Find(_T("-"))==0)
	{
		//strURL=strURL.Mid(1);
		GetDlgItem(IDC_EDIT1)->SetFocus();
		::MessageBox(this->m_hWnd,_T("先頭がハイフン記号'-'から始まるURLは設定できません。"),theApp.m_strThisAppName,MB_OK|MB_ICONINFORMATION);
		return;
	}

	//先頭の# ; はコメントなので無効化
	else if(strURL.Find(_T("#"))==0)
	{
		GetDlgItem(IDC_EDIT1)->SetFocus();
		::MessageBox(this->m_hWnd,_T("先頭がシャープ記号'#'から始まるURLは設定できません。"),theApp.m_strThisAppName,MB_OK|MB_ICONINFORMATION);
		return;
	}
	else if(strURL.Find(_T(";"))==0)
	{
		GetDlgItem(IDC_EDIT1)->SetFocus();
		::MessageBox(this->m_hWnd,_T("先頭がセミコロン記号';'から始まるURLは設定できません。"),theApp.m_strThisAppName,MB_OK|MB_ICONINFORMATION);
		return;
	}
	else
	{
		if(strURL.IsEmpty())
		{
			GetDlgItem(IDC_EDIT1)->SetFocus();
			::MessageBox(this->m_hWnd,_T("URLを入力して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONINFORMATION);
			return;
		}
		m_strURL=strURL;
	}
	CDialogEx::OnOK();
}

BOOL CDlgRDEdit::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetDlgItemText(IDC_EDIT1,m_strURL);
	((CButton*)(GetDlgItem(IDC_CHECK1))) 
						->SetCheck(m_bEnable ? 1:0);

	SHAutoComplete(GetDlgItem(IDC_EDIT1)->m_hWnd,SHACF_URLALL|SHACF_AUTOSUGGEST_FORCE_ON|SHACF_AUTOAPPEND_FORCE_ON);
	CIconHelper ICoHelper;
    ICoHelper       = theApp.LoadIcon(IDR_MAINFRAME);
	SetIcon(ICoHelper, TRUE);
	SetIcon(ICoHelper, FALSE);
	return FALSE;
}

// DlgURLEdit.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ThinBridgeChecker.h"
#include "DlgURLEdit.h"
#include "afxdialogex.h"


// CDlgURLEdit ダイアログ

IMPLEMENT_DYNAMIC(CDlgURLEdit, CDialogEx)

CDlgURLEdit::CDlgURLEdit(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgURLEdit::IDD, pParent)
{

}

CDlgURLEdit::~CDlgURLEdit()
{
}

void CDlgURLEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgURLEdit, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgURLEdit::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgURLEdit::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgURLEdit メッセージ ハンドラー


void CDlgURLEdit::OnBnClickedOk()
{
	this->GetDlgItemText(IDC_EDIT1,this->m_strURL);
	if(this->m_strURL.Find(_T("http://"))==0
	||this->m_strURL.Find(_T("https://"))==0
	|| this->m_strURL.Find(_T("about:")) == 0
	|| this->m_strURL.IsEmpty()

	)
		return CDialogEx::OnOK();
	else
	{
		AfxMessageBox(_T("URLを入力してください。\nhttp://または、https://が必要です。"),MB_OK|MB_ICONINFORMATION);
		return;
	}
	CDialogEx::OnOK();
}


void CDlgURLEdit::OnBnClickedCancel()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDialogEx::OnCancel();
}


BOOL CDlgURLEdit::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SHAutoComplete(GetDlgItem(IDC_EDIT1)->m_hWnd,SHACF_URLALL|SHACF_AUTOSUGGEST_FORCE_ON|SHACF_AUTOAPPEND_FORCE_ON);
	return FALSE;  // return TRUE unless you set the focus to a control
}

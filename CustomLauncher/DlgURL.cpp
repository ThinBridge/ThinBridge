// DlgURL.cpp : 実装ファイル
//

#include "stdafx.h"
#include "CustomLauncher.h"
#include "DlgURL.h"
#include "afxdialogex.h"


// CDlgURL ダイアログ

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


// CDlgURL メッセージ ハンドラー


void CDlgURL::OnBnClickedOk()
{
	GetDlgItemText(IDC_EDIT1,m_strURL);
	m_strURL.TrimLeft();
	m_strURL.TrimRight();
	CDialogEx::OnOK();
}


void CDlgURL::OnBnClickedCancel()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDialogEx::OnCancel();
}


BOOL CDlgURL::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SHAutoComplete(GetDlgItem(IDC_EDIT1)->m_hWnd,SHACF_URLALL|SHACF_AUTOSUGGEST_FORCE_ON|SHACF_AUTOAPPEND_FORCE_ON);
	return FALSE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

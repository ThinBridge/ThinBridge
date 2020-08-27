#include "stdafx.h"
#include "ThinBridge.h"
#include "DlgRemoteSetting.h"
#include "afxdialogex.h"


IMPLEMENT_DYNCREATE(CDlgRemoteSetting, CPropertyPage)

CDlgRemoteSetting::CDlgRemoteSetting()
	: CPropertyPage(CDlgRemoteSetting::IDD)
{

}

CDlgRemoteSetting::~CDlgRemoteSetting()
{
}

void CDlgRemoteSetting::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRemoteSetting, CPropertyPage)
END_MESSAGE_MAP()


BOOL CDlgRemoteSetting::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	SetDlgItemText(IDC_STATIC,_T("リモートブラウザーを設定します。\n\n  ・Microsoft RDP\n\n  ・VMware Horizon\n\n  ・Citrix XenApp\n\n\n※左のツリーから設定する項目を選択して下さい。"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

IMPLEMENT_DYNCREATE(CDlgRemoteSettingRD,CDlgRemoteSetting)
BOOL CDlgRemoteSettingRD::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	CString strText;
	strText = _T("リダイレクトするURLを設定します。\n\n");
	strText +=_T("  ・Microsoft RDP\n\n");
	strText +=_T("  ・VMware Horizon\n\n");
	strText +=_T("  ・Citrix XenApp\n\n");
	strText +=_T("  ・Mozilla Firefox\n\n");
	strText +=_T("  ・Google Chrome\n\n");
	strText +=_T("  ・Microsoft Edge\n\n");
	strText +=_T("  ・指定ブラウザー01-20\n\n");
	strText +=_T("\n※左のツリーから設定する項目を選択して下さい。");

	SetDlgItemText(IDC_STATIC,strText);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


IMPLEMENT_DYNCREATE(CDlgRemoteSettingGEN,CDlgRemoteSetting)
BOOL CDlgRemoteSettingGEN::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	SetDlgItemText(IDC_STATIC,_T("全般設定\n\n  ・URLリダイレクト全般設定\n\n  ・リソース設定\n\n\n※左のツリーから設定する項目を選択して下さい。"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

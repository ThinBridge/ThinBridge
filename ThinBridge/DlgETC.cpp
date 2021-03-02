#include "stdafx.h"
#include "ThinBridge.h"
#include "DlgETC.h"
#include "afxdialogex.h"


IMPLEMENT_DYNCREATE(CDlgETC, CPropertyPage)

CDlgETC::CDlgETC()
	: CPropertyPage(CDlgETC::IDD)
{
	m_pDebugDlg=NULL;
}

CDlgETC::~CDlgETC()
{
	if(m_pDebugDlg)
	{
		delete m_pDebugDlg;
		m_pDebugDlg=NULL;
	}
}

void CDlgETC::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgETC, CPropertyPage)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgETC::OnBnClickedButton1)
END_MESSAGE_MAP()


BOOL CDlgETC::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	BOOL bURLOnly = theApp.m_RedirectList.m_bURLOnly;
	((CButton*)GetDlgItem(IDC_CHK_GLOVAL_URL_ONLY))->SetCheck(bURLOnly?1:0);

	BOOL bTopURLOnly = theApp.m_RedirectList.m_bTopURLOnly;
	((CButton*)GetDlgItem(IDC_CHK_GLOVAL_TOP_PAGE_ONLY))->SetCheck(bTopURLOnly ? 1 : 0);

	BOOL bTraceLog = theApp.m_RedirectList.m_bTraceLog;
	((CButton*)GetDlgItem(IDC_CHK_GLOVAL_TRACE))->SetCheck(bTraceLog?1:0);

	BOOL bQuickRedirect = theApp.m_RedirectList.m_bQuickRedirect;
	((CButton*)GetDlgItem(IDC_CHK_GLOVAL_QUICK))->SetCheck(bQuickRedirect ? 1 : 0);


	CString strVersion;
	strVersion = theApp.GetVersionStr();
	GetDlgItem(IDC_STATIC_VER)->SetWindowText(strVersion);

	return FALSE;
}

LRESULT CDlgETC::Set_OK(WPARAM wParam, LPARAM lParam)
{
	BOOL bURLOnly = ((CButton*)GetDlgItem(IDC_CHK_GLOVAL_URL_ONLY))->GetCheck() == 1?TRUE:FALSE;
	theApp.m_RedirectListSaveData.m_bURLOnly = bURLOnly;

	BOOL bTraceLog = ((CButton*)GetDlgItem(IDC_CHK_GLOVAL_TRACE))->GetCheck() == 1?TRUE:FALSE;
	theApp.m_RedirectListSaveData.m_bTraceLog = bTraceLog;

	BOOL bQuickRedirect = ((CButton*)GetDlgItem(IDC_CHK_GLOVAL_QUICK))->GetCheck() == 1 ? TRUE : FALSE;
	theApp.m_RedirectListSaveData.m_bQuickRedirect = bQuickRedirect;

	BOOL bTopURLOnly = ((CButton*)GetDlgItem(IDC_CHK_GLOVAL_TOP_PAGE_ONLY))->GetCheck() == 1 ? TRUE : FALSE;
	theApp.m_RedirectListSaveData.m_bTopURLOnly= bTopURLOnly;

	return 0;
}


void CDlgETC::OnBnClickedButton1()
{
	BOOL bTraceLog = ((CButton*)GetDlgItem(IDC_CHK_GLOVAL_TRACE))->GetCheck() == 1?TRUE:FALSE;
	if(!bTraceLog)
	{
		::MessageBox(this->m_hWnd,_T("[TRACE Logを出力する]を有効にし、[設定保存]を行って下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
		return;
	}
	if(m_pDebugDlg)
	{
		delete m_pDebugDlg;
		m_pDebugDlg=NULL;
	}
	m_pDebugDlg = new CDlgDebugWnd;
	m_pDebugDlg->Create(IDD_DLG_DEBUG_WND,theApp.m_pMainWnd);
	m_pDebugDlg->ShowWindow(SW_SHOW);
}

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
	ON_BN_CLICKED(IDC_KEYTESTBUTTON, &CDlgETC::OnBnClickedKeytestbutton)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgETC::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_SC, &CDlgETC::OnBnClickedButtonSc)
END_MESSAGE_MAP()


void CDlgETC::OnBnClickedKeytestbutton()
{
	DWORD dSetting=0;
	CString strMsg;
	CString strSettingKey;
	if(((CButton*)GetDlgItem(IDC_CHK_SHIFT))->GetCheck()==1)
	{
		dSetting = dSetting | KEY_COMB_SHIFT;
		strSettingKey+=_T("Shift ");
	}
	if(((CButton*)GetDlgItem(IDC_CHK_CTRL))->GetCheck()==1)
	{
		dSetting = dSetting | KEY_COMB_CTRL;
		strSettingKey+=_T("Ctrl ");
	}
	if(((CButton*)GetDlgItem(IDC_CHK_ALT))->GetCheck()==1)
	{
		dSetting = dSetting | KEY_COMB_ALT;
		strSettingKey+=_T("Alt ");
	}

	if(((CButton*)GetDlgItem(IDC_CHK_LEFT))->GetCheck()==1)
	{
		dSetting = dSetting | KEY_COMB_LEFT;
		strSettingKey+=_T("← ");
	}
	if(((CButton*)GetDlgItem(IDC_CHK_UP))->GetCheck()==1)
	{
		dSetting = dSetting | KEY_COMB_UP;
		strSettingKey+=_T("↑ ");
	}
	if(((CButton*)GetDlgItem(IDC_CHK_RIGHT))->GetCheck()==1)
	{
		dSetting = dSetting | KEY_COMB_RIGHT;
		strSettingKey+=_T("→ ");
	}
	if(((CButton*)GetDlgItem(IDC_CHK_DOWN))->GetCheck()==1)
	{
		dSetting = dSetting | KEY_COMB_DOWN;
		strSettingKey+=_T("↓");
	}
	if(strSettingKey.IsEmpty())
	{
		::MessageBox(this->m_hWnd,_T("キーコンビネーションを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
		return;
	}
	strMsg.Format(_T("キーコンビネーションの確認画面\n設定したキーコンビネーションを押したまま、\nEnterキーを押すかOKボタンをクリックして下さい。\n設定値：%s"),strSettingKey);
	::MessageBox(this->m_hWnd,strMsg,theApp.m_strThisAppName,MB_OK|MB_ICONINFORMATION);
	{
		DWORD dwKC = theApp.GetKeyCombi();

		CString strResult;
		if((dwKC&KEY_COMB_SHIFT)==KEY_COMB_SHIFT)
			strResult+=_T("Shift ");
		if((dwKC&KEY_COMB_CTRL)==KEY_COMB_CTRL)
			strResult+=_T("Ctrl ");
		if((dwKC&KEY_COMB_ALT)==KEY_COMB_ALT)
			strResult+=_T("Alt ");
		if((dwKC&KEY_COMB_LEFT)==KEY_COMB_LEFT)
			strResult+=_T("← ");
		if((dwKC&KEY_COMB_UP)==KEY_COMB_UP)
			strResult+=_T("↑ ");
		if((dwKC&KEY_COMB_RIGHT)==KEY_COMB_RIGHT)
			strResult+=_T("→ ");
		if((dwKC&KEY_COMB_DOWN)==KEY_COMB_DOWN)
			strResult+=_T("↓");
		if(strResult.IsEmpty())
		{
			return;
			//strResult=_T("なし");
		}

		if(dSetting==dwKC)
		{
			strMsg.Format(_T("成功\nキーコンビネーションを確認しました。\n設定値：%s\n入力値：%s"),strSettingKey,strResult);
			::MessageBox(this->m_hWnd,strMsg,theApp.m_strThisAppName,MB_OK|MB_ICONINFORMATION);
		}
		else
		{
			strMsg.Format(_T("失敗\nキーコンビネーションの確認に失敗しました。\n設定値：%s\n入力値：%s"),strSettingKey,strResult);
			::MessageBox(this->m_hWnd,strMsg,theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
		}
	}
}

BOOL CDlgETC::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	BOOL bEnableDebugLog=theApp.SettingConf.m_bEnableDebugLog;
	((CButton*)GetDlgItem(IDC_CHECK4))->SetCheck(bEnableDebugLog?1:0);

	BOOL bURLOnly = theApp.m_RedirectList.m_bURLOnly;
	((CButton*)GetDlgItem(IDC_CHK_GLOVAL_URL_ONLY))->SetCheck(bURLOnly?1:0);

	BOOL bUseScript = theApp.m_RedirectList.m_bUseScript;
	((CButton*)GetDlgItem(IDC_CHK_GLOVAL_RULE_BASE))->SetCheck(bUseScript ? 1 : 0);

	//Office365に場合は、スクリプトを非表示
	if(theApp.m_bOffice365)
	{
		BOOL bTopURLOnly = theApp.m_RedirectList.m_bTopURLOnly;
		((CButton*)GetDlgItem(IDC_CHK_GLOVAL_TOP_PAGE_ONLY))->SetCheck(bTopURLOnly ? 1 : 0);

		if (GetDlgItem(IDC_CHK_GLOVAL_RULE_BASE))
		{
			GetDlgItem(IDC_CHK_GLOVAL_RULE_BASE)->ShowWindow(SW_HIDE);
		}
		if (GetDlgItem(IDC_BUTTON_SC))
		{
			GetDlgItem(IDC_BUTTON_SC)->ShowWindow(SW_HIDE);
		}
		if (GetDlgItem(IDC_STATIC_INFO))
		{
			GetDlgItem(IDC_STATIC_INFO)->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		if (GetDlgItem(IDC_CHK_GLOVAL_TOP_PAGE_ONLY))
		{
			GetDlgItem(IDC_CHK_GLOVAL_TOP_PAGE_ONLY)->ShowWindow(SW_HIDE);
		}
	}

	BOOL bTraceLog = theApp.m_RedirectList.m_bTraceLog;
	((CButton*)GetDlgItem(IDC_CHK_GLOVAL_TRACE))->SetCheck(bTraceLog?1:0);

	BOOL bQuickRedirect = theApp.m_RedirectList.m_bQuickRedirect;
	((CButton*)GetDlgItem(IDC_CHK_GLOVAL_QUICK))->SetCheck(bQuickRedirect ? 1 : 0);

	int iKeyCombination=0;
	iKeyCombination=theApp.SettingConf.m_iKeyCombination;

	((CButton*)GetDlgItem(IDC_CHK_SHIFT))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHK_CTRL))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHK_ALT))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHK_LEFT))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHK_UP))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHK_RIGHT))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHK_DOWN))->SetCheck(0);

	if((iKeyCombination&KEY_COMB_SHIFT)==KEY_COMB_SHIFT)
		((CButton*)GetDlgItem(IDC_CHK_SHIFT))->SetCheck(1);
	if((iKeyCombination&KEY_COMB_CTRL)==KEY_COMB_CTRL)
		((CButton*)GetDlgItem(IDC_CHK_CTRL))->SetCheck(1);
	if((iKeyCombination&KEY_COMB_ALT)==KEY_COMB_ALT)
		((CButton*)GetDlgItem(IDC_CHK_ALT))->SetCheck(1);
	if((iKeyCombination&KEY_COMB_LEFT)==KEY_COMB_LEFT)
		((CButton*)GetDlgItem(IDC_CHK_LEFT))->SetCheck(1);
	if((iKeyCombination&KEY_COMB_UP)==KEY_COMB_UP)
		((CButton*)GetDlgItem(IDC_CHK_UP))->SetCheck(1);
	if((iKeyCombination&KEY_COMB_RIGHT)==KEY_COMB_RIGHT)
		((CButton*)GetDlgItem(IDC_CHK_RIGHT))->SetCheck(1);
	if((iKeyCombination&KEY_COMB_DOWN)==KEY_COMB_DOWN)
		((CButton*)GetDlgItem(IDC_CHK_DOWN))->SetCheck(1);

	CString strVersion;
	strVersion = theApp.GetVersionStr();
	GetDlgItem(IDC_STATIC_VER)->SetWindowText(strVersion);

	return FALSE;
}

LRESULT CDlgETC::Set_OK(WPARAM wParam, LPARAM lParam)
{
	BOOL bEnableDebugLog = ((CButton*)GetDlgItem(IDC_CHECK4))->GetCheck() == 1?TRUE:FALSE;
	theApp.SettingConfMod.m_bEnableDebugLog=bEnableDebugLog;

	BOOL bURLOnly = ((CButton*)GetDlgItem(IDC_CHK_GLOVAL_URL_ONLY))->GetCheck() == 1?TRUE:FALSE;
	theApp.m_RedirectListSaveData.m_bURLOnly = bURLOnly;

	BOOL bTraceLog = ((CButton*)GetDlgItem(IDC_CHK_GLOVAL_TRACE))->GetCheck() == 1?TRUE:FALSE;
	theApp.m_RedirectListSaveData.m_bTraceLog = bTraceLog;

	BOOL bUseScript = ((CButton*)GetDlgItem(IDC_CHK_GLOVAL_RULE_BASE))->GetCheck() == 1?TRUE:FALSE;
	theApp.m_RedirectListSaveData.m_bUseScript = bUseScript;

	BOOL bQuickRedirect = ((CButton*)GetDlgItem(IDC_CHK_GLOVAL_QUICK))->GetCheck() == 1 ? TRUE : FALSE;
	theApp.m_RedirectListSaveData.m_bQuickRedirect = bQuickRedirect;

	//Office365の場合
	if (theApp.m_bOffice365)
	{
		BOOL bTopURLOnly = ((CButton*)GetDlgItem(IDC_CHK_GLOVAL_TOP_PAGE_ONLY))->GetCheck() == 1 ? TRUE : FALSE;
		theApp.m_RedirectListSaveData.m_bTopURLOnly= bTopURLOnly;
	}
	else
	{
		theApp.m_RedirectListSaveData.m_bTopURLOnly = FALSE;
	}

	DWORD dSetting=0;
	if(((CButton*)GetDlgItem(IDC_CHK_SHIFT))->GetCheck()==1)
		dSetting = dSetting | KEY_COMB_SHIFT;
	if(((CButton*)GetDlgItem(IDC_CHK_CTRL))->GetCheck()==1)
		dSetting = dSetting | KEY_COMB_CTRL;
	if(((CButton*)GetDlgItem(IDC_CHK_ALT))->GetCheck()==1)
		dSetting = dSetting | KEY_COMB_ALT;
	if(((CButton*)GetDlgItem(IDC_CHK_LEFT))->GetCheck()==1)
		dSetting = dSetting | KEY_COMB_LEFT;
	if(((CButton*)GetDlgItem(IDC_CHK_UP))->GetCheck()==1)
		dSetting = dSetting | KEY_COMB_UP;
	if(((CButton*)GetDlgItem(IDC_CHK_RIGHT))->GetCheck()==1)
		dSetting = dSetting | KEY_COMB_RIGHT;
	if(((CButton*)GetDlgItem(IDC_CHK_DOWN))->GetCheck()==1)
		dSetting = dSetting | KEY_COMB_DOWN;
	DWORD iKeyCombination=dSetting;
	theApp.SettingConfMod.m_iKeyCombination=iKeyCombination;
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


void CDlgETC::OnBnClickedButtonSc()
{
	BOOL bUseScript = ((CButton*)GetDlgItem(IDC_CHK_GLOVAL_RULE_BASE))->GetCheck() == 1?TRUE:FALSE;
	if(!bUseScript)
	{
		::MessageBox(this->m_hWnd,_T("[ThinBridgeスクリプトを利用する　ターゲットURLルールをスクリプトベースで設定する (エキスパート向け)]を有効にし、[設定保存]を行って下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
		return;
	}
	CDlgSCEditor DlgEditor;
	DlgEditor.m_strFilePath=theApp.m_strRedirect_Script_FilePath;
	DlgEditor.DoModal();
}

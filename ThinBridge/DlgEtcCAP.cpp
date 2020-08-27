#include "stdafx.h"
#include "ThinBridge.h"
#include "DlgEtcCAP.h"
#include "afxdialogex.h"
#include "URLInputDlg.h"

IMPLEMENT_DYNCREATE(CDlgEtcCAP, CPropertyPage)

CDlgEtcCAP::CDlgEtcCAP()
	: CPropertyPage(CDlgEtcCAP::IDD)
{
	m_iSolutionType=PROC_LIE;
	m_iKeyCombination=0;
}

CDlgEtcCAP::~CDlgEtcCAP()
{
}

void CDlgEtcCAP::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgEtcCAP, CPropertyPage)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_BUTTON_TEST_L, &CDlgEtcCAP::OnBnClickedButtonTestL)
	ON_BN_CLICKED(IDC_BUTTON_FDLG, &CDlgEtcCAP::OnBnClickedButtonFdlg)
	ON_BN_CLICKED(IDC_KEYTESTBUTTON, &CDlgEtcCAP::OnBnClickedKeytestbutton)
	ON_BN_CLICKED(IDC_RADIO_LCB, &CDlgEtcCAP::OnBnClickedRadioLcb)
	ON_BN_CLICKED(IDC_RADIO_LDB, &CDlgEtcCAP::OnBnClickedRadioLdb)
	ON_BN_CLICKED(IDC_RADIO_LIE, &CDlgEtcCAP::OnBnClickedRadioLie)
	ON_BN_CLICKED(IDC_RADIO_LFF, &CDlgEtcCAP::OnBnClickedRadioLff)
	ON_BN_CLICKED(IDC_RADIO_LCH, &CDlgEtcCAP::OnBnClickedRadioLch)
	ON_BN_CLICKED(IDC_RADIO_LEG, &CDlgEtcCAP::OnBnClickedRadioLeg)
END_MESSAGE_MAP()


void CDlgEtcCAP::OnBnClickedButtonTestL()
{
	this->GetSettings();
	CString strURL;
	CURLInputDlg Dlg;
	Dlg.m_strURL=strURL;
	if(IDOK==Dlg.DoModal())
	{
		strURL=Dlg.m_strURL;
	}
	else
	{
		return;
	}
	//TEST Exec
	theApp.Exec_Local_Browser(m_iSolutionType,m_strCustomBrowserPath,strURL);
}

void CDlgEtcCAP::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}


void CDlgEtcCAP::OnBnClickedKeytestbutton()
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

BOOL CDlgEtcCAP::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_iSolutionType=theApp.SettingConf.m_iSolutionType;
	m_strCustomBrowserPath=theApp.SettingConf.m_strCustomBrowserPath;
	m_iKeyCombination=theApp.SettingConf.m_iKeyCombination;

	EnableCustomPath(FALSE);


	((CButton*)GetDlgItem(IDC_RADIO_LDB))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LIE))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LFF))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LCH))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LEG))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LCB))->SetCheck(0);

	if(m_iSolutionType==PROC_LDefaultBrowser)
	{
		((CButton*)GetDlgItem(IDC_RADIO_LDB))->SetCheck(1);
	}
	else if(m_iSolutionType==PROC_LIE)
	{
		((CButton*)GetDlgItem(IDC_RADIO_LIE))->SetCheck(1);
	}
	else if(m_iSolutionType==PROC_LFirefox)
	{
		((CButton*)GetDlgItem(IDC_RADIO_LFF))->SetCheck(1);
	}
	else if(m_iSolutionType==PROC_LChrome)
	{
		((CButton*)GetDlgItem(IDC_RADIO_LCH))->SetCheck(1);
	}
	else if(m_iSolutionType==PROC_LEdge)
	{
		((CButton*)GetDlgItem(IDC_RADIO_LEG))->SetCheck(1);
	}
	else if(m_iSolutionType==PROC_LCustom)
	{
		((CButton*)GetDlgItem(IDC_RADIO_LCB))->SetCheck(1);
		EnableCustomPath(TRUE);
	}
	else
	{
		m_iSolutionType = PROC_LIE;
		((CButton*)GetDlgItem(IDC_RADIO_LIE))->SetCheck(1);
	}

	SetDlgItemText(IDC_EDIT_LCB,m_strCustomBrowserPath);


	((CButton*)GetDlgItem(IDC_CHK_SHIFT))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHK_CTRL))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHK_ALT))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHK_LEFT))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHK_UP))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHK_RIGHT))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHK_DOWN))->SetCheck(0);

	if((m_iKeyCombination&KEY_COMB_SHIFT)==KEY_COMB_SHIFT)
		((CButton*)GetDlgItem(IDC_CHK_SHIFT))->SetCheck(1);
	if((m_iKeyCombination&KEY_COMB_CTRL)==KEY_COMB_CTRL)
		((CButton*)GetDlgItem(IDC_CHK_CTRL))->SetCheck(1);
	if((m_iKeyCombination&KEY_COMB_ALT)==KEY_COMB_ALT)
		((CButton*)GetDlgItem(IDC_CHK_ALT))->SetCheck(1);
	if((m_iKeyCombination&KEY_COMB_LEFT)==KEY_COMB_LEFT)
		((CButton*)GetDlgItem(IDC_CHK_LEFT))->SetCheck(1);
	if((m_iKeyCombination&KEY_COMB_UP)==KEY_COMB_UP)
		((CButton*)GetDlgItem(IDC_CHK_UP))->SetCheck(1);
	if((m_iKeyCombination&KEY_COMB_RIGHT)==KEY_COMB_RIGHT)
		((CButton*)GetDlgItem(IDC_CHK_RIGHT))->SetCheck(1);
	if((m_iKeyCombination&KEY_COMB_DOWN)==KEY_COMB_DOWN)
		((CButton*)GetDlgItem(IDC_CHK_DOWN))->SetCheck(1);
	return FALSE;
}

void CDlgEtcCAP::GetSettings()
{
	//一般-----------------------------------------------------------------------↓
	if(((CButton*)GetDlgItem(IDC_RADIO_LDB))->GetCheck() == 1)
		m_iSolutionType = PROC_LDefaultBrowser;
	else if(((CButton*)GetDlgItem(IDC_RADIO_LIE))->GetCheck() == 1)
		m_iSolutionType = PROC_LIE;
	else if(((CButton*)GetDlgItem(IDC_RADIO_LFF))->GetCheck() == 1)
		m_iSolutionType = PROC_LFirefox;
	else if(((CButton*)GetDlgItem(IDC_RADIO_LCH))->GetCheck() == 1)
		m_iSolutionType = PROC_LChrome;
	else if(((CButton*)GetDlgItem(IDC_RADIO_LEG))->GetCheck() == 1)
		m_iSolutionType = PROC_LEdge;
	else if(((CButton*)GetDlgItem(IDC_RADIO_LCB))->GetCheck() == 1)
		m_iSolutionType = PROC_LCustom;
	else
		m_iSolutionType = PROC_LIE;


	GetDlgItemText(IDC_EDIT_LCB,m_strCustomBrowserPath);

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
	m_iKeyCombination=dSetting;

}

LRESULT CDlgEtcCAP::Set_OK(WPARAM wParam, LPARAM lParam)
{
	this->GetSettings();
	if(m_iSolutionType==PROC_LCustom)
	{
		if(m_strCustomBrowserPath.IsEmpty())
		{
			::MessageBox(this->m_hWnd,_T("[転送先ブラウザー設定(ローカル)]\n指定ブラウザーのパスを入力して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
			return 1;
		}
	}
	theApp.SettingConfMod.m_iSolutionType=m_iSolutionType;
	theApp.SettingConfMod.m_strCustomBrowserPath=m_strCustomBrowserPath;
	theApp.SettingConfMod.m_iKeyCombination=m_iKeyCombination;

	return 0;
}

void CDlgEtcCAP::OnBnClickedRadioLcb()
{
	if(((CButton*)GetDlgItem(IDC_RADIO_LCB))->GetCheck() == 1)
	{
		EnableCustomPath(TRUE);
	}
}

void CDlgEtcCAP::EnableCustomPath(BOOL bFlg)
{
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bFlg);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bFlg);
}

void CDlgEtcCAP::OnBnClickedRadioLdb()
{
	EnableCustomPath(FALSE);
}

void CDlgEtcCAP::OnBnClickedRadioLie()
{
	EnableCustomPath(FALSE);
}

void CDlgEtcCAP::OnBnClickedRadioLff()
{
	EnableCustomPath(FALSE);
}

void CDlgEtcCAP::OnBnClickedRadioLch()
{
	EnableCustomPath(FALSE);
}

void CDlgEtcCAP::OnBnClickedRadioLeg()
{
	EnableCustomPath(FALSE);
}

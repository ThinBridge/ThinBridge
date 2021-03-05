#include "stdafx.h"
#include "ThinBridge.h"
#include "DlgSetCX.h"
#include "afxdialogex.h"
#include "SettingsDialog.h"
#include "URLInputDlg.h"


IMPLEMENT_DYNCREATE(CDlgSetCX, CPropertyPage)

CDlgSetCX::CDlgSetCX()
	: CPropertyPage(CDlgSetCX::IDD)
{

}

CDlgSetCX::~CDlgSetCX()
{
}

void CDlgSetCX::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_Image);
}


BEGIN_MESSAGE_MAP(CDlgSetCX, CPropertyPage)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_BUTTON_TEST_CX, &CDlgSetCX::OnBnClickedButtonTestCx)
END_MESSAGE_MAP()


BOOL CDlgSetCX::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_strCitrix_AppName = theApp.m_RedirectList.m_strCitrix_AppName;
	//Citrix_XenApp-----------------------------------------------------------------↓
	SetDlgItemText(IDC_EDIT_CXAPP,m_strCitrix_AppName);
	//Citrix_XenApp-----------------------------------------------------------------↑
	CIconHelper ICoHelper;
	ICoHelper=theApp.LoadIcon(IDI_ICON_Ctrix);
	m_Image.SetIcon(ICoHelper);

	return FALSE;
}

LRESULT CDlgSetCX::Set_OK(WPARAM wParam, LPARAM lParam)
{
	//Citrix-----------------------------------------------------------------↓
	GetDlgItemText(IDC_EDIT_CXAPP,m_strCitrix_AppName);
	theApp.m_RedirectListSaveData.m_strCitrix_AppName=m_strCitrix_AppName;
	return 0;
}


void CDlgSetCX::OnBnClickedButtonTestCx()
{
	this->Set_OK(0,0);
	CString strURL;
	//citrix clientがインストールされていない。
	if(!theApp.IsCitrixInstalled())
	{
		CString strMsg=_T("Citrix Receiverがインストールされていないか、設定されていないため起動できません。");
		theApp.ShowTimeoutMessageBox(strMsg,theApp.m_strThisAppName,MB_OK|MB_ICONERROR,6000);
		return;
	}

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
	//TEST Connect VMware Horizon
	theApp.Exec_Citrix_XenApp(m_strCitrix_AppName,strURL);
}

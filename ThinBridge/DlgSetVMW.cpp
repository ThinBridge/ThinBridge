#include "stdafx.h"
#include "ThinBridge.h"
#include "DlgSetVMW.h"
#include "afxdialogex.h"
#include "SettingsDialog.h"
#include "URLInputDlg.h"


IMPLEMENT_DYNCREATE(CDlgSetVMW, CPropertyPage)

CDlgSetVMW::CDlgSetVMW()
	: CPropertyPage(CDlgSetVMW::IDD)
{

}

CDlgSetVMW::~CDlgSetVMW()
{
}

void CDlgSetVMW::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_Image);
}


BEGIN_MESSAGE_MAP(CDlgSetVMW, CPropertyPage)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_BUTTON_TEST_V, &CDlgSetVMW::OnBnClickedButtonTestV)
END_MESSAGE_MAP()


BOOL CDlgSetVMW::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_strHorizon_ConnectionServerName=theApp.m_RedirectList.m_strHorizon_ConnectionServerName;
	m_strHorizon_AppName=theApp.m_RedirectList.m_strHorizon_AppName;
	//VM_Horizon-----------------------------------------------------------------↓
	SetDlgItemText(IDC_EDIT_VHOST,m_strHorizon_ConnectionServerName);
	SetDlgItemText(IDC_EDIT_VAPP,m_strHorizon_AppName);
	//VM_Horizon-----------------------------------------------------------------↑
	CIconHelper ICoHelper;
	ICoHelper=theApp.LoadIcon(IDI_ICON_VMH);
	m_Image.SetIcon(ICoHelper);
	return FALSE;
}

LRESULT CDlgSetVMW::Set_OK(WPARAM wParam, LPARAM lParam)
{
	//VM_Horizon-----------------------------------------------------------------↓
	GetDlgItemText(IDC_EDIT_VHOST,m_strHorizon_ConnectionServerName);
	GetDlgItemText(IDC_EDIT_VAPP,m_strHorizon_AppName);

	theApp.m_RedirectListSaveData.m_strHorizon_ConnectionServerName = m_strHorizon_ConnectionServerName;
	theApp.m_RedirectListSaveData.m_strHorizon_AppName = m_strHorizon_AppName;
	return 0;
}

void CDlgSetVMW::OnBnClickedButtonTestV()
{
	this->Set_OK(0,0);
	CString strURL;

	//horizon clientがインストールされていない。
	if(!theApp.IsHorizonInstalled())
	{
		CString strMsg=_T("VMware Horizon Clientがインストールされていないか、設定されていないため起動できません。");
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
	theApp.Exec_VMware_Horizon(m_strHorizon_ConnectionServerName, m_strHorizon_AppName,strURL);
}

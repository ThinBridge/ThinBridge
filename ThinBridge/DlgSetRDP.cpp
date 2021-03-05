#include "stdafx.h"
#include "ThinBridge.h"
#include "DlgSetRDP.h"
#include "afxdialogex.h"
#include "SettingsDialog.h"
#include "URLInputDlg.h"
#include "DetailEdit.h"

IMPLEMENT_DYNCREATE(CDlgSetRDP, CPropertyPage)

CDlgSetRDP::CDlgSetRDP()
	: CPropertyPage(CDlgSetRDP::IDD)
{

}

CDlgSetRDP::~CDlgSetRDP()
{
}

void CDlgSetRDP::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_Image);
}


BEGIN_MESSAGE_MAP(CDlgSetRDP, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgSetIEC)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgSetRDP::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_ST, &CDlgSetRDP::OnBnClickedButtonSt)
	ON_BN_CLICKED(IDC_BUTTON_IE, &CDlgSetRDP::OnBnClickedButtonIe)
	ON_BN_CLICKED(IDC_BUTTON_FF, &CDlgSetRDP::OnBnClickedButtonFf)
	ON_BN_CLICKED(IDC_BUTTON_GC, &CDlgSetRDP::OnBnClickedButtonGc)
	ON_BN_CLICKED(IDC_BUTTON_CLR, &CDlgSetRDP::OnBnClickedButtonClr)
	ON_BN_CLICKED(IDC_RADIO1, &CDlgSetRDP::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CDlgSetRDP::OnBnClickedRadio2)
END_MESSAGE_MAP()

BOOL CDlgSetRDP::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_strServerName=theApp.m_RedirectList.m_strRDP_ServerName;
	m_bRemoteAppMode=theApp.m_RedirectList.m_bRemoteAppMode;
	m_strRemoteAppName=theApp.m_RedirectList.m_strRemoteAppName;
	m_strRemoteAppPath=theApp.m_RedirectList.m_strRemoteAppPath;
	m_strRemoteAppCommandLine=theApp.m_RedirectList.m_strRemoteAppCommandLine;
	m_bRedirect_Clipboard=theApp.m_RedirectList.m_bRedirect_Clipboard;
	m_bRedirect_Printer=theApp.m_RedirectList.m_bRedirect_Printer;
	m_bRedirect_Drive=theApp.m_RedirectList.m_bRedirect_Drive;

	//RDP-----------------------------------------------------------------↓
	SetDlgItemText(IDC_EDIT1,m_strServerName);
	if(m_bRemoteAppMode)
	{
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(1);
	}

	SetDlgItemText(IDC_EDIT2,m_strRemoteAppName);
	SetDlgItemText(IDC_EDIT3,m_strRemoteAppPath);
	SetDlgItemText(IDC_EDIT4,m_strRemoteAppCommandLine);

	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(m_bRedirect_Clipboard?1:0);
	((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(m_bRedirect_Printer?1:0);
	((CButton*)GetDlgItem(IDC_CHECK3))->SetCheck(m_bRedirect_Drive?1:0);
	//RDP-----------------------------------------------------------------↑

	CIconHelper ICoHelper;
	ICoHelper=theApp.LoadIcon(IDI_ICON_RDP);
	m_Image.SetIcon(ICoHelper);
	Change_RDP_App_DesktopState();
	return FALSE;
}

void CDlgSetRDP::OnDestroy() 
{
	CPropertyPage::OnDestroy();
}

LRESULT CDlgSetRDP::Set_OK(WPARAM wParam, LPARAM lParam)
{
	//RDP-----------------------------------------------------------------↓
	GetDlgItemText(IDC_EDIT1,m_strServerName);
	if(((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck() == 1)
	{
		m_bRemoteAppMode=TRUE;
	}
	else
	{
		m_bRemoteAppMode=FALSE;
	}

	GetDlgItemText(IDC_EDIT2,m_strRemoteAppName);
	GetDlgItemText(IDC_EDIT3,m_strRemoteAppPath);
	GetDlgItemText(IDC_EDIT4,m_strRemoteAppCommandLine);

	m_bRedirect_Clipboard = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck() == 1?TRUE:FALSE;
	m_bRedirect_Printer = ((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck() == 1?TRUE:FALSE;
	m_bRedirect_Drive = ((CButton*)GetDlgItem(IDC_CHECK3))->GetCheck() == 1?TRUE:FALSE;


	theApp.m_RedirectListSaveData.m_strRDP_ServerName = m_strServerName;
	theApp.m_RedirectListSaveData.m_bRemoteAppMode = m_bRemoteAppMode;
	theApp.m_RedirectListSaveData.m_strRemoteAppName = m_strRemoteAppName;
	theApp.m_RedirectListSaveData.m_strRemoteAppPath = m_strRemoteAppPath;
	theApp.m_RedirectListSaveData.m_strRemoteAppCommandLine = m_strRemoteAppCommandLine;
	theApp.m_RedirectListSaveData.m_bRedirect_Clipboard = m_bRedirect_Clipboard;
	theApp.m_RedirectListSaveData.m_bRedirect_Printer = m_bRedirect_Printer;
	theApp.m_RedirectListSaveData.m_bRedirect_Drive = m_bRedirect_Drive;

	return 0;
}


void CDlgSetRDP::OnBnClickedButton1()
{
	this->Set_OK(0,0);
	CString strURL;
	CURLInputDlg Dlg;
	Dlg.m_strURL=strURL;

	if(m_bRemoteAppMode)
	{
		if(IDOK==Dlg.DoModal())
		{
			strURL=Dlg.m_strURL;
		}
		else
		{
			return;
		}
	}
	//TEST Connect
	CStdioFile out;
	if(out.Open(theApp.m_strTestConnect_FileFullPath,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
	{
		CString strWriteData;
		//接続先のコンピューター名（or IPアドレス）
		if(!m_strServerName.IsEmpty())
		{
			strWriteData.Format(_T("full address:s:%s\n"),m_strServerName);
			out.WriteString(strWriteData);
		}

		//RemoteAppモードを有効
		strWriteData.Format(_T("remoteapplicationmode:i:%d\n"),m_bRemoteAppMode?1:0);
		out.WriteString(strWriteData);

		//RemoteAppの場合だけ。
		if(m_bRemoteAppMode)
		{
			//RemoteAppのアプリ名
			if(!m_strRemoteAppName.IsEmpty())
			{
				strWriteData.Format(_T("remoteapplicationname:s:%s\n"),m_strRemoteAppName);
				out.WriteString(strWriteData);
			}

			//RemoteAppで実行するEXEのフルパス
			if(!m_strRemoteAppPath.IsEmpty())
			{
				strWriteData.Format(_T("remoteapplicationprogram:s:%s\n"),m_strRemoteAppPath);
				out.WriteString(strWriteData);
				strWriteData.Format(_T("alternate shell:s:%s\n"),m_strRemoteAppPath);
				out.WriteString(strWriteData);
			}

			//CommandLine
			if(!m_strRemoteAppCommandLine.IsEmpty())
			{
				if(!strURL.IsEmpty())
				{
					strWriteData.Format(_T("remoteapplicationcmdline:s:%s %s\n"),m_strRemoteAppCommandLine,strURL);
				}
				else
				{
					strWriteData.Format(_T("remoteapplicationcmdline:s:%s\n"),m_strRemoteAppCommandLine);
				}
				out.WriteString(strWriteData);
			}
			else
			{
				if(!strURL.IsEmpty())
				{
					strWriteData.Format(_T("remoteapplicationcmdline:s:%s\n"),strURL);
					out.WriteString(strWriteData);
				}
			}
		}
		//クリップボードのリダイレクト
		strWriteData.Format(_T("redirectclipboard:i:%d\n"),m_bRedirect_Clipboard?1:0);
		out.WriteString(strWriteData);

		//Printerのリダイレクト
		strWriteData.Format(_T("redirectprinters:i:%d\n"),m_bRedirect_Printer?1:0);
		out.WriteString(strWriteData);

		//ローカルのディスクドライブをリモートに接続する
		strWriteData.Format(_T("RedirectDrives:i:%d\n"),m_bRedirect_Drive?1:0);
		out.WriteString(strWriteData);

		//templateファイルの内容を追加
		theApp.ReadTemplate();	
		out.WriteString(theApp.m_strRdp_Template_File_Data);
		//ReadOnceファイルの内容を追加
		out.WriteString(theApp.m_strRdp_ReadOnce_File_Data);

		out.Close();
	}
	theApp.Exec_MSTSC(theApp.m_strTestConnect_FileFullPath);
}


void CDlgSetRDP::OnBnClickedButtonSt()
{
	DetailEdit Dlg;
	Dlg.DoModal();
}


void CDlgSetRDP::OnBnClickedButtonIe()
{
	SetDlgItemText(IDC_EDIT2,_T("InternetExplorer"));
	SetDlgItemText(IDC_EDIT3,_T("C:\\Program Files\\Internet Explorer\\iexplore.exe"));
	SetDlgItemText(IDC_EDIT4,_T(""));
}


void CDlgSetRDP::OnBnClickedButtonFf()
{
	SetDlgItemText(IDC_EDIT2,_T("MozillaFirefox"));
	SetDlgItemText(IDC_EDIT3,_T("C:\\Program Files\\Mozilla Firefox\\firefox.exe"));
	SetDlgItemText(IDC_EDIT4,_T(""));
}


void CDlgSetRDP::OnBnClickedButtonGc()
{
	SetDlgItemText(IDC_EDIT2,_T("GoogleChrome"));
	SetDlgItemText(IDC_EDIT3,_T("C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe"));
	//http://support.citrix.com/article/CTX132057
	SetDlgItemText(IDC_EDIT4,_T(""));
}


void CDlgSetRDP::OnBnClickedButtonClr()
{
	SetDlgItemText(IDC_EDIT2,_T(""));
	SetDlgItemText(IDC_EDIT3,_T(""));
	SetDlgItemText(IDC_EDIT4,_T(""));
}


void CDlgSetRDP::OnBnClickedRadio1()
{
	Change_RDP_App_DesktopState();
}


void CDlgSetRDP::OnBnClickedRadio2()
{
	Change_RDP_App_DesktopState();
}

void CDlgSetRDP::Change_RDP_App_DesktopState()
{
	BOOL bFlg=FALSE;
	if(((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck() == 1)
		bFlg=TRUE;
	else
		bFlg=FALSE;

	//RDP-----------------------------------------------------------------↓
	//GetDlgItem(IDC_EDIT1)->EnableWindow(bFlg);
	//GetDlgItem(IDC_RADIO1)->EnableWindow(bFlg);
	//GetDlgItem(IDC_RADIO2)->EnableWindow(bFlg);
	GetDlgItem(IDC_EDIT2)->EnableWindow(bFlg);
	GetDlgItem(IDC_EDIT3)->EnableWindow(bFlg);
	GetDlgItem(IDC_EDIT4)->EnableWindow(bFlg);
	//GetDlgItem(IDC_CHECK1)->EnableWindow(bFlg);
	//GetDlgItem(IDC_CHECK2)->EnableWindow(bFlg);
	//GetDlgItem(IDC_CHECK3)->EnableWindow(bFlg);

	GetDlgItem(IDC_BUTTON_IE)->EnableWindow(bFlg);
	GetDlgItem(IDC_BUTTON_FF)->EnableWindow(bFlg);
	GetDlgItem(IDC_BUTTON_GC)->EnableWindow(bFlg);
	GetDlgItem(IDC_BUTTON_CLR)->EnableWindow(bFlg);

	//GetDlgItem(IDC_BUTTON1)->EnableWindow(bFlg);
	//GetDlgItem(IDC_BUTTON_ST)->EnableWindow(bFlg);
}


#include "stdafx.h"
#include "ThinBridge.h"
#include "RedirectDlg.h"
#include "afxdialogex.h"
#include "URLInputDlg.h"
#include "DetailEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


CRedirectDlg::CRedirectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRedirectDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bRemoteAppMode=TRUE;
	m_bRedirect_Clipboard=FALSE;
	m_bRedirect_Printer=FALSE;
	m_bRedirect_Drive=FALSE;
	m_iSolutionType=PROC_RDP;
	m_iKeyCombination=0;
}

void CRedirectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRedirectDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CRedirectDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CRedirectDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_IE, &CRedirectDlg::OnBnClickedButtonIe)
	ON_BN_CLICKED(IDC_BUTTON_FF, &CRedirectDlg::OnBnClickedButtonFf)
	ON_BN_CLICKED(IDC_BUTTON_CLR, &CRedirectDlg::OnBnClickedButtonClr)
	ON_BN_CLICKED(IDC_BUTTON_GC, &CRedirectDlg::OnBnClickedButtonGc)
	ON_BN_CLICKED(IDC_BUTTON_ST, &CRedirectDlg::OnBnClickedButtonSt)
	ON_BN_CLICKED(IDC_BUTTON_TEST_V, &CRedirectDlg::OnBnClickedButtonTestV)
	ON_BN_CLICKED(IDCANCEL, &CRedirectDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIO_RDP, &CRedirectDlg::OnBnClickedRadioRdp)
	ON_BN_CLICKED(IDC_RADIO_VMW, &CRedirectDlg::OnBnClickedRadioVmw)
	ON_BN_CLICKED(IDC_BUTTON_FDLG, &CRedirectDlg::OnBnClickedButtonFdlg)
	ON_BN_CLICKED(IDC_RADIO_LDB, &CRedirectDlg::OnBnClickedRadioLdb)
	ON_BN_CLICKED(IDC_RADIO_LIE, &CRedirectDlg::OnBnClickedRadioLie)
	ON_BN_CLICKED(IDC_RADIO_LFF, &CRedirectDlg::OnBnClickedRadioLff)
	ON_BN_CLICKED(IDC_RADIO_LCH, &CRedirectDlg::OnBnClickedRadioLch)
	ON_BN_CLICKED(IDC_RADIO_LEG, &CRedirectDlg::OnBnClickedRadioLeg)
	ON_BN_CLICKED(IDC_RADIO_LCB, &CRedirectDlg::OnBnClickedRadioLcb)
	ON_BN_CLICKED(IDC_BUTTON_TEST_L, &CRedirectDlg::OnBnClickedButtonTestL)
	ON_BN_CLICKED(IDC_RADIO1, &CRedirectDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CRedirectDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_BUTTON_TEST_CX, &CRedirectDlg::OnBnClickedButtonTestCx)
	ON_BN_CLICKED(IDC_RADIO_CX, &CRedirectDlg::OnBnClickedRadioCx)
	ON_BN_CLICKED(IDC_KEYTESTBUTTON, &CRedirectDlg::OnBnClickedKeytestbutton)
END_MESSAGE_MAP()


BOOL CRedirectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	m_strServerName=theApp.SettingConf.m_strServerName;
	m_bRemoteAppMode=theApp.SettingConf.m_bRemoteAppMode;
	m_strRemoteAppName=theApp.SettingConf.m_strRemoteAppName;
	m_strRemoteAppPath=theApp.SettingConf.m_strRemoteAppPath;
	m_strRemoteAppCommandLine=theApp.SettingConf.m_strRemoteAppCommandLine;
	m_bRedirect_Clipboard=theApp.SettingConf.m_bRedirect_Clipboard;
	m_bRedirect_Printer=theApp.SettingConf.m_bRedirect_Printer;
	m_bRedirect_Drive=theApp.SettingConf.m_bRedirect_Drive;

	m_bEnableDebugLog=theApp.SettingConf.m_bEnableDebugLog;

	m_strCustomBrowserPath=theApp.SettingConf.m_strCustomBrowserPath;

	m_iSolutionType=theApp.SettingConf.m_iSolutionType;
	m_strHorizon_ConnectionServerName=theApp.SettingConf.m_strHorizon_ConnectionServerName;
	m_strHorizon_AppName=theApp.SettingConf.m_strHorizon_AppName;
	m_strCitrix_AppName=theApp.SettingConf.m_strCitrix_AppName;
	SetDlgItemText(IDC_EDIT_LCB,m_strCustomBrowserPath);

	//VM_Horizon-----------------------------------------------------------------↓
	SetDlgItemText(IDC_EDIT_VHOST,m_strHorizon_ConnectionServerName);
	SetDlgItemText(IDC_EDIT_VAPP,m_strHorizon_AppName);
	//VM_Horizon-----------------------------------------------------------------↑

	//Citrix_XenApp-----------------------------------------------------------------↓
	SetDlgItemText(IDC_EDIT_CXAPP,m_strCitrix_AppName);
	//Citrix_XenApp-----------------------------------------------------------------↑

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

	((CButton*)GetDlgItem(IDC_RADIO_LDB))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LIE))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LFF))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LCH))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LEG))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LCB))->SetCheck(0);

	((CButton*)GetDlgItem(IDC_RADIO_RDP))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_VMW))->SetCheck(0);

	EnableCustomPath(FALSE);
	EnableVMwareHorizon(FALSE);
	EnableRDPWindow(FALSE);
	EnableCitrix(FALSE);

	//一般-----------------------------------------------------------------------↓
	if(m_iSolutionType==PROC_LDefaultBrowser)
	{
		((CButton*)GetDlgItem(IDC_RADIO_LDB))->SetCheck(1);
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(TRUE);
	}
	else if(m_iSolutionType==PROC_LIE)
	{
		((CButton*)GetDlgItem(IDC_RADIO_LIE))->SetCheck(1);
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(TRUE);
	}
	else if(m_iSolutionType==PROC_LFirefox)
	{
		((CButton*)GetDlgItem(IDC_RADIO_LFF))->SetCheck(1);
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(TRUE);
	}
	else if(m_iSolutionType==PROC_LChrome)
	{
		((CButton*)GetDlgItem(IDC_RADIO_LCH))->SetCheck(1);
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(TRUE);
	}
	else if(m_iSolutionType==PROC_LEdge)
	{
		((CButton*)GetDlgItem(IDC_RADIO_LEG))->SetCheck(1);
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(TRUE);
	}
	else if(m_iSolutionType==PROC_LCustom)
	{
		((CButton*)GetDlgItem(IDC_RADIO_LCB))->SetCheck(1);
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(TRUE);
		EnableCustomPath(TRUE);
	}
	else if(m_iSolutionType==PROC_VMWARE)
	{
		((CButton*)GetDlgItem(IDC_RADIO_VMW))->SetCheck(1);
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(FALSE);
		EnableVMwareHorizon(TRUE);
	}
	else if(m_iSolutionType==PROC_CITRIX)
	{
		((CButton*)GetDlgItem(IDC_RADIO_CX))->SetCheck(1);
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(FALSE);
		EnableCitrix(TRUE);
	}
	else if(m_iSolutionType==PROC_RDP)
	{
		((CButton*)GetDlgItem(IDC_RADIO_RDP))->SetCheck(1);
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(FALSE);
		EnableRDPWindow(TRUE);
		Change_RDP_App_DesktopState();
	}
	((CButton*)GetDlgItem(IDC_CHECK4))->SetCheck(m_bEnableDebugLog?1:0);
	//一般-----------------------------------------------------------------------↑

	m_iKeyCombination=theApp.SettingConf.m_iKeyCombination;

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

	//Ctrixの機能を表示する。
	if((theApp.m_iFeatureType&FE_CTX)==FE_CTX)
	{
		;
	}
	else
	{
		ShowHideCtrix();
	}

	return FALSE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CRedirectDlg::ShowHideCtrix()
{
	GetDlgItem(IDC_RADIO_CX)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_CX)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_CX2)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_CX3)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_CXAPP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_TEST_CX)->EnableWindow(FALSE);

	GetDlgItem(IDC_RADIO_CX)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_CX)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_CX2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_CX3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_CXAPP)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_TEST_CX)->ShowWindow(SW_HIDE);

	//もしCitrixが選択されている状態の場合は、VMWに変更
	if(m_iSolutionType==PROC_CITRIX)
	{
		((CButton*)GetDlgItem(IDC_RADIO_VMW))->SetCheck(1);
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(FALSE);
		EnableVMwareHorizon(TRUE);
	}
	return;
}

void CRedirectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CRedirectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CRedirectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRedirectDlg::OnBnClickedOk()
{
	int iRet=0;
	iRet = ::MessageBox(this->m_hWnd,_T("設定を保存しますか？"),theApp.m_strThisAppName,MB_ICONQUESTION | MB_OKCANCEL);
	if(iRet == IDCANCEL)
		return;

	this->GetSettings();
	//TEST Connect
	CStdioFile out;
	if(out.Open(theApp.m_strSetting_FileFullPath,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
	{
		CString strWriteData;

		//一般-----------------------------------------------------------------------↓
		//Solution_Type
		CString strSolType;
		strSolType=_T("RDP");
		if(m_iSolutionType==PROC_RDP)
			strSolType=_T("RDP");
		else if(m_iSolutionType==PROC_VMWARE)
			strSolType=_T("VMWARE");
		else if(m_iSolutionType==PROC_CITRIX)
			strSolType=_T("CITRIX");
		else if(m_iSolutionType==PROC_LDefaultBrowser)
			strSolType=_T("DefaultBrowser");
		else if(m_iSolutionType==PROC_LIE)
			strSolType=_T("IE");
		else if(m_iSolutionType==PROC_LFirefox)
			strSolType=_T("Firefox");
		else if(m_iSolutionType==PROC_LChrome)
			strSolType=_T("Chrome");
		else if(m_iSolutionType==PROC_LEdge)
			strSolType=_T("Edge");
		else if(m_iSolutionType==PROC_LCustom)
			strSolType=_T("Custom");
		strWriteData.Format(_T("Solution_Type=%s\n"),strSolType);
		out.WriteString(strWriteData);
		//LOG
		strWriteData.Format(_T("Enable_Log=%d\n"),m_bEnableDebugLog?1:0);
		out.WriteString(strWriteData);


		strWriteData.Format(_T("CustomBrowserPath=%s\n"),m_strCustomBrowserPath);
		out.WriteString(strWriteData);

		//VM_Horizon-----------------------------------------------------------------↓
		strWriteData.Format(_T("v_ConnectionServerName=%s\n"),m_strHorizon_ConnectionServerName);
		out.WriteString(strWriteData);
		strWriteData.Format(_T("v_AppName=%s\n"),m_strHorizon_AppName);
		out.WriteString(strWriteData);

		//Citrix_XenApp-----------------------------------------------------------------↓
		strWriteData.Format(_T("CX_AppName=%s\n"),m_strCitrix_AppName);
		out.WriteString(strWriteData);

		//RDP-----------------------------------------------------------------↓
		//接続先のコンピューター名（or IPアドレス）
		strWriteData.Format(_T("ServerName=%s\n"),m_strServerName);
		out.WriteString(strWriteData);

		//RemoteAppモードを有効
		strWriteData.Format(_T("RemoteAppMode=%d\n"),m_bRemoteAppMode?1:0);
		out.WriteString(strWriteData);

		//RemoteAppのアプリ名
		strWriteData.Format(_T("RemoteAppName=%s\n"),m_strRemoteAppName);
		out.WriteString(strWriteData);

		//RemoteAppで実行するEXEのフルパス
		strWriteData.Format(_T("RemoteAppPath=%s\n"),m_strRemoteAppPath);
		out.WriteString(strWriteData);

		//CommandLine
		strWriteData.Format(_T("RemoteAppCommandLine=%s\n"),m_strRemoteAppCommandLine);
		out.WriteString(strWriteData);

		//クリップボードのリダイレクト
		strWriteData.Format(_T("Redirect_Clipboard=%d\n"),m_bRedirect_Clipboard?1:0);
		out.WriteString(strWriteData);

		//Printerのリダイレクト
		strWriteData.Format(_T("Redirect_Printer=%d\n"),m_bRedirect_Printer?1:0);
		out.WriteString(strWriteData);

		//ローカルのディスクドライブをリモートに接続する
		strWriteData.Format(_T("Redirect_Drive=%d\n"),m_bRedirect_Drive?1:0);
		out.WriteString(strWriteData);

		//表示制限キーコンビネーション
		strWriteData.Format(_T("KeyCombination=%d\n"),m_iKeyCombination);
		out.WriteString(strWriteData);

		out.Close();
	}
//	CDialog::OnOK();
}

void CRedirectDlg::GetSettings()
{

	//一般-----------------------------------------------------------------------↓
	if(((CButton*)GetDlgItem(IDC_RADIO_RDP))->GetCheck() == 1)
		m_iSolutionType = PROC_RDP;
	else if(((CButton*)GetDlgItem(IDC_RADIO_VMW))->GetCheck() == 1)
		m_iSolutionType = PROC_VMWARE;
	else if(((CButton*)GetDlgItem(IDC_RADIO_CX))->GetCheck() == 1)
		m_iSolutionType = PROC_CITRIX;

	else if(((CButton*)GetDlgItem(IDC_RADIO_LDB))->GetCheck() == 1)
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


	m_bEnableDebugLog = ((CButton*)GetDlgItem(IDC_CHECK4))->GetCheck() == 1?TRUE:FALSE;

	//VM_Horizon-----------------------------------------------------------------↓
	GetDlgItemText(IDC_EDIT_VHOST,m_strHorizon_ConnectionServerName);
	GetDlgItemText(IDC_EDIT_VAPP,m_strHorizon_AppName);

	//Citrix-----------------------------------------------------------------↓
	GetDlgItemText(IDC_EDIT_CXAPP,m_strCitrix_AppName);

	GetDlgItemText(IDC_EDIT_LCB,m_strCustomBrowserPath);


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

void CRedirectDlg::OnBnClickedButton1()
{
	this->GetSettings();
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


void CRedirectDlg::OnBnClickedButtonIe()
{
	SetDlgItemText(IDC_EDIT2,_T("InternetExplorer"));
	SetDlgItemText(IDC_EDIT3,_T("C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe"));
	SetDlgItemText(IDC_EDIT4,_T(""));
}


void CRedirectDlg::OnBnClickedButtonFf()
{
	SetDlgItemText(IDC_EDIT2,_T("MozillaFirefox"));
	SetDlgItemText(IDC_EDIT3,_T("C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe"));
	SetDlgItemText(IDC_EDIT4,_T(""));
}


void CRedirectDlg::OnBnClickedButtonClr()
{
	SetDlgItemText(IDC_EDIT2,_T(""));
	SetDlgItemText(IDC_EDIT3,_T(""));
	SetDlgItemText(IDC_EDIT4,_T(""));
}


void CRedirectDlg::OnBnClickedButtonGc()
{
	SetDlgItemText(IDC_EDIT2,_T("GoogleChrome"));
	SetDlgItemText(IDC_EDIT3,_T("C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe"));
	//http://support.citrix.com/article/CTX132057
	SetDlgItemText(IDC_EDIT4,_T("--allow-no-sandbox-job --disable-gpu"));
}


void CRedirectDlg::OnBnClickedButtonSt()
{
	DetailEdit Dlg;
	Dlg.DoModal();
//	CString strFilePath;
//	strFilePath = theApp.m_strTemplate_FileFullPath;
//	ShellExecute(this->m_hWnd , _T("open") , _T("notepad.exe") , strFilePath , 0 , SW_SHOW);	
//
}

void CRedirectDlg::OnBnClickedButtonTestV()
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
	//TEST Connect VMware Horizon
	theApp.Exec_VMware_Horizon(m_strHorizon_ConnectionServerName, m_strHorizon_AppName,strURL);
}
void CRedirectDlg::OnBnClickedButtonTestCx()
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
	//TEST Connect VMware Horizon
	theApp.Exec_Citrix_XenApp(m_strCitrix_AppName,strURL);
}


void CRedirectDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}

void CRedirectDlg::OnBnClickedRadioRdp()
{
	if(((CButton*)GetDlgItem(IDC_RADIO_RDP))->GetCheck() == 1)
	{
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(FALSE);
		EnableCustomPath(FALSE);
		EnableVMwareHorizon(FALSE);
		EnableRDPWindow(TRUE);
		Change_RDP_App_DesktopState();
		EnableCitrix(FALSE);
	}
}

void CRedirectDlg::OnBnClickedRadioVmw()
{
	if(((CButton*)GetDlgItem(IDC_RADIO_VMW))->GetCheck() == 1)
	{
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(FALSE);
		EnableCustomPath(FALSE);
		EnableVMwareHorizon(TRUE);
		EnableRDPWindow(FALSE);
		EnableCitrix(FALSE);
	}
}

void CRedirectDlg::OnBnClickedRadioCx()
{
	if(((CButton*)GetDlgItem(IDC_RADIO_CX))->GetCheck() == 1)
	{
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(FALSE);
		EnableCustomPath(FALSE);
		EnableVMwareHorizon(FALSE);
		EnableRDPWindow(FALSE);
		EnableCitrix(TRUE);
	}
}

void CRedirectDlg::EnableVMwareHorizon(BOOL bFlg)
{
	//VM_Horizon-----------------------------------------------------------------↓
	GetDlgItem(IDC_EDIT_VHOST)->EnableWindow(bFlg);
	GetDlgItem(IDC_EDIT_VAPP)->EnableWindow(bFlg);
	GetDlgItem(IDC_BUTTON_TEST_V)->EnableWindow(bFlg);
}

void CRedirectDlg::EnableCitrix(BOOL bFlg)
{
	//Citrix-----------------------------------------------------------------↓
	GetDlgItem(IDC_EDIT_CXAPP)->EnableWindow(bFlg);
	GetDlgItem(IDC_BUTTON_TEST_CX)->EnableWindow(bFlg);
}

void CRedirectDlg::EnableRDPWindow(BOOL bFlg)
{
	//RDP-----------------------------------------------------------------↓
	GetDlgItem(IDC_EDIT1)->EnableWindow(bFlg);
	GetDlgItem(IDC_RADIO1)->EnableWindow(bFlg);
	GetDlgItem(IDC_RADIO2)->EnableWindow(bFlg);
	GetDlgItem(IDC_EDIT2)->EnableWindow(bFlg);
	GetDlgItem(IDC_EDIT3)->EnableWindow(bFlg);
	GetDlgItem(IDC_EDIT4)->EnableWindow(bFlg);
	GetDlgItem(IDC_CHECK1)->EnableWindow(bFlg);
	GetDlgItem(IDC_CHECK2)->EnableWindow(bFlg);
	GetDlgItem(IDC_CHECK3)->EnableWindow(bFlg);

	GetDlgItem(IDC_BUTTON_IE)->EnableWindow(bFlg);
	GetDlgItem(IDC_BUTTON_FF)->EnableWindow(bFlg);
	GetDlgItem(IDC_BUTTON_GC)->EnableWindow(bFlg);
	GetDlgItem(IDC_BUTTON_CLR)->EnableWindow(bFlg);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(bFlg);
	GetDlgItem(IDC_BUTTON_ST)->EnableWindow(bFlg);
}

void CRedirectDlg::EnableCustomPath(BOOL bFlg)
{
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bFlg);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bFlg);
}
void CRedirectDlg::OnBnClickedButtonFdlg()
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

void CRedirectDlg::OnBnClickedRadioLdb()
{
	if(((CButton*)GetDlgItem(IDC_RADIO_LDB))->GetCheck() == 1)
	{
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(TRUE);
		EnableCustomPath(FALSE);
		EnableVMwareHorizon(FALSE);
		EnableRDPWindow(FALSE);
		EnableCitrix(FALSE);
	}
}

void CRedirectDlg::OnBnClickedRadioLie()
{
	if(((CButton*)GetDlgItem(IDC_RADIO_LIE))->GetCheck() == 1)
	{
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(TRUE);
		EnableCustomPath(FALSE);
		EnableVMwareHorizon(FALSE);
		EnableRDPWindow(FALSE);
		EnableCitrix(FALSE);
	}
}

void CRedirectDlg::OnBnClickedRadioLff()
{
	if(((CButton*)GetDlgItem(IDC_RADIO_LFF))->GetCheck() == 1)
	{
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(TRUE);
		EnableCustomPath(FALSE);
		EnableVMwareHorizon(FALSE);
		EnableRDPWindow(FALSE);
		EnableCitrix(FALSE);
	}
}

void CRedirectDlg::OnBnClickedRadioLch()
{
	if(((CButton*)GetDlgItem(IDC_RADIO_LCH))->GetCheck() == 1)
	{
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(TRUE);
		EnableCustomPath(FALSE);
		EnableVMwareHorizon(FALSE);
		EnableRDPWindow(FALSE);
		EnableCitrix(FALSE);
	}
}

void CRedirectDlg::OnBnClickedRadioLeg()
{
	if(((CButton*)GetDlgItem(IDC_RADIO_LEG))->GetCheck() == 1)
	{
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(TRUE);
		EnableCustomPath(FALSE);
		EnableVMwareHorizon(FALSE);
		EnableRDPWindow(FALSE);
		EnableCitrix(FALSE);
	}
}

void CRedirectDlg::OnBnClickedRadioLcb()
{
	if(((CButton*)GetDlgItem(IDC_RADIO_LCB))->GetCheck() == 1)
	{
		GetDlgItem(IDC_BUTTON_TEST_L)->EnableWindow(TRUE);
		EnableCustomPath(TRUE);
		EnableVMwareHorizon(FALSE);
		EnableRDPWindow(FALSE);
		EnableCitrix(FALSE);
	}
}

void CRedirectDlg::OnBnClickedButtonTestL()
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

//RemoteAPP
void CRedirectDlg::OnBnClickedRadio1()
{
	EnableRDPWindow(TRUE);
	Change_RDP_App_DesktopState();
}

//公開デスクトップ
void CRedirectDlg::OnBnClickedRadio2()
{
	EnableRDPWindow(TRUE);
	Change_RDP_App_DesktopState();
}

void CRedirectDlg::Change_RDP_App_DesktopState()
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


void CRedirectDlg::OnBnClickedKeytestbutton()
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

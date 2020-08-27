#include "stdafx.h"
#include "ThinBridge.h"
#include "ProgressDlg.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(ProgressDlg, CDialog)

ProgressDlg::ProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ProgressDlg::IDD, pParent)
{
	m_iSolutionType=PROC_RDP;
}

ProgressDlg::~ProgressDlg()
{
}

void ProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_Prg);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_Image);
}

void ProgressDlg::SetMsg(CString str)
{
	SetDlgItemText(IDC_STATIC_MSG,str);
}


BEGIN_MESSAGE_MAP(ProgressDlg, CDialog)
	ON_BN_CLICKED(IDOK, &ProgressDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &ProgressDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


void ProgressDlg::OnBnClickedOk()
{
	//CDialog::OnOK();
}


void ProgressDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}


BOOL ProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CIconHelper ICoHelper;
	if(m_iSolutionType==PROC_LDefaultBrowser)
	{
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
	}
	else if(m_iSolutionType==PROC_LIE)
	{
		ICoHelper=theApp.LoadIcon(IDI_ICON_IE);
	}
	else if(m_iSolutionType==PROC_LFirefox)
	{
		ICoHelper=theApp.LoadIcon(IDI_ICON_Firefox);
	}
	else if(m_iSolutionType==PROC_LChrome)
	{
		ICoHelper=theApp.LoadIcon(IDI_ICON_Chrome);
	}
	else if(m_iSolutionType==PROC_LEdge)
	{
		ICoHelper=theApp.LoadIcon(IDI_ICON_Edge);
	}
	else if(m_iSolutionType==PROC_LCustom)
	{
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
	}
	else if(m_iSolutionType==PROC_RDP)
	{
		ICoHelper=theApp.LoadIcon(IDI_ICON_RDP);
	}
	else if(m_iSolutionType==PROC_VMWARE)
	{
		ICoHelper=theApp.LoadIcon(IDI_ICON_VMH);
	}
	else if(m_iSolutionType==PROC_CITRIX)
	{
		ICoHelper=theApp.LoadIcon(IDI_ICON_Ctrix);
	}
	else
	{
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
	}
	m_Image.SetIcon(ICoHelper);

	//::SendMessage(GetDlgItem(IDC_PROGRESS1)->m_hWnd,PBM_SETMARQUEE, TRUE, 0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

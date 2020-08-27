#include "stdafx.h"
#include "ThinBridge.h"
#include "DlgMsgBox.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CDlgMsgBox, CDialogEx)

CDlgMsgBox::CDlgMsgBox(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgMsgBox::IDD, pParent)
{

}

CDlgMsgBox::~CDlgMsgBox()
{
}

void CDlgMsgBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
}


BEGIN_MESSAGE_MAP(CDlgMsgBox, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgMsgBox::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgMsgBox::OnBnClickedCancel)
END_MESSAGE_MAP()


void CDlgMsgBox::OnBnClickedOk()
{
	CDialogEx::OnOK();
}


void CDlgMsgBox::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


BOOL CDlgMsgBox::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	if(!m_strTitle.IsEmpty())
	{
		this->SetWindowText(m_strTitle);
	}
	m_Edit.LimitText(1024*1024*5);
	m_Edit.SetWindowText(m_strMsg);
	m_Edit.SetFocus();
	int SelPosS=0;
	int SelPosE=0;
	m_Edit.PostMessage(EM_SETSEL, (WPARAM)SelPosS, (LPARAM)SelPosE);
	//SendMessage(（HWND)IDC_EDIT, EM_LIMITTEXT, 0, (LPARAM)1024*1024*5);
	return FALSE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


BOOL CDlgMsgBox::PreTranslateMessage(MSG* pMsg)
{
	ASSERT(pMsg != NULL);
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);
	CString strLine;

	if(pMsg->hwnd==m_Edit.m_hWnd)
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			if(pMsg->wParam == 'A')
			{
				if(::GetKeyState(VK_CONTROL)<0)
				{
					m_Edit.PostMessage(EM_SETSEL, (WPARAM)0, (LPARAM)-1);
					return TRUE;
				}
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

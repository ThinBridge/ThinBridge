
// CustomLauncherDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "CustomLauncher.h"
#include "CustomLauncherDlg.h"
#include "afxdialogex.h"
#include "DlgURL.h"
#include "locale.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
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


// CCustomLauncherDlg ダイアログ




CCustomLauncherDlg::CCustomLauncherDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCustomLauncherDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCustomLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCustomLauncherDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CCustomLauncherDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CCustomLauncherDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CCustomLauncherDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CCustomLauncherDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CCustomLauncherDlg メッセージ ハンドラー

BOOL CCustomLauncherDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
//	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
//	ASSERT(IDM_ABOUTBOX < 0xF000);

	//CMenu* pSysMenu = GetSystemMenu(FALSE);
	//if (pSysMenu != NULL)
	//{
	//	BOOL bNameValid;
	//	CString strAboutMenu;
	//	bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
	//	ASSERT(bNameValid);
	//	if (!strAboutMenu.IsEmpty())
	//	{
	//		pSysMenu->AppendMenu(MF_SEPARATOR);
	//		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	//	}
	//}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	int iKeyCombination=0;
	iKeyCombination=theApp.m_iKeyCombination;

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

	SetDlgItemText(IDC_EDIT1,theApp.m_strCommand1);
	SetDlgItemText(IDC_EDIT2,theApp.m_strCommand2);

	return FALSE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CCustomLauncherDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
//	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
//	{
//		CAboutDlg dlgAbout;
//		dlgAbout.DoModal();
//	}
//	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CCustomLauncherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CCustomLauncherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCustomLauncherDlg::OnBnClickedOk()
{
	int iRet=0;
	iRet = ::MessageBox(this->m_hWnd,_T("設定を保存しますか？"),theApp.m_strExeNameNoExt,MB_ICONQUESTION | MB_OKCANCEL);
	if(iRet == IDCANCEL)
		return;

	CString strCommand1;
	CString strCommand2;

	GetDlgItemText(IDC_EDIT1,strCommand1);
	GetDlgItemText(IDC_EDIT2,strCommand2);

	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	if(strCommand1.IsEmpty())
	{
		CString strMsg=_T("コマンドライン1を入力して下さい。");
		::MessageBox(NULL,strMsg,theApp.m_strExeNameNoExt,MB_ICONERROR);
		GetDlgItem(IDC_EDIT1)->SetFocus();
		return;
	}

	strCommand2.TrimLeft();
	strCommand2.TrimRight();
	theApp.m_strCommand1=strCommand1;
	theApp.m_strCommand2=strCommand2;


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
	theApp.m_iKeyCombination=iKeyCombination;

	_wsetlocale(LC_ALL, _T("jpn")); 
	CStdioFile out;
	if(out.Open(theApp.m_strSetting_FileFullPath,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
	{
		//command1
		CString strWriteData;
		strWriteData.Format(_T("Command1=%s\n"),theApp.m_strCommand1);
		out.WriteString(strWriteData);

		//command2
		strWriteData.Format(_T("Command2=%s\n"),theApp.m_strCommand2);
		out.WriteString(strWriteData);

		//表示制限キーコンビネーション
		strWriteData.Format(_T("KeyCombination=%d\n"),theApp.m_iKeyCombination);
		out.WriteString(strWriteData);
		out.Close();
	}
	else
	{
		CString strSaveMsg;
		strSaveMsg.Format(_T("設定ファイルの保存に失敗しました。\n\n別のプログラムがファイルを開いているか、書込権限が不足しています。操作を完了できません。ファイルを閉じてから再実行してください。\n\n%s"),theApp.m_strSetting_FileFullPath);
		::MessageBox(this->m_hWnd,strSaveMsg,theApp.m_strExeNameNoExt,MB_OK|MB_ICONERROR );
	}
	
//	CDialogEx::OnOK();
}


void CCustomLauncherDlg::OnBnClickedCancel()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDialogEx::OnCancel();
}


void CCustomLauncherDlg::OnBnClickedButton1()
{
	CString strCommand1;
	CString strCommand2;

	GetDlgItemText(IDC_EDIT1,strCommand1);
	GetDlgItemText(IDC_EDIT2,strCommand2);

	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	if(strCommand1.IsEmpty())
	{
		CString strMsg=_T("コマンドライン1を入力して下さい。");
		::MessageBox(NULL,strMsg,theApp.m_strExeNameNoExt,MB_ICONERROR);
		GetDlgItem(IDC_EDIT1)->SetFocus();
		return;
	}
	strCommand2.TrimLeft();
	strCommand2.TrimRight();


	CDlgURL Dlg;
	Dlg.DoModal();
	CString strURL;
	strURL=Dlg.m_strURL;
	theApp.CustomBrowserStart(strCommand1,strCommand2,strURL);
}


void CCustomLauncherDlg::OnBnClickedButton2()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT1,fileDlg.GetPathName());
	}
}

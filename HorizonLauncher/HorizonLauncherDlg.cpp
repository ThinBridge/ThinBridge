
// HorizonLauncherDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "HorizonLauncher.h"
#include "HorizonLauncherDlg.h"
#include "afxdialogex.h"
#include "DlgURL.h"
#include "locale.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHorizonLauncherDlg ダイアログ




CHorizonLauncherDlg::CHorizonLauncherDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHorizonLauncherDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHorizonLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHorizonLauncherDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CHorizonLauncherDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CHorizonLauncherDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CHorizonLauncherDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CHorizonLauncherDlg メッセージ ハンドラー

BOOL CHorizonLauncherDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
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

	SetDlgItemText(IDC_EDIT_VHOST,theApp.m_strServerName);
	SetDlgItemText(IDC_EDIT_VAPP,theApp.m_strAppName);

	return FALSE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CHorizonLauncherDlg::OnPaint()
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
HCURSOR CHorizonLauncherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHorizonLauncherDlg::OnBnClickedOk()
{
	int iRet=0;
	iRet = ::MessageBox(this->m_hWnd,_T("設定を保存しますか？"),theApp.m_strExeNameNoExt,MB_ICONQUESTION | MB_OKCANCEL);
	if(iRet == IDCANCEL)
		return;

	CString strServerName;
	CString strAppName;

	GetDlgItemText(IDC_EDIT_VHOST,strServerName);
	GetDlgItemText(IDC_EDIT_VAPP,strAppName);

	strServerName.TrimLeft();
	strServerName.TrimRight();
	strAppName.TrimLeft();
	strAppName.TrimRight();
	if(strServerName.IsEmpty())
	{
		CString strMsg=_T("Connectionサーバーを入力して下さい。");
		::MessageBox(NULL,strMsg,theApp.m_strExeNameNoExt,MB_ICONERROR);
		GetDlgItem(IDC_EDIT_VHOST)->SetFocus();
		return;
	}
	if(strAppName.IsEmpty())
	{
		CString strMsg=_T("アプリ名を入力して下さい。");
		::MessageBox(NULL,strMsg,theApp.m_strExeNameNoExt,MB_ICONERROR);
		GetDlgItem(IDC_EDIT_VAPP)->SetFocus();
		return;
	}
	theApp.m_strServerName = strServerName;
	theApp.m_strAppName = strAppName;

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
		//ServerName
		CString strWriteData;
		strWriteData.Format(_T("ServerName=%s\n"),theApp.m_strServerName);
		out.WriteString(strWriteData);

		//AppName
		strWriteData.Format(_T("AppName=%s\n"),theApp.m_strAppName);
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


void CHorizonLauncherDlg::OnBnClickedCancel()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDialogEx::OnCancel();
}


void CHorizonLauncherDlg::OnBnClickedButton1()
{
	CString strServerName;
	CString strAppName;

	GetDlgItemText(IDC_EDIT_VHOST,strServerName);
	GetDlgItemText(IDC_EDIT_VAPP,strAppName);

	strServerName.TrimLeft();
	strServerName.TrimRight();
	strAppName.TrimLeft();
	strAppName.TrimRight();

	if(strServerName.IsEmpty())
	{
		CString strMsg=_T("Connectionサーバーを入力して下さい。");
		::MessageBox(NULL,strMsg,theApp.m_strExeNameNoExt,MB_ICONERROR);
		GetDlgItem(IDC_EDIT_VHOST)->SetFocus();
		return;
	}

	if(strAppName.IsEmpty())
	{
		CString strMsg=_T("アプリ名を入力して下さい。");
		::MessageBox(NULL,strMsg,theApp.m_strExeNameNoExt,MB_ICONERROR);
		GetDlgItem(IDC_EDIT_VAPP)->SetFocus();
		return;
	}

	CDlgURL Dlg;
	Dlg.DoModal();
	CString strURL;
	strURL=Dlg.m_strURL;
	theApp.CustomBrowserStart(strServerName,strAppName,strURL);
}

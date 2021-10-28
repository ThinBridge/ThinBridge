
// ThinBridgeRuleUpdaterDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ThinBridgeRuleUpdater.h"
#include "ThinBridgeRuleUpdaterDlg.h"
#include "afxdialogex.h"
#include "locale.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CThinBridgeRuleUpdaterDlg ダイアログ
CThinBridgeRuleUpdaterDlg::CThinBridgeRuleUpdaterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CThinBridgeRuleUpdaterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CThinBridgeRuleUpdaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CThinBridgeRuleUpdaterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CThinBridgeRuleUpdaterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CThinBridgeRuleUpdaterDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CThinBridgeRuleUpdaterDlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_WRITEINI, &CThinBridgeRuleUpdaterDlg::OnBnClickedButtonWriteini)
	ON_BN_CLICKED(IDC_BUTTON_WRITEINI_NOW, &CThinBridgeRuleUpdaterDlg::OnBnClickedButtonWriteiniNow)
	ON_BN_CLICKED(IDC_BUTTON1, &CThinBridgeRuleUpdaterDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CThinBridgeRuleUpdaterDlg メッセージ ハンドラー

BOOL CThinBridgeRuleUpdaterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	SetDlgItemText(IDC_EDIT1,theApp.SettingConf.m_strConfigServerURL1);

	//GPOで設定されている場合は、表示とテスト実行のみ。
	if (theApp.SettingConf.IsGPO())
	{
		GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_WRITEINI)->EnableWindow(FALSE);
		SetDlgItemText(IDC_STATIC_MSG,_T("※管理者により設定されているため変更不可"));

	}

	return FALSE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CThinBridgeRuleUpdaterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CThinBridgeRuleUpdaterDlg::OnPaint()
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
HCURSOR CThinBridgeRuleUpdaterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CThinBridgeRuleUpdaterDlg::OnBnClickedButtonTest()
{
	CConfData SettingConfData;

	SettingConfData.Copy(&theApp.SettingConf);
	CString strCommand1;
	GetDlgItemText(IDC_EDIT1, strCommand1);

	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	SettingConfData.m_strConfigServerURL1 = strCommand1;

	CWaitCursor cur;
	CString strOutPutDataString;
	strOutPutDataString = theApp.WriteThinBridgeBHO(&SettingConfData,FALSE,TRUE);
	CDlgMsgBox MsgBox;
	MsgBox.m_strMsg = strOutPutDataString;
	MsgBox.m_strTitle = _T("接続＆取得データテスト結果");
	MsgBox.DoModal();
	return;
}

void CThinBridgeRuleUpdaterDlg::OnBnClickedOk()
{
	int iRet=0;
	iRet = ::MessageBox(this->m_hWnd,_T("設定のみを保存しますか？"),theApp.m_strExeNameNoExt,MB_ICONQUESTION | MB_OKCANCEL);
	if(iRet == IDCANCEL)
		return;

	CString strCommand1;

	GetDlgItemText(IDC_EDIT1,strCommand1);

	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	theApp.SettingConf.m_strConfigServerURL1 =strCommand1;

	if(!theApp.SettingConf.WriteSetting(theApp.m_strSetting_FileFullPath))
	{
		CString strSaveMsg;
		strSaveMsg.Format(_T("設定ファイルの保存に失敗しました。\n\n別のプログラムがファイルを開いているか、書込権限が不足しています。操作を完了できません。ファイルを閉じてから再実行してください。\n\n%s"),theApp.m_strSetting_FileFullPath);
		::MessageBox(this->m_hWnd,strSaveMsg,theApp.m_strExeNameNoExt,MB_OK|MB_ICONERROR );
	}
}

void CThinBridgeRuleUpdaterDlg::OnBnClickedButtonWriteini()
{
	int iRet = 0;
	iRet = ::MessageBox(this->m_hWnd, _T("ThinBridgeBHO.iniの更新と設定保存を行いますか？"), theApp.m_strExeNameNoExt, MB_ICONQUESTION | MB_OKCANCEL);
	if (iRet == IDCANCEL)
		return;
	CWaitCursor cur;

	CString strCommand1;

	GetDlgItemText(IDC_EDIT1, strCommand1);

	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	theApp.SettingConf.m_strConfigServerURL1 = strCommand1;

	if (!theApp.SettingConf.WriteSetting(theApp.m_strSetting_FileFullPath))
	{
		CString strSaveMsg;
		strSaveMsg.Format(_T("設定ファイルの保存に失敗しました。\n\n別のプログラムがファイルを開いているか、書込権限が不足しています。操作を完了できません。ファイルを閉じてから再実行してください。\n\n%s"), theApp.m_strSetting_FileFullPath);
		::MessageBox(this->m_hWnd, strSaveMsg, theApp.m_strExeNameNoExt, MB_OK | MB_ICONERROR);
		return;
	}

	CString strOutPutDataString;
	strOutPutDataString= theApp.WriteThinBridgeBHO(&theApp.SettingConf,TRUE,TRUE);
	if (strOutPutDataString.IsEmpty())
	{
		CString strSaveMsg;
		strSaveMsg.Format(_T("ThinBridgeBHO.iniファイルの保存に失敗しました。\n\n別のプログラムがファイルを開いているか、書込権限が不足しています。操作を完了できません。ファイルを閉じてから再実行してください。\n\n%s"), theApp.m_strThinBridgeBHOFileFullPath);
		::MessageBox(this->m_hWnd, strSaveMsg, theApp.m_strExeNameNoExt, MB_OK | MB_ICONERROR);
		return;
	}
	else
	{
		CDlgMsgBox MsgBox;
		CString strSaveMsg;
		if (strOutPutDataString.Find(_T("【同一内容 変更なし】")) == 0)
		{
			CString strOutData;
			for (int j = 0; j < theApp.m_strArrayBHODataOrg.GetSize(); j++)
			{
				strOutData += theApp.m_strArrayBHODataOrg.GetAt(j);
				strOutData += _T("\r\n");
			}
			strSaveMsg.Format(_T("%s\r\n%s"), strOutPutDataString, strOutData);
			MsgBox.m_strMsg = strSaveMsg;
			MsgBox.m_strTitle = _T("設定ファイル保存 同一内容");
		}
		else if (strOutPutDataString.Find(_T("エラー：")) == 0)
		{
			strSaveMsg = strOutPutDataString;
			MsgBox.m_strMsg = strSaveMsg;
			MsgBox.m_strTitle = _T("設定ファイル保存 エラー");
		}
		else
		{
			strSaveMsg = strOutPutDataString;
			MsgBox.m_strMsg = strSaveMsg;
			MsgBox.m_strTitle = _T("設定ファイル保存 成功");
		}
		CStdioFile out;
		CTime time = CTime::GetCurrentTime();
		if (out.Open(theApp.m_strExecLogFileFullPath, CFile::modeWrite | CFile::shareDenyNone | CFile::modeCreate | CFile::modeNoTruncate))
		{
			TRY
			{
				out.SeekToEnd();
				out.WriteString(_T("\n手動更新//////////////////////////////////////////////////////////////////////////////////////////////////\n"));
				out.WriteString(time.Format(_T("%Y-%m-%d %H:%M:%S")));
				out.WriteString(_T("\n処理結果："));
				out.WriteString(theApp.GetErrorMsg());
				out.WriteString(_T("\n"));
				strSaveMsg.Replace(_T("\r\n"), _T("\n"));
				out.WriteString(strSaveMsg);
				out.Close();
			}
				CATCH(CFileException, eP) {}
			END_CATCH
		}
		MsgBox.DoModal();
	}
}

void CThinBridgeRuleUpdaterDlg::OnBnClickedButtonWriteiniNow()
{
	int iRet = 0;
	iRet = ::MessageBox(this->m_hWnd, _T("ThinBridgeBHO.iniの更新を行いますか？"), theApp.m_strExeNameNoExt, MB_ICONQUESTION | MB_OKCANCEL);
	if (iRet == IDCANCEL)
		return;
	CWaitCursor cur;

	CString strCommand1;

	GetDlgItemText(IDC_EDIT1, strCommand1);

	strCommand1.TrimLeft();
	strCommand1.TrimRight();
	theApp.SettingConf.m_strConfigServerURL1 = strCommand1;

	CString strOutPutDataString;
	strOutPutDataString = theApp.WriteThinBridgeBHO(&theApp.SettingConf, TRUE, TRUE);
	if (strOutPutDataString.IsEmpty())
	{
		CString strSaveMsg;
		strSaveMsg.Format(_T("ThinBridgeBHO.iniファイルの保存に失敗しました。\n\n別のプログラムがファイルを開いているか、書込権限が不足しています。操作を完了できません。ファイルを閉じてから再実行してください。\n\n%s"), theApp.m_strThinBridgeBHOFileFullPath);
		::MessageBox(this->m_hWnd, strSaveMsg, theApp.m_strExeNameNoExt, MB_OK | MB_ICONERROR);
		return;
	}
	else
	{
		CDlgMsgBox MsgBox;
		CString strSaveMsg;
		if (strOutPutDataString.Find(_T("【同一内容 変更なし】")) == 0)
		{
			CString strOutData;
			for (int j = 0; j < theApp.m_strArrayBHODataOrg.GetSize(); j++)
			{
				strOutData += theApp.m_strArrayBHODataOrg.GetAt(j);
				strOutData += _T("\r\n");
			}
			strSaveMsg.Format(_T("%s\r\n%s"), strOutPutDataString, strOutData);
			MsgBox.m_strMsg = strSaveMsg;
			MsgBox.m_strTitle = _T("設定ファイル保存 同一内容");
		}
		else if (strOutPutDataString.Find(_T("エラー：")) == 0)
		{
			strSaveMsg= strOutPutDataString;
			MsgBox.m_strMsg = strSaveMsg;
			MsgBox.m_strTitle = _T("設定ファイル保存 エラー");
		}
		else
		{
			strSaveMsg = strOutPutDataString;
			MsgBox.m_strMsg = strSaveMsg;
			MsgBox.m_strTitle = _T("設定ファイル保存 成功");
		}
		CStdioFile out;
		CTime time = CTime::GetCurrentTime();
		if (out.Open(theApp.m_strExecLogFileFullPath, CFile::modeWrite | CFile::shareDenyNone | CFile::modeCreate | CFile::modeNoTruncate))
		{
			TRY
			{
				out.SeekToEnd();
				out.WriteString(_T("\n手動更新//////////////////////////////////////////////////////////////////////////////////////////////////\n"));
				out.WriteString(time.Format(_T("%Y-%m-%d %H:%M:%S")));
				out.WriteString(_T("\n処理結果："));
				out.WriteString(theApp.GetErrorMsg());
				out.WriteString(_T("\n"));
				strSaveMsg.Replace(_T("\r\n"), _T("\n"));
				out.WriteString(strSaveMsg);
				out.Close();
			}
				CATCH(CFileException, eP) {}
			END_CATCH
		}
		MsgBox.DoModal();
	}
}

void CThinBridgeRuleUpdaterDlg::OnBnClickedButton1()
{
	CDlgSrv Dlg;
	Dlg.m_strConfigServerURL2 = theApp.SettingConf.m_strConfigServerURL2;
	Dlg.m_strConfigServerURL3 = theApp.SettingConf.m_strConfigServerURL3;
	Dlg.m_strConfigServerURL4 = theApp.SettingConf.m_strConfigServerURL4;
	Dlg.m_strConfigServerURL5 = theApp.SettingConf.m_strConfigServerURL5;
	Dlg.m_strConfigServerURL6 = theApp.SettingConf.m_strConfigServerURL6;

	Dlg.m_iInterval = theApp.SettingConf.m_iInterval;
	Dlg.m_iServerConnectionRule = theApp.SettingConf.m_iServerConnectionRule;

	if (IDOK == Dlg.DoModal())
	{
		theApp.SettingConf.m_strConfigServerURL2 = Dlg.m_strConfigServerURL2;
		theApp.SettingConf.m_strConfigServerURL3 = Dlg.m_strConfigServerURL3;
		theApp.SettingConf.m_strConfigServerURL4 = Dlg.m_strConfigServerURL4;
		theApp.SettingConf.m_strConfigServerURL5 = Dlg.m_strConfigServerURL5;
		theApp.SettingConf.m_strConfigServerURL6 = Dlg.m_strConfigServerURL6;

		theApp.SettingConf.m_iInterval= Dlg.m_iInterval;
		theApp.SettingConf.m_iServerConnectionRule = Dlg.m_iServerConnectionRule;
	}
}


void CThinBridgeRuleUpdaterDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

// CDlgMsgBox ダイアログ
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


// CDlgMsgBox メッセージ ハンドラー
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
	if (!m_strTitle.IsEmpty())
	{
		this->SetWindowText(m_strTitle);
	}
	m_Edit.LimitText(1024 * 1024 * 5);
	m_Edit.SetWindowText(m_strMsg);
	m_Edit.SetFocus();
	int SelPosS = 0;
	int SelPosE = 0;
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

	if (pMsg->hwnd == m_Edit.m_hWnd)
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			if (pMsg->wParam == 'A')
			{
				if (::GetKeyState(VK_CONTROL) < 0)
				{
					m_Edit.PostMessage(EM_SETSEL, (WPARAM)0, (LPARAM)-1);
					return TRUE;
				}
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}




// CDlgSrv ダイアログ

IMPLEMENT_DYNAMIC(CDlgSrv, CDialogEx)

CDlgSrv::CDlgSrv(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CDlgSrv::~CDlgSrv()
{
}

void CDlgSrv::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSrv, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgSrv::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSrv メッセージ ハンドラー


BOOL CDlgSrv::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetDlgItemText(IDC_EDIT2, m_strConfigServerURL2);
	SetDlgItemText(IDC_EDIT3, m_strConfigServerURL3);
	SetDlgItemText(IDC_EDIT4, m_strConfigServerURL4);
	SetDlgItemText(IDC_EDIT5, m_strConfigServerURL5);
	SetDlgItemText(IDC_EDIT6, m_strConfigServerURL6);

	if(m_iServerConnectionRule==0)
	{
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(1);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(1);
	}
	SetDlgItemInt(IDC_EDIT_INT, m_iInterval);

	//GPOで設定されている場合は、表示のみ。
	if (theApp.SettingConf.IsGPO())
	{
		GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT3)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT4)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT5)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT6)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_INT)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
	return FALSE;
}


void CDlgSrv::OnOK()
{
	//GPOで設定されている場合は、表示のみ。
	if(!theApp.SettingConf.IsGPO())
	{
		GetDlgItemText(IDC_EDIT2, m_strConfigServerURL2);
		GetDlgItemText(IDC_EDIT3, m_strConfigServerURL3);
		GetDlgItemText(IDC_EDIT4, m_strConfigServerURL4);
		GetDlgItemText(IDC_EDIT5, m_strConfigServerURL5);
		GetDlgItemText(IDC_EDIT6, m_strConfigServerURL6);
		if (((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck() == 1)
			m_iServerConnectionRule=0;
		else if (((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck() == 1)
			m_iServerConnectionRule = 1;
		else
			m_iServerConnectionRule = 0;

		m_iInterval=GetDlgItemInt(IDC_EDIT_INT);
		if(m_iInterval<0|| m_iInterval>60)
			m_iInterval=0;
		
	}
	CDialogEx::OnOK();
}


void CDlgSrv::OnBnClickedOk()
{
	OnOK();
}


// ThinBridgeCheckerDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ThinBridgeChecker.h"
#include "ThinBridgeCheckerDlg.h"
#include "afxdialogex.h"
#include "DlgURLEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CThinBridgeCheckerDlg ダイアログ




CThinBridgeCheckerDlg::CThinBridgeCheckerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CThinBridgeCheckerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pMsgDlg=NULL;
}

void CThinBridgeCheckerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
}

BEGIN_MESSAGE_MAP(CThinBridgeCheckerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CThinBridgeCheckerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CThinBridgeCheckerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CThinBridgeCheckerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CThinBridgeCheckerDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON6, &CThinBridgeCheckerDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CThinBridgeCheckerDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CThinBridgeCheckerDlg::OnBnClickedButton8)
END_MESSAGE_MAP()


// CThinBridgeCheckerDlg メッセージ ハンドラー

BOOL CThinBridgeCheckerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(1);

	TCHAR szPath[MAX_PATH+1]={0};
	TCHAR szDrive[_MAX_DRIVE]={0};
	TCHAR szDir[_MAX_DIR]={0};
	TCHAR szFileName[_MAX_FNAME]={0};
	TCHAR szExt[_MAX_EXT]={0};
	GetModuleFileName(AfxGetInstanceHandle(),szPath,MAX_PATH);
	_tsplitpath_s(szPath, szDrive,_MAX_DRIVE, szDir,_MAX_DIR, szFileName,_MAX_FNAME, szExt,_MAX_EXT); 
	m_strExeFolderPath=szDrive;
	m_strExeFolderPath+=szDir;
	m_Edit.LimitText(1024*1024*10);
	return FALSE;
}

void CThinBridgeCheckerDlg::OnPaint()
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

HCURSOR CThinBridgeCheckerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CThinBridgeCheckerDlg::OnBnClickedOk()
{
	Command_WINTOPMOST_T(1);

	_wsetlocale(LC_ALL, _T("jpn")); 

	SetDlgText(NULL);
	if(GetNativeIEProcCnt()>0)
	{
		AfxMessageBox(_T("IEのプロセスが起動しています。お手数ですが情報収集を行う前にIEを終了してください。"),MB_OK|MB_ICONSTOP);
		if (IDOK == AfxMessageBox(_T("IEのプロセスを強制終了します。よろしいですか？"), MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2))
		{
			this->TermIE();
		}
		else
			return;
	}
	RESULT_DATA data={0};
	m_stResultData=data;

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	if(m_pMsgDlg)
	{
		delete m_pMsgDlg;
	}
	m_pMsgDlg=new DlgMsg(this);
	m_pMsgDlg->Create(IDD_DIALOG2);
	m_pMsgDlg->CenterWindow(this);
	m_pMsgDlg->ShowWindow(SW_SHOW);
	SetDlgText(_T("情報収集中...."));
	SetDlgText(_T("しばらくお待ち下さい...."));
	SetDlgText(NULL);

	CString strRet;
	CTime timeDlgC = CTime::GetCurrentTime();

	m_pMsgDlg->SetFuncName(_T("実行環境基本情報"));

	//OS
	SetDlgText(_T("--実行環境基本情報--"));
	TCHAR szPath[MAX_PATH+1]={0};
	GetModuleFileName(AfxGetInstanceHandle(),szPath,MAX_PATH);
	SetDlgText(szPath);
	SetDlgText(GetOSInfo());
	SetDlgText(GetOSKernelVersion());
	SetDlgText(GetTridentVersionStr());

	TCHAR szPCName[128] = { 0 };
	DWORD dPCNameBufLen = 120;
	::GetComputerName(szPCName, &dPCNameBufLen);
	CString strPCName;
	strPCName.Format(_T("ComputerName %s"),szPCName);
	SetDlgText(strPCName);
	SetDlgText(_T("\n"));

	CString strPathVDI;
	strPathVDI.Format(_T("%s%s"), GetThinBridgeBasePath(), _T("ThinBridgeCTXServer.exe"));
	if (!PathFileExists(strPathVDI))
	{
		m_pMsgDlg->SetFuncName(_T("ThinBridgeBHO.dll情報"));
		SetDlgText(_T("--ThinBridgeBHO.dll情報--"));
		SetDlgText(ThinBridgeBHOReg());
		SetDlgText(_T("\n"));

		if(Is64BitWindows())
		{
			m_pMsgDlg->SetFuncName(_T("ThinBridgeBHO64.dll情報"));
			SetDlgText(_T("--ThinBridgeBHO64.dll情報--"));
			SetDlgText(ThinBridgeBHO64Reg());
			SetDlgText(_T("\n"));
		}

		m_pMsgDlg->SetFuncName(_T("ThinBridgeBHO 作成チェック"));
		SetDlgText(_T("--ThinBridgeBHO 作成チェック--"));
		SetDlgText(ThinBridgeBHOCreate());
		SetDlgText(_T("\n"));

		DoMessage();

		m_pMsgDlg->SetFuncName(_T("IEサードパーティ製ブラウザー拡張値チェック"));
		SetDlgText(_T("--IEサードパーティ製ブラウザー拡張値チェック--"));
		SetDlgText(IE3rdBHO());
		SetDlgText(_T("\n"));

		m_pMsgDlg->SetFuncName(_T("IE GPO 設定値チェック"));
		SetDlgText(_T("--IE GPO設定値チェック--"));
		SetDlgText(IE_BHO_PolicieOLD());
		SetDlgText(IE_BHO_PolicieNEW());

		DoMessage();
		m_pMsgDlg->SetFuncName(_T("[アドオンの管理]ThinBridgeアドオンチェック"));
		SetDlgText(_T("--[アドオンの管理]ThinBridgeアドオンチェック--"));
		SetDlgText(IEThinBridgeBlock());
		SetDlgText(_T("\n"));
	}
	DoMessage();
	m_pMsgDlg->SetFuncName(_T("ThinBridge.EXE情報"));
	SetDlgText(_T("--ThinBridge.EXE情報--"));
	SetDlgText(GetThinBridgeEXEVersion());
	if (!PathFileExists(strPathVDI))
		SetDlgText(GetThinBridgeEXEReg());
	SetDlgText(_T("\n"));


	//ThinBridgeモジュールインストール状況
	//path
	//version
	DoMessage();
	m_pMsgDlg->SetFuncName(_T("ThinBridgeインストール情報"));
	SetDlgText(_T("--ThinBridgeインストール情報--"));
	SetDlgText(ThinBridgeInfomationNormal());

	if(((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck() == 1)	
	{
		m_pMsgDlg->SetFuncName(_T("ThinBridgeBHO.ini"));
		SetDlgText(_T("\n"));
		SetDlgText(_T("--ThinBridgeBHO.ini--"));
		CString strFilePath;
		strFilePath=GetThinBridgeBasePath();
		strFilePath+=_T("ThinBridgeBHO.ini");
		CString strTempBuffer;
		CStdioFile in;
		if ( in.Open(strFilePath,CFile::modeRead|CFile::shareDenyNone) )
		{
			CString strTemp;
			SetDlgText(_T("#----------ThinBridgeBHO.ini----------START"));
			strTempBuffer=strFilePath;
			strTempBuffer+=_T("\r\n");
			while(in.ReadString(strTemp))
			{
				strTempBuffer+=strTemp;
				strTempBuffer+=_T("\r\n");
			}
			in.Close();
			if(!strTempBuffer.IsEmpty())
			{
				SetDlgText(strTempBuffer);
				SetDlgText(_T("#----------ThinBridgeBHO.ini----------END"));
				SetDlgText(_T("\n"));
			}
		}
		else
		{
			strRet.Format(_T("*ERROR* ThinBridgeBHO.ini Failed(File not found) [%s]"),strFilePath);
			m_stResultData.m_Error+=1;
			SetDlgText(strRet);
		}
	}
	SetDlgText(_T("\n"));

	m_pMsgDlg->SetFuncName(_T("IE 実行環境詳細情報"));
	SetDlgText(_T("--IE 実行環境詳細情報--"));
	DoMessage();

	//Wait Cursor
	{
		CWaitCursor wait;
		if (!PathFileExists(strPathVDI))
		{
			IEStart();
		}
		DoMessage();

		m_pMsgDlg->SetFuncName(_T("実行環境詳細情報"));
		SetDlgText(_T("--実行環境詳細情報--"));
		DWORD processID=0;
		processID = GetCurrentProcessId();
		SetDlgText(GetAllModules(processID));
		SetDlgText(_T("\n"));

		m_pMsgDlg->SetFuncName(_T("実行環境PROCESS"));
		SetDlgText(_T("--実行環境PROCESS--"));
		strRet = GetNativeProc();
		if(!strRet.IsEmpty())
			SetDlgText(strRet);

		SetDlgText(_T("\n"));
	}
	m_pMsgDlg->ShowWindow(SW_HIDE);
	m_pMsgDlg->DestroyWindow();
	delete m_pMsgDlg;
	m_pMsgDlg=NULL;

	CString strResult;
	strResult.Format(_T("情報収集が完了しました。\r\n  成功：%d件\r\n  警告：%d件\r\n  失敗：%d件\r\n"),m_stResultData.m_Success,m_stResultData.m_Warning,m_stResultData.m_Error);
	CString strD(strResult);

	CString strInit;
	strInit.Format(_T("--収集日時情報--\r\n%s\r\n%s\r\n"),timeDlgC.Format(_T("%Y-%m-%d %X")),strD);

	CString strPrevData;
	GetDlgItemText(IDC_EDIT1,strPrevData);
	strInit += strPrevData;
	SetDlgItemText(IDC_EDIT1,strInit);
	strD.Replace(_T("\r\n"),_T("\n"));
	AfxMessageBox(strD,MB_OK|MB_ICONINFORMATION);
	Command_WINTOPMOST_T(0);
	GetDlgItem(IDOK)->EnableWindow(TRUE);

//	CDialogEx::OnOK();
}
//OS環境情報
//IE情報
//ThinBridgeBHO.dll情報
//ThinBridge.exe情報
//ThinBridge設定
//
CString CThinBridgeCheckerDlg::ExecChk()
{
	_wsetlocale(LC_ALL, _T("jpn"));
	CString strRet;
	CString strAllLog;

	RESULT_DATA data = { 0 };
	m_stResultData = data;

	CTime timeDlgC = CTime::GetCurrentTime();
	//OS
	strAllLog+=_T("--実行環境基本情報--\r\n");
	TCHAR szPath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(AfxGetInstanceHandle(), szPath, MAX_PATH);
	strAllLog += szPath;
	strAllLog += _T("\r\n");
	strAllLog += GetOSInfo();
	strAllLog += _T("\r\n");
	strAllLog += GetOSKernelVersion();
	strAllLog += _T("\r\n");
	TCHAR szPCName[128] = { 0 };
	DWORD dPCNameBufLen = 120;
	::GetComputerName(szPCName, &dPCNameBufLen);
	CString strPCName;
	strPCName.Format(_T("ComputerName %s"), szPCName);
	strAllLog += strPCName;
	strAllLog += _T("\r\n");

	strAllLog += _T("\r\n");
	CString strPathVDI;
	strPathVDI.Format(_T("%s%s"), GetThinBridgeBasePath(), _T("ThinBridgeCTXServer.exe"));
	if (!PathFileExists(strPathVDI))
	{
		strAllLog += _T("--ThinBridgeBHO.dll情報--\r\n");
		strAllLog += ThinBridgeBHOReg();
		strAllLog += _T("\r\n");
		strAllLog += _T("\r\n");

		if (Is64BitWindows())
		{
			strAllLog += _T("--ThinBridgeBHO64.dll情報--\r\n");
			strAllLog += ThinBridgeBHO64Reg();
			strAllLog += _T("\r\n");;
		}

		strAllLog += _T("\r\n");
		strAllLog += _T("--ThinBridgeBHO 作成チェック--\r\n");
		strAllLog += ThinBridgeBHOCreate();
		strAllLog += _T("\r\n");;

		strAllLog += _T("\r\n");
		strAllLog += _T("--IEサードパーティ製ブラウザー拡張値チェック--\r\n");
		strAllLog += IE3rdBHO();
		strAllLog += _T("\r\n");;

		strAllLog += _T("\r\n");
		strAllLog += _T("--IE GPO設定値チェック--\r\n");
		strAllLog += IE_BHO_PolicieOLD();
		strAllLog += IE_BHO_PolicieNEW();

		strAllLog += _T("\r\n");
		strAllLog += _T("--[アドオンの管理]ThinBridgeアドオンチェック--\r\n");
		strAllLog += IEThinBridgeBlock();
		strAllLog += _T("\r\n");;
	}
	strAllLog += _T("\r\n");
	strAllLog += _T("--ThinBridge.EXE情報--\r\n");
	strAllLog += GetThinBridgeEXEVersion();
	if (!PathFileExists(strPathVDI))
		strAllLog += GetThinBridgeEXEReg();
	strAllLog += _T("\r\n");;

	//ThinBridgeモジュールインストール状況
	//path
	//version
	strAllLog += _T("\r\n");
	strAllLog += _T("--ThinBridgeインストール情報--\r\n");
	strAllLog += ThinBridgeInfomationNormal();
	strAllLog += _T("\r\n");;
	strAllLog += _T("--ThinBridgeBHO.ini--\r\n");
	CString strFilePath;
	strFilePath = GetThinBridgeBasePath();
	strFilePath += _T("ThinBridgeBHO.ini");
	CString strTempBuffer;
	CStdioFile in;
	if (in.Open(strFilePath, CFile::modeRead | CFile::shareDenyNone))
	{
		CString strTemp;
		strAllLog += _T("\r\n");
		strAllLog += _T("#----------ThinBridgeBHO.ini----------START\r\n");
		strTempBuffer = strFilePath;
		strTempBuffer += _T("\r\n");
		while (in.ReadString(strTemp))
		{
			strTempBuffer += strTemp;
			strTempBuffer += _T("\r\n");
		}
		in.Close();
		if (!strTempBuffer.IsEmpty())
		{
			strAllLog += strTempBuffer;
			strAllLog += _T("#----------ThinBridgeBHO.ini----------END");
			strAllLog += _T("\r\n");;
		}
	}
	else
	{
		strRet.Format(_T("*WARNING* ThinBridgeBHO.ini Failed(File not found) [%s]"), strFilePath);
		m_stResultData.m_Warning += 1;
		strAllLog += strRet;
	}
	strAllLog += _T("\r\n");;
	CString strResult;
	strResult.Format(_T("情報収集完了\r\n  成功：%d件\r\n  警告：%d件\r\n  失敗：%d件\r\n"), m_stResultData.m_Success, m_stResultData.m_Warning, m_stResultData.m_Error);
	CString strD(strResult);

	CString strInit;
	strInit.Format(_T("--収集日時情報--\r\n%s\r\n%s\r\n"), timeDlgC.Format(_T("%Y-%m-%d %X")), strD);

	CString strRetVal;
	strRetVal = strInit;
	strRetVal += strAllLog;
	return strRetVal;

}
void CThinBridgeCheckerDlg::OnBnClickedButton1()
{
	this->m_Edit.SetSel(0,-1);
	this->m_Edit.Copy();
	AfxMessageBox(_T("クリップボードにコピーしました。"),MB_OK|MB_ICONINFORMATION);
}


void CThinBridgeCheckerDlg::OnBnClickedButton3()
{
	if(GetNativeIEProcCnt()<1)
	{
		AfxMessageBox(_T("IEは起動していません。"),MB_OK|MB_ICONINFORMATION);
		return;
	}
	if(IDOK==AfxMessageBox(_T("IEのプロセスを強制終了します。よろしいですか？"),MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2))
	{
		this->TermIE();
		AfxMessageBox(_T("IEを強制終了しました。"),MB_OK|MB_ICONINFORMATION);

	}
	
}

void CThinBridgeCheckerDlg::OnBnClickedButton2()
{
	SetDlgText(NULL);
}

void CThinBridgeCheckerDlg::OnBnClickedButton6()
{
	Command_WINTOPMOST_T(2);
	CDlgURLEdit Dlg;
	if(Dlg.DoModal()==IDOK)
	{
		CString strURL;
		strURL = Dlg.m_strURL;
		if(GetNativeIEProcCnt()>1)
		{
			this->TermIE();
		}
		IEStart(strURL);
	}
}


void CThinBridgeCheckerDlg::OnBnClickedButton7()
{
	Command_WINTOPMOST_T(2);
	TCHAR szBat[]=_T("ThinBridgeSetting.exe");
	CString strBatPath;
	BOOL bConfig=FALSE;
	CString strMsg;
	strBatPath.Format(_T("%s%s"),m_strExeFolderPath,szBat);
	if (!PathFileExists(strBatPath))
	{
		strBatPath.Format(_T("%s%s"), GetThinBridgeBasePath(), szBat);
		if (!PathFileExists(strBatPath))
		{
			strMsg.Format(_T("%sが見つからないため、起動できません。"), szBat);
			AfxMessageBox(strMsg, MB_OK | MB_ICONSTOP);
			if (::GetKeyState(VK_SHIFT) >= 0)
				return;
			strBatPath.Format(_T("%s%s"), GetThinBridgeBasePath(),_T("ThinBridge.exe"));
			bConfig=TRUE;
		}
	}
	if (PathFileExists(strBatPath))
	{
		Command_WINTOPMOST_T(2);
		CString IEcmd;
		if(bConfig)
			IEcmd.Format(_T("\"%s\" /Config"), strBatPath);
		else
			IEcmd.Format(_T("\"%s\""),strBatPath);
		STARTUPINFO si={0};
		PROCESS_INFORMATION pi={0};
		si.cb = sizeof( si );
		if(!CreateProcess( NULL,(LPTSTR)(LPCTSTR)IEcmd,NULL, NULL, TRUE, 0, NULL,NULL, &si, &pi ))
		{
			AfxMessageBox(_T("実行に失敗しました。管理者権限で再実行してください。"),MB_OK|MB_ICONSTOP);
		}
	}
	else
	{
		strMsg.Format(_T("%sが見つからないため、起動できません。"),szBat);
		AfxMessageBox(strMsg,MB_OK|MB_ICONSTOP);
		return;
	}
}


void CThinBridgeCheckerDlg::OnBnClickedButton8()
{
	Command_WINTOPMOST_T(2);
	CDlgURLEdit Dlg;
	if (Dlg.DoModal() == IDOK)
	{
		CString strURL;
		strURL = Dlg.m_strURL;
		if (GetNativeChromeProcCnt() < 1)
		{
			ChromeStart(NULL);
			if(strURL.IsEmpty()|| strURL==_T("about:blank"))
				return;
			Sleep(5000);
		}
		ChromeStart(strURL);
	}
}

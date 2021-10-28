
// ThinBridgeChecker.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "ThinBridgeChecker.h"
#include "ThinBridgeCheckerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CThinBridgeCheckerApp

BEGIN_MESSAGE_MAP(CThinBridgeCheckerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CThinBridgeCheckerApp コンストラクション

CThinBridgeCheckerApp::CThinBridgeCheckerApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CThinBridgeCheckerApp オブジェクトです。

CThinBridgeCheckerApp theApp;


// CThinBridgeCheckerApp 初期化

BOOL CThinBridgeCheckerApp::InitInstance()
{
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
    if (!AfxOleInit())
    {
        return FALSE;
    }
	AfxOleGetMessageFilter()->EnableNotRespondingDialog(FALSE);
	AfxOleGetMessageFilter()->EnableBusyDialog(FALSE);


	AfxEnableControlContainer();

	TCHAR szPath[MAX_PATH+1]={0};
	GetModuleFileName(AfxGetInstanceHandle(),szPath,MAX_PATH);
	CString strZonePath;
	strZonePath = szPath;
	strZonePath += _T(":Zone.Identifier");
	::DeleteFile(strZonePath);
	SetLastError(NO_ERROR);


	// ダイアログにシェル ツリー ビューまたはシェル リスト ビュー コントロールが
	// 含まれている場合にシェル マネージャーを作成します。
	//CShellManager *pShellManager = new CShellManager;

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	//SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));

	CThinBridgeCheckerDlg dlg;
	m_pMainWnd = &dlg;
	//コマンドラインあり。
	if (m_lpCmdLine[0] != '\0')
	{
		CString Command1;
		//パラメータが1つ以上
		if (__argc >= 2)
		{
			Command1 = CString(__wargv[1]);
			Command1.Replace(_T("\""), _T(""));
			Command1.TrimLeft();
			Command1.TrimRight();
		}
		if (Command1.CompareNoCase(_T("/log")) == 0)
		{
			TCHAR szPath[MAX_PATH + 1] = { 0 };
			TCHAR szDrive[_MAX_DRIVE] = { 0 };
			TCHAR szDir[_MAX_DIR] = { 0 };
			TCHAR szFileName[_MAX_FNAME] = { 0 };
			TCHAR szExt[_MAX_EXT] = { 0 };
			GetModuleFileName(AfxGetInstanceHandle(), szPath, MAX_PATH);
			_tsplitpath_s(szPath, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFileName, _MAX_FNAME, szExt, _MAX_EXT);

			CString strExecLogFileFullPath;
			CString strExecLogFileDir;
			strExecLogFileFullPath = szDrive;
			strExecLogFileFullPath += szDir;
			//strExecLogFileFullPath += _T("TBUpdateLog\\");
			//::CreateDirectory(m_strExecLogFileFullPath, NULL);
			strExecLogFileDir = strExecLogFileFullPath;
			strExecLogFileFullPath += _T("ThinBridgeInstall.log");
			::CopyFile(strExecLogFileFullPath + ".bak4", strExecLogFileFullPath + ".bak5", FALSE);
			::CopyFile(strExecLogFileFullPath + ".bak3", strExecLogFileFullPath + ".bak4", FALSE);
			::CopyFile(strExecLogFileFullPath + ".bak2", strExecLogFileFullPath + ".bak3", FALSE);
			::CopyFile(strExecLogFileFullPath + ".bak1", strExecLogFileFullPath + ".bak2", FALSE);
			::CopyFile(strExecLogFileFullPath, strExecLogFileFullPath + ".bak1", FALSE);
			SetLastError(NO_ERROR);

			CString strLog;
			strLog = dlg.ExecChk();
			CStdioFile out;
			if (out.Open(strExecLogFileFullPath, CFile::modeWrite | CFile::shareDenyNone | CFile::modeCreate))
			{
				TRY
				{
					strLog.Replace(_T("\r\n"), _T("\n"));
					out.WriteString(strLog);
					out.Close();
				}
				CATCH(CFileException, eP) {}
				END_CATCH
			}
			return FALSE;
		}
	}
	else
	{
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: ダイアログが <OK> で消された時のコードを
			//  記述してください。
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: ダイアログが <キャンセル> で消された時のコードを
			//  記述してください。
		}
	}
	// 上で作成されたシェル マネージャーを削除します。
	//if (pShellManager != NULL)
	//{
	//	delete pShellManager;
	//}

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}


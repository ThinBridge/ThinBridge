
// CustomLauncher.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "CustomLauncher.h"
#include "CustomLauncherDlg.h"
#include "locale.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCustomLauncherApp

BEGIN_MESSAGE_MAP(CCustomLauncherApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCustomLauncherApp コンストラクション

CCustomLauncherApp::CCustomLauncherApp()
{
	m_iKeyCombination=0;
}


// 唯一の CCustomLauncherApp オブジェクトです。

CCustomLauncherApp theApp;


// CCustomLauncherApp 初期化

BOOL CCustomLauncherApp::InitInstance()
{
	//pn.exe /Qlaunch "Farm1:iexplore.exe" /param:"URL"


	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls={0};
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
//	SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));

	TCHAR szPath[MAX_PATH+1]={0};
	TCHAR szDrive[_MAX_DRIVE]={0};
	TCHAR szDir[_MAX_DIR]={0};
	TCHAR szFileName[_MAX_FNAME]={0};
	TCHAR szExt[_MAX_EXT]={0};
	GetModuleFileName(AfxGetInstanceHandle(),szPath,MAX_PATH);
	_tsplitpath_s(szPath, szDrive,_MAX_DRIVE, szDir,_MAX_DIR, szFileName,_MAX_FNAME, szExt,_MAX_EXT); 

	m_strExeFullPath=szPath;
	m_strExeNameNoExt = szFileName;

	//設定データのロード
	m_strSetting_FileFullPath = szDrive;
	m_strSetting_FileFullPath += szDir;
	m_strSetting_FileFullPath += m_strExeNameNoExt;
	m_strSetting_FileFullPath += _T(".conf");
	this->ReadSetting();

	CString strMsg;
	CString Command1;
	CString Command2;
	//コマンドラインあり。
	if(m_lpCmdLine[0] != '\0')
	{
		//パラメータが1つだけ
		if(__argc ==2)
		{
			Command1 = CString(__wargv[1]);
			Command1.Replace(_T("\""),_T(""));
			Command1.TrimLeft();
			Command1.TrimRight();

			if(!Command1.IsEmpty())
			{
				//URLかFilePathの場合は、強制的にm_CommandParamとする。
				if(SBUtil::IsURL(Command1))
				{
					m_CommandParam=Command1;
				}
				//filepath
				else if(Command1.Find(_T(":"))==1)
				{
					m_CommandParam=Command1;
				}
				//-は、オプション
				else if(Command1.Find(_T("-"))==0)
				{
					m_OptionParam=Command1;
				}
				// /は、オプション
				else if(Command1.Find(_T("/"))==0)
				{
					m_OptionParam=Command1;
				}
			}
		}
		//コマンドラインが3つ以上、0番は、EXEパス
		else if(__argc>=3)
		{
			Command1 = CString(__wargv[1]);
			Command2 = CString(__wargv[2]);
			Command1.Replace(_T("\""),_T(""));
			Command1.TrimLeft();
			Command1.TrimRight();

			Command2.Replace(_T("\""),_T(""));
			Command2.TrimLeft();
			Command2.TrimRight();

			if(!Command1.IsEmpty())
			{
				//URLかFilePathの場合は、強制的にm_CommandParamとする。
				if(SBUtil::IsURL(Command1))
				{
					m_CommandParam=Command1;
				}
				//filepath
				else if(Command1.Find(_T(":"))==1)
				{
					m_CommandParam=Command1;
				}
			}
			if(!Command2.IsEmpty())
			{
				//URLかFilePathの場合は、強制的にm_CommandParamとする。
				if(SBUtil::IsURL(Command2))
				{
					m_CommandParam=Command2;
				}
				else
				{
					m_OptionParam=Command2;
				}
			}
		}
		//.confのCommand1がある場合。	
		if(!m_strCommand1.IsEmpty())
		{
			this->CustomBrowserStart(m_strCommand1,m_strCommand2,m_CommandParam);;
		}
		//Command Lineなし
		else
		{
			strMsg.Format(_T("%s"),_T("起動用のコマンド1がセットされていません。"));
			::MessageBox(NULL,strMsg,m_strExeNameNoExt,MB_ICONERROR);
		}
	}
	else
	{
		CCustomLauncherDlg dlg;
		//表示制限されている場合
		if(m_iKeyCombination)
		{
			strMsg.Format(_T("%s\n%s"),_T("この機能へのアクセスは、システム管理者により制限されています。 "),_T("This Feature has been disabled by your administrator."));
			::MessageBox(NULL,strMsg,m_strExeNameNoExt,MB_ICONERROR);
			//Keyキーコンビネーションが一致した場合は、設定画面を表示する。
			if(bValidKeyCombi())
			{
				m_pMainWnd = &dlg;
				dlg.DoModal();
			}
		}
		else
		{
			m_pMainWnd = &dlg;
			dlg.DoModal();
		}
	}
	return FALSE;
}

void CCustomLauncherApp::CustomBrowserStart(CString strCommand1,CString strCommand2,CString strURL)
{
	CString IEcmd;
	IEcmd=_T("");
	if(!strCommand2.IsEmpty())
	{
		strCommand2.Replace(_T("<URL>"),strURL);
		IEcmd.Format(_T("\"%s\" %s"),strCommand1,strCommand2);
	}
	else
	{
		IEcmd.Format(_T("\"%s\""),strCommand1);
	}
	CString strPathQ;
	strPathQ.Format(_T("\"%s\""),strCommand1);
	STARTUPINFO si={0};
	PROCESS_INFORMATION pi={0};
	si.cb = sizeof( si );
	if(!CreateProcess( NULL,(LPTSTR)(LPCTSTR)IEcmd,NULL, NULL, FALSE, 0, NULL,NULL, &si, &pi ))
	{
		SetLastError(NO_ERROR);
		//Retry
		STARTUPINFO si2={0};
		PROCESS_INFORMATION pi2={0};
		si2.cb = sizeof( si2 );
		if(!CreateProcess( strPathQ,(LPTSTR)(LPCTSTR)strCommand2,NULL, NULL, FALSE, 0, NULL,NULL, &si2, &pi2 ))
		{
			SetLastError(NO_ERROR);
			if(::ShellExecute(NULL,_T("open"),strPathQ,strCommand2,NULL, SW_SHOW) <= HINSTANCE(32))
			{
				SetLastError(NO_ERROR);
				::ShellExecute(NULL,NULL,IEcmd,NULL,NULL, SW_SHOW);
			}
		}
		else
		{
			if(pi2.hThread)
			{
				CloseHandle( pi2.hThread );  // スレッドのハンドルは使わないのですぐ破棄
				pi2.hThread=0;
			}
			if(pi2.hProcess)
			{
				CloseHandle( pi2.hProcess ); // もうプロセスのハンドルは使わないので破棄
				pi2.hProcess=0;
			}
		}
	}
	else
	{
		if(pi.hThread)
		{
			CloseHandle( pi.hThread );  // スレッドのハンドルは使わないのですぐ破棄
			pi.hThread=0;
		}
		if(pi.hProcess)
		{
			CloseHandle( pi.hProcess ); // もうプロセスのハンドルは使わないので破棄
			pi.hProcess=0;
		}
	}
}
void CCustomLauncherApp::ReadSetting()
{
	_wsetlocale(LC_ALL, _T("jpn")); 
	CStdioFile in;
	CString strTemp;
	CString strLogMsgLine;
	if(in.Open(m_strSetting_FileFullPath,CFile::modeRead|CFile::shareDenyWrite))
	{
		CString strTemp2;
		CString strTemp3;
		CStringArray strArray;
		while(in.ReadString(strTemp))
		{
			strTemp.TrimLeft();
			strTemp.TrimRight();
			strArray.RemoveAll();
			SBUtil::Split(strArray,strTemp,_T("="));
			strLogMsgLine += strTemp;
			strLogMsgLine += "\n";

			if(strArray.GetSize() >= 2)
			{
				strTemp2=strArray.GetAt(0);
				strTemp2.TrimLeft();
				strTemp2.TrimRight();

				strTemp3=strArray.GetAt(1);
				strTemp3.TrimLeft();
				strTemp3.TrimRight();

				if(strTemp2.Find(_T(";"))==0)
				{
					;
				}
				else if(strTemp2.Find(_T("#"))==0)
				{
					;
				}
				else
				{
					if(strTemp2.CompareNoCase(_T("Command1"))==0)
					{
						m_strCommand1=strTemp3;
					}
					else if(strTemp2.CompareNoCase(_T("Command2"))==0)
					{
						m_strCommand2=strTemp3;
					}
					else if(strTemp2.CompareNoCase(_T("KeyCombination"))==0)
					{
						if(!strTemp3.IsEmpty())
							m_iKeyCombination=_ttoi(strTemp3);
						else
							m_iKeyCombination=0;
					}
				}
			}
		}
		in.Close();
	}
}

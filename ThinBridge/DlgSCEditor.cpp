#include "stdafx.h"
#include "ThinBridge.h"
#include "DlgSCEditor.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CDlgSCEditor, CDialogEx)

CDlgSCEditor::CDlgSCEditor(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSCEditor::IDD, pParent)
{

}

CDlgSCEditor::~CDlgSCEditor()
{
}

void CDlgSCEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
}


BEGIN_MESSAGE_MAP(CDlgSCEditor, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgSCEditor::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgSCEditor::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_FUNC, &CDlgSCEditor::OnBnClickedButtonFunc)
	ON_BN_CLICKED(IDC_BUTTON_ZONE, &CDlgSCEditor::OnBnClickedButtonZone)
	ON_BN_CLICKED(IDC_BUTTON_TOP, &CDlgSCEditor::OnBnClickedButtonTop)
	ON_BN_CLICKED(IDC_BUTTON_URL, &CDlgSCEditor::OnBnClickedButtonUrl)
	ON_BN_CLICKED(IDOK, &CDlgSCEditor::OnBnClickedOk)
	ON_WM_ACTIVATE()
	ON_WM_NCACTIVATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_EVENT, &CDlgSCEditor::OnBnClickedButtonEvent)
END_MESSAGE_MAP()


BOOL CDlgSCEditor::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_autoResize.AddSnapToRightBottom(GetDlgItem(IDC_EDIT1));
	bFirstFlg = TRUE;
	m_SelPosS=0;
	m_SelPosE=0;
	m_Edit.LimitText(1024*1024*10);
	this->ReadText();

	this->m_Edit.SetSel(0,0);
	this->m_Edit.SetFocus();

	CFont* pFont = m_Edit.GetFont();
	TEXTMETRIC tm={0};

	// get the control's DC, too
	CDC* pDC = m_Edit.GetDC();

	// Select the font that the control uses by default into the DC.
	// We must do this because the control may or may not be using
	// that font at this exact moment
	CFont* pOldFont = pDC->SelectObject(pFont);

	// Retrieve text metrics for that font and return the previously
	// selected font.
	pDC->GetTextMetrics(&tm);
	pDC->SelectObject(pOldFont);

	// Get an identity rectangle and map it to dialog units
	CRect rect(0, 0, 100, 1);
	::MapDialogRect((HWND)this, rect);

	// We now know that 100 dialog units are rect.Width() screen units,
	// so we can multiply screen units by 100 and divide by rect.Width()
	// to find dialog units from screen units. tm.tmAveCharWidth is
	// the width of _one_ character, so setting the tabs at every
	// four characters means we also multiply by four.
	int iTab=(4 * tm.tmAveCharWidth * 100) / rect.Width();
	iTab=iTab/2;
	m_Edit.SetTabStops(iTab);


	return FALSE;  // return TRUE unless you set the focus to a control
}

BOOL CDlgSCEditor::PreTranslateMessage(MSG* pMsg)
{
	ASSERT(pMsg != NULL);
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);
	CString strLine;

	if (pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE)
		{
			//OnCancel();
			return TRUE;
		}
	}
	if(pMsg->hwnd==m_Edit.m_hWnd)
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			if(pMsg->wParam == VK_TAB)
			{
				int nPos = LOWORD(m_Edit.CharFromPos(m_Edit.GetCaretPos()));
				m_Edit.SetSel(nPos, nPos);
				m_Edit.ReplaceSel(_T("\t"), TRUE);
				return TRUE;
			}
			else if(pMsg->wParam == 'S')
			{
				if(::GetKeyState(VK_CONTROL)<0)
				{
					PostMessage(WM_COMMAND,IDOK);
					return TRUE;
				}
			}
			else if(pMsg->wParam == 'A')
			{
				if(::GetKeyState(VK_CONTROL)<0)
				{
					m_Edit.PostMessage(EM_SETSEL, (WPARAM)0, (LPARAM)-1);
					return TRUE;
				}
			}

		}
		if(pMsg->message == WM_MOUSEMOVE
		||pMsg->message  == WM_LBUTTONDOWN
		||pMsg->message  == WM_KEYDOWN
		||pMsg->message  == WM_KEYUP
		||pMsg->message  == WM_LBUTTONDOWN
		)
		{
			DWORD dwStart=0, dwEnd=0;
			DWORD dwCursorPos=0;
			DWORD dwLineIndex=0;
			CPoint   pt;
			LRESULT lr={0};
			dwCursorPos = m_Edit.SendMessage(EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd);
			if (pMsg->message == WM_MOUSEMOVE && dwStart == dwEnd)
				return lr;
			lr = CDialogEx::PreTranslateMessage(pMsg);

			dwLineIndex = m_Edit.SendMessage(EM_LINEFROMCHAR,-1,0);
			strLine.Format(_T("%d行"),dwLineIndex+1);
			SetDlgItemText(IDC_STATIC_LINE,strLine);
			return lr;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
BOOL CDlgSCEditor::OnNcActivate(BOOL bActive)
{
	if(bActive)
	{
		if(bFirstFlg)
		{
			bFirstFlg = FALSE;
		}
		else
			m_Edit.PostMessage(EM_SETSEL, (WPARAM)m_SelPosS, (LPARAM)m_SelPosE);
	}
	else
	{
		m_Edit.GetSel(m_SelPosS,m_SelPosE);
	}
	return CDialogEx::OnNcActivate(bActive);
}


void CDlgSCEditor::OnPaint()
{
	CPaintDC dc(this);
}

void CDlgSCEditor::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
}

void CDlgSCEditor::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	m_autoResize.Resize(this);	
}

void CDlgSCEditor::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


void CDlgSCEditor::OnBnClickedButton1()
{
//	this->m_Edit.SetSel(0, -1);
//	this->m_Edit.Copy();
//	this->m_Edit.SetFocus();

	int nPos = LOWORD(m_Edit.CharFromPos(m_Edit.GetCaretPos()));
	m_Edit.SetSel(nPos, nPos);
	CString strHelpString;
	strHelpString = _T("'##################################################################################\r\n");
	strHelpString += _T("'ThinBridge Script ヘルプ\r\n");
	strHelpString += _T("'ファンクション　OnRedirectにリダイレクト処理を記述します。\r\n");
	strHelpString += _T("'スクリプト言語は、VBScriptベースになります。\r\n");
	strHelpString += _T("'戻り値として、起動したいブラウザーを記述します。\r\n");
	strHelpString += _T("\r\n");
	strHelpString += _T("'  Microsoft RDPにリダイレクトする場合\r\n'    OnRedirect=\"RDP\"\r\n");
	strHelpString += _T("'  VMware Horizonにリダイレクトする場合\r\n'    OnRedirect=\"VMware\"\r\n");
	strHelpString += _T("'  Citrix XenAppにリダイレクトする場合\r\n'    OnRedirect=\"Citrix\"\r\n");
	strHelpString += _T("'  Mozilla Firefoxにリダイレクトする場合\r\n'    OnRedirect=\"Firefox\"\r\n");
	strHelpString += _T("'  Google Chromeにリダイレクトする場合\r\n'    OnRedirect=\"Chrome\"\r\n");
	strHelpString += _T("'  Microsoft Edgeにリダイレクトする場合\r\n'    OnRedirect=\"Edge\"\r\n");
	strHelpString += _T("'  指定ブラウザー01にリダイレクトする場合\r\n'    OnRedirect=\"Custom01\"\r\n");
	strHelpString += _T("'  指定ブラウザー20にリダイレクトする場合\r\n'    OnRedirect=\"Custom20\"\r\n");
	strHelpString += _T("'  ※複数のブラウザーを起動したい場合は、半角スペースで区切ります。\r\n'    OnRedirect=\"Firefox Chrome Edge\"\r\n");
	strHelpString += _T("'    URLを判定した結果(リダイレクトする、しない)は、処理効率を上げるためメモリにキャッシュされます。\r\n");
	strHelpString += _T("'    キャッシュされているURLと一致した場合には、キャッシュに格納された結果を利用します。(このスクリプトは、毎回呼び出されません)\r\n");
	strHelpString += _T("'    判定結果をキャッシュしたくない場合は、戻り値に[NoCache]を追加して下さい。\r\n");
	strHelpString += _T("'    OnRedirect=\"NoCache Firefox\"\r\n");
	strHelpString += _T("'------------------------------------------------------------\r\n");
	strHelpString += _T("'グローバル変数(予約語)：\r\n");
	strHelpString += _T("'以下のグローバル変数を利用することが可能です。\r\n");
	strHelpString += _T("\r\n");
	strHelpString += _T("'TB_Global_URL\t(文字列値)\r\n");
	strHelpString += _T("'  画面遷移先のURL情報\r\n");
	strHelpString += _T("\r\n");
	strHelpString += _T("'TB_Global_TOP_PAGE\t(真偽値)\r\n");
	strHelpString += _T("'  トップURLかサブURL(Frame/iFrame)かの情報：True(トップURL)　False(サブURL)\r\n");
	strHelpString += _T("\r\n");
	strHelpString += _T("'TB_Global_ZONE\t(文字列値)\r\n");
	strHelpString += _T("'  URLのゾーン情報：\r\n");
	strHelpString += _T("'   LOCAL_INTRANET_ZONE(ローカルイントラネットゾーン)\r\n");
	strHelpString += _T("'   INTERNET_ZONE(インターネットゾーン)\r\n");
	strHelpString += _T("'   TRUSTED_ZONE(信頼済みサイトゾーン)\r\n");
	strHelpString += _T("'   UNTRUSTED_ZONE(制限付きサイトゾーン)\r\n");
	strHelpString += _T("\r\n");
	strHelpString += _T("'TB_Global_EVENT\t(文字列値)\r\n");
	strHelpString += _T("'  呼び出し元のイベント名：\r\n");
	strHelpString += _T("'  BeforeNavigate (ナビゲート前)\r\n");
	strHelpString += _T("'  NavigateComplete (ナビゲート完了後)\r\n");
	strHelpString += _T("'------------------------------------------------------------\r\n");
	strHelpString += _T("'例)http://www.yahoo.co.jpの場合は、Firefoxにリダイレクトする。\r\n");
	strHelpString += _T("'Function OnRedirect()\r\n");
	strHelpString += _T("'  IF TB_Global_URL=\"http://www.yahoo.co.jp\" Then\r\n");
	strHelpString += _T("'    OnRedirect=\"Firefox\"\r\n");
	strHelpString += _T("'  End IF\r\n");
	strHelpString += _T("'End Function\r\n");
	strHelpString += _T("'##################################################################################\r\n");

	m_Edit.ReplaceSel(strHelpString, TRUE);
	this->m_Edit.SetFocus();

}


void CDlgSCEditor::OnBnClickedButtonFunc()
{
	int nPos = LOWORD(m_Edit.CharFromPos(m_Edit.GetCaretPos()));
	m_Edit.SetSel(nPos, nPos);
	CString strText;
	strText=_T("Function OnRedirect()\r\n");
	strText+=_T("  OnRedirect=\"RDP\"\r\n");
	strText+=_T("  Exit Function\r\n\r\n");
	strText+=_T("  OnRedirect=\"VMware\"\r\n");
	strText+=_T("  Exit Function\r\n\r\n");
	strText+=_T("  OnRedirect=\"Citrix\"\r\n");
	strText+=_T("  Exit Function\r\n\r\n");
	strText+=_T("  OnRedirect=\"Firefox\"\r\n");
	strText+=_T("  Exit Function\r\n\r\n");
	strText+=_T("  OnRedirect=\"Chrome\"\r\n");
	strText+=_T("  Exit Function\r\n\r\n");
	strText+=_T("  OnRedirect=\"Edge\"\r\n");
	strText+=_T("  Exit Function\r\n\r\n");
	strText+=_T("  OnRedirect=\"Custom01\"\r\n");
	strText+=_T("  Exit Function\r\n\r\n");
	strText+=_T("End Function\r\n");

	m_Edit.ReplaceSel(strText,TRUE);


	this->m_Edit.SetFocus();
}


void CDlgSCEditor::OnBnClickedButtonZone()
{
	int nPos = LOWORD(m_Edit.CharFromPos(m_Edit.GetCaretPos()));
	m_Edit.SetSel(nPos, nPos);
	CString strText;
	strText = _T("'ローカルイントラネットゾーン\r\nIF TB_Global_ZONE=\"LOCAL_INTRANET_ZONE\" Then\r\n\r\n");
	strText+=_T("'インターネットゾーン\r\nElseIF TB_Global_ZONE=\"INTERNET_ZONE\" Then\r\n\r\n");
	strText+=_T("'信頼済みサイトゾーン\r\nElseIF TB_Global_ZONE=\"TRUSTED_ZONE\" Then\r\n\r\n");
	strText+=_T("'制限付きサイトゾーン\r\nElseIF TB_Global_ZONE=\"UNTRUSTED_ZONE\" Then\r\n\r\n");
	strText+=_T("End IF\r\n");

	m_Edit.ReplaceSel(strText, TRUE);
	this->m_Edit.SetFocus();
}


void CDlgSCEditor::OnBnClickedButtonTop()
{
	int nPos = LOWORD(m_Edit.CharFromPos(m_Edit.GetCaretPos()));
	m_Edit.SetSel(nPos, nPos);
	m_Edit.ReplaceSel(_T("TB_Global_TOP_PAGE\r\n"), TRUE);
	this->m_Edit.SetFocus();
}


void CDlgSCEditor::OnBnClickedButtonUrl()
{
	int nPos = LOWORD(m_Edit.CharFromPos(m_Edit.GetCaretPos()));
	m_Edit.SetSel(nPos, nPos);
	m_Edit.ReplaceSel(_T("TB_Global_URL\r\n"), TRUE);
	this->m_Edit.SetFocus();
}


void CDlgSCEditor::OnBnClickedButtonEvent()
{
	int nPos = LOWORD(m_Edit.CharFromPos(m_Edit.GetCaretPos()));
	m_Edit.SetSel(nPos, nPos);
	m_Edit.ReplaceSel(_T("'BeforeNavigate (ナビゲート前)\r\nIF TB_Global_EVENT=\"BeforeNavigate\" Then\r\n\r\n'NavigateComplete (ナビゲート完了後)\r\nElseIF TB_Global_EVENT=\"NavigateComplete\" Then\r\n\r\nEnd IF\r\n"), TRUE);
	this->m_Edit.SetFocus();
}


void CDlgSCEditor::OnBnClickedOk()
{
	int iRet=0;
	iRet = ::MessageBox(this->m_hWnd,_T("保存しますか？"),theApp.m_strThisAppName,MB_ICONQUESTION | MB_OKCANCEL);
	if(iRet == IDCANCEL)
		return;
	WriteText();
	//CDialogEx::OnOK();
}

#pragma warning(disable:4244)
void CDlgSCEditor::ReadText()
{
	CStdioFile file;
	CString strText;
	CString strData;
	CString strLine;
	if(file.Open(m_strFilePath,CFile::modeRead|CFile::shareDenyNone))
	{
		int iLineNum=0;
		int iCommentStartLine=0;
		BOOL bFindComment=FALSE;
		while(file.ReadString(strLine))
		{
			iLineNum++;
			if(strLine==_T("'#############↓↓ここから下は削除しないで下さい↓↓#############################"))
			{
				iCommentStartLine=iLineNum;
				bFindComment=TRUE;
				continue;
			}
			if(bFindComment && iCommentStartLine+4 > iLineNum)
				continue;
			strData+=strLine;
			strData+=_T("\r\n");
		}
		strText = strData;
//		UINT iFileSize=file.GetLength();
//		byte* pBuffer=new byte[iFileSize+2];
//		memset(pBuffer,0x00,iFileSize+2);
//		file.Read(pBuffer,iFileSize);
//		strText = pBuffer;
//		delete [] pBuffer;
		file.Close();
	}
	m_Edit.SetWindowText(strText);
}

void CDlgSCEditor::WriteText()
{
	::CopyFile(m_strFilePath+".bak4", m_strFilePath+".bak5", FALSE);
	::CopyFile(m_strFilePath+".bak3", m_strFilePath+".bak4", FALSE);
	::CopyFile(m_strFilePath+".bak2", m_strFilePath+".bak3", FALSE);
	::CopyFile(m_strFilePath+".bak1", m_strFilePath+".bak2", FALSE);
	::CopyFile(m_strFilePath, m_strFilePath+".bak1", FALSE);
	SetLastError(NO_ERROR);

	CFile file;
	CString strText;
	m_Edit.GetWindowText(strText);

	CString strComment;
	strComment =_T("'#############↓↓ここから下は削除しないで下さい↓↓#############################\r\n");
	strComment +=_T("'## Redirect Script File for ThinBridge\r\n");
	CTime time = CTime::GetCurrentTime();
	strComment += _T("'## ");
	strComment += time.Format(_T("%Y-%m-%d %X"));
	strComment += _T("\r\n");
	strComment +=_T("'#############↑↑ここから上は削除しないで下さい↑↑#############################\r\n");

	CString strData;
	strData = strComment;
	strData += strText;

	CStringA strDataA;
	strDataA = strData;
	if(file.Open(m_strFilePath,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite))
	{
		file.Write((LPCSTR)strDataA,strDataA.GetLength());
		file.Close();
	}
	else
	{
		CString strSaveMsg;
		strSaveMsg.Format(_T("ファイルの保存に失敗しました。\n\n別のプログラムがファイルを開いているか、書込権限が不足しています。操作を完了できません。ファイルを閉じてから再実行してください。\n\n%s"),m_strFilePath);
		::MessageBox(this->m_hWnd,strSaveMsg,theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
	}
}


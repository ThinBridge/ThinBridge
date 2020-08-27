#include "stdafx.h"
#include "ThinBridge.h"
#include "DlgEditURLML.h"
#include "afxdialogex.h"
#include "DlgMsgBox.h"

IMPLEMENT_DYNAMIC(CDlgEditURLML, CDialogEx)

CDlgEditURLML::CDlgEditURLML(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgEditURLML::IDD, pParent)
{

}

CDlgEditURLML::~CDlgEditURLML()
{
}

void CDlgEditURLML::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
}


BEGIN_MESSAGE_MAP(CDlgEditURLML, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgEditURLML::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDlgEditURLML::OnBnClickedOk)
	ON_WM_ACTIVATE()
	ON_WM_NCACTIVATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


void CDlgEditURLML::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

BOOL CDlgEditURLML::bValidData()
{
	BOOL bRet=FALSE;
	CString strText;
	CString strValidData;
	m_Edit.GetWindowText(strText);

	m_strArrayResult.RemoveAll();
	m_strArrayResult_Enable.RemoveAll();

	CStringArray strArray;
	CMapStringToPtr strMapDupChk;
	int iCnt=0;
	int iCntMax=0;
	void* dwpLineIndex={0};
	DWORD dwLine=0;
	CString strTempLine;
	CString strTempLineOrg;

	CString strErrMsg;
	CString strErrMsgLine;
	int iErrCnt=0;

	SBUtil::Split_TrimBlank(strArray,strText,_T("\r\n"));
	strText.Empty();

	//重複と禁則文字チェック
	iCntMax = strArray.GetCount();
	BOOL bEnable=TRUE;

	for(iCnt=0;iCnt < iCntMax;iCnt++)
	{
		bEnable=TRUE;
		strTempLine.Empty();
		strTempLineOrg.Empty();
		strTempLine = strArray.GetAt(iCnt);
		strTempLineOrg = strTempLine;
		strText+=strTempLine;
		strText+="\r\n";

		//先頭のハイフン-チェック
		if(strTempLine.Find(_T("-"))==0)
		{
			strErrMsgLine.Format(_T("禁則：%d行目のデータ[%s]は、先頭がハイフン記号'-'から始まるため設定できません。\r\n"),iCnt+1,strTempLine);
			strErrMsg+=strErrMsgLine;
			iErrCnt++;
			continue;
		}
		//先頭の#チェック
		if(strTempLine.Find(_T("#"))==0)
		{
			//#しかない場合は、だめ。
			if(strTempLine.GetLength()==1)
			{
				strErrMsgLine.Format(_T("禁則：%d行目のデータ[%s]は、#記号のみの為、設定できません。\r\n"),iCnt+1,strTempLine);
				strErrMsg+=strErrMsgLine;
				iErrCnt++;
				continue;
			}
			else
				bEnable=FALSE;
		}
		//先頭の;チェック
		if(strTempLine.Find(_T(";"))==0)
		{
			//#しかない場合は、だめ。
			if(strTempLine.GetLength()==1)
			{
				strErrMsgLine.Format(_T("禁則：%d行目のデータ[%s]は、;記号のみの為、設定できません。\r\n"),iCnt+1,strTempLine);
				strErrMsg+=strErrMsgLine;
				iErrCnt++;
				continue;
			}
			else
				bEnable=FALSE;
		}

		//commentを削除して追加する。
		if(!bEnable)
		{
			strTempLine=strTempLine.Mid(1);
		}

		//重複チェック
		if(strMapDupChk.Lookup(strTempLine,dwpLineIndex))
		{
			dwLine=0;
			dwLine = (DWORD)dwpLineIndex;
			//Lookup出来た場合は、重複している。
			strErrMsgLine.Format(_T("重複：%d行目のデータ[%s]は、%d行目に既に登録されています。\r\n"),iCnt+1,strTempLine,dwLine+1);
			strErrMsg+=strErrMsgLine;
			iErrCnt++;
		}
		//重複無し。
		else
		{
			dwLine=iCnt;
			dwpLineIndex = (void*)dwLine;
			strMapDupChk.SetAt(strTempLine,dwpLineIndex);
			m_strArrayResult.Add(strTempLine);
			if(bEnable)
				m_strArrayResult_Enable.Add(_T("○"));
			else
				m_strArrayResult_Enable.Add(_T("－"));
			strValidData+=strTempLineOrg;
			strValidData+=_T("\r\n");
		}
	}
	if(!strErrMsg.IsEmpty())
	{
		CString strErrMsgResult;
		strErrMsgResult.Format(_T("【データエラー】\r\n※%d個のエラーがあります。\r\n更新に失敗しました。データを修正して下さい。\r\n=====================================================================\r\n%s====================================================================="),iErrCnt,strErrMsg);
		CDlgMsgBox MsgBox;
		MsgBox.m_strMsg=strErrMsgResult;
		if(IDOK==MsgBox.DoModal())
		{
			int iRet = 0;
			iRet = ::MessageBox(this->m_hWnd,_T("データエラーを自動的に修正しますか？\n\n※「はい」を選択するとエラーの発生している行を削除します。\n元に戻すことはできません。"),theApp.m_strThisAppName,MB_ICONQUESTION | MB_YESNO|MB_DEFBUTTON2);
			if(iRet == IDYES)
			{
				//念の為にもう一度ダメ押し。
				iRet = 0;
				iRet = ::MessageBox(this->m_hWnd,_T("データエラーの自動修正を実行します。"),theApp.m_strThisAppName,MB_ICONASTERISK | MB_YESNO|MB_DEFBUTTON2);
				if(iRet == IDYES)
					strText=strValidData;
			}
		}
		m_SelPosS=0;
		m_SelPosE=0;
		m_Edit.SetFocus();
		m_Edit.SetWindowText(strText);
		m_Edit.PostMessage(EM_SETSEL, (WPARAM)m_SelPosS, (LPARAM)m_SelPosE);
		return FALSE;
	}
	else
	{
		m_strEditData = strText;
		bRet = TRUE;
	}
	return bRet;
}

void CDlgEditURLML::OnBnClickedOk()
{
	int iRet=0;
	iRet = ::MessageBox(this->m_hWnd,_T("設定を更新しますか？\n※空白行は、自動的に切り詰められます。"),theApp.m_strThisAppName,MB_ICONQUESTION | MB_OKCANCEL);
	if(iRet == IDCANCEL)
		return;

	//重複と禁則文字チェック
	if(bValidData())
	{
		CDialogEx::OnOK();
	}
}

BOOL CDlgEditURLML::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	bFirstFlg = TRUE;
	m_SelPosS=0;
	m_SelPosE=0;
	m_Edit.LimitText(1024*1024*5);
	m_Edit.SetFocus();
	m_Edit.SetWindowText(m_strEditData);
	if(!m_strTitle.IsEmpty())
		SetWindowText(m_strTitle);
	return FALSE;
}
void CDlgEditURLML::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
}


BOOL CDlgEditURLML::PreTranslateMessage(MSG* pMsg)
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
			if(pMsg->wParam == 'A')
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


BOOL CDlgEditURLML::OnNcActivate(BOOL bActive)
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


void CDlgEditURLML::OnPaint()
{
	CPaintDC dc(this);
}



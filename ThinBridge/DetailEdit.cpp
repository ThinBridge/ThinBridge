#include "stdafx.h"
#include "ThinBridge.h"
#include "DetailEdit.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(DetailEdit, CDialogEx)

DetailEdit::DetailEdit(CWnd* pParent /*=NULL*/)
	: CDialogEx(DetailEdit::IDD, pParent)
{

}

DetailEdit::~DetailEdit()
{
}

void DetailEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
}


BEGIN_MESSAGE_MAP(DetailEdit, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &DetailEdit::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &DetailEdit::OnBnClickedOk)
	ON_WM_ACTIVATE()
	ON_WM_NCACTIVATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


void DetailEdit::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

void DetailEdit::OnBnClickedOk()
{
	int iRet=0;
	iRet = ::MessageBox(this->m_hWnd,_T("設定を保存しますか？"),theApp.m_strThisAppName,MB_ICONQUESTION | MB_OKCANCEL);
	if(iRet == IDCANCEL)
		return;
	WriteText();
	CDialogEx::OnOK();
}

BOOL DetailEdit::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	bFirstFlg = TRUE;
	m_strFilePath = theApp.m_strTemplate_FileFullPath;
	m_SelPosS=0;
	m_SelPosE=0;
	ReadText();
	m_Edit.SetFocus();

	return FALSE;
}
#pragma warning(disable:4244)
void DetailEdit::ReadText()
{
	//CFile file;
	//CString strText;
	//if(file.Open(m_strFilePath,CFile::modeRead|CFile::shareDenyNone))
	//{
	//	UINT iFileSize=file.GetLength();
	//	byte* pBuffer=new byte[iFileSize+2];
	//	memset(pBuffer,0x00,iFileSize+2);
	//	file.Read(pBuffer,iFileSize);
	//	strText = pBuffer;
	//	delete [] pBuffer;
	//	file.Close();
	//}
	//m_Edit.SetWindowText(strText);

	CStdioFile in;
	CString strTemp;
	CString strText;
	if(in.Open(m_strFilePath,CFile::modeRead|CFile::shareDenyWrite))
	{
		while(in.ReadString(strTemp))
		{
			strTemp.TrimLeft();
			strTemp.TrimRight();
			if(!strTemp.IsEmpty())
			{
				strText += strTemp;
				strText += _T("\r\n");
			}
		}
		in.Close();
	}
	m_Edit.SetWindowText(strText);

}

void DetailEdit::WriteText()
{
	//CFile file;
	//CString strText;
	//m_Edit.GetWindowText(strText);

	//if(file.Open(m_strFilePath,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite))
	//{
	//	file.Write((LPCTSTR)strText,strText.GetLength());
	//	file.Close();
	//}

	CString strText;
	m_Edit.GetWindowText(strText);
	CStdioFile out;
	if(out.Open(m_strFilePath,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
	{
		CString strWriteData;
		strWriteData=strText;
		out.WriteString(strWriteData);
		out.Close();
	}
	else
	{
		CString strSaveMsg;
		strSaveMsg.Format(_T("RDP詳細設定パラメータファイルンの保存に失敗しました。\n\n別のプログラムがファイルを開いているか、書込権限が不足しています。操作を完了できません。ファイルを閉じてから再実行してください。\n\n%s"),m_strFilePath);
		::MessageBox(this->m_hWnd,strSaveMsg,theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
	}
}


void DetailEdit::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
}


BOOL DetailEdit::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL DetailEdit::OnNcActivate(BOOL bActive)
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


void DetailEdit::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

#pragma once


// DlgMsg ダイアログ

class DlgMsg : public CDialogEx
{
	DECLARE_DYNAMIC(DlgMsg)

public:
	DlgMsg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~DlgMsg();

// ダイアログ データ
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	int m_iCnt;
	void SetFuncName(LPCTSTR lp)
	{
		m_Func.SetWindowText(lp);
	}
	void StepIt()
	{
		m_iCnt++;
		CString strMsg;
		strMsg.Format(_T("Step %08d"),m_iCnt);
		m_Step.SetWindowText(strMsg);
	}
	CStatic m_Step;
	CStatic m_Func;
};

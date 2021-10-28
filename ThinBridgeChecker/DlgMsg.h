#pragma once


// DlgMsg �_�C�A���O

class DlgMsg : public CDialogEx
{
	DECLARE_DYNAMIC(DlgMsg)

public:
	DlgMsg(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~DlgMsg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

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

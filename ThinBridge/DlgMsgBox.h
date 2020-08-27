#pragma once


class CDlgMsgBox : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMsgBox)

public:
	CDlgMsgBox(CWnd* pParent = NULL);
	virtual ~CDlgMsgBox();
	CString m_strMsg;
	CString m_strTitle;
// ダイアログ データ
	enum { IDD = IDD_DLG_MSGBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	CEdit m_Edit;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

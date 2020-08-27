#pragma once


class CURLInputDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CURLInputDlg)

public:
	CURLInputDlg(CWnd* pParent = NULL);
	virtual ~CURLInputDlg();
	CString m_strURL;
// ダイアログ データ
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEdit1();
};

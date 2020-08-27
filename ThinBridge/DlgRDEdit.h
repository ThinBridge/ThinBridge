#pragma once


class CDlgRDEdit : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRDEdit)

public:
	CDlgRDEdit(CWnd* pParent = NULL);
	virtual ~CDlgRDEdit();
	CString m_strURL;
	BOOL m_bEnable;

// ダイアログ データ
	enum { IDD = IDD_DLG_RD_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};

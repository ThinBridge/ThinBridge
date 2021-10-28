#pragma once


// CDlgURLEdit ダイアログ

class CDlgURLEdit : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgURLEdit)

public:
	CDlgURLEdit(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CDlgURLEdit();

// ダイアログ データ
	enum { IDD = IDD_DIALOG1 };
	CString m_strURL;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};

#pragma once


// CDlgURL ダイアログ

class CDlgURL : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgURL)

public:
	CDlgURL(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CDlgURL();
	CString m_strURL;
// ダイアログ データ
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};

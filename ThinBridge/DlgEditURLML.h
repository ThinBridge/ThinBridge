#pragma once


class CDlgEditURLML : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgEditURLML)

public:
	CDlgEditURLML(CWnd* pParent = NULL);
	virtual ~CDlgEditURLML();

// ダイアログ データ
	enum { IDD = IDD_DLG_EDIT_URL_ML };
	CString m_strEditData;
	CString m_strTitle;
	CStringArray m_strArrayResult;
	CStringArray m_strArrayResult_Enable;
	BOOL bValidData();

protected:
	BOOL bFirstFlg;
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Edit;
	int m_SelPosS;
	int m_SelPosE;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnPaint();
};

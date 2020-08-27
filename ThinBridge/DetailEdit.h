#pragma once


class DetailEdit : public CDialogEx
{
	DECLARE_DYNAMIC(DetailEdit)

public:
	DetailEdit(CWnd* pParent = NULL);
	virtual ~DetailEdit();

	enum { IDD = IDD_DIALOG3 };
	CString m_strFilePath;
	BOOL bFirstFlg;
	void ReadText();
	void WriteText();
protected:
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

#pragma once


class CDlgSetCX : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgSetCX)

public:
	CDlgSetCX();
	virtual ~CDlgSetCX();

	CString m_strCitrix_AppName;

// ダイアログ データ
	enum { IDD = IDD_DLG_CX };
	CStatic		m_Image;
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTestCx();
};

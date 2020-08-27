#pragma once


class CDlgSetVMW : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgSetVMW)

public:
	CDlgSetVMW();
	virtual ~CDlgSetVMW();
	CString m_strHorizon_ConnectionServerName;
	CString m_strHorizon_AppName;

// ダイアログ データ
	enum { IDD = IDD_DLG_VMW };
	CStatic		m_Image;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonTestV();
};

#pragma once


class CDlgEco : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgEco)

public:
	CDlgEco();
	virtual ~CDlgEco();
	CComboBox	m_Combo;

// ダイアログ データ
	enum { IDD = IDD_DLG_ECO };
	void Change_TabState();
	void Change_MemState();
	void Change_CPUState();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedChkIeCapTab();
	afx_msg void OnBnClickedChkIeCapMem();
	afx_msg void OnBnClickedChkIeCapCpu();
};

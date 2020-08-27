#pragma once


class CDlgEtcCAP : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgEtcCAP)

public:
	CDlgEtcCAP();
	virtual ~CDlgEtcCAP();
	void GetSettings();
// ダイアログ データ
	enum { IDD = IDD_DLG_ETC_CAP };
	UINT m_iSolutionType;
	int m_iKeyCombination;
	CString m_strCustomBrowserPath;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	void EnableCustomPath(BOOL bFlg);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTestL();
	afx_msg void OnBnClickedButtonFdlg();
	afx_msg void OnBnClickedKeytestbutton();
	afx_msg void OnBnClickedRadioLcb();
	afx_msg void OnBnClickedRadioLdb();
	afx_msg void OnBnClickedRadioLie();
	afx_msg void OnBnClickedRadioLff();
	afx_msg void OnBnClickedRadioLch();
	afx_msg void OnBnClickedRadioLeg();
};

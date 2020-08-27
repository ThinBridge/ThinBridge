
#pragma once


class CRedirectDlg : public CDialog
{
public:
	CString m_strServerName;
	BOOL m_bRemoteAppMode;
	CString m_strRemoteAppName;
	CString m_strRemoteAppPath;
	CString m_strRemoteAppCommandLine;
	BOOL m_bRedirect_Clipboard;
	BOOL m_bRedirect_Printer;
	BOOL m_bRedirect_Drive;
	BOOL m_bEnableDebugLog;

	UINT m_iSolutionType;
	CString m_strHorizon_ConnectionServerName;
	CString m_strHorizon_AppName;
	CString m_strCitrix_AppName;

	int m_iKeyCombination;


	CString m_strCustomBrowserPath;

	void EnableVMwareHorizon(BOOL bFlg);
	void EnableCitrix(BOOL bFlg);

	void ShowHideCtrix();

	void EnableRDPWindow(BOOL bFlg);
	void EnableCustomPath(BOOL bFlg);
	void Change_RDP_App_DesktopState();

	void GetSettings();

	CRedirectDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_THIN_BRIDGE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);


protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonIe();
	afx_msg void OnBnClickedButtonFf();
	afx_msg void OnBnClickedButtonClr();
	afx_msg void OnBnClickedButtonGc();
	afx_msg void OnBnClickedButtonSt();
	afx_msg void OnBnClickedButtonTestV();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedRadioRdp();
	afx_msg void OnBnClickedRadioVmw();
	afx_msg void OnBnClickedButtonFdlg();
	afx_msg void OnBnClickedRadioLdb();
	afx_msg void OnBnClickedRadioLie();
	afx_msg void OnBnClickedRadioLff();
	afx_msg void OnBnClickedRadioLch();
	afx_msg void OnBnClickedRadioLeg();
	afx_msg void OnBnClickedRadioLcb();
	afx_msg void OnBnClickedButtonTestL();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedButtonTestCx();
	afx_msg void OnBnClickedRadioCx();
	afx_msg void OnBnClickedKeytestbutton();
};

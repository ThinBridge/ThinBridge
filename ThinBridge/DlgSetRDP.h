#pragma once


class CDlgSetRDP : public CPropertyPage
{
	//DECLARE_DYNAMIC(CDlgSetRDP)
	DECLARE_DYNCREATE(CDlgSetRDP)

public:
	CDlgSetRDP();
	virtual ~CDlgSetRDP();

	enum { IDD = IDD_DLG_RDP };
	CStatic		m_Image;

	CString m_strServerName;
	BOOL m_bRemoteAppMode;
	CString m_strRemoteAppName;
	CString m_strRemoteAppPath;
	CString m_strRemoteAppCommandLine;
	BOOL m_bRedirect_Clipboard;
	BOOL m_bRedirect_Printer;
	BOOL m_bRedirect_Drive;
	void Change_RDP_App_DesktopState();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonSt();
	afx_msg void OnBnClickedButtonIe();
	afx_msg void OnBnClickedButtonFf();
	afx_msg void OnBnClickedButtonGc();
	afx_msg void OnBnClickedButtonClr();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
};

#pragma once


class ProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(ProgressDlg)

public:
	ProgressDlg(CWnd* pParent = NULL);
	virtual ~ProgressDlg();
	void SetMsg(CString str);
	CProgressCtrl	m_Prg;
	UINT m_iSolutionType;
	
	enum { IDD = IDD_DIALOG1 };
	CStatic	m_Image;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};

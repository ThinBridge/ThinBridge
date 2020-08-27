#pragma once
#include "afxwin.h"
#include "SettingsDialog.h"


class CDlgSCEditor : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSCEditor)

public:
	CDlgSCEditor(CWnd* pParent = NULL);
	virtual ~CDlgSCEditor();
	autoresize::CAutoResize m_autoResize;
	CString m_strFilePath;
// ダイアログ データ
	enum { IDD = IDD_DLG_SC_EDITOR };

protected:
	BOOL bFirstFlg;
	virtual void DoDataExchange(CDataExchange* pDX);
	void ReadText();
	void WriteText();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit m_Edit;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonFunc();
	afx_msg void OnBnClickedButtonZone();
	afx_msg void OnBnClickedButtonTop();
	afx_msg void OnBnClickedButtonUrl();
	afx_msg void OnBnClickedOk();

	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnPaint();
	int m_SelPosS;
	int m_SelPosE;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonEvent();
};

#pragma once


#include "DlgDebugWnd.h"
class CDlgETC : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgETC)

public:
	CDlgETC();
	virtual ~CDlgETC();
	CDlgDebugWnd* m_pDebugDlg;
// ダイアログ データ
	enum { IDD = IDD_DLG_ETC };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};

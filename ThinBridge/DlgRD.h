#pragma once
#include "afxwin.h"


class CDlgRuleBase : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgRuleBase)

public:
	CDlgRuleBase();
	virtual ~CDlgRuleBase();
	CURLRedirectDataClass m_URD;
	CURLRedirectDataClass m_URDSave;

	enum { IDD = IDD_DLG_RD_RDP };

	enum LIST_INDEX
	{
		URL,
		ENABLE
	};
	int DuplicateChk(CListCtrl* ptrList,LPCTSTR sURL);
	void PopIns(CListCtrl* ptrList);
	void PopDel(CListCtrl* ptrList);
	void PopUpdate(CListCtrl* ptrList);
	void PopEditAll(CListCtrl* ptrList);
	void PopUp(CListCtrl* ptrList);
	void PopDown(CListCtrl* ptrList);

	void UpdateListCounter(CListCtrl* ptrList);

	CListCtrl	m_List;
	CListCtrl	m_List2;
	CStatic		m_Image;
	CComboBox	m_Combo;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy(){CPropertyPage::OnDestroy();}
	afx_msg void OnEnableCtrl();

	afx_msg void OnButtonPopIns();
	afx_msg void OnButtonPopDel();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnBnClickedButtonEditall();
	afx_msg void OnButtonUp();
	afx_msg void OnButtonDown();

	afx_msg void OnButtonPopIns2();
	afx_msg void OnButtonPopDel2();
	afx_msg void OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonUp2();
	afx_msg void OnButtonDown2();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()


public:
	afx_msg void OnBnClickedButtonEditall2();
	afx_msg void OnBnClickedButtonUpdate2();
};

//RDP
class CDlgRD : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgRD)
public:
	CDlgRD(){};
	virtual ~CDlgRD(){};
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
};

//VMware
class CDlgVM : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgVM)
public:
	CDlgVM(){};
	virtual ~CDlgVM(){};
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
};

//Ctrix
class CDlgCTX : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCTX)
public:
	CDlgCTX(){};
	virtual ~CDlgCTX(){};
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
};

//Firefox
class CDlgFF : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgFF)
public:
	CDlgFF(){};
	virtual ~CDlgFF(){};
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
};

//chrome
class CDlgCHR : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCHR)
public:
	CDlgCHR(){};
	virtual ~CDlgCHR(){};
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
};


//Edge
class CDlgEDG : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgEDG)
public:
	CDlgEDG(){};
	virtual ~CDlgEDG(){};
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
};

//Custom01
class CDlgCU01 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU01)
public:
	CDlgCU01(){};
	virtual ~CDlgCU01(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom02
class CDlgCU02 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU02)
public:
	CDlgCU02(){};
	virtual ~CDlgCU02(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom03
class CDlgCU03 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU03)
public:
	CDlgCU03(){};
	virtual ~CDlgCU03(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom04
class CDlgCU04 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU04)
public:
	CDlgCU04(){};
	virtual ~CDlgCU04(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom05
class CDlgCU05 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU05)
public:
	CDlgCU05(){};
	virtual ~CDlgCU05(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom06
class CDlgCU06 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU06)
public:
	CDlgCU06(){};
	virtual ~CDlgCU06(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom07
class CDlgCU07 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU07)
public:
	CDlgCU07(){};
	virtual ~CDlgCU07(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom08
class CDlgCU08 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU08)
public:
	CDlgCU08(){};
	virtual ~CDlgCU08(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom09
class CDlgCU09 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU09)
public:
	CDlgCU09(){};
	virtual ~CDlgCU09(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom10
class CDlgCU10 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU10)
public:
	CDlgCU10(){};
	virtual ~CDlgCU10(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom11
class CDlgCU11 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU11)
public:
	CDlgCU11(){};
	virtual ~CDlgCU11(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom12
class CDlgCU12 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU12)
public:
	CDlgCU12(){};
	virtual ~CDlgCU12(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom13
class CDlgCU13 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU13)
public:
	CDlgCU13(){};
	virtual ~CDlgCU13(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom14
class CDlgCU14 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU14)
public:
	CDlgCU14(){};
	virtual ~CDlgCU14(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom15
class CDlgCU15 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU15)
public:
	CDlgCU15(){};
	virtual ~CDlgCU15(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom16
class CDlgCU16 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU16)
public:
	CDlgCU16(){};
	virtual ~CDlgCU16(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom17
class CDlgCU17 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU17)
public:
	CDlgCU17(){};
	virtual ~CDlgCU17(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom18
class CDlgCU18 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU18)
public:
	CDlgCU18(){};
	virtual ~CDlgCU18(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom19
class CDlgCU19 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU19)
public:
	CDlgCU19(){};
	virtual ~CDlgCU19(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Custom20
class CDlgCU20 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgCU20)
public:
	CDlgCU20(){};
	virtual ~CDlgCU20(){};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	DECLARE_MESSAGE_MAP()
public:
};

//Office365
class CDlgO365 : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgO365)
public:
	CDlgO365() {};
	virtual ~CDlgO365() {};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	afx_msg void OnBnClickedButtonOffice365();
	afx_msg void OnBnClickedButtonReload();
	DECLARE_MESSAGE_MAP()
public:
};
//IDD_DLG_RD_CH_SWITCH
class CDlgChromeSwitcher : public CDlgRuleBase
{
	DECLARE_DYNCREATE(CDlgChromeSwitcher)
public:
	CDlgChromeSwitcher() {};
	virtual ~CDlgChromeSwitcher() {};
	void OnEnableCtrl();
protected:
	virtual BOOL OnInitDialog();
	LRESULT Set_OK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonFdlg();
	afx_msg void OnBnClickedButtonChromeSwitcher();
	DECLARE_MESSAGE_MAP()
public:
};

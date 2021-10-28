
// TBo365URLSyncDlg.h : ヘッダー ファイル
//

#pragma once


// CTBo365URLSyncDlg ダイアログ
class CTBo365URLSyncDlg : public CDialogEx
{
// コンストラクション
public:
	CTBo365URLSyncDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
	enum { IDD = IDD_TBo365URLSync_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート
	enum LIST_INDEX
	{
		URL
	};

	CListCtrl	m_List;
	CListCtrl	m_List2;
	int DuplicateChk(CListCtrl* ptrList, LPCTSTR sURL);
	void PopIns(CListCtrl* ptrList);
	void PopDel(CListCtrl* ptrList);
	void PopUpdate(CListCtrl* ptrList);
	void PopEditAll(CListCtrl* ptrList);
	void PopUp(CListCtrl* ptrList);
	void PopDown(CListCtrl* ptrList);

	void UpdateListCounter(CListCtrl* ptrList);

// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnBnClickedButtonIns();
	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonEditall();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnBnClickedButtonIns2();
	afx_msg void OnBnClickedButtonUpdate2();
	afx_msg void OnBnClickedButtonDel2();
	afx_msg void OnBnClickedButtonEditall2();
	afx_msg void OnBnClickedButtonUp2();
	afx_msg void OnBnClickedButtonDown2();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnBnClickedButtonWriteini();
};


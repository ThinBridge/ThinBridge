
// ThinBridgeRuleUpdaterDlg.h : ヘッダー ファイル
//

#pragma once


// CThinBridgeRuleUpdaterDlg ダイアログ
class CThinBridgeRuleUpdaterDlg : public CDialogEx
{
// コンストラクション
public:
	CThinBridgeRuleUpdaterDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
	enum { IDD = IDD_ThinBridgeRuleUpdater_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

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
	afx_msg void OnBnClickedButtonWriteini();
	afx_msg void OnBnClickedButtonWriteiniNow();
	afx_msg void OnBnClickedButton1();
};

class CIconHelper
{
private:
	HICON m_hICON;
public:
	CIconHelper() { m_hICON = NULL; }
	~CIconHelper() { Release(); }
	void Release() {
		if (m_hICON)
		{
			::DestroyIcon(m_hICON);
			m_hICON = NULL;
		}
	}
	void SetIcon(const HICON ico) {
		if (ico)
		{
			Release();
			m_hICON = ico;
		}
	}
	HICON GetIcon() {
		return m_hICON;
	}
	operator HICON() { return GetIcon(); }
	HICON operator=(HICON ico) {
		SetIcon(ico);
		return GetIcon();
	}
};

class CDlgMsgBox : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMsgBox)

public:
	CDlgMsgBox(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CDlgMsgBox();
	CString m_strMsg;
	CString m_strTitle;
	// ダイアログ データ
	enum { IDD = IDD_DLG_MSGBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	CEdit m_Edit;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

class CDlgSrv : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSrv)

public:
	CDlgSrv(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~CDlgSrv();
	CString m_strConfigServerURL2;
	CString m_strConfigServerURL3;
	CString m_strConfigServerURL4;
	CString m_strConfigServerURL5;
	CString m_strConfigServerURL6;
	int m_iInterval;
	int m_iServerConnectionRule;

	// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedOk();
};

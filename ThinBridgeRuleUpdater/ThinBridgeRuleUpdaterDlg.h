
// ThinBridgeRuleUpdaterDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once


// CThinBridgeRuleUpdaterDlg �_�C�A���O
class CThinBridgeRuleUpdaterDlg : public CDialogEx
{
// �R���X�g���N�V����
public:
	CThinBridgeRuleUpdaterDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^�[

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ThinBridgeRuleUpdater_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g

// ����
protected:
	HICON m_hIcon;

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
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
	CDlgMsgBox(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CDlgMsgBox();
	CString m_strMsg;
	CString m_strTitle;
	// �_�C�A���O �f�[�^
	enum { IDD = IDD_DLG_MSGBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

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
	CDlgSrv(CWnd* pParent = nullptr);   // �W���R���X�g���N�^�[
	virtual ~CDlgSrv();
	CString m_strConfigServerURL2;
	CString m_strConfigServerURL3;
	CString m_strConfigServerURL4;
	CString m_strConfigServerURL5;
	CString m_strConfigServerURL6;
	int m_iInterval;
	int m_iServerConnectionRule;

	// �_�C�A���O �f�[�^
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedOk();
};

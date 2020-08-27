#pragma once
#include "afxwin.h"
#include "SettingsDialog.h"

class CWinDebugMonitor
{
private:
    enum {
        TIMEOUT_WIN_DEBUG    =    50,
    };

    struct dbwin_buffer
    {
        DWORD   dwProcessId;
        char    data[4096*2-sizeof(DWORD)];
    };

private:
	SECURITY_ATTRIBUTES     m_SA;
	SECURITY_DESCRIPTOR     m_SD;

    //HANDLE m_hDBWinMutex;
    HANDLE m_hDBMonBuffer;
    HANDLE m_hEventBufferReady;
    HANDLE m_hEventDataReady;

    HANDLE m_hWinDebugMonitorThread;
    BOOL m_bWinDebugMonStopped;
    struct dbwin_buffer *m_pDBBuffer;

public:
    DWORD Initialize();
    void Unintialize();
    DWORD WinDebugMonitorProcess();
    static DWORD WINAPI WinDebugMonitorThread(void *pData);

public:
    CWinDebugMonitor(){
		SECURITY_ATTRIBUTES     sa={0};
		SECURITY_DESCRIPTOR     sd={0};
		m_SA = sa;
		m_SD = sd;
		m_pEdit=NULL;
	}
    ~CWinDebugMonitor(){}
	CEdit* m_pEdit;
	//CString m_strOutPut;

public:
    virtual void OutputWinDebugString(const char *str) {};
};

class CDlgDebugWnd : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDebugWnd)

public:
	CDlgDebugWnd(CWnd* pParent = NULL);
	virtual ~CDlgDebugWnd();
	CWinDebugMonitor m_WDMon;
	int GetOututDebugString();
	autoresize::CAutoResize m_autoResize;

// ダイアログ データ
	enum { IDD = IDD_DLG_DEBUG_WND };

protected:
	BOOL bFirstFlg;

	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnPaint();
	CEdit m_Edit;
	int m_SelPosS;
	int m_SelPosE;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

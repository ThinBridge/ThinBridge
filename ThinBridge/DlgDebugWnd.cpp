#include "stdafx.h"
#include "ThinBridge.h"
#include "DlgDebugWnd.h"
#include "afxdialogex.h"

DWORD CWinDebugMonitor::Initialize()
{
    DWORD errorCode = 0;
    BOOL bSuccessful = FALSE;

    SetLastError(0);

	m_SA.nLength = sizeof(SECURITY_ATTRIBUTES) ;
	m_SA.bInheritHandle = TRUE ;
	m_SA.lpSecurityDescriptor = &m_SD;

	BOOL bstat = InitializeSecurityDescriptor(&m_SA, SECURITY_DESCRIPTOR_REVISION) ;
	if (!bstat) return -1 ;

	bstat = SetSecurityDescriptorDacl(&m_SA, TRUE, (PACL)NULL, FALSE) ;
	if (!bstat) return -2 ;

    // Event: buffer ready
    // ---------------------------------------------------------
    char DBWIN_BUFFER_READY[] = "DBWIN_BUFFER_READY";
    m_hEventBufferReady = ::OpenEventA(
        STANDARD_RIGHTS_READ,
        FALSE,
        DBWIN_BUFFER_READY
        );

    if (m_hEventBufferReady == NULL) {
        m_hEventBufferReady = ::CreateEventA(
            &m_SA,
            FALSE,    // auto-reset
            TRUE,    // initial state: signaled
            DBWIN_BUFFER_READY
            );

        if (m_hEventBufferReady == NULL) {
            errorCode = GetLastError();
            return errorCode;
        }
    }

    // Event: data ready
    // ---------------------------------------------------------
    char DBWIN_DATA_READY[] = "DBWIN_DATA_READY";
    m_hEventDataReady = ::OpenEventA(
        SYNCHRONIZE,
        FALSE,
        DBWIN_DATA_READY
        );

    if (m_hEventDataReady == NULL) {
        m_hEventDataReady = ::CreateEventA(
            &m_SA,
            FALSE,    // auto-reset
            FALSE,    // initial state: nonsignaled
            DBWIN_DATA_READY
            );

        if (m_hEventDataReady == NULL) {
            errorCode = GetLastError();
            return errorCode;
        }
    }

    // Shared memory
    // ---------------------------------------------------------
    char DBWIN_BUFFER[] = "DBWIN_BUFFER";
    m_hDBMonBuffer = ::OpenFileMappingA(
        FILE_MAP_READ,
        FALSE,
        DBWIN_BUFFER
        );

    if (m_hDBMonBuffer == NULL) {
        m_hDBMonBuffer = ::CreateFileMappingA(
            INVALID_HANDLE_VALUE,
            &m_SA,
            PAGE_READWRITE,
            0,
            sizeof(struct dbwin_buffer),
            DBWIN_BUFFER
            );

        if (m_hDBMonBuffer == NULL)
		{
            errorCode = GetLastError();
            return errorCode;
        }
    }

    m_pDBBuffer = (struct dbwin_buffer *)::MapViewOfFile(
        m_hDBMonBuffer,
        FILE_MAP_READ,
        0,
        0,
        sizeof(struct dbwin_buffer)
        );

    if (m_pDBBuffer == NULL) {
        errorCode = GetLastError();
        return errorCode;
    }

    // Monitoring thread
    // ---------------------------------------------------------
    m_bWinDebugMonStopped = FALSE;

    m_hWinDebugMonitorThread = ::CreateThread(
        NULL,
        0,
        WinDebugMonitorThread,
        this,
        0,
        NULL
        );

    if (m_hWinDebugMonitorThread == NULL) {
        m_bWinDebugMonStopped = TRUE;
        errorCode = GetLastError();
        return errorCode;
    }

    // set monitor thread's priority to highest
    // ---------------------------------------------------------
    bSuccessful = ::SetPriorityClass(
        ::GetCurrentProcess(),
        REALTIME_PRIORITY_CLASS
        );

    bSuccessful = ::SetThreadPriority(
        m_hWinDebugMonitorThread,
        THREAD_PRIORITY_TIME_CRITICAL
        );

    return errorCode;
}
void CWinDebugMonitor::Unintialize()
{
    if (m_hWinDebugMonitorThread != NULL) {
        m_bWinDebugMonStopped = TRUE;
        ::WaitForSingleObject(m_hWinDebugMonitorThread, INFINITE);
    }

    if (m_hDBMonBuffer != NULL) {
        ::UnmapViewOfFile(m_pDBBuffer);
        CloseHandle(m_hDBMonBuffer);
        m_hDBMonBuffer = NULL;
    }

    if (m_hEventBufferReady != NULL) {
        CloseHandle(m_hEventBufferReady);
        m_hEventBufferReady = NULL;
    }

    if (m_hEventDataReady != NULL) {
        CloseHandle(m_hEventDataReady);
        m_hEventDataReady = NULL;
    }

    m_pDBBuffer = NULL;
}
DWORD CWinDebugMonitor::WinDebugMonitorProcess()
{
    DWORD ret = 0;

    // wait for data ready
    ret = ::WaitForSingleObject(m_hEventDataReady, TIMEOUT_WIN_DEBUG);
    if (ret == WAIT_OBJECT_0)
	{
		CStringA strLineA=m_pDBBuffer->data;
		CString strLine(strLineA);
		CString strSingleLine;
		strLine.TrimLeft();
		strLine.TrimRight();
		BOOL bRet=FALSE;
		if(!strLine.IsEmpty())
		{
			//ThinBridgeBHOのPrefixをチェック(TBBHO:)
			if(strLine.Find(_T("ThinBridge:"))==0)
			{
				strLine=strLine.Mid(11);
				bRet=TRUE;
			}
			else
				bRet=FALSE;
		}
		else
			bRet=FALSE;
        //OutputWinDebugString(m_pDBBuffer->data);
        SetEvent(m_hEventBufferReady);
		if(bRet)
		{
			if(m_pEdit&&!strLine.IsEmpty())
			{
				strSingleLine.Format(_T("%s\r\n"),strLine);
				int ndx = m_pEdit->GetWindowTextLength();
				m_pEdit->SendMessage(EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
				m_pEdit->SendMessage(EM_REPLACESEL, 0, (LPARAM)((LPCTSTR) strSingleLine));
			}
	    }
	}
    return ret;
} 
DWORD WINAPI CWinDebugMonitor::WinDebugMonitorThread(void *pData)
{
    CWinDebugMonitor *_this = (CWinDebugMonitor *)pData;

    if (_this != NULL) {
        while (!_this->m_bWinDebugMonStopped) {
            _this->WinDebugMonitorProcess();
			::SleepEx(10,FALSE);
        }
    }

    return 0;
}

IMPLEMENT_DYNAMIC(CDlgDebugWnd, CDialogEx)

CDlgDebugWnd::CDlgDebugWnd(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgDebugWnd::IDD, pParent)
{

}

CDlgDebugWnd::~CDlgDebugWnd()
{
	m_WDMon.Unintialize();
}

void CDlgDebugWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
}


BEGIN_MESSAGE_MAP(CDlgDebugWnd, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgDebugWnd::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgDebugWnd::OnBnClickedCancel)
	ON_WM_ACTIVATE()
	ON_WM_NCACTIVATE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgDebugWnd::OnBnClickedButton1)
	ON_WM_SIZE()
END_MESSAGE_MAP()


BOOL CDlgDebugWnd::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_autoResize.AddSnapToRightBottom(GetDlgItem(IDC_EDIT1));

	bFirstFlg = TRUE;
	m_SelPosS=0;
	m_SelPosE=0;
	m_Edit.LimitText(1024*1024*10);
	m_Edit.SetFocus();

	m_WDMon.m_pEdit=&this->m_Edit;

	int iRet = m_WDMon.Initialize();
	if(iRet)
	{
		CString strMsg;
		strMsg.Format(_T("トレースログの接続に失敗しました。IEをリスタートしてから再度実行して下さい。\nCode 0x%04x:%04d"),iRet,iRet);
		::MessageBox(this->m_hWnd,strMsg,theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
		m_WDMon.Unintialize();
		CDialogEx::OnCancel();
	}
	return FALSE;  // return TRUE unless you set the focus to a control
}

void CDlgDebugWnd::OnBnClickedOk()
{
	m_Edit.SetWindowText(_T(""));
}


void CDlgDebugWnd::OnBnClickedCancel()
{
	m_WDMon.Unintialize();
	CDialogEx::OnCancel();
}
BOOL CDlgDebugWnd::PreTranslateMessage(MSG* pMsg)
{
	ASSERT(pMsg != NULL);
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);
	CString strLine;

	if (pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE)
		{
			//OnCancel();
			return TRUE;
		}
	}
	if(pMsg->hwnd==m_Edit.m_hWnd)
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			if(pMsg->wParam == 'A')
			{
				if(::GetKeyState(VK_CONTROL)<0)
				{
					m_Edit.PostMessage(EM_SETSEL, (WPARAM)0, (LPARAM)-1);
					return TRUE;
				}
			}
		}
		if(pMsg->message == WM_MOUSEMOVE
		||pMsg->message  == WM_LBUTTONDOWN
		||pMsg->message  == WM_KEYDOWN
		||pMsg->message  == WM_KEYUP
		||pMsg->message  == WM_LBUTTONDOWN
		)
		{
			DWORD dwStart=0, dwEnd=0;
			DWORD dwCursorPos=0;
			DWORD dwLineIndex=0;
			CPoint   pt;
			LRESULT lr={0};
			dwCursorPos = m_Edit.SendMessage(EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd);
			if (pMsg->message == WM_MOUSEMOVE && dwStart == dwEnd)
				return lr;
			lr = CDialogEx::PreTranslateMessage(pMsg);

			dwLineIndex = m_Edit.SendMessage(EM_LINEFROMCHAR,-1,0);
			strLine.Format(_T("%d行"),dwLineIndex+1);
			SetDlgItemText(IDC_STATIC_LINE,strLine);
			return lr;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
BOOL CDlgDebugWnd::OnNcActivate(BOOL bActive)
{
	if(bActive)
	{
		if(bFirstFlg)
		{
			bFirstFlg = FALSE;
		}
		else
			m_Edit.PostMessage(EM_SETSEL, (WPARAM)m_SelPosS, (LPARAM)m_SelPosE);
	}
	else
	{
		m_Edit.GetSel(m_SelPosS,m_SelPosE);
	}
	return CDialogEx::OnNcActivate(bActive);
}


void CDlgDebugWnd::OnPaint()
{
	CPaintDC dc(this);
}

void CDlgDebugWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
}



void CDlgDebugWnd::OnBnClickedButton1()
{
	this->m_Edit.SetSel(0, -1);
	this->m_Edit.Copy();
	this->m_Edit.SetFocus();
}


void CDlgDebugWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	m_autoResize.Resize(this);	
}

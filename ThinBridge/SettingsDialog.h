#pragma once


typedef class _PageInfo		PAGE_INFO;
class _PageInfo
{
public:
	BOOL		bViewClass;
	UINT		nID;
	CWnd		*pWnd;
	CWnd		*pWndParent;
	CString		csCaption;
	CString		csParentCaption;
};

namespace autoresize
{
    enum ResizeSpecifier
	{
        LEFT	= 0x01,
		RIGHT	= 0x02,
		TOP		= 0x04,
		BOTTOM	= 0x08,
        NO_HOR_RESIZE = 0x10,
		NO_VERT_RESIZE = 0x20,
    };

    struct ResizeSpec
	{
        HWND m_hwnd;
        CRect m_rc;
        UINT m_resizeSpec;
    };

    class CAutoResize
    {
    public:
        CAutoResize()
        {
        }
        virtual ~CAutoResize()
        {
            if (m_list.GetSize())
			{
                m_list.RemoveAll();
            }
        }

        bool AddSnap(CWnd* wnd, CRect rc, UINT resizeSpec)
        {
            HWND hwnd = wnd->GetSafeHwnd();
            if (!::IsWindow(hwnd))
			{
                TRACE1("CAutoResize::AddSnap: hwnd=%x is not a window! \n", hwnd);
                return false;
            }
			int iMax=m_list.GetSize();
			int i=0;
            for (i = 0; i < iMax; ++i)
			{
                if (m_list[i].m_hwnd == hwnd)
				{
                    break;
                }
            }
            if (resizeSpec == 0)
			{
                if (i < m_list.GetSize())
				{
                    TRACE1("CAutoResize::AddSnap: removing hwnd=%x\n", hwnd);
                    m_list.RemoveAt(i);
                    return true;
                }
                TRACE1("CAutoResize::AddSnap: failed to remove hwnd=%x, not registered. \n", hwnd);
                return false;
            }

            if ((resizeSpec & (LEFT | RIGHT)) == (LEFT | RIGHT)  && (resizeSpec & NO_HOR_RESIZE))
			{
                TRACE1("CAutoResize::AddSnap: both LEFT and RIGHT are specified, but NO_HOR_RESIZE is also specified. %x\n",
                    resizeSpec);
                return false;
            }
            if ((resizeSpec & (TOP | BOTTOM)) == (TOP | BOTTOM) && (resizeSpec & NO_VERT_RESIZE))
			{
                TRACE1("CAutoResize::AddSnap: both TOP and BOTTOM are specified, but NO_VERT_RESIZE is also specified. %x\n",
                    resizeSpec);
                return false;
            }

            if (rc == CRect(INT_MIN, INT_MIN, INT_MIN, INT_MIN))
			{
                CWnd* wndParent = wnd->GetParent();
                if (wndParent == NULL)
				{
                    TRACE0("AddSnapTo: no parent window! \n");
                    return false;
                }
                CRect rcParent;
                wndParent->GetClientRect(&rcParent);
                wnd->GetWindowRect(&rc);
                wndParent->ScreenToClient(&rc);
                // no need to calc left/top, 'cause rcParent is always (0,0)-(?,?).
                rc.BottomRight() = rcParent.BottomRight() - rc.BottomRight();
            }
            else if (rc == CRect(INT_MAX, INT_MAX, INT_MAX, INT_MAX))
			{
                CWnd* wndParent = wnd->GetParent();
                if (wndParent == NULL)
				{
                    TRACE0("AddSnapTo: no parent window! \n");
                    return false;
                }
                CRect rcParent;
                wndParent->GetClientRect(&rcParent);
                wnd->GetWindowRect(&rc);
                wndParent->ScreenToClient(&rc);
                // no need to calc left/top, 'cause rcParent is always (0,0)-(?,?).
                rc.BottomRight() = rcParent.BottomRight() - rc.BottomRight();
				rc.bottom=0;
				rc.right=0;
            }

            if (i >= m_list.GetSize())
			{
                autoresize::ResizeSpec spec;
                spec.m_hwnd = NULL;
                m_list.Add(spec);
            }

            m_list[i].m_hwnd = hwnd;
            m_list[i].m_rc = rc;
            m_list[i].m_resizeSpec = resizeSpec;

            return true;
        }

        bool AddSnapToRight(CWnd* wnd, UINT opt = 0, int cx = INT_MIN)
        {
            return AddSnap(wnd, CRect(INT_MIN, INT_MIN, cx, INT_MIN), RIGHT | opt);
        }
        bool AddSnapToBottom(CWnd* wnd, UINT opt = 0, int cy = INT_MIN)
        {
            return AddSnap(wnd, CRect(INT_MIN, INT_MIN, INT_MIN, cy), BOTTOM | opt);
        }
        bool AddSnapToRightBottom(CWnd* wnd, int cx = INT_MIN, int cy = INT_MIN)
        {
            return AddSnap(wnd, CRect(INT_MIN, INT_MIN, cx, cy), RIGHT | BOTTOM);
        }
        bool AddSnapToFullBottom(CWnd* wnd)
        {
            return AddSnap(wnd, CRect(INT_MAX, INT_MAX,INT_MAX,INT_MAX), RIGHT | BOTTOM);
        }
        void Resize(CWnd* parent)
        {
            //
            // This version uses DeferWindowPos
            //
            HDWP hdwp = BeginDeferWindowPos(m_list.GetSize());
            if (hdwp == NULL)
			{
                return;
            }


            CRect rc;
            parent->GetClientRect(&rc);
			int iMax=m_list.GetSize();
            for (int i = 0; i < iMax; ++i)
			{
                autoresize::ResizeSpec& spec = m_list[i];
                ASSERT(spec.m_resizeSpec != 0);

                CWnd* wnd = CWnd::FromHandle(spec.m_hwnd);
                CRect rcNew;
                wnd->GetWindowRect(&rcNew);
                parent->ScreenToClient(&rcNew);

                int width = rcNew.Width();
                int height = rcNew.Height();

                if (spec.m_resizeSpec & LEFT)
				{
                    rcNew.left = rc.left + spec.m_rc.left;
                    if (spec.m_resizeSpec & NO_HOR_RESIZE)
					{
                        rcNew.right = rcNew.left + width;
                    }
                }
                if (spec.m_resizeSpec & RIGHT)
				{
                    rcNew.right = rc.right - spec.m_rc.right;
                    if (spec.m_resizeSpec & NO_HOR_RESIZE)
					{
                        rcNew.left = rcNew.right - width;
                    }
                }
                if (spec.m_resizeSpec & TOP)
				{
                    rcNew.top = rc.top + spec.m_rc.right;
                    if (spec.m_resizeSpec & NO_VERT_RESIZE)
					{
                        rcNew.bottom = rcNew.top + height;
                    }
                }
                if (spec.m_resizeSpec & BOTTOM)
				{
                    rcNew.bottom = rc.bottom - spec.m_rc.bottom;
                    if (spec.m_resizeSpec & NO_VERT_RESIZE)
					{
                        rcNew.top = rcNew.bottom - height;
                    }
                }
                DeferWindowPos(hdwp, wnd->GetSafeHwnd(), NULL, rcNew.left, rcNew.top, rcNew.Width(), rcNew.Height(), SWP_NOZORDER);
            }

            EndDeferWindowPos(hdwp);
        }
        CArray<autoresize::ResizeSpec, autoresize::ResizeSpec&> m_list;
    };

}
class CPrefsStatic : public CStatic
{
public:
	CPrefsStatic()
	{
		m_textClr = ::GetSysColor (COLOR_3DFACE);
		m_fontWeight = FW_NORMAL;
		m_fontSize = 12;
	}

public:

public:
   CString m_csFontName;

	void SetConstantText(LPCTSTR pText)	{m_csConstantText = pText;}

   int m_fontSize, m_fontWeight;
   BOOL m_grayText;
   COLORREF m_textClr;
public:
	virtual ~CPrefsStatic()
	{
		if (m_bm.GetSafeHandle())
		{
			m_bm.DeleteObject();
		}

		if (m_captionFont.GetSafeHandle())
		{
			m_captionFont.DeleteObject();
		}

		if (m_nameFont.GetSafeHandle())
		{
			m_nameFont.DeleteObject();
		}
	}

protected:
	DECLARE_DYNAMIC(CPrefsStatic)
	CFont m_captionFont, m_nameFont;
	CBitmap m_bm;
	CString m_csConstantText;
	void MakeCaptionBitmap();

protected:
	//{{AFX_MSG(CPrefsStatic)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnSetText (WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


typedef CTypedPtrArray <CPtrArray, PAGE_INFO*>		PAGE_LIST;
typedef CMap<CWnd *, CWnd *, DWORD, DWORD&>			WNDTREE_MAP;

class CSettingsDialog : public CDialog
{
public:
	CSettingsDialog(CWnd* pParent = NULL);
	virtual ~CSettingsDialog();
public:
	BOOL Create();
	void ExpandTree();
	BOOL DestroyPages();
	BOOL CreatePage( const PAGE_INFO *pInfo);
	void ShowPage(const PAGE_INFO *pInfo, UINT nShow = SW_SHOW);
	void SetLogoText(CString sText);
	void SetTitle(CString sTitle);
	HTREEITEM GetNextItemCOrS(HTREEITEM hItem);
	HTREEITEM FindItem(const CString &csCaption);
	HTREEITEM FindItem(CWnd *pWnd);
	CWnd* AddPage(CRuntimeClass *pWndClass, LPCTSTR pCaption, UINT nID,LPCTSTR pParentCaption);
	CWnd* AddPage(CRuntimeClass *pWndClass, LPCTSTR pCaption, UINT nID = 0,CWnd *pDlgParent = NULL);
	autoresize::CAutoResize m_autoResize;

	PAGE_LIST		m_pInfo;
	CRect			m_FrameRect;
	CString			m_csTitle;
	CString			m_csLogoText;
	WNDTREE_MAP		m_wndMap;
	CWnd*			m_pParent;
// Dialog Data
	//{{AFX_DATA(CSettingsDialog)
	enum { IDD = IDD_DLG_SETTING };
	CStatic			m_PageFrame;
	CTreeCtrl		m_TreeCtrl;
	CPrefsStatic	m_CaptionBarCtrl;
	//}}AFX_DATA


	virtual BOOL CreateWnd(CWnd *pWnd, CCreateContext *pContext = NULL);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingsDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	void ExpandBranch(HTREEITEM hti);
	void InitTreeCtrl();
	BOOL RefreshData();

	//{{AFX_MSG(CSettingsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetDispInfoTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg void OnPreferenceHelp();
	afx_msg void OnApply();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

class CSettingsDialogCAP : public CSettingsDialog
{
public:
	CSettingsDialogCAP(CWnd* pParent = NULL)
	{
	}
	virtual ~CSettingsDialogCAP()
	{
	}
protected:
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

};

#include "stdafx.h"
#include "ThinBridge.h"
#include "SettingsDialog.h"
#include "DlgMsgBox.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CPrefsStatic, CStatic)
BEGIN_MESSAGE_MAP(CPrefsStatic, CStatic)
	//{{AFX_MSG_MAP(CPrefsStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE( WM_SETTEXT, OnSetText )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
void CPrefsStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CFont *pOldFont = NULL;
	// Set default font
	if (m_csFontName=="")
	{
		CWnd *pWndParent = GetParent();
		if (pWndParent)
		{
			dc.SelectObject (pWndParent->GetFont());
		}
	}
	else
	{
		// create a font, if we need to
		if (m_captionFont.GetSafeHandle()==NULL)
		{
			m_captionFont.CreateFont( m_fontSize, 
												0, 0, 0, 
												m_fontWeight,
												0, 0, 0, SHIFTJIS_CHARSET,
												OUT_DEFAULT_PRECIS,
												CLIP_DEFAULT_PRECIS,
												DEFAULT_QUALITY,
												FF_MODERN,
												m_csFontName);
		}

		if (m_captionFont.GetSafeHandle()!=NULL)
			pOldFont = dc.SelectObject(&m_captionFont);
	}

	// Draw text
	CString strText;
	GetWindowText(strText);

   dc.SetTextColor(m_textClr);
   dc.SetBkMode(TRANSPARENT);

	// vertical center
   CRect cr;
   GetClientRect(cr); 

	cr.left+=5;
	dc.DrawText( strText, cr, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	if (pOldFont)
		dc.SelectObject(pOldFont);
}

BOOL CPrefsStatic::OnEraseBkgnd(CDC* pDC) 
{
   if (!m_bm.GetSafeHandle())
   {
      MakeCaptionBitmap();
   }

   if (m_bm.GetSafeHandle())
   {
      CRect cr;
      GetClientRect(cr);

      CDC memDC;
      memDC.CreateCompatibleDC(pDC);

      CBitmap *pOB = memDC.SelectObject(&m_bm);

      pDC->BitBlt(0,0,cr.Width(), cr.Height(), &memDC, 0,0, SRCCOPY);

      memDC.SelectObject(pOB);

   }

	return TRUE;
}

LRESULT CPrefsStatic::OnSetText(WPARAM wParam, LPARAM lParam)
{
	DefWindowProc (WM_SETTEXT, wParam, lParam);
	Invalidate(TRUE);
	return (1);
}
   

static void PaintRect(CDC& dc, int x, int y, int w, int h, COLORREF color)
{
	CBrush brush(color);
	CBrush* pOldBrush = dc.SelectObject(&brush);
	dc.PatBlt(x, y, w, h, PATCOPY);
	dc.SelectObject(pOldBrush);
}

void CPrefsStatic::MakeCaptionBitmap()
{
	if (m_bm.m_hObject)
		return;								   // already have bitmap; return

   CRect cr;
   GetClientRect(cr);
   int w = cr.Width();
   int h = cr.Height();

	// Create bitmap same size as caption area and select into memory DC
	//
	CWindowDC dcWin(this);
	CDC dc;
	dc.CreateCompatibleDC(&dcWin);
	m_bm.DeleteObject();
	m_bm.CreateCompatibleBitmap(&dcWin, w, h);
	CBitmap* pOldBitmap = dc.SelectObject(&m_bm);

   COLORREF clrBG = ::GetSysColor(COLOR_3DFACE); // background color
	int r = GetRValue(clrBG);				// red..
	int g = GetGValue(clrBG);				// ..green
	int b = GetBValue(clrBG);				// ..blue color vals
	int x = 8*cr.right/8;					// start 5/6 of the way right
	int w1 = x - cr.left;					// width of area to shade

   int NCOLORSHADES = 128;		// this many shades in gradient

	int xDelta= max( w / NCOLORSHADES , 1);	// width of one shade band

	PaintRect(dc, x, 0, cr.right-x, h, clrBG);

	while (x > xDelta) 
   {												// paint bands right to left
		x -= xDelta;							// next band
		int wmx2 = (w1-x)*(w1-x);			// w minus x squared
		int w2  = w1*w1;						// w squared
		PaintRect(dc, x, 0, xDelta, h,	
			RGB(r-(r*wmx2)/w2, g-(g*wmx2)/w2, b-(b*wmx2)/w2));
	}

	PaintRect(dc,0,0,x,h,RGB(0,0,0));  // whatever's left ==> black

	// draw the 'constant' text

	// create a font, if we need to
	if (m_nameFont.GetSafeHandle()==NULL)
	{
		m_nameFont.CreateFont( 18, 0, 0, 0, FW_BOLD,
											0, 0, 0, ANSI_CHARSET,
											OUT_DEFAULT_PRECIS,
											CLIP_DEFAULT_PRECIS,
											DEFAULT_QUALITY,
											FF_MODERN,
											m_csFontName);	
	}

	CFont * OldFont = dc.SelectObject(&m_nameFont);

	// back up a little
	cr.right-=5;

	// draw text in DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor( ::GetSysColor( COLOR_3DHILIGHT));
	dc.DrawText( m_csConstantText, cr + CPoint(1,1), DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	dc.SetTextColor( ::GetSysColor( COLOR_3DSHADOW));
	dc.DrawText( m_csConstantText, cr, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

	// restore old font
	dc.SelectObject(OldFont);

	// Restore DC
	dc.SelectObject(pOldBitmap);
}

CSettingsDialog::CSettingsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDialog::IDD, pParent)
{
	m_pInfo.RemoveAll();	// Empty the page info
	m_csTitle = _T("");
	m_pParent = pParent;	// Parent wnd pointer, must be null for modal dialog
}

CSettingsDialog::~CSettingsDialog()
{
	// Clean up garbage
	for (int i=0; i<m_pInfo.GetSize(); i++)
	{
		PAGE_INFO *pInfo = (PAGE_INFO *)m_pInfo.GetAt(i);
		if (pInfo) 
		{
			delete pInfo;
		}
	}
	m_pInfo.RemoveAll();
}

void CSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsDialog)
	DDX_Control(pDX, IDC_PAGE_FRAME, m_PageFrame);
	DDX_Control(pDX, IDC_TREE_CTRL, m_TreeCtrl);
	DDX_Control(pDX, IDC_CAPTION_BAR, m_CaptionBarCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsDialog, CDialog)
	//{{AFX_MSG_MAP(CSettingsDialog)
	ON_NOTIFY(TVN_GETDISPINFO, IDC_TREE_CTRL, OnGetDispInfoTreeCtrl)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CTRL, OnTreeSelChanged)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CSettingsDialog::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CSettingsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CIconHelper ICoHelper;
    ICoHelper       = theApp.LoadIcon(IDR_MAINFRAME);
	SetIcon(ICoHelper, TRUE);
	SetIcon(ICoHelper, FALSE);

	SetWindowText(m_csTitle);

	m_CaptionBarCtrl.m_textClr     = ::GetSysColor(COLOR_3DFACE);
	m_CaptionBarCtrl.m_fontWeight  = FW_BOLD;
	m_CaptionBarCtrl.m_fontSize    = 14;
	m_CaptionBarCtrl.m_csFontName  = _T("Verdana");
	m_CaptionBarCtrl.SetConstantText(m_csLogoText);

	m_PageFrame.GetWindowRect(m_FrameRect);
	ScreenToClient(m_FrameRect);
	m_FrameRect.DeflateRect(2,2);
	InitTreeCtrl();	
	ExpandTree();

	for (int i=0; i<m_pInfo.GetSize(); i++)
	{
		PAGE_INFO *pInfo = (PAGE_INFO *)m_pInfo.GetAt(i);
		if (pInfo) 
		{
			if(theApp.m_pDlgMsg)
			{
				theApp.m_pDlgMsg->SetMsg(pInfo->csCaption);
				theApp.m_pDlgMsg->m_Prg.StepIt();
			}
			ShowPage(pInfo);
		}
	}

	if (m_pInfo.GetAt(0)) ShowPage(m_pInfo.GetAt(0));

	if(theApp.m_pDlgMsg)
	{
		::Sleep(100);
		delete theApp.m_pDlgMsg;
		theApp.m_pDlgMsg=NULL;
	}

	TCHAR szPCName[128]={0};
	DWORD dPCNameBufLen=120;
	CString strPCName;
	::GetComputerName(szPCName,&dPCNameBufLen);
	strPCName = szPCName;
	SetDlgItemText(IDC_STATIC_PCNAME,strPCName);


	return TRUE;
}

void CSettingsDialog::InitTreeCtrl()
{
	for (int i=0; i < m_pInfo.GetSize(); i++)
	{
		PAGE_INFO *pInfo = (PAGE_INFO *)m_pInfo.GetAt(i);
		if (!pInfo) continue;
		TV_INSERTSTRUCT tvi={0};
		if (pInfo->pWndParent) tvi.hParent = FindItem(pInfo->pWndParent);
		else tvi.hParent = FindItem(pInfo->csParentCaption);
		tvi.hInsertAfter = TVI_LAST;
		tvi.item.cchTextMax = 0;
		tvi.item.pszText = LPSTR_TEXTCALLBACK;
		tvi.item.lParam = (long)pInfo;
		tvi.item.mask = TVIF_PARAM | TVIF_TEXT;
		HTREEITEM hTree = m_TreeCtrl.InsertItem(&tvi);
		if (hTree && pInfo->pWnd)
		{
			DWORD dwTree = (DWORD)hTree;
			m_wndMap.SetAt(pInfo->pWnd, dwTree);
		}
	}
}

CWnd* CSettingsDialog::AddPage(CRuntimeClass  *pWndClass, 
							   LPCTSTR pCaption, UINT nID, 
							   CWnd *pWndParent)
{
	CWnd* pWnd = NULL;
	if (m_hWnd)
	{
		ASSERT(0);
		return pWnd;
	}
	if (pWndClass) pWnd = (CWnd*) pWndClass->CreateObject();	

	PAGE_INFO *pInfo = new PAGE_INFO;
	pInfo->bViewClass = TRUE;
	if (pWnd)
	{
		if (pWnd->IsKindOf(RUNTIME_CLASS(CDialog)))	pInfo->bViewClass = FALSE;
	}

	pInfo->nID = nID;
	pInfo->pWnd = pWnd;
	pInfo->csCaption = pCaption;
	pInfo->pWndParent = pWndParent;
	pInfo->csParentCaption.Empty();
	m_pInfo.Add(pInfo);
	return pWnd;
}

CWnd* CSettingsDialog::AddPage(CRuntimeClass *pWndClass, LPCTSTR pCaption, 
							   UINT nID, LPCTSTR pParentCaption)
{
	CWnd* pWnd = NULL;
	if (m_hWnd)
	{
		ASSERT(0);
		return pWnd;
	}
	if (pWndClass) pWnd = (CWnd*) pWndClass->CreateObject();
	PAGE_INFO *pInfo = new PAGE_INFO;
	pInfo->bViewClass = TRUE;
	if (pWnd)
	{
		if (pWnd->IsKindOf(RUNTIME_CLASS(CDialog)))	pInfo->bViewClass = FALSE;
	}
	pInfo->nID = nID;
	pInfo->pWnd = pWnd;
	pInfo->csCaption = pCaption;
	pInfo->pWndParent = NULL;
	pInfo->csParentCaption = pParentCaption;
	m_pInfo.Add(pInfo);
	return pWnd;
}


HTREEITEM CSettingsDialog::FindItem(CWnd *pWnd)
{
	if (pWnd == NULL) return TVI_ROOT;
	else
	{
		DWORD dwHTree;
		if (m_wndMap.Lookup(pWnd, dwHTree)) return (HTREEITEM)dwHTree;
		else
		{
			ASSERT(FALSE);
			return (TVI_ROOT);
		}
	}
}

HTREEITEM CSettingsDialog::FindItem(const CString &csCaption)
{
	if(m_TreeCtrl.GetCount() == 0) return TVI_ROOT;
	if(csCaption.IsEmpty()) return TVI_ROOT;

	HTREEITEM hCurrent  = m_TreeCtrl.GetRootItem();
	while(hCurrent)
	{
		CString strText = m_TreeCtrl.GetItemText(hCurrent);
		if(!strText.CompareNoCase(csCaption))	return hCurrent;
		hCurrent = GetNextItemCOrS(hCurrent);
	}
	return TVI_ROOT;

}

HTREEITEM CSettingsDialog::GetNextItemCOrS(HTREEITEM hItem)
{
	HTREEITEM	hti={0};
	if( m_TreeCtrl.ItemHasChildren( hItem ) )
		return m_TreeCtrl.GetChildItem( hItem );
	else
	{
		while( (hti = m_TreeCtrl.GetNextSiblingItem( hItem )) == NULL )
		{
			if( (hItem = m_TreeCtrl.GetParentItem( hItem ) ) == NULL )
				return NULL;
		}
	}
	return hti;
}

void CSettingsDialog::ShowPage(const PAGE_INFO *pInfo, UINT nShow)
{
	if (!pInfo) return;
	m_CaptionBarCtrl.SetWindowText(_T(""));

	if (pInfo->pWnd)
	{
		if (!::IsWindow(pInfo->pWnd->m_hWnd))
		{
			CreatePage(pInfo);
			pInfo->pWnd->SetWindowPos (&m_TreeCtrl,0,0,0,0,0);
			pInfo->pWnd->MoveWindow(m_FrameRect.left, m_FrameRect.top, m_FrameRect.Width(), m_FrameRect.Height());
			if( pInfo->pWnd->IsKindOf( RUNTIME_CLASS(CView) ) )
				((CView*)pInfo->pWnd)->OnInitialUpdate();
		}
		
		pInfo->pWnd->ShowWindow(nShow);
		if (nShow == SW_SHOW)
		{
			pInfo->pWnd->SetFocus();
			HTREEITEM hItem = FindItem(pInfo->pWnd);
			if (hItem) m_TreeCtrl.SelectItem(hItem);
		}
	}
	if (nShow == SW_SHOW)
		m_CaptionBarCtrl.SetWindowText(pInfo->csCaption);
}

BOOL CSettingsDialog::CreatePage(const PAGE_INFO *pInfo)
{
	BOOL bCode = FALSE;

	if (!pInfo || !pInfo->pWnd) return(FALSE);
	if (!::IsWindow(pInfo->pWnd->m_hWnd))
	{
		if (pInfo->pWnd->IsKindOf(RUNTIME_CLASS(CDialog)))
		{
			CDialog *pDlg = (CDialog*)pInfo->pWnd;
			bCode = pDlg->Create(pInfo->nID, this);
			pDlg->ModifyStyle(WS_CAPTION, 0);
		}
		else if (pInfo->pWnd->IsKindOf(RUNTIME_CLASS(CWnd)))
		{
			CWnd *pWnd = (CWnd*)pInfo->pWnd;
			bCode = CreateWnd(pInfo->pWnd);
			pWnd->ModifyStyle(WS_BORDER|WS_THICKFRAME, 0);
		}
	}
	return(bCode);
}

BOOL CSettingsDialog::CreateWnd(CWnd *pWnd, CCreateContext *pContext)
{
	CCreateContext context;	
	if (pContext == NULL)	
	{
		context.m_pCurrentFrame = (CFrameWnd*) this;
		context.m_pCurrentDoc = NULL;
		context.m_pNewViewClass = NULL;
		pContext = &context;

	}
	
    ASSERT_KINDOF(CWnd, pWnd);
	ASSERT(pWnd->m_hWnd == NULL);
	
	DWORD dwStyle = AFX_WS_DEFAULT_VIEW;
	CRect rect;
	if (!pWnd->Create(NULL, NULL, dwStyle, rect, this, 0, pContext))
	{
		TRACE0("Warning: couldn't create wnd in CSettingsDialog.\n");
		return FALSE;
	}
	return(TRUE);
}

void CSettingsDialog::OnGetDispInfoTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	if (pTVDispInfo->item.lParam)
	{
		if (pTVDispInfo->item.mask & TVIF_TEXT)
		{
			PAGE_INFO *pInfo = (PAGE_INFO *)pTVDispInfo->item.lParam;
			pTVDispInfo->item.pszText=pInfo->csCaption.GetBufferSetLength(pInfo->csCaption.GetLength());
			pInfo->csCaption.ReleaseBuffer();
		}
	}
	
	*pResult = 0;
}

void CSettingsDialog::OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	if (pNMTreeView->itemNew.lParam)
	{
		PAGE_INFO *pInfo = (PAGE_INFO *) pNMTreeView->itemOld.lParam;
		ShowPage(pInfo, SW_HIDE);
		pInfo = (PAGE_INFO *) pNMTreeView->itemNew.lParam;
		ShowPage(pInfo, SW_SHOW);
	}
	*pResult = 0;
}

void CSettingsDialog::OnCancel() 
{
	DestroyPages();
	CDialog::OnCancel();
}

void CSettingsDialog::OnApply() 
{
}

void CSettingsDialog::OnOK() 
{
	int iRet=0;
	iRet = ::MessageBox(this->m_hWnd,_T("設定を保存しますか？"),theApp.m_strThisAppName,MB_ICONQUESTION | MB_OKCANCEL);
	if(iRet == IDCANCEL)
		return;

	RefreshData();
	for (int i=0; i<m_pInfo.GetSize(); i++)
	{
		PAGE_INFO *pInfo = (PAGE_INFO *)m_pInfo.GetAt(i);
		if (pInfo && pInfo->pWnd)
		{
			if(theApp.IsWnd(pInfo->pWnd))
			{
				if(pInfo->pWnd->SendMessage(ID_SETTING_OK,0,0)!=0)
				{
					ShowPage(pInfo,SW_SHOW);
					return;
				}
			}
		}
	}
	//file backup
	::CopyFile(theApp.m_strRedirectFilePath+".bak4", theApp.m_strRedirectFilePath+".bak5", FALSE);
	::CopyFile(theApp.m_strRedirectFilePath+".bak3", theApp.m_strRedirectFilePath+".bak4", FALSE);
	::CopyFile(theApp.m_strRedirectFilePath+".bak2", theApp.m_strRedirectFilePath+".bak3", FALSE);
	::CopyFile(theApp.m_strRedirectFilePath+".bak1", theApp.m_strRedirectFilePath+".bak2", FALSE);
	::CopyFile(theApp.m_strRedirectFilePath, theApp.m_strRedirectFilePath+".bak1", FALSE);

	//読み取り専用のチェックを外す。
	DWORD dwAttributes = 0;
	SetFileAttributes(theApp.m_strRedirectFilePath, dwAttributes);
	//::DeleteFile(theApp.m_strRedirectFilePath);
	SetLastError(NO_ERROR);
	BOOL bWriteFlg=FALSE;
	CString strSaveMsg;
	CString strOutPutDataString;	
	for(int i=0;i<3;i++)
	{
		strOutPutDataString.Empty();
		if(theApp.m_RedirectListSaveData.SaveData(theApp.m_strRedirectFilePath,strOutPutDataString))
		{
			bWriteFlg=TRUE;
			//差分を出力する。
			CTime time = CTime::GetCurrentTime();
			CString strLogFileNow;
			CString strHistoryDir;
			strHistoryDir = theApp.m_strExeFolderPath;
			strHistoryDir += _T("History\\");
			::CreateDirectory(strHistoryDir, NULL);

			CString strBefore;
			strBefore = strHistoryDir;
			strLogFileNow.Format(_T("%s_%s_Before.ini"), _T("ThinBridgeBHO"), time.Format(_T("%Y-%m-%d-%H%M%S")));
			strBefore += strLogFileNow;
			::CopyFile(theApp.m_strRedirectFilePath + ".bak1", strBefore, FALSE);
			SetFileAttributes(strBefore, 0);
			this->LogRotateETC(strHistoryDir, _T("ThinBridgeBHO_"), _T("_Before.ini"));

			CString strAfter;
			strAfter = strHistoryDir;
			strLogFileNow.Format(_T("%s_%s_After.ini"), _T("ThinBridgeBHO"), time.Format(_T("%Y-%m-%d-%H%M%S")));
			strAfter += strLogFileNow;
			::CopyFile(theApp.m_strRedirectFilePath, strAfter, FALSE);
			SetFileAttributes(strAfter, 0);
			this->LogRotateETC(strHistoryDir, _T("ThinBridgeBHO_"), _T("_After.ini"));


			CString strDiffFile;
			strDiffFile = strHistoryDir;
			strLogFileNow.Format(_T("%s_%s_Diff.ini"), _T("ThinBridgeBHO"), time.Format(_T("%Y-%m-%d-%H%M%S")));
			strDiffFile += strLogFileNow;
			this->LogRotateETC(strHistoryDir, _T("ThinBridgeBHO_"), _T("_Diff.ini"));

			CompareFiles(strBefore, strAfter, strDiffFile);
			break;
		}
		::Sleep(500);
	}
	if(!bWriteFlg)
	{
		strSaveMsg.Format(_T("設定ファイルの保存に失敗しました。\n\n別のプログラムがファイルを開いているか、書込権限が不足しています。操作を完了できません。ファイルを閉じてから再実行してください。\n\n%s"),theApp.m_strRedirectFilePath);
		::MessageBox(this->m_hWnd,strSaveMsg,theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
	}
	else
	{
		strOutPutDataString.Replace(_T("\n"),_T("\r\n"));
		strSaveMsg.Format(_T("【成功】\r\n設定ファイルの保存に成功しました。\r\n\r\n%s\r\n%s"),theApp.m_strRedirectFilePath,strOutPutDataString);
		CDlgMsgBox MsgBox;
		MsgBox.m_strMsg=strSaveMsg;
		MsgBox.m_strTitle=_T("設定ファイル保存 成功");
		MsgBox.DoModal();
	}

//	DestroyPages();
//	CDialog::OnOK();	
}

BOOL CSettingsDialog::RefreshData()
{
	PAGE_INFO *pInfoErr = NULL, *pInfo;
	for (int i = 0; i < m_pInfo.GetSize(); i++)
	{
		pInfo = (PAGE_INFO *)m_pInfo.GetAt(i);
		if (pInfo && pInfo->pWnd)
		{
			if (::IsWindow(pInfo->pWnd->m_hWnd))
				pInfo->pWnd->UpdateData(TRUE);
		}
	}
	return (TRUE);		
}

BOOL CSettingsDialog::DestroyPages()
{
	for (int i=0; i<m_pInfo.GetSize(); i++)
	{
		PAGE_INFO *pInfo = (PAGE_INFO *)m_pInfo.GetAt(i);
		if (pInfo && pInfo->pWnd)
		{
//			if (::IsWindow(pInfo->pWnd->m_hWnd))
			{
				pInfo->pWnd->DestroyWindow();
				if (!(pInfo->bViewClass))  
					delete(pInfo->pWnd);
			}
		}
	}
	return(true);
}

void CSettingsDialog::OnPreferenceHelp() 
{
}

BOOL CSettingsDialog::PreTranslateMessage(MSG* pMsg) 
{
	ASSERT(pMsg != NULL);
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);

	if (pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE)
		{
			//OnCancel();
			return TRUE;
		}
	}
	if (CWnd::PreTranslateMessage(pMsg)) return TRUE;

	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	if (pFrameWnd != NULL && pFrameWnd->m_bHelpMode) return FALSE;

	pFrameWnd = GetParentFrame();
	while (pFrameWnd != NULL)
	{
		if (pFrameWnd->PreTranslateMessage(pMsg)) return TRUE;
		pFrameWnd = pFrameWnd->GetParentFrame();
	}

	return PreTranslateInput(pMsg);	
//	return CDialog::PreTranslateMessage(pMsg);
}

void CSettingsDialog::SetTitle(CString sTitle)
{
	m_csTitle = sTitle;
}

void CSettingsDialog::SetLogoText(CString sText)
{
	m_csLogoText= sText;
}

void CSettingsDialog::ExpandTree()
{
	HTREEITEM hti = m_TreeCtrl.GetRootItem();
	while (hti)
	{
		ExpandBranch(hti);
		hti = m_TreeCtrl.GetNextSiblingItem(hti);
	}
}

void CSettingsDialog::ExpandBranch(HTREEITEM hti)
{
	if( m_TreeCtrl.ItemHasChildren( hti ) )
	{
		m_TreeCtrl.Expand( hti, TVE_EXPAND );
		hti = m_TreeCtrl.GetChildItem( hti );
		do
		{
			ExpandBranch( hti );
		} while( (hti = m_TreeCtrl.GetNextSiblingItem( hti )) != NULL );
	}
	m_TreeCtrl.EnsureVisible( m_TreeCtrl.GetSelectedItem() );
}

BOOL CSettingsDialog::Create()
{
	return CDialog::Create(CSettingsDialog::IDD);
}


void CSettingsDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
}


void CSettingsDialog::OnBnClickedOk()
{
	CSettingsDialog::OnOK();
}


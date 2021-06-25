#include "stdafx.h"
#include "ThinBridge.h"
#include "DlgRD.h"
#include "afxdialogex.h"
#include "DlgRDEdit.h"
#include "DlgEditURLML.h"

IMPLEMENT_DYNCREATE(CDlgRuleBase, CPropertyPage)

CDlgRuleBase::CDlgRuleBase()
	: CPropertyPage(CDlgRuleBase::IDD)
{
}

CDlgRuleBase::~CDlgRuleBase()
{
}

void CDlgRuleBase::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRuleBase)
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_LIST2, m_List2);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_Image);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO1, m_Combo);
}

BEGIN_MESSAGE_MAP(CDlgRuleBase, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgRuleBase)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON_INS, OnButtonPopIns)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonPopDel)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnButtonDown)

	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnDblclkList2)
	ON_BN_CLICKED(IDC_BUTTON_INS2, OnButtonPopIns2)
	ON_BN_CLICKED(IDC_BUTTON_DEL2, OnButtonPopDel2)
	ON_BN_CLICKED(IDC_BUTTON_UP2, OnButtonUp2)
	ON_BN_CLICKED(IDC_BUTTON_DOWN2, OnButtonDown2)

	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	//}}AFX_MSG_MAP
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnBnClickedButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON_EDITALL,OnBnClickedButtonEditall)
	ON_BN_CLICKED(IDC_BUTTON_EDITALL2,OnBnClickedButtonEditall2)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE2,OnBnClickedButtonUpdate2)
END_MESSAGE_MAP()

void CDlgRuleBase::OnEnableCtrl()
{
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;

	if (GetDlgItem(IDC_EDIT1))
		GetDlgItem(IDC_EDIT1)->EnableWindow(bChk);
	if (GetDlgItem(IDC_COMBO1))
		GetDlgItem(IDC_COMBO1)->EnableWindow(bChk);

	if (GetDlgItem(IDC_LIST1))
		GetDlgItem(IDC_LIST1)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_INS))
		GetDlgItem(IDC_BUTTON_INS)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_DEL))
		GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_UP))
		GetDlgItem(IDC_BUTTON_UP)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_DOWN))
		GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_UPDATE))
		GetDlgItem(IDC_BUTTON_UPDATE)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_EDITALL))
		GetDlgItem(IDC_BUTTON_EDITALL)->EnableWindow(bChk);

	if (GetDlgItem(IDC_LIST2))
		GetDlgItem(IDC_LIST2)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_INS2))
		GetDlgItem(IDC_BUTTON_INS2)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_DEL2))
		GetDlgItem(IDC_BUTTON_DEL2)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_UP2))
		GetDlgItem(IDC_BUTTON_UP2)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_DOWN2))
		GetDlgItem(IDC_BUTTON_DOWN2)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_UPDATE2))
		GetDlgItem(IDC_BUTTON_UPDATE2)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_EDITALL2))
		GetDlgItem(IDC_BUTTON_EDITALL2)->EnableWindow(bChk);
	return;
}

int CDlgRuleBase::DuplicateChk(CListCtrl* ptrList,LPCTSTR sURL)
{
	int iRet=-1; //重複なしは、-1を返す。
	CString strURL;
	int iSelCount = -1;
	while((iSelCount = ptrList->GetNextItem(iSelCount,LVNI_ALL)) != -1)
	{
		strURL.Empty();
		strURL = ptrList->GetItemText(iSelCount,URL);
		if(strURL==sURL)
		{
			iRet=iSelCount;
			break;	
		}
	}
	return iRet;
}

void CDlgRuleBase::UpdateListCounter(CListCtrl* ptrList)
{
	CString str;
	str.Format(_T("%d件"),ptrList->GetItemCount());
	if(ptrList==&this->m_List)
	{
		SetDlgItemText(IDC_STATIC_CNT,str);
	}
	else
	{
		SetDlgItemText(IDC_STATIC_CNT2,str);
	}
}

void CDlgRuleBase::PopIns(CListCtrl* ptrList)
{
	CDlgRDEdit Dlg(this);
	Dlg.m_bEnable=TRUE;

	if(Dlg.DoModal() == IDOK)
	{
		int iRet = DuplicateChk(ptrList,Dlg.m_strURL);
		//重複なし。
		if(iRet==-1)
		{
			int index = ptrList->GetItemCount();
			int iItem = ptrList->InsertItem( index, _T(""));
			CString strTemp;
			ptrList->SetItemText(iItem,URL,Dlg.m_strURL);
			strTemp = Dlg.m_bEnable ? _T("○"):_T("－");
			ptrList->SetItemText(iItem,ENABLE,strTemp);
			ptrList->SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);
		}
		else
		{
			ptrList->SetFocus();
			ptrList->SetItemState(iRet, LVIS_SELECTED, LVIS_SELECTED);
			CString strErrMsg;
			strErrMsg.Format(_T("このURLは、既に登録されています。\n%d行目\n%s\n"),iRet+1,Dlg.m_strURL);
			::MessageBox(this->m_hWnd,strErrMsg,theApp.m_strThisAppName,MB_OK|MB_ICONWARNING);
		}
	}
	UpdateListCounter(ptrList);
}

void CDlgRuleBase::PopDel(CListCtrl* ptrList)
{
	int iSelCount = 0;
	iSelCount = ptrList->GetSelectedCount();
	if ( iSelCount == 0 )
		return;

	int nItemCount = ptrList->GetItemCount();	
	while(nItemCount--)
	{
		if(ptrList->GetItemState(nItemCount, LVIS_SELECTED) == LVIS_SELECTED)
		{
			ptrList->DeleteItem(nItemCount);
			{
				int nItemNowCount = ptrList->GetItemCount();	
				if(nItemNowCount == nItemCount)
					ptrList->SetItemState(nItemCount-1, LVIS_SELECTED, LVIS_SELECTED);
				else if(nItemNowCount>0)
					ptrList->SetItemState(nItemCount, LVIS_SELECTED, LVIS_SELECTED);
				break;
			}
		}
	}
	UpdateListCounter(ptrList);
}

void CDlgRuleBase::PopUpdate(CListCtrl* ptrList)
{
	int iSelCount = 0;
	iSelCount = ptrList->GetSelectedCount();
	if ( iSelCount != 1 )
		return ;
	iSelCount = ptrList->GetNextItem(-1, LVNI_ALL |LVNI_SELECTED);
	if(iSelCount != -1)
	{
		CDlgRDEdit Dlg(this);
		BOOL bEnable = FALSE;
		CString strURL;
		CString strTemp;
		strURL = ptrList->GetItemText(iSelCount,URL);
		strTemp = ptrList->GetItemText(iSelCount,ENABLE);
		bEnable = strTemp==_T("○")?TRUE:FALSE;

		Dlg.m_strURL=strURL;
		Dlg.m_bEnable=bEnable;

		if(Dlg.DoModal() == IDOK)
		{
			int iRet = DuplicateChk(ptrList,Dlg.m_strURL);
			//重複なし。
			if(iRet==-1 || iRet==iSelCount)
			{
				CString strTemp;
				ptrList->SetItemText(iSelCount,URL,Dlg.m_strURL);
				strTemp = Dlg.m_bEnable ? _T("○"):_T("－");
				ptrList->SetItemText(iSelCount,ENABLE,strTemp);
			}
			else
			{
				ptrList->SetFocus();
				ptrList->SetItemState(iRet, LVIS_SELECTED, LVIS_SELECTED);
				CString strErrMsg;
				strErrMsg.Format(_T("このURLは、既に登録されています。\n%d行目\n%s\n"),iRet+1,Dlg.m_strURL);
				::MessageBox(this->m_hWnd,strErrMsg,theApp.m_strThisAppName,MB_OK|MB_ICONWARNING);
			}
		}
	}
	UpdateListCounter(ptrList);
}

void CDlgRuleBase::PopEditAll(CListCtrl* ptrList)
{
	CString strData;

	CString strURL;
	CString strTemp;
	int iSelCount = -1;
	while((iSelCount = ptrList->GetNextItem(iSelCount,LVNI_ALL)) != -1)
	{
		strURL.Empty();
		strTemp.Empty();
		strURL = ptrList->GetItemText(iSelCount,URL);
		strTemp = ptrList->GetItemText(iSelCount,ENABLE);
		if(strTemp==_T("－"))
		{
			strData+=_T("#");
		}
		strData+=strURL;
		strData+=_T("\r\n");
	}

	CDlgEditURLML Dlg;
	if(ptrList==&m_List)
		Dlg.m_strTitle = _T("【ターゲットURL ルール】 一括登録・編集");
	else
		Dlg.m_strTitle = _T("【除外URL ルール】 一括登録・編集");

	Dlg.m_strEditData=strData;
	if(IDOK==Dlg.DoModal())
	{
		//一旦全部消す。
		ptrList->DeleteAllItems();
		int iMaxCnt=0;
		iMaxCnt = Dlg.m_strArrayResult.GetCount();
		CString strLineData;
		for(int i=0;i<iMaxCnt;i++)
		{
			strLineData=Dlg.m_strArrayResult.GetAt(i);
			int index = ptrList->GetItemCount();
			int iItem = ptrList->InsertItem( index, _T(""));
			CString strLowString;
			ptrList->SetItemText(iItem,URL,strLineData);
			strLowString = Dlg.m_strArrayResult_Enable.GetAt(i);
			ptrList->SetItemText(iItem,ENABLE,strLowString);
		}
	}
	UpdateListCounter(ptrList);
}

void CDlgRuleBase::PopUp(CListCtrl* ptrList)
{
	int iSelCount = 0;
	int iAfterPos = 0;
	iSelCount = ptrList->GetSelectedCount();
	if ( iSelCount != 1 )
		return ;
	iSelCount = ptrList->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if(iSelCount == -1)
		return;

	CString strURL;
	CString strEnable;
	strURL = ptrList->GetItemText(iSelCount,URL);
	strEnable = ptrList->GetItemText(iSelCount,ENABLE);
	iAfterPos = iSelCount - 1;
	if(iAfterPos <0 )
	{
		ptrList->SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		ptrList->SetFocus();
		return;
	}
	ptrList->DeleteItem(iSelCount);
	int iItem = ptrList->InsertItem( iAfterPos, _T(""));
	ptrList->SetItemText(iItem,URL,strURL);
	ptrList->SetItemText(iItem,ENABLE,strEnable);
	ptrList->SetItemState(iAfterPos, LVIS_SELECTED, LVIS_SELECTED);
	ptrList->SetFocus();
}

void CDlgRuleBase::PopDown(CListCtrl* ptrList)
{
	int iSelCount = 0;
	int iAfterPos = 0;
	iSelCount = ptrList->GetSelectedCount();
	if ( iSelCount != 1 )
		return ;
	iSelCount = ptrList->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if(iSelCount == -1)
		return;

	CString strURL;
	CString strEnable;
	strURL = ptrList->GetItemText(iSelCount,URL);
	strEnable = ptrList->GetItemText(iSelCount,ENABLE);
	iAfterPos = iSelCount + 1;
	if(iAfterPos >= ptrList->GetItemCount() )
	{
		ptrList->SetItemState(iSelCount, LVIS_SELECTED, LVIS_SELECTED);
		ptrList->SetFocus();
		return;
	}
	ptrList->DeleteItem(iSelCount);
	int iItem = ptrList->InsertItem( iAfterPos, _T(""));
	ptrList->SetItemText(iItem,URL,strURL);
	ptrList->SetItemText(iItem,ENABLE,strEnable);
	ptrList->SetItemState(iAfterPos, LVIS_SELECTED, LVIS_SELECTED);
	ptrList->SetFocus();
}

void CDlgRuleBase::OnButtonPopIns() 
{
	PopIns(&m_List);
}
void CDlgRuleBase::OnButtonPopIns2() 
{
	PopIns(&m_List2);
}

void CDlgRuleBase::OnButtonPopDel() 
{
	PopDel(&m_List);
}
void CDlgRuleBase::OnButtonPopDel2() 
{
	PopDel(&m_List2);
}

void CDlgRuleBase::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	PopUpdate(&m_List);
	*pResult = 0;
}
void CDlgRuleBase::OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	PopUpdate(&m_List2);
	*pResult = 0;
}

void CDlgRuleBase::OnButtonUp() 
{
	PopUp(&m_List);
}
void CDlgRuleBase::OnButtonUp2() 
{
	PopUp(&m_List2);
}

void CDlgRuleBase::OnButtonDown() 
{
	PopDown(&m_List);
}

void CDlgRuleBase::OnButtonDown2()
{
	PopDown(&m_List2);
}

void CDlgRuleBase::OnBnClickedButtonUpdate()
{
	PopUpdate(&m_List);
}

void CDlgRuleBase::OnBnClickedButtonUpdate2()
{
	PopUpdate(&m_List2);
}

void CDlgRuleBase::OnBnClickedButtonEditall()
{
	PopEditAll(&m_List);
}

void CDlgRuleBase::OnBnClickedButtonEditall2()
{
	PopEditAll(&m_List2);
}

static LPCTSTR gRedirectPageActionList[] = {
	_T("リダイレクトページを閉じる[背景色　青]"),
	_T("前のページに戻る(前のページがない場合は、閉じる)[背景色　黄]"),
	_T("リダイレクトページを表示したままにする[背景色　白]"),
};


BOOL CDlgRuleBase::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_List.InsertColumn(URL,_T("ターゲットURL ルール"),LVCFMT_LEFT,700);
	m_List.InsertColumn(ENABLE,_T("有効"),LVCFMT_CENTER,40);
	ListView_SetExtendedListViewStyle(m_List.m_hWnd,LVS_EX_FULLROWSELECT);

	m_List2.InsertColumn(URL,_T("除外URL ルール"),LVCFMT_LEFT,700);
	m_List2.InsertColumn(ENABLE,_T("有効"),LVCFMT_CENTER,40);
	ListView_SetExtendedListViewStyle(m_List2.m_hWnd,LVS_EX_FULLROWSELECT);

	if(m_URD.m_bDisabled)
		((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->SetCheck(1);
	else
		((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->SetCheck(0);

	if(m_Combo.m_hWnd)
	{
		for(int i=0;i<3;i++)
		{
			m_Combo.AddString(gRedirectPageActionList[i]);
		}

		if( 0 <= m_URD.m_dRedirectPageAction && m_URD.m_dRedirectPageAction <= 2)
			m_Combo.SetCurSel(m_URD.m_dRedirectPageAction);
		else
			m_Combo.SetCurSel(0);
	}

	if(GetDlgItem(IDC_EDIT1))
	{
		if( 1 <= m_URD.m_dwCloseTimeout && m_URD.m_dwCloseTimeout <= 60)
			SetDlgItemInt(IDC_EDIT1,m_URD.m_dwCloseTimeout);
		else
			SetDlgItemInt(IDC_EDIT1,3);
	}

	int iMaxCnt=0;
	BOOL bEnable=TRUE;
	CString strLineData;

	//対象List
	iMaxCnt = m_URD.m_arr_URL.GetCount();
	for(int i=0;i<iMaxCnt;i++)
	{
		bEnable=TRUE;
		strLineData = m_URD.m_arr_URL.GetAt(i);
		int index = this->m_List.GetItemCount();
		int iItem = this->m_List.InsertItem( index, _T(""));

		if(strLineData.Find(_T("#"))==0)
		{
			bEnable=FALSE;
			strLineData=strLineData.Mid(1);
		}
		else if(strLineData.Find(_T(";"))==0)
		{
			bEnable=FALSE;
			strLineData=strLineData.Mid(1);
		}
		this->m_List.SetItemText(iItem,URL,strLineData);
		this->m_List.SetItemText(iItem,ENABLE,bEnable?_T("○"):_T("－"));
	}

	//除外対象List
	iMaxCnt = m_URD.m_arr_URL_EX.GetCount();
	for(int i=0;i<iMaxCnt;i++)
	{
		bEnable=TRUE;
		strLineData = m_URD.m_arr_URL_EX.GetAt(i);
		int index = this->m_List2.GetItemCount();
		int iItem = this->m_List2.InsertItem( index, _T(""));

		if(strLineData.Find(_T("#"))==0)
		{
			bEnable=FALSE;
			strLineData=strLineData.Mid(1);
		}
		else if(strLineData.Find(_T(";"))==0)
		{
			bEnable=FALSE;
			strLineData=strLineData.Mid(1);
		}
		this->m_List2.SetItemText(iItem,URL,strLineData);
		this->m_List2.SetItemText(iItem,ENABLE,bEnable?_T("○"):_T("－"));
	}

	OnEnableCtrl();
	UpdateListCounter(&this->m_List);
	UpdateListCounter(&this->m_List2);
	return TRUE;
}

LRESULT CDlgRuleBase::Set_OK(WPARAM wParam, LPARAM lParam)
{
	m_URDSave.m_strExecType=m_URD.m_strExecType;

	if(((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck())
		m_URDSave.m_bDisabled=TRUE;
	else
		m_URDSave.m_bDisabled=FALSE;

	int iID=0;
	if(m_Combo.m_hWnd)
	{
		iID = m_Combo.GetCurSel();
		if(0<=iID && iID<=2)
			m_URDSave.m_dRedirectPageAction=iID;
		else
			m_URDSave.m_dRedirectPageAction=0;
	}

	if(GetDlgItem(IDC_EDIT1))
	{
		int iTimeout=GetDlgItemInt(IDC_EDIT1);
		if( 1 <= iTimeout && iTimeout <= 60)
			m_URDSave.m_dwCloseTimeout=iTimeout;
		else
			m_URDSave.m_dwCloseTimeout=3;
	}

	CString strData;
	CString strURL;
	CString strTemp;
	int iSelCount = -1;

	m_URDSave.m_arr_URL.RemoveAll();
	while((iSelCount = m_List.GetNextItem(iSelCount,LVNI_ALL)) != -1)
	{
		strURL.Empty();
		strTemp.Empty();
		strURL = m_List.GetItemText(iSelCount,URL);
		strURL.TrimLeft();
		strURL.TrimRight();
		strTemp = m_List.GetItemText(iSelCount,ENABLE);
		strTemp.TrimLeft();
		strTemp.TrimRight();

		if(!strURL.IsEmpty())
		{
			if(strTemp==_T("－"))
			{
				strData =_T("#");
				strData += strURL;
			}
			else
			{
				strData = strURL;
			}
			m_URDSave.m_arr_URL.Add(strData);
		}
	}

	//除外対象List
	m_URDSave.m_arr_URL_EX.RemoveAll();
	iSelCount = -1;
	while((iSelCount = m_List2.GetNextItem(iSelCount,LVNI_ALL)) != -1)
	{
		strURL.Empty();
		strTemp.Empty();
		strURL = m_List2.GetItemText(iSelCount,URL);
		strURL.TrimLeft();
		strURL.TrimRight();
		strTemp = m_List2.GetItemText(iSelCount,ENABLE);
		strTemp.TrimLeft();
		strTemp.TrimRight();

		if(!strURL.IsEmpty())
		{
			if(strTemp==_T("－"))
			{
				strData =_T("#");
				strData += strURL;
			}
			else
			{
				strData = strURL;
			}
			m_URDSave.m_arr_URL_EX.Add(strData);
		}
	}
	return 0;
}


//RDP
IMPLEMENT_DYNCREATE(CDlgRD, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgRD,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
END_MESSAGE_MAP()

BOOL CDlgRD::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pRDP)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pRDP);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDI_ICON_RDP);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgRD::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
		theApp.m_RedirectListSaveData.m_pRDP->Copy(&m_URDSave);
	return 0;
}


//VMware
IMPLEMENT_DYNCREATE(CDlgVM, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgVM,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
END_MESSAGE_MAP()
BOOL CDlgVM::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pVMW)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pVMW);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDI_ICON_VMH);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgVM::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
		theApp.m_RedirectListSaveData.m_pVMW->Copy(&m_URDSave);
	return 0;
}

//Citrix
IMPLEMENT_DYNCREATE(CDlgCTX, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCTX,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
END_MESSAGE_MAP()
BOOL CDlgCTX::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCTX)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCTX);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDI_ICON_Ctrix);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCTX::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
		theApp.m_RedirectListSaveData.m_pCTX->Copy(&m_URDSave);
	return 0;
}

//Firefox
IMPLEMENT_DYNCREATE(CDlgFF, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgFF,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
END_MESSAGE_MAP()
BOOL CDlgFF::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pFirefox)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pFirefox);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDI_ICON_Firefox);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgFF::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
		theApp.m_RedirectListSaveData.m_pFirefox->Copy(&m_URDSave);
	return 0;
}

//Chrome
IMPLEMENT_DYNCREATE(CDlgCHR, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCHR,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
END_MESSAGE_MAP()
BOOL CDlgCHR::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pChrome)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pChrome);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDI_ICON_Chrome);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCHR::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
		theApp.m_RedirectListSaveData.m_pChrome->Copy(&m_URDSave);
	return 0;
}

//Edge
IMPLEMENT_DYNCREATE(CDlgEDG, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgEDG,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
END_MESSAGE_MAP()
BOOL CDlgEDG::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pEdge)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pEdge);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDI_ICON_Edge);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgEDG::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
		theApp.m_RedirectListSaveData.m_pEdge->Copy(&m_URDSave);
	return 0;
}

//IE
IMPLEMENT_DYNCREATE(CDlgIE, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgIE, CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK, Set_OK)
END_MESSAGE_MAP()
BOOL CDlgIE::OnInitDialog()
{
	if (theApp.m_RedirectList.m_pIE)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pIE);
	}
	BOOL bRet = CDlgRuleBase::OnInitDialog();

	//IEとして不要な設定を非表示にする。
	if (GetDlgItem(IDC_STATIC_IEG))
	{
		GetDlgItem(IDC_STATIC_IEG)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_IEG)->ShowWindow(SW_HIDE);
	}
	if (GetDlgItem(IDC_STATIC_TY))
	{
		GetDlgItem(IDC_STATIC_TY)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_TY)->ShowWindow(SW_HIDE);
	}
	if (GetDlgItem(IDC_COMBO1))
	{
		GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO1)->ShowWindow(SW_HIDE);
	}
	if (GetDlgItem(IDC_STATIC_SEC))
	{
		GetDlgItem(IDC_STATIC_SEC)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_SEC)->ShowWindow(SW_HIDE);
	}
	if (GetDlgItem(IDC_EDIT1))
	{
		GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT1)->ShowWindow(SW_HIDE);
	}
	if (GetDlgItem(IDC_STATIC_SEC2))
	{
		GetDlgItem(IDC_STATIC_SEC2)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_SEC2)->ShowWindow(SW_HIDE);
	}

	if (bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper = theApp.LoadIcon(IDI_ICON_IE);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgIE::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if (CDlgRuleBase::Set_OK(wParam, lParam) == 0)
		theApp.m_RedirectListSaveData.m_pIE->Copy(&m_URDSave);
	return 0;
}


//Custom01
IMPLEMENT_DYNCREATE(CDlgCU01, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU01,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU01::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom01)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom01);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();
	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU01::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー01 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom01->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU01::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU01::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom02
IMPLEMENT_DYNCREATE(CDlgCU02, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU02,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU02::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom02)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom02);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU02::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー02 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom02->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU02::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU02::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom03
IMPLEMENT_DYNCREATE(CDlgCU03, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU03,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU03::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom03)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom03);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU03::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー03 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom03->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU03::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU03::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom04
IMPLEMENT_DYNCREATE(CDlgCU04, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU04,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU04::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom04)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom04);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU04::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー04 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom04->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU04::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU04::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom05
IMPLEMENT_DYNCREATE(CDlgCU05, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU05,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU05::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom05)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom05);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU05::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー05 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom05->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU05::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU05::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom06
IMPLEMENT_DYNCREATE(CDlgCU06, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU06,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU06::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom06)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom06);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU06::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー06 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom06->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU06::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU06::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom07
IMPLEMENT_DYNCREATE(CDlgCU07, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU07,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU07::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom07)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom07);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU07::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー07 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom07->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU07::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU07::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom08
IMPLEMENT_DYNCREATE(CDlgCU08, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU08,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU08::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom08)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom08);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU08::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー08 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom08->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU08::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU08::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom09
IMPLEMENT_DYNCREATE(CDlgCU09, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU09,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU09::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom09)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom09);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU09::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー09 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom09->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU09::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU09::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom10
IMPLEMENT_DYNCREATE(CDlgCU10, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU10,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU10::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom10)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom10);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU10::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー10 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom10->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU10::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU10::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom11
IMPLEMENT_DYNCREATE(CDlgCU11, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU11,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU11::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom11)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom11);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU11::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー11 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom11->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU11::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU11::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom12
IMPLEMENT_DYNCREATE(CDlgCU12, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU12,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU12::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom12)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom12);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU12::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー12 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom12->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU12::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU12::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom13
IMPLEMENT_DYNCREATE(CDlgCU13, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU13,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU13::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom13)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom13);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU13::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー13 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom13->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU13::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU13::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom14
IMPLEMENT_DYNCREATE(CDlgCU14, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU14,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU14::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom14)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom14);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU14::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー14 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom14->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU14::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU14::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom15
IMPLEMENT_DYNCREATE(CDlgCU15, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU15,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU15::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom15)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom15);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU15::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー15 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom15->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU15::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU15::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom16
IMPLEMENT_DYNCREATE(CDlgCU16, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU16,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU16::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom16)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom16);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU16::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー16 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom16->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU16::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU16::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom17
IMPLEMENT_DYNCREATE(CDlgCU17, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU17,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU17::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom17)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom17);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU17::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー17 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom17->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU17::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU17::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom18
IMPLEMENT_DYNCREATE(CDlgCU18, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU18,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU18::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom18)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom18);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU18::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー18 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom18->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU18::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU18::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom19
IMPLEMENT_DYNCREATE(CDlgCU19, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU19,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU19::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom19)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom19);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();

	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU19::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー19 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom19->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU19::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU19::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}

//Custom20
IMPLEMENT_DYNCREATE(CDlgCU20, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgCU20,CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK,Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG,OnBnClickedButtonFdlg)
END_MESSAGE_MAP()
BOOL CDlgCU20::OnInitDialog() 
{
	if(theApp.m_RedirectList.m_pCustom20)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom20);
	}
	BOOL bRet =CDlgRuleBase::OnInitDialog();
	SetDlgItemText(IDC_EDIT_LCB,m_URD.m_strExecExeFullPath);
	OnEnableCtrl();
	if(bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgCU20::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if(CDlgRuleBase::Set_OK(wParam,lParam)==0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB,strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if(strExePath.IsEmpty())
		{
			if(!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd,_T("[指定ブラウザー20 (ローカル)]\n指定ブラウザーのパスを設定して下さい。"),theApp.m_strThisAppName,MB_OK|MB_ICONERROR );
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath=strExePath;
		theApp.m_RedirectListSaveData.m_pCustom20->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgCU20::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk=FALSE;
	bChk=((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck()?FALSE:TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
}
void CDlgCU20::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter=_T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter,this);
	CString strTitle;
	strTitle=_T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB,fileDlg.GetPathName());
	}
}
static LPCTSTR gO365ActionList[] = {
	_T("Microsoft RDP"),
	_T("VMware Horizon"),
	_T("Citrix Virtual Apps"),
	_T("Mozilla Firefox"),
	_T("Google Chrome"),
	_T("Microsoft Edge"),
};
//Office365
IMPLEMENT_DYNCREATE(CDlgO365, CDlgRuleBase)
void CDlgO365::DoDataExchange(CDataExchange* pDX)
{
	CDlgRuleBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO2, m_Combo2);
}

BEGIN_MESSAGE_MAP(CDlgO365, CDlgRuleBase)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnDblclkList2)
	ON_MESSAGE(ID_SETTING_OK, Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG, OnBnClickedButtonFdlg)
	ON_BN_CLICKED(IDC_BUTTON_O365, OnBnClickedButtonOffice365)
	ON_BN_CLICKED(IDC_BUTTON_O365_RELOAD, OnBnClickedButtonReload)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedCopy1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedCopy2)

END_MESSAGE_MAP()

void CDlgO365::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
}
void CDlgO365::OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
}

BOOL CDlgO365::OnInitDialog()
{
	if (theApp.m_RedirectList.m_pCustom20)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom20);
	}
	CPropertyPage::OnInitDialog();

	m_List.InsertColumn(URL, _T("ターゲットURL ルール"), LVCFMT_LEFT, 800);
	ListView_SetExtendedListViewStyle(m_List.m_hWnd, LVS_EX_FULLROWSELECT);

	m_List2.InsertColumn(URL, _T("除外URL ルール"), LVCFMT_LEFT, 800);
	ListView_SetExtendedListViewStyle(m_List2.m_hWnd, LVS_EX_FULLROWSELECT);

	if (m_URD.m_bDisabled)
		((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->SetCheck(1);
	else
		((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->SetCheck(0);

	if (m_Combo.m_hWnd)
	{
		for (int i = 0; i < 3; i++)
		{
			m_Combo.AddString(gRedirectPageActionList[i]);
		}

		if (0 <= m_URD.m_dRedirectPageAction && m_URD.m_dRedirectPageAction <= 2)
			m_Combo.SetCurSel(m_URD.m_dRedirectPageAction);
		else
			m_Combo.SetCurSel(0);
	}

	if (GetDlgItem(IDC_EDIT1))
	{
		if (1 <= m_URD.m_dwCloseTimeout && m_URD.m_dwCloseTimeout <= 60)
			SetDlgItemInt(IDC_EDIT1, m_URD.m_dwCloseTimeout);
		else
			SetDlgItemInt(IDC_EDIT1, 3);
	}

	int iMaxCnt = 0;
	CString strLineData;

	//対象List
	iMaxCnt = m_URD.m_arr_URL.GetCount();
	for (int i = 0; i < iMaxCnt; i++)
	{
		strLineData = m_URD.m_arr_URL.GetAt(i);
		int index = this->m_List.GetItemCount();
		int iItem = this->m_List.InsertItem(index, _T(""));

		if (strLineData.Find(_T("#")) == 0)
		{
			continue;
		}
		else if (strLineData.Find(_T(";")) == 0)
		{
			continue;
		}
		this->m_List.SetItemText(iItem, URL, strLineData);
	}

	//除外対象List
	iMaxCnt = m_URD.m_arr_URL_EX.GetCount();
	for (int i = 0; i < iMaxCnt; i++)
	{
		strLineData = m_URD.m_arr_URL_EX.GetAt(i);
		int index = this->m_List2.GetItemCount();
		int iItem = this->m_List2.InsertItem(index, _T(""));

		if (strLineData.Find(_T("#")) == 0)
		{
			continue;
		}
		else if (strLineData.Find(_T(";")) == 0)
		{
			continue;
		}
		this->m_List2.SetItemText(iItem, URL, strLineData);
	}

	UpdateListCounter(&this->m_List);
	UpdateListCounter(&this->m_List2);


	//Citrix特殊環境フラグ
	if (theApp.m_bCitrixCustomEnv)
	{
		CString strTBChromeSwitcher;
		strTBChromeSwitcher= _T("TBChromeSwitcher.exe");
		m_Combo2.AddString(strTBChromeSwitcher);
		if (m_URD.m_strExecExeFullPath.IsEmpty())
			m_URD.m_strExecExeFullPath = _T("TBChromeSwitcher.exe");
		m_Combo2.SetWindowText( m_URD.m_strExecExeFullPath);
	}
	else
	{
		if (m_Combo2.m_hWnd)
		{
			for (int i = 0; i < 6; i++)
			{
				m_Combo2.AddString(gO365ActionList[i]);
			}

			if (m_URD.m_strExecExeFullPath.CompareNoCase(_T("RDP")) == 0)
			{
				m_Combo2.SetCurSel(0);
				m_Combo2.SetWindowText(gO365ActionList[0]);
			}
			else if (m_URD.m_strExecExeFullPath.CompareNoCase(_T("VMWARE")) == 0)
			{
				m_Combo2.SetCurSel(1);
				m_Combo2.SetWindowText(gO365ActionList[1]);
			}
			else if (m_URD.m_strExecExeFullPath.CompareNoCase(_T("CITRIX")) == 0)
			{
				m_Combo2.SetCurSel(2);
				m_Combo2.SetWindowText(gO365ActionList[2]);
			}
			else if (m_URD.m_strExecExeFullPath.CompareNoCase(_T("Firefox")) == 0)
			{
				m_Combo2.SetCurSel(3);
				m_Combo2.SetWindowText(gO365ActionList[3]);
			}
			else if (m_URD.m_strExecExeFullPath.CompareNoCase(_T("Chrome")) == 0)
			{
				m_Combo2.SetCurSel(4);
				m_Combo2.SetWindowText(gO365ActionList[4]);
			}
			else if (m_URD.m_strExecExeFullPath.CompareNoCase(_T("Edge")) == 0)
			{
				m_Combo2.SetCurSel(5);
				m_Combo2.SetWindowText(gO365ActionList[5]);
			}
			else
			{
				m_Combo2.SetWindowText(m_URD.m_strExecExeFullPath);
			}
		}
	}

	OnEnableCtrl();
	CIconHelper ICoHelper;
	if(theApp.m_bCitrixCustomEnv)
		ICoHelper = theApp.LoadIcon(IDI_ICON_Chrome);
	else
		ICoHelper = theApp.LoadIcon(IDI_ICON_o365);
	m_Image.SetIcon(ICoHelper);
	return TRUE;
}
LRESULT CDlgO365::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if (CDlgRuleBase::Set_OK(wParam, lParam) == 0)
	{
		CString strExePath;
		m_Combo2.GetWindowText(strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if (strExePath.IsEmpty())
		{
			if (!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd, _T("[Office365 WebApps]\nブラウザー種別を設定して下さい。"), theApp.m_strThisAppName, MB_OK | MB_ICONERROR);
				return 1;
			}
		}
		if (strExePath.CompareNoCase(gO365ActionList[0]) == 0)
		{
			strExePath=_T("RDP");
		}
		else if (strExePath.CompareNoCase(gO365ActionList[1]) == 0)
		{
			strExePath = _T("VMWARE");
		}
		else if (strExePath.CompareNoCase(gO365ActionList[2]) == 0)
		{
			strExePath = _T("CITRIX");
		}
		else if (strExePath.CompareNoCase(gO365ActionList[3]) == 0)
		{
			strExePath = _T("Firefox");
		}
		else if (strExePath.CompareNoCase(gO365ActionList[4]) == 0)
		{
			strExePath = _T("Chrome");
		}
		else if (strExePath.CompareNoCase(gO365ActionList[5]) == 0)
		{
			strExePath = _T("Edge");
		}
		m_URDSave.m_strExecExeFullPath = strExePath;
		theApp.m_RedirectListSaveData.m_pCustom20->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgO365::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk = FALSE;
	bChk = ((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck() ? FALSE : TRUE;
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
	GetDlgItem(IDC_COMBO2)->EnableWindow(bChk);
}
void CDlgO365::CopyList(CListCtrl* ptrList)
{
	CWaitCursor wc;
	CString strData;

	CString strURL;
	int iSelCount = -1;
	while ((iSelCount = ptrList->GetNextItem(iSelCount, LVNI_ALL)) != -1)
	{
		strURL.Empty();
		strURL = ptrList->GetItemText(iSelCount, URL);
		strData += strURL;
		strData += _T("\r\n");
	}
	if (!::OpenClipboard(NULL))
		return;

	int	nByte = (strData.GetLength() + 1) * sizeof(TCHAR);
	HGLOBAL hText = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, nByte);

	if (hText == NULL)
		return;

	BYTE *	pText = (BYTE *) ::GlobalLock(hText);

	if (pText == NULL)
		return;

	::memcpy(pText, (LPCTSTR)strData, nByte);
	::GlobalUnlock(hText);
	::OpenClipboard(NULL);
	::EmptyClipboard();
	::SetClipboardData(CF_UNICODETEXT, hText);
	::CloseClipboard();
}
void CDlgO365::OnBnClickedCopy1()
{
	CopyList(&m_List);
}
void CDlgO365::OnBnClickedCopy2()
{
	CopyList(&m_List2);
}

void CDlgO365::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter = _T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter, this);
	CString strTitle;
	strTitle = _T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		m_Combo2.SetWindowText( fileDlg.GetPathName());
	}
}
void CDlgO365::OnBnClickedButtonOffice365()
{
	CString IEcmd;
	IEcmd = _T("");
	IEcmd.Format(_T("\"%s\""), theApp.m_strO365ToolFullPath);
	CString strPathQ;
	strPathQ.Format(_T("\"%s\""), theApp.m_strO365ToolFullPath);
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(si);
	if (!CreateProcess(NULL, (LPTSTR)(LPCTSTR)IEcmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		SetLastError(NO_ERROR);
		//Retry
		STARTUPINFO si2 = { 0 };
		PROCESS_INFORMATION pi2 = { 0 };
		si2.cb = sizeof(si2);
		if (!CreateProcess(strPathQ,NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si2, &pi2))
		{
			SetLastError(NO_ERROR);
			if (::ShellExecute(NULL, _T("open"), strPathQ,NULL, NULL, SW_SHOW) <= HINSTANCE(32))
			{
				SetLastError(NO_ERROR);
				::ShellExecute(NULL, NULL, IEcmd, NULL, NULL, SW_SHOW);
			}
		}
		else
		{
			if (pi2.hThread)
			{
				CloseHandle(pi2.hThread);  // スレッドのハンドルは使わないのですぐ破棄
				pi2.hThread = 0;
			}
			if (pi2.hProcess)
			{
				CloseHandle(pi2.hProcess); // もうプロセスのハンドルは使わないので破棄
				pi2.hProcess = 0;
			}
		}
	}
	else
	{
		if (pi.hThread)
		{
			CloseHandle(pi.hThread);  // スレッドのハンドルは使わないのですぐ破棄
			pi.hThread = 0;
		}
		if (pi.hProcess)
		{
			CloseHandle(pi.hProcess); // もうプロセスのハンドルは使わないので破棄
			pi.hProcess = 0;
		}
	}
}
void CDlgO365::OnBnClickedButtonReload()
{
	int iMaxCnt = 0;
	CString strLineData;

	CURLRedirectList RedirectList;
	RedirectList.SetArrayData(theApp.m_strRedirectFilePath);
	m_URD.Copy(RedirectList.m_pCustom20);

	this->m_List.DeleteAllItems();
	//対象List
	iMaxCnt = m_URD.m_arr_URL.GetCount();
	for (int i = 0; i < iMaxCnt; i++)
	{
		strLineData = m_URD.m_arr_URL.GetAt(i);
		int index = this->m_List.GetItemCount();
		int iItem = this->m_List.InsertItem(index, _T(""));

		if (strLineData.Find(_T("#")) == 0)
		{
			continue;
		}
		else if (strLineData.Find(_T(";")) == 0)
		{
			continue;
		}
		this->m_List.SetItemText(iItem, URL, strLineData);
	}

	//除外対象List
	this->m_List2.DeleteAllItems();
	iMaxCnt = m_URD.m_arr_URL_EX.GetCount();
	for (int i = 0; i < iMaxCnt; i++)
	{
		strLineData = m_URD.m_arr_URL_EX.GetAt(i);
		int index = this->m_List2.GetItemCount();
		int iItem = this->m_List2.InsertItem(index, _T(""));

		if (strLineData.Find(_T("#")) == 0)
		{
			continue;
		}
		else if (strLineData.Find(_T(";")) == 0)
		{
			continue;
		}
		this->m_List2.SetItemText(iItem, URL, strLineData);
	}
	UpdateListCounter(&this->m_List);
	UpdateListCounter(&this->m_List2);
}

//CDlgChromeSwitcher
IMPLEMENT_DYNCREATE(CDlgChromeSwitcher, CDlgRuleBase)
BEGIN_MESSAGE_MAP(CDlgChromeSwitcher, CDlgRuleBase)
	ON_MESSAGE(ID_SETTING_OK, Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)
	ON_BN_CLICKED(IDC_BUTTON_FDLG, OnBnClickedButtonFdlg)
	ON_BN_CLICKED(IDC_BUTTON_CSW, OnBnClickedButtonChromeSwitcher)
END_MESSAGE_MAP()
BOOL CDlgChromeSwitcher::OnInitDialog()
{
	if (theApp.m_RedirectList.m_pCustom19)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom19);
	}
	BOOL bRet = CDlgRuleBase::OnInitDialog();
	if(m_URD.m_strExecExeFullPath.IsEmpty())
		m_URD.m_strExecExeFullPath=_T("TBChromeSwitcher.exe");

	SetDlgItemText(IDC_EDIT_LCB, m_URD.m_strExecExeFullPath);
	OnEnableCtrl();
	if (bRet)
	{
		CIconHelper ICoHelper;
		ICoHelper = theApp.LoadIcon(IDI_ICON_Chrome);
		m_Image.SetIcon(ICoHelper);
	}
	return bRet;
}
LRESULT CDlgChromeSwitcher::Set_OK(WPARAM wParam, LPARAM lParam)
{
	if (CDlgRuleBase::Set_OK(wParam, lParam) == 0)
	{
		CString strExePath;
		GetDlgItemText(IDC_EDIT_LCB, strExePath);
		strExePath.TrimLeft();
		strExePath.TrimRight();
		if (strExePath.IsEmpty())
		{
			if (!m_URDSave.m_bDisabled)
			{
				::MessageBox(this->m_hWnd, _T("[Chrome(自動切り換え)]\n指定ブラウザーのパスを設定して下さい。"), theApp.m_strThisAppName, MB_OK | MB_ICONERROR);
				return 1;
			}
		}
		m_URDSave.m_strExecExeFullPath = strExePath;
		theApp.m_RedirectListSaveData.m_pCustom19->Copy(&m_URDSave);
	}
	return 0;
}
void CDlgChromeSwitcher::OnEnableCtrl()
{
	CDlgRuleBase::OnEnableCtrl();
	BOOL bChk = FALSE;
	bChk = ((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck() ? FALSE : TRUE;
	GetDlgItem(IDC_EDIT_LCB)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_FDLG)->EnableWindow(bChk);
	if (PathFileExists(theApp.m_strChromeSwitcherFullPath))
	{
		GetDlgItem(IDC_BUTTON_CSW)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_CSW)->EnableWindow(FALSE);
	}
}
void CDlgChromeSwitcher::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter = _T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter, this);
	CString strTitle;
	strTitle = _T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		SetDlgItemText(IDC_EDIT_LCB, fileDlg.GetPathName());
	}
}
void CDlgChromeSwitcher::OnBnClickedButtonChromeSwitcher()
{
	CString IEcmd;
	IEcmd = _T("");
	IEcmd.Format(_T("\"%s\""), theApp.m_strChromeSwitcherFullPath);
	CString strPathQ;
	strPathQ.Format(_T("\"%s\""), theApp.m_strChromeSwitcherFullPath);
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(si);
	if (!CreateProcess(NULL, (LPTSTR)(LPCTSTR)IEcmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		SetLastError(NO_ERROR);
		//Retry
		STARTUPINFO si2 = { 0 };
		PROCESS_INFORMATION pi2 = { 0 };
		si2.cb = sizeof(si2);
		if (!CreateProcess(strPathQ, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si2, &pi2))
		{
			SetLastError(NO_ERROR);
			if (::ShellExecute(NULL, _T("open"), strPathQ, NULL, NULL, SW_SHOW) <= HINSTANCE(32))
			{
				SetLastError(NO_ERROR);
				::ShellExecute(NULL, NULL, IEcmd, NULL, NULL, SW_SHOW);
			}
		}
		else
		{
			if (pi2.hThread)
			{
				CloseHandle(pi2.hThread);  // スレッドのハンドルは使わないのですぐ破棄
				pi2.hThread = 0;
			}
			if (pi2.hProcess)
			{
				CloseHandle(pi2.hProcess); // もうプロセスのハンドルは使わないので破棄
				pi2.hProcess = 0;
			}
		}
	}
	else
	{
		if (pi.hThread)
		{
			CloseHandle(pi.hThread);  // スレッドのハンドルは使わないのですぐ破棄
			pi.hThread = 0;
		}
		if (pi.hProcess)
		{
			CloseHandle(pi.hProcess); // もうプロセスのハンドルは使わないので破棄
			pi.hProcess = 0;
		}
	}
}
//////////////////////////////////////////////////////
//CDlgDMZ
IMPLEMENT_DYNCREATE(CDlgDMZ, CPropertyPage)
void CDlgDMZ::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRuleBase)
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_Image);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgDMZ, CPropertyPage)
	ON_MESSAGE(ID_SETTING_OK, Set_OK)
	ON_BN_CLICKED(IDC_CHECK_DISABLE, OnEnableCtrl)

	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON_INS, OnButtonPopIns)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonPopDel)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnButtonDown)

	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnBnClickedButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON_EDITALL, OnBnClickedButtonEditall)

END_MESSAGE_MAP()
BOOL CDlgDMZ::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	if (theApp.m_RedirectList.m_pCustom18)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pCustom18);
	}

	m_List.InsertColumn(URL, _T("共用URL ルール"), LVCFMT_LEFT, 700);
	m_List.InsertColumn(ENABLE, _T("有効"), LVCFMT_CENTER, 40);
	ListView_SetExtendedListViewStyle(m_List.m_hWnd, LVS_EX_FULLROWSELECT);

	if (m_URD.m_bDisabled)
		((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->SetCheck(1);
	else
		((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->SetCheck(0);

	int iMaxCnt = 0;
	BOOL bEnable = TRUE;
	CString strLineData;

	//対象List
	iMaxCnt = m_URD.m_arr_URL.GetCount();
	for (int i = 0; i < iMaxCnt; i++)
	{
		bEnable = TRUE;
		strLineData = m_URD.m_arr_URL.GetAt(i);
		int index = this->m_List.GetItemCount();
		int iItem = this->m_List.InsertItem(index, _T(""));

		if (strLineData.Find(_T("#")) == 0)
		{
			bEnable = FALSE;
			strLineData = strLineData.Mid(1);
		}
		else if (strLineData.Find(_T(";")) == 0)
		{
			bEnable = FALSE;
			strLineData = strLineData.Mid(1);
		}
		this->m_List.SetItemText(iItem, URL, strLineData);
		this->m_List.SetItemText(iItem, ENABLE, bEnable ? _T("○") : _T("－"));
	}

	OnEnableCtrl();
	UpdateListCounter(&this->m_List);
	m_URD.m_strExecExeFullPath = _T("");
	CIconHelper ICoHelper;
	if (theApp.m_bCitrixCustomEnv)
		ICoHelper = theApp.LoadIcon(IDR_MAINFRAME);
	else
		ICoHelper = theApp.LoadIcon(IDI_ICON_DMZ);

	m_Image.SetIcon(ICoHelper);
	return TRUE;
}
LRESULT CDlgDMZ::Set_OK(WPARAM wParam, LPARAM lParam)
{
	m_URDSave.m_strExecType = m_URD.m_strExecType;

	if (((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck())
		m_URDSave.m_bDisabled = TRUE;
	else
		m_URDSave.m_bDisabled = FALSE;

	CString strData;
	CString strURL;
	CString strTemp;
	int iSelCount = -1;

	m_URDSave.m_arr_URL.RemoveAll();
	while ((iSelCount = m_List.GetNextItem(iSelCount, LVNI_ALL)) != -1)
	{
		strURL.Empty();
		strTemp.Empty();
		strURL = m_List.GetItemText(iSelCount, URL);
		strURL.TrimLeft();
		strURL.TrimRight();
		strTemp = m_List.GetItemText(iSelCount, ENABLE);
		strTemp.TrimLeft();
		strTemp.TrimRight();

		if (!strURL.IsEmpty())
		{
			if (strTemp == _T("－"))
			{
				strData = _T("#");
				strData += strURL;
			}
			else
			{
				strData = strURL;
			}
			m_URDSave.m_arr_URL.Add(strData);
		}
	}

	//除外対象List
	m_URDSave.m_arr_URL_EX.RemoveAll();
	m_URDSave.m_strExecExeFullPath = _T("");
	theApp.m_RedirectListSaveData.m_pCustom18->Copy(&m_URDSave);
	return 0;
}
void CDlgDMZ::OnEnableCtrl()
{
	BOOL bChk = FALSE;
	bChk = ((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck() ? FALSE : TRUE;

	if (GetDlgItem(IDC_LIST1))
		GetDlgItem(IDC_LIST1)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_INS))
		GetDlgItem(IDC_BUTTON_INS)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_DEL))
		GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_UP))
		GetDlgItem(IDC_BUTTON_UP)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_DOWN))
		GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_UPDATE))
		GetDlgItem(IDC_BUTTON_UPDATE)->EnableWindow(bChk);
	if (GetDlgItem(IDC_BUTTON_EDITALL))
		GetDlgItem(IDC_BUTTON_EDITALL)->EnableWindow(bChk);
	return;
}
int CDlgDMZ::DuplicateChk(CListCtrl* ptrList, LPCTSTR sURL)
{
	int iRet = -1; //重複なしは、-1を返す。
	CString strURL;
	int iSelCount = -1;
	while ((iSelCount = ptrList->GetNextItem(iSelCount, LVNI_ALL)) != -1)
	{
		strURL.Empty();
		strURL = ptrList->GetItemText(iSelCount, URL);
		if (strURL == sURL)
		{
			iRet = iSelCount;
			break;
		}
	}
	return iRet;
}
void CDlgDMZ::UpdateListCounter(CListCtrl* ptrList)
{
	CString str;
	str.Format(_T("%d件"), ptrList->GetItemCount());
	if (ptrList == &this->m_List)
	{
		SetDlgItemText(IDC_STATIC_CNT, str);
	}
}
void CDlgDMZ::PopIns(CListCtrl* ptrList)
{
	CDlgRDEdit Dlg(this);
	Dlg.m_bEnable = TRUE;

	if (Dlg.DoModal() == IDOK)
	{
		int iRet = DuplicateChk(ptrList, Dlg.m_strURL);
		//重複なし。
		if (iRet == -1)
		{
			int index = ptrList->GetItemCount();
			int iItem = ptrList->InsertItem(index, _T(""));
			CString strTemp;
			ptrList->SetItemText(iItem, URL, Dlg.m_strURL);
			strTemp = Dlg.m_bEnable ? _T("○") : _T("－");
			ptrList->SetItemText(iItem, ENABLE, strTemp);
			ptrList->SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);
		}
		else
		{
			ptrList->SetFocus();
			ptrList->SetItemState(iRet, LVIS_SELECTED, LVIS_SELECTED);
			CString strErrMsg;
			strErrMsg.Format(_T("このURLは、既に登録されています。\n%d行目\n%s\n"), iRet + 1, Dlg.m_strURL);
			::MessageBox(this->m_hWnd, strErrMsg, theApp.m_strThisAppName, MB_OK | MB_ICONWARNING);
		}
	}
	UpdateListCounter(ptrList);
}
void CDlgDMZ::PopDel(CListCtrl* ptrList)
{
	int iSelCount = 0;
	iSelCount = ptrList->GetSelectedCount();
	if (iSelCount == 0)
		return;

	int nItemCount = ptrList->GetItemCount();
	while (nItemCount--)
	{
		if (ptrList->GetItemState(nItemCount, LVIS_SELECTED) == LVIS_SELECTED)
		{
			ptrList->DeleteItem(nItemCount);
			{
				int nItemNowCount = ptrList->GetItemCount();
				if (nItemNowCount == nItemCount)
					ptrList->SetItemState(nItemCount - 1, LVIS_SELECTED, LVIS_SELECTED);
				else if (nItemNowCount > 0)
					ptrList->SetItemState(nItemCount, LVIS_SELECTED, LVIS_SELECTED);
				break;
			}
		}
	}
	UpdateListCounter(ptrList);
}

void CDlgDMZ::PopUpdate(CListCtrl* ptrList)
{
	int iSelCount = 0;
	iSelCount = ptrList->GetSelectedCount();
	if (iSelCount != 1)
		return;
	iSelCount = ptrList->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (iSelCount != -1)
	{
		CDlgRDEdit Dlg(this);
		BOOL bEnable = FALSE;
		CString strURL;
		CString strTemp;
		strURL = ptrList->GetItemText(iSelCount, URL);
		strTemp = ptrList->GetItemText(iSelCount, ENABLE);
		bEnable = strTemp == _T("○") ? TRUE : FALSE;

		Dlg.m_strURL = strURL;
		Dlg.m_bEnable = bEnable;

		if (Dlg.DoModal() == IDOK)
		{
			int iRet = DuplicateChk(ptrList, Dlg.m_strURL);
			//重複なし。
			if (iRet == -1 || iRet == iSelCount)
			{
				CString strTemp;
				ptrList->SetItemText(iSelCount, URL, Dlg.m_strURL);
				strTemp = Dlg.m_bEnable ? _T("○") : _T("－");
				ptrList->SetItemText(iSelCount, ENABLE, strTemp);
			}
			else
			{
				ptrList->SetFocus();
				ptrList->SetItemState(iRet, LVIS_SELECTED, LVIS_SELECTED);
				CString strErrMsg;
				strErrMsg.Format(_T("このURLは、既に登録されています。\n%d行目\n%s\n"), iRet + 1, Dlg.m_strURL);
				::MessageBox(this->m_hWnd, strErrMsg, theApp.m_strThisAppName, MB_OK | MB_ICONWARNING);
			}
		}
	}
	UpdateListCounter(ptrList);
}
void CDlgDMZ::PopEditAll(CListCtrl* ptrList)
{
	CString strData;

	CString strURL;
	CString strTemp;
	int iSelCount = -1;
	while ((iSelCount = ptrList->GetNextItem(iSelCount, LVNI_ALL)) != -1)
	{
		strURL.Empty();
		strTemp.Empty();
		strURL = ptrList->GetItemText(iSelCount, URL);
		strTemp = ptrList->GetItemText(iSelCount, ENABLE);
		if (strTemp == _T("－"))
		{
			strData += _T("#");
		}
		strData += strURL;
		strData += _T("\r\n");
	}

	CDlgEditURLML Dlg;
	if (ptrList == &m_List)
		Dlg.m_strTitle = _T("【共用URL ルール】 一括登録・編集");

	Dlg.m_strEditData = strData;
	if (IDOK == Dlg.DoModal())
	{
		//一旦全部消す。
		ptrList->DeleteAllItems();
		int iMaxCnt = 0;
		iMaxCnt = Dlg.m_strArrayResult.GetCount();
		CString strLineData;
		for (int i = 0; i < iMaxCnt; i++)
		{
			strLineData = Dlg.m_strArrayResult.GetAt(i);
			int index = ptrList->GetItemCount();
			int iItem = ptrList->InsertItem(index, _T(""));
			CString strLowString;
			ptrList->SetItemText(iItem, URL, strLineData);
			strLowString = Dlg.m_strArrayResult_Enable.GetAt(i);
			ptrList->SetItemText(iItem, ENABLE, strLowString);
		}
	}
	UpdateListCounter(ptrList);
}
void CDlgDMZ::PopUp(CListCtrl* ptrList)
{
	int iSelCount = 0;
	int iAfterPos = 0;
	iSelCount = ptrList->GetSelectedCount();
	if (iSelCount != 1)
		return;
	iSelCount = ptrList->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (iSelCount == -1)
		return;

	CString strURL;
	CString strEnable;
	strURL = ptrList->GetItemText(iSelCount, URL);
	strEnable = ptrList->GetItemText(iSelCount, ENABLE);
	iAfterPos = iSelCount - 1;
	if (iAfterPos < 0)
	{
		ptrList->SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		ptrList->SetFocus();
		return;
	}
	ptrList->DeleteItem(iSelCount);
	int iItem = ptrList->InsertItem(iAfterPos, _T(""));
	ptrList->SetItemText(iItem, URL, strURL);
	ptrList->SetItemText(iItem, ENABLE, strEnable);
	ptrList->SetItemState(iAfterPos, LVIS_SELECTED, LVIS_SELECTED);
	ptrList->SetFocus();
}
void CDlgDMZ::PopDown(CListCtrl* ptrList)
{
	int iSelCount = 0;
	int iAfterPos = 0;
	iSelCount = ptrList->GetSelectedCount();
	if (iSelCount != 1)
		return;
	iSelCount = ptrList->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (iSelCount == -1)
		return;

	CString strURL;
	CString strEnable;
	strURL = ptrList->GetItemText(iSelCount, URL);
	strEnable = ptrList->GetItemText(iSelCount, ENABLE);
	iAfterPos = iSelCount + 1;
	if (iAfterPos >= ptrList->GetItemCount())
	{
		ptrList->SetItemState(iSelCount, LVIS_SELECTED, LVIS_SELECTED);
		ptrList->SetFocus();
		return;
	}
	ptrList->DeleteItem(iSelCount);
	int iItem = ptrList->InsertItem(iAfterPos, _T(""));
	ptrList->SetItemText(iItem, URL, strURL);
	ptrList->SetItemText(iItem, ENABLE, strEnable);
	ptrList->SetItemState(iAfterPos, LVIS_SELECTED, LVIS_SELECTED);
	ptrList->SetFocus();
}

void CDlgDMZ::OnButtonPopIns()
{
	PopIns(&m_List);
}

void CDlgDMZ::OnButtonPopDel()
{
	PopDel(&m_List);
}

void CDlgDMZ::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	PopUpdate(&m_List);
	*pResult = 0;
}
void CDlgDMZ::OnButtonUp()
{
	PopUp(&m_List);
}
void CDlgDMZ::OnButtonDown()
{
	PopDown(&m_List);
}
void CDlgDMZ::OnBnClickedButtonUpdate()
{
	PopUpdate(&m_List);
}
void CDlgDMZ::OnBnClickedButtonEditall()
{
	PopEditAll(&m_List);
}

//Default
static LPCTSTR gDefaultActionList[] = {
	_T("Microsoft RDP"),
	_T("VMware Horizon"),
	_T("Citrix Virtual Apps"),
	_T("Mozilla Firefox"),
	_T("Google Chrome"),
	_T("Microsoft Edge"),
	_T("Internet Explorer"), 
	_T("操作中のブラウザーで開く(他のブラウザーにリダイレクトしない)"),
};

IMPLEMENT_DYNCREATE(CDlgDefault, CPropertyPage)
CDlgDefault::CDlgDefault()
	: CPropertyPage(CDlgDefault::IDD)
{
}

void CDlgDefault::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO2, m_Combo2);
}

BEGIN_MESSAGE_MAP(CDlgDefault, CPropertyPage)
	//{{AFX_MSG_MAP(CDlgDefault)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_FDLG, OnBnClickedButtonFdlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(ID_SETTING_OK, Set_OK)
END_MESSAGE_MAP()

BOOL CDlgDefault::OnInitDialog()
{
	if (theApp.m_RedirectList.m_pDefault)
	{
		m_URD.Copy(theApp.m_RedirectList.m_pDefault);
	}
	CPropertyPage::OnInitDialog();

	m_URD.m_bDisabled=FALSE;
	if (m_Combo2.m_hWnd)
	{
		for (int i = 0; i < 8; i++)
		{
			m_Combo2.AddString(gDefaultActionList[i]);
		}

		if (m_URD.m_strExecExeFullPath.CompareNoCase(_T("RDP")) == 0)
		{
			m_Combo2.SetCurSel(0);
			m_Combo2.SetWindowText(gDefaultActionList[0]);
		}
		else if (m_URD.m_strExecExeFullPath.CompareNoCase(_T("VMWARE")) == 0)
		{
			m_Combo2.SetCurSel(1);
			m_Combo2.SetWindowText(gDefaultActionList[1]);
		}
		else if (m_URD.m_strExecExeFullPath.CompareNoCase(_T("CITRIX")) == 0)
		{
			m_Combo2.SetCurSel(2);
			m_Combo2.SetWindowText(gDefaultActionList[2]);
		}
		else if (m_URD.m_strExecExeFullPath.CompareNoCase(_T("Firefox")) == 0)
		{
			m_Combo2.SetCurSel(3);
			m_Combo2.SetWindowText(gDefaultActionList[3]);
		}
		else if (m_URD.m_strExecExeFullPath.CompareNoCase(_T("Chrome")) == 0)
		{
			m_Combo2.SetCurSel(4);
			m_Combo2.SetWindowText(gDefaultActionList[4]);
		}
		else if (m_URD.m_strExecExeFullPath.CompareNoCase(_T("Edge")) == 0)
		{
			m_Combo2.SetCurSel(5);
			m_Combo2.SetWindowText(gDefaultActionList[5]);
		}
		else if (m_URD.m_strExecExeFullPath.CompareNoCase(_T("IE")) == 0)
		{
			m_Combo2.SetCurSel(6);
			m_Combo2.SetWindowText(gDefaultActionList[6]);
		}
		else
		{
			if (m_URD.m_strExecExeFullPath.IsEmpty())
			{
				m_Combo2.SetCurSel(7);
				m_Combo2.SetWindowText(gDefaultActionList[7]);
			}
			else
				m_Combo2.SetWindowText(m_URD.m_strExecExeFullPath);
		}
	}
	return TRUE;
}
LRESULT CDlgDefault::Set_OK(WPARAM wParam, LPARAM lParam)
{
	m_URDSave.m_strExecType = m_URD.m_strExecType;
	m_URDSave.m_bDisabled = FALSE;
	m_URDSave.m_arr_URL.RemoveAll();
	m_URDSave.m_arr_URL_EX.RemoveAll();

	CString strExePath;
	m_Combo2.GetWindowText(strExePath);
	strExePath.TrimLeft();
	strExePath.TrimRight();
	if (strExePath.IsEmpty())
	{
		if (!m_URDSave.m_bDisabled)
		{
			::MessageBox(this->m_hWnd, _T("[その他(未定義URL)]\nブラウザー種別を設定して下さい。"), theApp.m_strThisAppName, MB_OK | MB_ICONERROR);
			return 1;
		}
	}
	if (strExePath.CompareNoCase(gDefaultActionList[0]) == 0)
	{
		strExePath = _T("RDP");
	}
	else if (strExePath.CompareNoCase(gDefaultActionList[1]) == 0)
	{
		strExePath = _T("VMWARE");
	}
	else if (strExePath.CompareNoCase(gDefaultActionList[2]) == 0)
	{
		strExePath = _T("CITRIX");
	}
	else if (strExePath.CompareNoCase(gDefaultActionList[3]) == 0)
	{
		strExePath = _T("Firefox");
	}
	else if (strExePath.CompareNoCase(gDefaultActionList[4]) == 0)
	{
		strExePath = _T("Chrome");
	}
	else if (strExePath.CompareNoCase(gDefaultActionList[5]) == 0)
	{
		strExePath = _T("Edge");
	}
	else if (strExePath.CompareNoCase(gDefaultActionList[6]) == 0)
	{
		strExePath = _T("IE");
	}
	else if (strExePath.CompareNoCase(gDefaultActionList[7]) == 0)
	{
		strExePath = _T("");
	}

	m_URDSave.m_strExecExeFullPath = strExePath;
	theApp.m_RedirectListSaveData.m_pDefault->Copy(&m_URDSave);
	return 0;
}
void CDlgDefault::OnBnClickedButtonFdlg()
{
	CString szFilter;
	szFilter = _T("実行ファイル(*.exe)|*.exe|全てのファイル(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter, this);
	CString strTitle;
	strTitle = _T("開く");
	fileDlg.m_ofn.lpstrTitle = strTitle.GetBuffer(0);
	if (fileDlg.DoModal() == IDOK)
	{
		m_Combo2.SetWindowText(fileDlg.GetPathName());
	}
}

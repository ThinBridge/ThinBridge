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

	GetDlgItem(IDC_CHECK_TOP_PAGE_ONLY)->EnableWindow(bChk);
	GetDlgItem(IDC_EDIT1)->EnableWindow(bChk);
	GetDlgItem(IDC_COMBO1)->EnableWindow(bChk);

	GetDlgItem(IDC_CHECK_INTERNET)->EnableWindow(bChk);
	GetDlgItem(IDC_CHECK_INTRANET)->EnableWindow(bChk);
	GetDlgItem(IDC_CHECK_TRUST)->EnableWindow(bChk);
	GetDlgItem(IDC_CHECK_UNTRUST)->EnableWindow(bChk);

	GetDlgItem(IDC_LIST1)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_INS)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_UP)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_UPDATE)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_EDITALL)->EnableWindow(bChk);

	GetDlgItem(IDC_LIST2)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_INS2)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_DEL2)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_UP2)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_DOWN2)->EnableWindow(bChk);
	GetDlgItem(IDC_BUTTON_UPDATE2)->EnableWindow(bChk);
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
			int iRet = DuplicateChk(&m_List,Dlg.m_strURL);
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

	if(m_URD.m_bTopPageOnly)
		((CButton*)GetDlgItem(IDC_CHECK_TOP_PAGE_ONLY))->SetCheck(1);
	else
		((CButton*)GetDlgItem(IDC_CHECK_TOP_PAGE_ONLY))->SetCheck(0);

	for(int i=0;i<3;i++)
	{
		m_Combo.AddString(gRedirectPageActionList[i]);
	}

	if( 0 <= m_URD.m_dRedirectPageAction && m_URD.m_dRedirectPageAction <= 2)
		m_Combo.SetCurSel(m_URD.m_dRedirectPageAction);
	else
		m_Combo.SetCurSel(0);

	if( 1 <= m_URD.m_dwCloseTimeout && m_URD.m_dwCloseTimeout <= 60)
		SetDlgItemInt(IDC_EDIT1,m_URD.m_dwCloseTimeout);
	else
		SetDlgItemInt(IDC_EDIT1,3);

	((CButton*)GetDlgItem(IDC_CHECK_INTERNET))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_INTRANET))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_TRUST))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_UNTRUST))->SetCheck(0);

	if((m_URD.m_dwZone&INTERNET_ZONE)==INTERNET_ZONE)
		((CButton*)GetDlgItem(IDC_CHECK_INTERNET))->SetCheck(1);

	if((m_URD.m_dwZone&INTRANET_ZONE)==INTRANET_ZONE)
		((CButton*)GetDlgItem(IDC_CHECK_INTRANET))->SetCheck(1);

	if((m_URD.m_dwZone&TRUSTED_ZONE)==TRUSTED_ZONE)
		((CButton*)GetDlgItem(IDC_CHECK_TRUST))->SetCheck(1);

	if((m_URD.m_dwZone&UNTRUSTED_ZONE)==UNTRUSTED_ZONE)
		((CButton*)GetDlgItem(IDC_CHECK_UNTRUST))->SetCheck(1);

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


	//CIconHelper ICoHelper;
	//ICoHelper=theApp.LoadIcon(IDR_MAINFRAME);
	//m_Image.SetIcon(ICoHelper);

	return TRUE;
}

LRESULT CDlgRuleBase::Set_OK(WPARAM wParam, LPARAM lParam)
{
	m_URDSave.m_strExecType=m_URD.m_strExecType;

	if(((CButton*)GetDlgItem(IDC_CHECK_DISABLE))->GetCheck())
		m_URDSave.m_bDisabled=TRUE;
	else
		m_URDSave.m_bDisabled=FALSE;

	if(((CButton*)GetDlgItem(IDC_CHECK_TOP_PAGE_ONLY))->GetCheck())
		m_URDSave.m_bTopPageOnly=TRUE;
	else
		m_URDSave.m_bTopPageOnly=FALSE;

	int iID=0;
	iID = m_Combo.GetCurSel();
	if(0<=iID && iID<=2)
		m_URDSave.m_dRedirectPageAction=iID;
	else
		m_URDSave.m_dRedirectPageAction=0;

	int iTimeout=GetDlgItemInt(IDC_EDIT1);
	if( 1 <= iTimeout && iTimeout <= 60)
		m_URDSave.m_dwCloseTimeout=iTimeout;
	else
		m_URDSave.m_dwCloseTimeout=3;

	int iZone=0;
	if(((CButton*)GetDlgItem(IDC_CHECK_INTERNET))->GetCheck())
		iZone=INTERNET_ZONE;
	if(((CButton*)GetDlgItem(IDC_CHECK_INTRANET))->GetCheck())
		iZone|=INTRANET_ZONE;
	if(((CButton*)GetDlgItem(IDC_CHECK_TRUST))->GetCheck())
		iZone|=TRUSTED_ZONE;
	if(((CButton*)GetDlgItem(IDC_CHECK_UNTRUST))->GetCheck())
		iZone|=UNTRUSTED_ZONE;
	m_URDSave.m_dwZone=iZone;

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

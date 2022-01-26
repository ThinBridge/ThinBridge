#pragma once
#include "resource.h"       // メイン シンボル
#include <shlguid.h>
#include <exdispid.h>
#include <atlstr.h>
#include <atlcoll.h>
#include <atlfile.h>
#include "locale.h"
#include "atltime.h"
#include <activscp.h>
#include <atlfile.h>
#include <string>
#include "TBGlobal.h"
#include "SBUtil.h"
#include "AtlStdioFile.h"

#define SECTION_DMZ	"CUSTOM18"
#define SECTION_CHROME_SWITCHER	"CUSTOM19"
#define SECTION_O365	"CUSTOM20"

//////////////////////////////////////////////////////////////////////////////////////////////////
class CURLRedirectDataClass
{
public:
	CURLRedirectDataClass()
	{
		m_bDisabled=FALSE;
		m_bTopPageOnly=FALSE;
		m_dRedirectPageAction=0;
		m_dwCloseTimeout=3;
	}
	virtual ~CURLRedirectDataClass()
	{
		;
	}
	void Clear()
	{
		m_strHitReason.Empty();
		m_bDisabled=FALSE;
		m_bTopPageOnly=FALSE;
		m_dRedirectPageAction=0;
		m_dwCloseTimeout=3;
		m_strExecType.Empty();
		m_strExecExeFullPath.Empty();
		m_arr_URL.RemoveAll();
		m_arr_URL_EX.RemoveAll();
	}
	void Copy(CURLRedirectDataClass* ptr)
	{
		if(ptr==NULL)return;
		m_bDisabled = ptr->m_bDisabled;
		m_bTopPageOnly = ptr->m_bTopPageOnly;
		m_dRedirectPageAction = ptr->m_dRedirectPageAction;
		m_dwCloseTimeout = ptr->m_dwCloseTimeout;
		m_strExecType = ptr->m_strExecType;
		m_strExecExeFullPath = ptr->m_strExecExeFullPath;
		m_arr_URL.Copy(ptr->m_arr_URL);
		m_arr_URL_EX.Copy(ptr->m_arr_URL_EX);
	}
	int wildcmp(const char *wild, const char *string)
	{
	  const char *cp = NULL, *mp = NULL;

	  while ((*string) && (*wild != '*')) {
		if ((*wild != *string) && (*wild != '?')) {
		  return 0;
		}
		wild++;
		string++;
	  }

	  while (*string) {
		if (*wild == '*') {
		  if (!*++wild) {
			return 1;
		  }
		  mp = wild;
		  cp = string+1;
		} else if ((*wild == *string) || (*wild == '?')) {
		  wild++;
		  string++;
		} else {
		  wild = mp;
		  string = cp++;
		}
	  }

	  while (*wild == '*') {
		wild++;
	  }
	  return !*wild;
	}

	BOOL IsRedirect(LPCTSTR pURL)
	{
		CString strTemp;
		BOOL bRet=FALSE;
		m_strHitReason.Empty();

		//Include List
		if(bRet==FALSE)
		{
			int imax = (int)m_arr_URL.GetCount();
			for(int i=0;i<imax;i++)
			{
				strTemp.Empty();
				strTemp = m_arr_URL.GetAt(i);
				CStringA strURL_A(pURL);
				CStringA strList_A(strTemp);

				//ワイルドカード対応
				if(wildcmp(strList_A,strURL_A))
				{
					//Hitした場合
					bRet = TRUE;
					m_strHitReason.Format(_T("ターゲットURLルール：%s"),strTemp);
					break;
				}
			}
		}
		//除外の確認
		//除外にヒットした場合は、FALSEに戻す。
		if(bRet==TRUE)
		{
			int imax2 = (int)m_arr_URL_EX.GetCount();
			for(int j=0;j<imax2;j++)
			{
				strTemp.Empty();
				strTemp = m_arr_URL_EX.GetAt(j);
				CStringA strURL_A(pURL);
				CStringA strList_A(strTemp);

				//ワイルドカード対応
				if(wildcmp(strList_A,strURL_A))
				{
					//除外にヒットした場合は、FALSEに戻す。
					//Hitした場合
					bRet = FALSE;
					m_strHitReason.Format(_T("除外URLルール：%s"),strTemp);
					break;
				}
			}
		}
		return bRet;
	}
public:
	BOOL m_bDisabled;
	BOOL m_bTopPageOnly;
	DWORD m_dRedirectPageAction;
	DWORD m_dwCloseTimeout;

	CString m_strExecType;
	CString m_strExecExeFullPath;
	CAtlArray<CAtlString> m_arr_URL;
	CAtlArray<CAtlString> m_arr_URL_EX;
	CString m_strHitReason;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
class CURLRedirectList
{
public:
	CAtlArray<intptr_t> m_arr_RedirectBrowser;
	BOOL m_bURLOnly;
	BOOL m_bQuickRedirect;
	BOOL m_bUseNeutralSite;
	BOOL m_bTopURLOnly;

	CString m_strHitReasonAll;
	CString m_strSettingFilePath;

	CURLRedirectList()
	{
		m_bURLOnly=FALSE;
		m_bQuickRedirect = FALSE;
		m_bUseNeutralSite = FALSE;
		m_bTopURLOnly = FALSE;
	}

	virtual ~CURLRedirectList()
	{
		Clear();
	}
	void Clear()
	{
		m_bURLOnly=FALSE;
		m_bQuickRedirect = FALSE;
		m_bUseNeutralSite = FALSE;
		m_bTopURLOnly = FALSE;
		m_strHitReasonAll.Empty();
		CURLRedirectDataClass* ptr=NULL;
		int imax = (int)m_arr_RedirectBrowser.GetCount();
		for(int i=0;i<imax;i++)
		{
			ptr=(CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
			if(ptr)
			{
				delete ptr;
				ptr=NULL;
			}
		}
		m_arr_RedirectBrowser.RemoveAll();
	}

	void SetArrayData(LPCTSTR lPath)
	{
		if(lPath==NULL)return;
		this->Clear();
		CAtlStdioFile in;
		HRESULT		hr={0};

		hr = in.OpenFile(lPath,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
		if(hr==S_OK)
		{
			CURLRedirectDataClass* pRedirectData=NULL;
			CStringA strTemp;
			CString strTempU;
			CString strTempUpper;

			CString strExecExeFullPath;
			CString strExecType;
			CAtlArray<CAtlString> arr_URL;
			CAtlArray<CAtlString> arr_URL_EX;

			BOOL bTopPageOnly=FALSE;
			DWORD dRedirectPageAction=0;
			DWORD dwCloseTimeout=3;

			BOOL bDisabled=FALSE;
			BOOL bFirstSection=FALSE;
			while(in.ReadLineA(strTemp))
			{
				strTemp.TrimLeft();
				strTemp.TrimRight();
				strTempU=strTemp;
				strTempUpper=strTempU;
				strTempUpper.MakeUpper();
				//空白の場合は、次へ
				if(strTempU.IsEmpty())
					continue;

				if(strTempU.Find(_T(";"))==0)
				{
					;
				}
				else if(strTempU.Find(_T("#"))==0)
				{
					;
				}
				else if(strTempU.Find(_T("["))==0)
				{
					if(!strExecType.IsEmpty())
					{
						//無効では、無い場合(有効なとき)
						if(!bDisabled)
						{
							pRedirectData = new CURLRedirectDataClass;
							pRedirectData->m_bDisabled=bDisabled;

							pRedirectData->m_strExecType=strExecType;
							pRedirectData->m_strExecExeFullPath=strExecExeFullPath;

							pRedirectData->m_arr_URL.Copy(arr_URL);
							pRedirectData->m_arr_URL_EX.Copy(arr_URL_EX);
							pRedirectData->m_bTopPageOnly=bTopPageOnly;
							pRedirectData->m_dRedirectPageAction=dRedirectPageAction;
							pRedirectData->m_dwCloseTimeout=dwCloseTimeout;
							m_arr_RedirectBrowser.Add((intptr_t)pRedirectData);
						}
						bTopPageOnly=FALSE;
						dRedirectPageAction=0;
						dwCloseTimeout=3;
						strExecExeFullPath.Empty();
						strExecType.Empty();
						arr_URL.RemoveAll();
						arr_URL_EX.RemoveAll();
						pRedirectData = NULL;
						bDisabled=FALSE;
					}
					strExecType=strTempU.Mid(1,strTempU.ReverseFind(']')-1);
					bFirstSection=TRUE;
					strTempU.Empty();
				}
				if(!bFirstSection)
					continue;

				if(strTempU.IsEmpty())
					continue;
				else if(strTempU.Find(_T("@"))==0)
				{
					if(strTempUpper.Find(_T("@TOP_PAGE_ONLY"))==0)
					{
						bTopPageOnly = TRUE;
						if(strExecType==_T("GLOBAL"))
						{
							m_bURLOnly=TRUE;
						}
					}
					else if(strTempUpper.Find(_T("@REDIRECT_PAGE_ACTION:"))==0)
					{
						int iPosC=0;
						CString strDig;
						strDig = _T("@REDIRECT_PAGE_ACTION:");
						iPosC=strDig.GetLength();
						strDig = strTempUpper.Mid(iPosC);
						dRedirectPageAction = _ttoi(strDig);
						if(dRedirectPageAction < 0)dRedirectPageAction=0;
						else if(dRedirectPageAction > 2)dRedirectPageAction=2;
					}
					else if(strTempUpper.Find(_T("@CLOSE_TIMEOUT:"))==0)
					{
						int iPosC=0;
						CString strDig;
						strDig = _T("@CLOSE_TIMEOUT:");
						iPosC=strDig.GetLength();
						strDig = strTempUpper.Mid(iPosC);
						dwCloseTimeout = _ttoi(strDig);
						if(dwCloseTimeout < 1)dwCloseTimeout=3;
						else if(dwCloseTimeout > 60)dwCloseTimeout=60;
					}
					else if(strTempUpper.Find(_T("@BROWSER_PATH:"))==0)
					{
						int iPosC=0;
						CString strPathDig;
						strPathDig = _T("@BROWSER_PATH:");
						iPosC=strPathDig.GetLength();
						strPathDig = strTempU.Mid(iPosC);
						strPathDig.TrimLeft();
						strPathDig.TrimRight();
						strExecExeFullPath = strPathDig;
					}
					else if(strTempUpper.Find(_T("@DISABLED"))==0)
					{
						bDisabled = TRUE;
					}
					else if(strTempUpper.Find(_T("@INTRANET_ZONE"))==0)
					{
						if (strExecType == _T("GLOBAL"))
						{
							m_bQuickRedirect = TRUE;
						}
					}
					else if(strTempUpper.Find(_T("@TRUSTED_ZONE"))==0)
					{
						if (strExecType == _T("GLOBAL"))
						{
							m_bUseNeutralSite = TRUE;
						}
					}
					else if(strTempUpper.Find(_T("@INTERNET_ZONE"))==0)
					{
						;
					}
					else if(strTempUpper.Find(_T("@UNTRUSTED_ZONE"))==0)
					{
						if (strExecType == _T("GLOBAL"))
						{
							m_bTopURLOnly = TRUE;
						}
					}

				}
				else if(strTempU.Find(_T("-"))==0)
				{
					arr_URL_EX.Add(strTempU.Mid(1));
				}
				else
				{
					arr_URL.Add(strTempU);
				}
			}
			in.Close();
			if(!strExecType.IsEmpty())
			{
				if(pRedirectData==NULL)
				{
					//無効では、無い場合(有効なとき)
					if(!bDisabled)
					{
						pRedirectData = new CURLRedirectDataClass;
						pRedirectData->m_bDisabled=bDisabled;
						pRedirectData->m_strExecType=strExecType;
						pRedirectData->m_strExecExeFullPath=strExecExeFullPath;
						pRedirectData->m_arr_URL.Copy(arr_URL);
						pRedirectData->m_arr_URL_EX.Copy(arr_URL_EX);
						pRedirectData->m_bTopPageOnly=bTopPageOnly;
						pRedirectData->m_dRedirectPageAction=dRedirectPageAction;
						pRedirectData->m_dwCloseTimeout=dwCloseTimeout;
						m_arr_RedirectBrowser.Add((intptr_t)pRedirectData);
					}
					bTopPageOnly=FALSE;
					dRedirectPageAction=0;
					dwCloseTimeout=3;
					strExecType.Empty();
					strExecExeFullPath.Empty();
					arr_URL.RemoveAll();
					arr_URL_EX.RemoveAll();
					pRedirectData = NULL;
					bDisabled=FALSE;
				}
			}
		}
	}

	BOOL IsRedirect(LPCTSTR sURL,BOOL bTopPage,CAtlArray<intptr_t> *arr_RedirectBrowserHit)
	{
		CString strURL;
		strURL = sURL;
		arr_RedirectBrowserHit->RemoveAll();
		m_strHitReasonAll.Empty();

		CStringA strURL_A(strURL);

		if(m_bURLOnly)
		{
			SBUtil::Trim_URLOnly(sURL,strURL);
		}

		BOOL bRet=FALSE;
		CURLRedirectDataClass* pRedirectData=NULL;
		int imax = (int)m_arr_RedirectBrowser.GetCount();

		//Office365対応、ニュートラルサイト(共用URL)を利用する
		if (m_bUseNeutralSite)
		{
			//ニュートラルサイトを始めにチェック
			//HITしたら、リダイレクトをしない。
			//HITしない場合は、ニュートラルサイトを除くルールの判定を行う。
			BOOL bDMZFlg = FALSE;
			for (int i = 0; i < imax; i++)
			{
				pRedirectData = (CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
				if (pRedirectData)
				{
					if (pRedirectData->m_strExecType == _T(SECTION_DMZ))
					{
						//frameの場合でオブジェクトの判定条件がTOPのみの場合は、次へ。
						if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
							break;
						//リダイレクトを無効にするリストにHITした。
						if (pRedirectData->IsRedirect(strURL))
						{
							bDMZFlg = TRUE;
							if (!pRedirectData->m_strHitReason.IsEmpty())
							{
								CString strReasonLine;
								strReasonLine.Format(_T("HIT [%s] %s / "), pRedirectData->m_strExecType, pRedirectData->m_strHitReason);
								m_strHitReasonAll += strReasonLine;
							}
						}
						break;
					}
				}
			}
			//ニュートラルサイトルールにHITした場合は、リダイレクトしない。
			if (bDMZFlg)
			{
				//リダイレクトしない。
				return FALSE;
			}
			//ニュートラルサイトを除くルールを判定する。
			for (int i = 0; i < imax; i++)
			{
				pRedirectData = (CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
				if (pRedirectData)
				{
					//ニュートラルサイト
					if (pRedirectData->m_strExecType == _T(SECTION_DMZ))
					{
						//スキップする。
						continue;
					}

					//frameの場合でオブジェクトの判定条件がTOPのみの場合は、次へ。
					if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
						continue;
					BOOL bRe = pRedirectData->IsRedirect(strURL);
					CString strReasonLine;
					if (bRe)
					{
						arr_RedirectBrowserHit->Add((intptr_t)pRedirectData);
						bRet = TRUE;
						//続行する。最後までチェック。
					}
					if (!pRedirectData->m_strHitReason.IsEmpty())
					{
						if (bRe)
							strReasonLine.Format(_T("HIT [%s] %s / "), pRedirectData->m_strExecType, pRedirectData->m_strHitReason);
						else
							strReasonLine.Format(_T("SKIP [%s] %s / "), pRedirectData->m_strExecType, pRedirectData->m_strHitReason);
						m_strHitReasonAll += strReasonLine;
					}
					//例外 [CUSTOM19]の場合は、[CUSTOM20]をチェックしない。CUSTOM19とCUSTOM20はセットとみなす。
					if (bRet)
					{
						if (pRedirectData->m_strExecType == _T("CUSTOM19"))
						{
							//CUSTOM19でhitしているので、次のCUSTOM20はチェック不要
							break;
						}
					}
				}
			}
		}
		//通常の処理
		else
		{
			for(int i=0;i<imax;i++)
			{
				pRedirectData = (CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
				if(pRedirectData)
				{
					//frameの場合でオブジェクトの判定条件がTOPのみの場合は、次へ。
					if(bTopPage == FALSE && pRedirectData->m_bTopPageOnly==TRUE)
						continue;
					BOOL bRe=pRedirectData->IsRedirect(strURL);
					CString strReasonLine;
					if(bRe)
					{
						arr_RedirectBrowserHit->Add((intptr_t)pRedirectData);
						bRet = TRUE;
						//続行する。最後までチェック。
					}
					if(!pRedirectData->m_strHitReason.IsEmpty())
					{
						if(bRe)
							strReasonLine.Format(_T("HIT [%s] %s / "),pRedirectData->m_strExecType,pRedirectData->m_strHitReason);
						else
							strReasonLine.Format(_T("SKIP [%s] %s / "),pRedirectData->m_strExecType,pRedirectData->m_strHitReason);
						m_strHitReasonAll+=strReasonLine;
					}
				}
			}
		}
		return bRet;
	}
};


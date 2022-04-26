#pragma once
#include "resource.h"       // メイン シンボル
#include <shlguid.h> 
#include <exdispid.h> 
#include "BHORedirector_i.h"
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
		m_arr_ExcludeGroup.RemoveAll();
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
		m_arr_ExcludeGroup.Copy(ptr->m_arr_ExcludeGroup);
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
				if(!wildcmp(strList_A,strURL_A))continue;

				//Hitした場合
				bRet = TRUE;
				m_strHitReason.Format(_T("ターゲットURLルール：%s"),strTemp);
				break;
			}
		}
		//除外の確認
		//除外にヒットした場合は、FALSEに戻す。
		if(bRet==TRUE)
		{
			int imax = (int)m_arr_URL_EX.GetCount();
			for(int i=0;i<imax;i++)
			{
				strTemp.Empty();
				strTemp = m_arr_URL_EX.GetAt(i);
				CStringA strURL_A(pURL);
				CStringA strList_A(strTemp);

				//ワイルドカード対応
				if(!wildcmp(strList_A,strURL_A))continue;

				//除外にヒットした場合は、FALSEに戻す。
				//Hitした場合
				bRet = FALSE;
				m_strHitReason.Format(_T("除外URLルール：%s"),strTemp);
				break;
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
	CAtlArray<CAtlString> m_arr_ExcludeGroup;
	CString m_strHitReason;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
class CHitDataElement
{
public:
	CHitDataElement(){}
	~CHitDataElement(){}
	CAtlArray<intptr_t> m_arr_RedirectBrowserHit;
	CString m_strHitReason;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
class CHitCachedData
{
public:
	CHitCachedData(){}
	~CHitCachedData(){Clear();}

	void Clear()
	{
		POSITION pos={0};
		pos = m_MapHitURLs.GetStartPosition();
		CHitDataElement* ptr=NULL;
		while (pos)
		{
			ptr = NULL;
			ptr = (CHitDataElement*)m_MapHitURLs.GetValueAt(pos);
			if(ptr)
			{
				delete ptr;
				ptr=NULL;
			}
			m_MapHitURLs.GetNext(pos);
		}
		m_MapHitURLs.RemoveAll();
	}

	void AddData(LPCTSTR sURL,LPCTSTR strHitReason,CAtlArray<intptr_t> *arr_RedirectBrowserHit)
	{
		int iMaxCache=(int)m_MapHitURLs.GetCount();
		CHitDataElement* ptr=NULL;
		//キャッシュ溢れの場合
		if(iMaxCache >= 64)
		{
			POSITION pos={0};
			pos = m_MapHitURLs.GetStartPosition();
			while (pos)
			{
				//単純に一番古い物を消す。
				ptr=NULL;
				ptr = (CHitDataElement*)m_MapHitURLs.GetValueAt(pos);
				if(ptr)
				{
					delete ptr;
					ptr=NULL;
				}
				m_MapHitURLs.RemoveAtPos(pos);
				break;
			}
		}
		ptr=new CHitDataElement();
		ptr->m_strHitReason = strHitReason;
		ptr->m_arr_RedirectBrowserHit.Copy(*arr_RedirectBrowserHit);
		m_MapHitURLs.SetAt(sURL,(intptr_t)ptr);
	}

	BOOL IsHItURLCached(LPCTSTR sURL,CString& pstrReason,CAtlArray<intptr_t> *arr_RedirectBrowserHit)
	{
		CHitDataElement* ptr=NULL;
		intptr_t iVal=0;
		if(m_MapHitURLs.Lookup(sURL,iVal))
		{
			ptr=(CHitDataElement*)iVal;
			if(ptr)
			{
				if(m_gbTraceLog)
				{
					pstrReason = _T("[Cache_Hit]");
					pstrReason += ptr->m_strHitReason;
				}
				arr_RedirectBrowserHit->Copy(ptr->m_arr_RedirectBrowserHit);
				return TRUE;
			}
		}
		return FALSE;
	}
	//リダイレクト対象のURLとClassObjectsをMapに積んでおくため
	CAtlMap<ATL::CString, intptr_t> m_MapHitURLs;
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
	CString m_strScript_FilePath;

	//キャッシュ関連
	BOOL m_bDataCached;
	DWORD m_dFileSize;
	FILETIME m_FileTime;

	//リダイレクト対象URLのHitキャッシュ管理クラスオブジェクト
	CHitCachedData m_HitURLCache_Manager;

	//リダイレクト対象外のURLをMapに積んでおくため
	CAtlMap<ATL::CString, intptr_t> m_MapPassThroughURLs;

	CURLRedirectList()
	{
		m_bDataCached=FALSE;
		m_dFileSize=0;
		FILETIME FileTimeZero={0};
		m_FileTime=FileTimeZero;
		m_bURLOnly=FALSE;
		m_gbTraceLog=FALSE;
		m_bQuickRedirect = FALSE;
		m_bUseNeutralSite =FALSE;
		m_bTopURLOnly=FALSE;
	}

	virtual ~CURLRedirectList()
	{
		Clear();
	}

	void Clear()
	{
		m_bURLOnly=FALSE;
		m_gbTraceLog=FALSE;
		m_bQuickRedirect = FALSE;
		m_bUseNeutralSite =FALSE;
		m_bTopURLOnly=FALSE;
		m_HitURLCache_Manager.Clear();
		m_MapPassThroughURLs.RemoveAll();

		m_bDataCached=FALSE;
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

	void SetArrayDataDelayLoad(LPCTSTR lPath)
	{
		if(lPath==NULL)return;
		m_strSettingFilePath=lPath;
		m_bDataCached=FALSE;
	}

	BOOL IsPassThroughURL(LPCTSTR sURL)
	{
		intptr_t iVal=0;
		if(m_MapPassThroughURLs.Lookup(sURL,iVal))
		{
			return TRUE;
		}
		return FALSE;
	}

	BOOL IsChangeSettingFile()
	{
		BOOL bRet=FALSE;
		DWORD dFileSize=0;
		FILETIME FileTime={0};

		HANDLE hFile={0};
		hFile = CreateFile(m_strSettingFilePath,
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			return TRUE;
		}
		else
		{
			dFileSize=GetFileSize(hFile,NULL);
			//ファイルサイズが異なる場合
			if(dFileSize != m_dFileSize)
			{
				bRet=TRUE;
			}
			else
			{
				GetFileTime(hFile,NULL,NULL,&FileTime);
				//更新日が異なる場合
				if(FileTime.dwHighDateTime != m_FileTime.dwHighDateTime
				 ||FileTime.dwLowDateTime !=  m_FileTime.dwLowDateTime)
					bRet=TRUE;
			}
		}
		CloseHandle(hFile);
		return bRet;
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
			m_dFileSize=0;
			FILETIME FileTimeZero={0};
			m_FileTime=FileTimeZero;

			m_dFileSize=GetFileSize(in.m_h,NULL);
			GetFileTime(in.m_h,NULL,NULL,&m_FileTime);

			CURLRedirectDataClass* pRedirectData=NULL;
			CStringA strTemp;
			CString strTempU;
			CString strTempUpper;

			CString strExecExeFullPath;
			CString strExecType;
			CAtlArray<CAtlString> arr_URL;
			CAtlArray<CAtlString> arr_URL_EX;
			CAtlArray<CAtlString> arr_ExcludeGroup;

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
							pRedirectData->m_arr_ExcludeGroup.Copy(arr_ExcludeGroup);
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
						arr_ExcludeGroup.RemoveAll();
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
						if(strExecType==_T("GLOBAL"))
						{
							m_gbTraceLog = dRedirectPageAction==1?TRUE:FALSE;
						}
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
					else if (strTempUpper.Find(_T("@EXCLUDE_GROUP")) == 0)
					{
						int iPosC = 0;
						CString strName;
						strName = _T("@EXCLUDE_GROUP:");
						iPosC = strName.GetLength();
						strName = strTemp.Mid(iPosC);
						strName.TrimLeft();
						strName.TrimRight();
						arr_ExcludeGroup.Add(strName);
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
						pRedirectData->m_arr_ExcludeGroup.Copy(arr_ExcludeGroup);
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
					arr_ExcludeGroup.RemoveAll();
					pRedirectData = NULL;
					bDisabled=FALSE;
				}
			}
			m_bDataCached=TRUE;
		}
	}

	BOOL IsRedirect(LPCTSTR sURL,BOOL bTopPage,CAtlArray<intptr_t> *arr_RedirectBrowserHit)
	{
		CString strURL;
		strURL = sURL;
		arr_RedirectBrowserHit->RemoveAll();
		m_strHitReasonAll.Empty();

		CStringA strURL_A(strURL);

		//Dataがキャッシュされていない場合は、ここでロード
		if(!m_bDataCached)
		{
			SetArrayData(m_strSettingFilePath);
			m_MapPassThroughURLs.RemoveAll();
			m_HitURLCache_Manager.Clear();
			if(m_bURLOnly)
			{
				SBUtil::Trim_URLOnly(sURL,strURL);
			}
		}
		//メモリにキャッシュしている情報の更新チェック
		else
		{
			//File Size 更新日が異なるか?
			if(IsChangeSettingFile())
			{
				//キャッシュを破棄してメモリに積み直す
				m_bDataCached=FALSE;
				SetArrayData(m_strSettingFilePath);
				m_MapPassThroughURLs.RemoveAll();
				m_HitURLCache_Manager.Clear();

				if(m_bURLOnly)
				{
					SBUtil::Trim_URLOnly(sURL,strURL);
				}
			}
			//更新されていない、メモリに積んている内容のスルーURLリストをチェックする。
			//Hitキャッシュを確認し、キャッシュしている場合は、それを返す。
			else
			{
				if(m_bURLOnly)
				{
					SBUtil::Trim_URLOnly(sURL,strURL);
				}
				if(IsPassThroughURL(strURL))
					return FALSE;

				if(m_HitURLCache_Manager.IsHItURLCached(strURL,m_strHitReasonAll,arr_RedirectBrowserHit))
					return TRUE;
			}
		}

		BOOL bRet=FALSE;
		CURLRedirectDataClass* pRedirectData=NULL;
		CURLRedirectDataClass* pRedirectData_NeutralSite = NULL;
		CURLRedirectDataClass* pRedirectData_IE = NULL;
		CURLRedirectDataClass* pRedirectData_Default = NULL;
		int imax = (int)m_arr_RedirectBrowser.GetCount();

		for (int i = 0; i < imax; i++)
		{
			pRedirectData=NULL;
			pRedirectData = (CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
			if (pRedirectData)
			{
				//IEサイト
				if (pRedirectData->m_strExecType == _T("IE"))
				{
					pRedirectData_IE = pRedirectData;
				}
				//Defaultサイト
				else if (pRedirectData->m_strExecType == _T("Default"))
				{
					pRedirectData_Default = pRedirectData;
				}
				//ニュートラルサイト(Custom18)
				else if (pRedirectData->m_strExecType == _T("CUSTOM18"))
				{
					pRedirectData_NeutralSite = pRedirectData;
				}
			}
		}
		//Office365対応、ニュートラルサイト(共用URL)を利用する
		if (m_bUseNeutralSite)
		{
			//ニュートラルサイト(Custom18)を始めにチェック
			//HITしたら、リダイレクトをしない。
			//HITしない場合は、ニュートラルサイトを除くルールの判定を行う。
			pRedirectData = pRedirectData_NeutralSite;
			if (pRedirectData)
			{
				//[CUSTOM18]
				if (pRedirectData->m_strExecType == _T("CUSTOM18"))
				{
					//frameの場合でオブジェクトの判定条件がTOPのみの場合は、次へ。
					if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
						;
					else
					{
						//リダイレクトを無効にするリストにHITした。
						if (pRedirectData->IsRedirect(strURL))
						{
							if (m_gbTraceLog)
							{
								if (!pRedirectData->m_strHitReason.IsEmpty())
								{
									CString strReasonLine;
									strReasonLine.Format(_T("HIT [%s] %s / "), pRedirectData->m_strExecType, pRedirectData->m_strHitReason);
									m_strHitReasonAll += strReasonLine;
								}
							}
							//ニュートラルサイトルールにHITした場合は、リダイレクトしない。
							return FALSE;
						}
					}
				}
			}
			//IEサイトをチェック
			//HITしたら、リダイレクトをしない。
			pRedirectData = pRedirectData_IE;
			if (pRedirectData)
			{
				//[IE]
				if (pRedirectData->m_strExecType == _T("IE"))
				{
					//frameの場合でオブジェクトの判定条件がTOPのみの場合は、次へ。
					if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
						;
					else
					{
						//リダイレクトを無効にするリストにHITした。
						if (pRedirectData->IsRedirect(strURL))
						{
							if (m_gbTraceLog)
							{
								if (!pRedirectData->m_strHitReason.IsEmpty())
								{
									CString strReasonLine;
									strReasonLine.Format(_T("HIT [%s] %s / "), pRedirectData->m_strExecType, pRedirectData->m_strHitReason);
									m_strHitReasonAll += strReasonLine;
								}
							}
							//IEサイトHITした場合は、リダイレクトしない。
							return FALSE;
						}
					}
				}
			}

			//ニュートラルサイトを除くルールを判定する。
			for (int i = 0; i < imax; i++)
			{
				pRedirectData = (CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
				if (pRedirectData)
				{
					//ニュートラルサイト [CUSTOM18]
					if (pRedirectData->m_strExecType == _T("CUSTOM18"))
					{
						//スキップする。
						continue;
					}

					//IEサイト [IE]
					if (pRedirectData->m_strExecType == _T("IE"))
					{
						//スキップする。
						continue;
					}

					//Defaultサイト [Default]
					if (pRedirectData->m_strExecType == _T("Default"))
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
					if (m_gbTraceLog)
					{
						if (!pRedirectData->m_strHitReason.IsEmpty())
						{
							if (bRe)
								strReasonLine.Format(_T("HIT [%s] %s / "), pRedirectData->m_strExecType, pRedirectData->m_strHitReason);
							else
								strReasonLine.Format(_T("SKIP [%s] %s / "), pRedirectData->m_strExecType, pRedirectData->m_strHitReason);
							m_strHitReasonAll += strReasonLine;
						}
					}

					//例外 [CUSTOM19]の場合は、[CUSTOM20]をチェックしない。CUSTOM19とCUSTOM20はセットとみなす。
					if(bRet)
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
		//以前のバージョン（通常の処理）
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
					//IEサイト [IE]
					if (pRedirectData->m_strExecType == _T("IE"))
					{
						//スキップする。
						continue;
					}

					//Defaultサイト [Default]
					if (pRedirectData->m_strExecType == _T("Default"))
					{
						//スキップする。
						continue;
					}
					BOOL bRe=pRedirectData->IsRedirect(strURL);
					CString strReasonLine;
					if(bRe)
					{
						arr_RedirectBrowserHit->Add((intptr_t)pRedirectData);
						bRet = TRUE;
						//続行する。最後までチェック。
					}
					if(m_gbTraceLog)
					{
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
		}
		//HITしたものがない。
		if(bRet==FALSE)
		{
			//Defaultブラウザーへリダイレクトする。
			if (pRedirectData_Default)
			{
				//TopURLの場合だけ
				if (bTopPage == TRUE)
				{
					//何かしら設定されている。
					if(!pRedirectData_Default->m_strExecExeFullPath.IsEmpty())
					{
						//IE以外の場合（IEの場合は、何もしない。）
						if(pRedirectData_Default->m_strExecExeFullPath.CompareNoCase(_T("IE")) != 0)
						{
							arr_RedirectBrowserHit->Add((intptr_t)pRedirectData_Default);
							return TRUE;
						}
					}
				}
			}
			//#define MAX_PassThroughURL 100
			int iMaxCache=(int)m_MapPassThroughURLs.GetCount();
			CStringA strURL_A(strURL);
			if(iMaxCache < 256)
				m_MapPassThroughURLs.SetAt(strURL,0);
			//キャッシュ溢れの場合
			else
			{
				POSITION pos={0};
				pos = m_MapPassThroughURLs.GetStartPosition();
				while (pos)
				{
					//単純に一番古い物を消す。
					m_MapPassThroughURLs.RemoveAtPos(pos);
					break;
				}
			}
		}
		//HIT キャッシュされていない、新規URL
		else //bRet==TRUE
		{
			m_HitURLCache_Manager.AddData(strURL,m_strHitReasonAll,arr_RedirectBrowserHit);
		}
		return bRet;
	}

	//MS-EdgeのIEMode用
	BOOL IsRedirectIEMode(LPCTSTR sURL, BOOL bTopPage, CAtlArray<intptr_t> *arr_RedirectBrowserHit)
	{
		CString strURL;
		strURL = sURL;
		arr_RedirectBrowserHit->RemoveAll();
		m_strHitReasonAll.Empty();

		CStringA strURL_A(strURL);
		if (m_bURLOnly)
		{
			SBUtil::Trim_URLOnly(sURL, strURL);
		}

		//Dataがキャッシュされていない場合は、ここでロード
		if (!m_bDataCached)
		{
			SetArrayData(m_strSettingFilePath);
			m_MapPassThroughURLs.RemoveAll();
			m_HitURLCache_Manager.Clear();
		}
		//メモリにキャッシュしている情報の更新チェック
		else
		{
			//File Size 更新日が異なるか?
			if (IsChangeSettingFile())
			{
				//キャッシュを破棄してメモリに積み直す
				m_bDataCached = FALSE;
				SetArrayData(m_strSettingFilePath);
				m_MapPassThroughURLs.RemoveAll();
				m_HitURLCache_Manager.Clear();
			}
		}
		//メモリに積んている内容のスルーURLリストをチェックする。
		//Hitキャッシュを確認し、キャッシュしている場合は、それを返す。
		if (IsPassThroughURL(strURL))
			return FALSE;

		if (m_HitURLCache_Manager.IsHItURLCached(strURL, m_strHitReasonAll, arr_RedirectBrowserHit))
			return TRUE;

		BOOL bRet = FALSE;
		CURLRedirectDataClass* pRedirectData = NULL;
		CURLRedirectDataClass* pRedirectData_NeutralSite = NULL;
		//CURLRedirectDataClass* pRedirectData_IE = NULL;
		CURLRedirectDataClass* pRedirectData_O365 = NULL;
		CURLRedirectDataClass* pRedirectData_Default = NULL;
		CURLRedirectDataClass* pRedirectData_Edge = NULL;
		int imax = (int)m_arr_RedirectBrowser.GetCount();

		for (int i = 0; i < imax; i++)
		{
			pRedirectData = NULL;
			pRedirectData = (CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
			if (pRedirectData)
			{
				//Edgeサイト
				if (pRedirectData->m_strExecType == _T("Edge"))
				{
					pRedirectData_Edge = pRedirectData;
				}
				////IEサイト
				//else if (pRedirectData->m_strExecType == _T("IE"))
				//{
				//	pRedirectData_IE = pRedirectData;
				//}
				//Defaultサイト
				else if (pRedirectData->m_strExecType == _T("Default"))
				{
					pRedirectData_Default = pRedirectData;
				}
				//ニュートラルサイト(Custom18)
				else if (pRedirectData->m_strExecType == _T("CUSTOM18"))
				{
					pRedirectData_NeutralSite = pRedirectData;
				}
				//Office365サイト(Custom20)
				else if (pRedirectData->m_strExecType == _T("CUSTOM20"))
				{
					pRedirectData_O365 = pRedirectData;
				}
			}
		}

		//ニュートラルサイト(Custom18)を始めにチェック
		//HITしたら、リダイレクトをしない。
		//HITしない場合は、ニュートラルサイトを除くルールの判定を行う。
		pRedirectData = pRedirectData_NeutralSite;
		if (pRedirectData)
		{
			//[CUSTOM18]
			if (pRedirectData->m_strExecType == _T("CUSTOM18"))
			{
				//frameの場合でオブジェクトの判定条件がTOPのみの場合は、次へ。
				if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
					;
				else
				{
					//リダイレクトを無効にするリストにHITした。
					if (pRedirectData->IsRedirect(strURL))
					{
						if (m_gbTraceLog)
						{
							if (!pRedirectData->m_strHitReason.IsEmpty())
							{
								CString strReasonLine;
								strReasonLine.Format(_T("HIT [%s] %s / "), pRedirectData->m_strExecType, pRedirectData->m_strHitReason);
								m_strHitReasonAll += strReasonLine;
							}
						}
						//ニュートラルサイトルールにHITした場合は、リダイレクトしない。
						return FALSE;
					}
				}
			}
		}

		//IEサイトをチェック
		//HITしたら、リダイレクトをしない。
		//pRedirectData = pRedirectData_IE;
		//if (pRedirectData)
		//{
		//	//[IE]
		//	if (pRedirectData->m_strExecType == _T("IE"))
		//	{
		//		//frameの場合でオブジェクトの判定条件がTOPのみの場合は、次へ。
		//		if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
		//			;
		//		else
		//		{
		//			//リダイレクトを無効にするリストにHITした。
		//			if (pRedirectData->IsRedirect(strURL))
		//			{
		//				if (m_gbTraceLog)
		//				{
		//					if (!pRedirectData->m_strHitReason.IsEmpty())
		//					{
		//						CString strReasonLine;
		//						strReasonLine.Format(_T("HIT [%s] %s / "), pRedirectData->m_strExecType, pRedirectData->m_strHitReason);
		//						m_strHitReasonAll += strReasonLine;
		//					}
		//				}
		//				//IEサイトHITした場合は、リダイレクトしない。
		//				return FALSE;
		//			}
		//		}
		//	}
		//}

		//Edge
		pRedirectData = pRedirectData_Edge;
		if (pRedirectData)
		{
			//[Edge]
			if (pRedirectData->m_strExecType == _T("Edge"))
			{
				//frameの場合でオブジェクトの判定条件がTOPのみの場合は、次へ。
				if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
					;
				else
				{
					//EdgeリストにHITした。
					if (pRedirectData->IsRedirect(strURL))
					{
						if (m_gbTraceLog)
						{
							if (!pRedirectData->m_strHitReason.IsEmpty())
							{
								CString strReasonLine;
								strReasonLine.Format(_T("HIT [%s] %s / "), pRedirectData->m_strExecType, pRedirectData->m_strHitReason);
								m_strHitReasonAll += strReasonLine;
							}
						}
						//リダイレクトしない。
						return FALSE;
					}
				}
			}
		}

		//Edge O365
		pRedirectData = pRedirectData_O365;
		if (pRedirectData)
		{
			//[Custm20]
			if (pRedirectData->m_strExecType == _T("CUSTOM20"))
			{
				if(pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("Edge"))==0)
				{
					//frameの場合でオブジェクトの判定条件がTOPのみの場合は、次へ。
					if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
						;
					else
					{
						//EdgeリストにHITした。
						if (pRedirectData->IsRedirect(strURL))
						{
							if (m_gbTraceLog)
							{
								if (!pRedirectData->m_strHitReason.IsEmpty())
								{
									CString strReasonLine;
									strReasonLine.Format(_T("HIT [%s] %s / "), pRedirectData->m_strExecType, pRedirectData->m_strHitReason);
									m_strHitReasonAll += strReasonLine;
								}
							}
							//リダイレクトしない。
							return FALSE;
						}
					}
				}
			}
		}

		//ニュートラルサイトとEdgeを除くルールを判定する。
		for (int i = 0; i < imax; i++)
		{
			pRedirectData = (CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
			if (pRedirectData)
			{
				//ニュートラルサイト [CUSTOM18]
				if (pRedirectData->m_strExecType == _T("CUSTOM18"))
				{
					//スキップする。
					continue;
				}

				//Edge[Edge]
				if (pRedirectData->m_strExecType == _T("Edge"))
				{
					//スキップする。
					continue;
				}

				//Defaultサイト [Default]
				if (pRedirectData->m_strExecType == _T("Default"))
				{
					//スキップする。
					continue;
				}

				//Edge O365
				if (pRedirectData->m_strExecType == _T("CUSTOM20"))
				{
					if (pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("Edge")) == 0)
					{
						//スキップする。
						continue;
					}
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
				if (m_gbTraceLog)
				{
					if (!pRedirectData->m_strHitReason.IsEmpty())
					{
						if (bRe)
							strReasonLine.Format(_T("HIT [%s] %s / "), pRedirectData->m_strExecType, pRedirectData->m_strHitReason);
						else
							strReasonLine.Format(_T("SKIP [%s] %s / "), pRedirectData->m_strExecType, pRedirectData->m_strHitReason);
						m_strHitReasonAll += strReasonLine;
					}
				}
			}
		}
		//HITしたものがない。
		if (bRet == FALSE)
		{
			//Defaultブラウザーへリダイレクトする。
			if (pRedirectData_Default)
			{
				//TopURLの場合だけ
				if (bTopPage == TRUE)
				{
					//何かしら設定されている。
					if (!pRedirectData_Default->m_strExecExeFullPath.IsEmpty())
					{
						//Edge以外の場合（Edgeの場合は、何もしない。）
						if (pRedirectData_Default->m_strExecExeFullPath.CompareNoCase(_T("Edge")) != 0)
						{
							arr_RedirectBrowserHit->Add((intptr_t)pRedirectData_Default);
							return TRUE;
						}
					}
				}
			}
			//#define MAX_PassThroughURL 100
			int iMaxCache = (int)m_MapPassThroughURLs.GetCount();
			CStringA strURL_A(strURL);
			if (iMaxCache < 256)
				m_MapPassThroughURLs.SetAt(strURL, 0);
			//キャッシュ溢れの場合
			else
			{
				POSITION pos = { 0 };
				pos = m_MapPassThroughURLs.GetStartPosition();
				while (pos)
				{
					//単純に一番古い物を消す。
					m_MapPassThroughURLs.RemoveAtPos(pos);
					break;
				}
			}
		}
		//HIT キャッシュされていない、新規URL
		else //bRet==TRUE
		{
			m_HitURLCache_Manager.AddData(strURL, m_strHitReasonAll, arr_RedirectBrowserHit);
		}
		return bRet;
	}

	//VirtIE用(ThinApp Virtual App)
	BOOL IsRedirectVOS(LPCTSTR sURL, BOOL bTopPage, CAtlArray<intptr_t> *arr_RedirectBrowserHit,LPCTSTR sLabel)
	{
		CString strURL;
		strURL = sURL;
		arr_RedirectBrowserHit->RemoveAll();
		m_strHitReasonAll.Empty();

		CStringA strURL_A(strURL);
		if (m_bURLOnly)
		{
			SBUtil::Trim_URLOnly(sURL, strURL);
		}

		//Dataがキャッシュされていない場合は、ここでロード
		if (!m_bDataCached)
		{
			SetArrayData(m_strSettingFilePath);
			m_MapPassThroughURLs.RemoveAll();
			m_HitURLCache_Manager.Clear();
		}
		//メモリにキャッシュしている情報の更新チェック
		else
		{
			//File Size 更新日が異なるか?
			if (IsChangeSettingFile())
			{
				//キャッシュを破棄してメモリに積み直す
				m_bDataCached = FALSE;
				SetArrayData(m_strSettingFilePath);
				m_MapPassThroughURLs.RemoveAll();
				m_HitURLCache_Manager.Clear();
			}
		}
		//メモリに積んている内容のスルーURLリストをチェックする。
		//Hitキャッシュを確認し、キャッシュしている場合は、それを返す。
		if (IsPassThroughURL(strURL))
			return FALSE;

		if (m_HitURLCache_Manager.IsHItURLCached(strURL, m_strHitReasonAll, arr_RedirectBrowserHit))
			return TRUE;

		BOOL bRet = FALSE;
		CURLRedirectDataClass* pRedirectData = NULL;
		CURLRedirectDataClass* pRedirectData_NeutralSite = NULL;
		CURLRedirectDataClass* pRedirectData_Default = NULL;
		CURLRedirectDataClass* pRedirectData_Label = NULL;
		int imax = (int)m_arr_RedirectBrowser.GetCount();

		for (int i = 0; i < imax; i++)
		{
			pRedirectData = NULL;
			pRedirectData = (CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
			if (pRedirectData)
			{
				//指定サイト
				if (pRedirectData->m_strExecType.CompareNoCase(sLabel)==0)
				{
					pRedirectData_Label = pRedirectData;
				}
				//Defaultサイト
				else if (pRedirectData->m_strExecType == _T("Default"))
				{
					pRedirectData_Default = pRedirectData;
				}
				//ニュートラルサイト(Custom18)
				else if (pRedirectData->m_strExecType == _T("CUSTOM18"))
				{
					pRedirectData_NeutralSite = pRedirectData;
				}
			}
		}

		//ニュートラルサイト(Custom18)を始めにチェック
		//HITしたら、リダイレクトをしない。
		//HITしない場合は、ニュートラルサイトを除くルールの判定を行う。
		pRedirectData = pRedirectData_NeutralSite;
		if (pRedirectData)
		{
			//[CUSTOM18]
			if (pRedirectData->m_strExecType == _T("CUSTOM18"))
			{
				//frameの場合でオブジェクトの判定条件がTOPのみの場合は、次へ。
				if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
					;
				else
				{
					//リダイレクトを無効にするリストにHITした。
					if (pRedirectData->IsRedirect(strURL))
					{
						if (m_gbTraceLog)
						{
							if (!pRedirectData->m_strHitReason.IsEmpty())
							{
								CString strReasonLine;
								strReasonLine.Format(_T("HIT [%s] %s / "), pRedirectData->m_strExecType, pRedirectData->m_strHitReason);
								m_strHitReasonAll += strReasonLine;
							}
						}
						//ニュートラルサイトルールにHITした場合は、リダイレクトしない。
						return FALSE;
					}
				}
			}
		}

		//Label
		pRedirectData = pRedirectData_Label;
		if (pRedirectData)
		{
			//[Edge]
			if (pRedirectData->m_strExecType == sLabel)
			{
				//frameの場合でオブジェクトの判定条件がTOPのみの場合は、次へ。
				if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
					;
				else
				{
					//リストにHITした。
					if (pRedirectData->IsRedirect(strURL))
					{
						if (m_gbTraceLog)
						{
							if (!pRedirectData->m_strHitReason.IsEmpty())
							{
								CString strReasonLine;
								strReasonLine.Format(_T("HIT [%s] %s / "), pRedirectData->m_strExecType, pRedirectData->m_strHitReason);
								m_strHitReasonAll += strReasonLine;
							}
						}
						//リダイレクトしない。
						return FALSE;
					}
				}
			}
		}

		//ニュートラルサイトとEdgeを除くルールを判定する。
		for (int i = 0; i < imax; i++)
		{
			pRedirectData = (CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
			if (pRedirectData)
			{
				//ニュートラルサイト [CUSTOM18]
				if (pRedirectData->m_strExecType == _T("CUSTOM18"))
				{
					//スキップする。
					continue;
				}

				//Edge[Edge]
				if (pRedirectData->m_strExecType.CompareNoCase(sLabel)==0)
				{
					//スキップする。
					continue;
				}

				//Defaultサイト [Default]
				if (pRedirectData->m_strExecType == _T("Default"))
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
				if (m_gbTraceLog)
				{
					if (!pRedirectData->m_strHitReason.IsEmpty())
					{
						if (bRe)
							strReasonLine.Format(_T("HIT [%s] %s / "), pRedirectData->m_strExecType, pRedirectData->m_strHitReason);
						else
							strReasonLine.Format(_T("SKIP [%s] %s / "), pRedirectData->m_strExecType, pRedirectData->m_strHitReason);
						m_strHitReasonAll += strReasonLine;
					}
				}
			}
		}
		//HITしたものがない。
		if (bRet == FALSE)
		{
			//Defaultブラウザーへリダイレクトする。
			if (pRedirectData_Default)
			{
				//TopURLの場合だけ
				if (bTopPage == TRUE)
				{
					//何かしら設定されている。
					if (!pRedirectData_Default->m_strExecExeFullPath.IsEmpty())
					{
						arr_RedirectBrowserHit->Add((intptr_t)pRedirectData_Default);
						return TRUE;
					}
				}
			}
			//#define MAX_PassThroughURL 100
			int iMaxCache = (int)m_MapPassThroughURLs.GetCount();
			CStringA strURL_A(strURL);
			if (iMaxCache < 256)
				m_MapPassThroughURLs.SetAt(strURL, 0);
			//キャッシュ溢れの場合
			else
			{
				POSITION pos = { 0 };
				pos = m_MapPassThroughURLs.GetStartPosition();
				while (pos)
				{
					//単純に一番古い物を消す。
					m_MapPassThroughURLs.RemoveAtPos(pos);
					break;
				}
			}
		}
		//HIT キャッシュされていない、新規URL
		else //bRet==TRUE
		{
			m_HitURLCache_Manager.AddData(strURL, m_strHitReasonAll, arr_RedirectBrowserHit);
		}
		return bRet;
	}

};


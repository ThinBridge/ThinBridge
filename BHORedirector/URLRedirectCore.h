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
#include "ScriptHost.h"
#include "AtlStdioFile.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
class CURLRedirectDataClass
{
public:
	CURLRedirectDataClass()
	{
		m_bDisabled=FALSE;
		m_bTopPageOnly=FALSE;
		m_dwZone=ZONE_NA;
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
		m_dwZone=ZONE_NA;
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
		m_dwZone = ptr->m_dwZone;
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

	BOOL IsRedirect(LPCTSTR pURL,DWORD dwZone)
	{
		CString strTemp;
		BOOL bRet=FALSE;
		m_strHitReason.Empty();

		//Zone予約後判定
		if(m_dwZone)
		{
			//INTRANET_ZONE;
			//TRUSTED_ZONE;
			//INTERNET_ZONE;
			//UNTRUSTED_ZONE;
			if( (m_dwZone & dwZone) == dwZone)
			{
				//Hitした場合
				bRet = TRUE;
				if(m_gbTraceLog)
				{
					if(dwZone==INTRANET_ZONE)
					{
						m_strHitReason=_T("ローカルイントラネットゾーン");
					}
					else if(dwZone==INTERNET_ZONE)
					{
						m_strHitReason=_T("インターネットゾーン");
					}
					else if(dwZone==TRUSTED_ZONE)
					{
						m_strHitReason=_T("信頼済みサイトゾーン");
					}
					else if(dwZone==UNTRUSTED_ZONE)
					{
						m_strHitReason=_T("制限付きサイトゾーン");
					}
				}
			}
		}
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
					if(m_gbTraceLog)
					{
						m_strHitReason.Format(_T("ターゲットURLルール：%s"),strTemp);
					}
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
					if(m_gbTraceLog)
					{
						m_strHitReason.Format(_T("除外URLルール：%s"),strTemp);
					}
					break;
				}
			}
		}
		return bRet;
	}
public:
	BOOL m_bDisabled;
	BOOL m_bTopPageOnly;
	DWORD m_dwZone;
	DWORD m_dRedirectPageAction;
	DWORD m_dwCloseTimeout;

	CString m_strExecType;
	CString m_strExecExeFullPath;
	CAtlArray<CAtlString> m_arr_URL;
	CAtlArray<CAtlString> m_arr_URL_EX;
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

#ifdef _DEBUG
//		CString logmsg;
//		logmsg.Format(_T("ThinBridge:BHO\tHitURLCache\t%d\t%s\n"),iMaxCache,sURL);
//		ATLTRACE(logmsg);
#endif
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
class CScriptSrcMgr
{
public:
	CScriptSrcMgr()
	{
		m_bDataCached=FALSE;
		m_dFileSize=0;
		FILETIME FileTimeZero={0};
		m_FileTime=FileTimeZero;
	}
	~CScriptSrcMgr()
	{
	}
	void Clear()
	{
		m_bDataCached=FALSE;
		m_dFileSize=0;
		FILETIME FileTimeZero={0};
		m_FileTime=FileTimeZero;
		m_strSrc.Empty();
	}
	void SetDataDelayLoad(LPCTSTR lPath)
	{
		if(lPath==NULL)return;
		m_strFilePath=lPath;
		m_bDataCached=FALSE;
	}
	void SetData()
	{
		this->Clear();
		CAtlFile file;
		CString strText;
		HRESULT		hr={0};

		hr = file.Create(m_strFilePath,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
		if(hr==S_OK)
		{
			m_dFileSize=0;
			FILETIME FileTimeZero={0};
			m_FileTime=FileTimeZero;
			GetFileTime(file.m_h,NULL,NULL,&m_FileTime);

			UINT iFileSize=0;
			iFileSize = GetFileSize(file,NULL);
			m_dFileSize = iFileSize;

			byte* pBuffer=new byte[iFileSize+2];
			memset(pBuffer,0x00,iFileSize+2);
			file.Read(pBuffer,iFileSize);
			strText = pBuffer;
			delete [] pBuffer;
			file.Close();
		}
		m_strSrc=strText;
	}

	CString m_strFilePath;
	CString m_strSrc;
	//キャッシュ関連
	BOOL m_bDataCached;
	DWORD m_dFileSize;
	FILETIME m_FileTime;
	BOOL IsChangeSettingFile()
	{
		BOOL bRet=FALSE;
		DWORD dFileSize=0;
		FILETIME FileTime={0};

		HANDLE hFile={0};
		hFile = CreateFile(m_strFilePath,
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

};
//////////////////////////////////////////////////////////////////////////////////////////////////
class CURLRedirectList
{
public:
	CAtlArray<intptr_t> m_arr_RedirectBrowser;
	BOOL m_bURLOnly;
	BOOL m_bUseScript;

	CString m_strHitReasonAll;
	CString m_strSettingFilePath;
	CString m_strScript_FilePath;

	CScriptSrcMgr m_ScMgr;

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
		m_bUseScript=FALSE;
	}

	virtual ~CURLRedirectList()
	{
		Clear();
	}
	void Clear()
	{
		m_bURLOnly=FALSE;
		m_gbTraceLog=FALSE;
		m_bUseScript=FALSE;
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
			//m_MapPassThroughURLs.SetAt(sURL,++iVal);
#ifdef _DEBUG
			CString logmsg;
			logmsg.Format(_T("ThinBridge:BHO\tIsPassThroughURL\t%s\n"),sURL);
			//OutputDebugString(logmsg);
#endif
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
		//hr = in.Create(lPath,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
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

			BOOL bTopPageOnly=FALSE;
			DWORD dRedirectPageAction=0;
			DWORD dwCloseTimeout=3;

			DWORD dwZone=ZONE_NA;
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
							pRedirectData->m_dwZone=dwZone;
							m_arr_RedirectBrowser.Add((intptr_t)pRedirectData);
						}
						bTopPageOnly=FALSE;
						dRedirectPageAction=0;
						dwCloseTimeout=3;
						dwZone=ZONE_NA;
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
						if(strExecType==_T("GLOBAL"))
						{
							m_bUseScript = dwCloseTimeout==1?TRUE:FALSE;
						}
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
						dwZone |= INTRANET_ZONE;
					}
					else if(strTempUpper.Find(_T("@TRUSTED_ZONE"))==0)
					{
						dwZone |= TRUSTED_ZONE;
					}
					else if(strTempUpper.Find(_T("@INTERNET_ZONE"))==0)
					{
						dwZone |= INTERNET_ZONE;
					}
					else if(strTempUpper.Find(_T("@UNTRUSTED_ZONE"))==0)
					{
						dwZone |= UNTRUSTED_ZONE;
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
						pRedirectData->m_dwZone=dwZone;
						m_arr_RedirectBrowser.Add((intptr_t)pRedirectData);
					}
					bTopPageOnly=FALSE;
					dRedirectPageAction=0;
					dwCloseTimeout=3;
					dwZone=ZONE_NA;
					strExecType.Empty();
					strExecExeFullPath.Empty();
					arr_URL.RemoveAll();
					arr_URL_EX.RemoveAll();
					pRedirectData = NULL;
					bDisabled=FALSE;
				}
			}
			m_bDataCached=TRUE;
		}
	}

	BOOL IsRedirect(LPCTSTR sURL,BOOL bTopPage,DWORD dwZone,CAtlArray<intptr_t> *arr_RedirectBrowserHit,BOOL bEventBeforeNavigate)
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
			if(m_bUseScript)
			{
				m_ScMgr.SetData();
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

				//グローバルURLメモリのURLリストをクリアする。
				Glogal_SetSkipURL_Clear();

				if(m_bURLOnly)
				{
					SBUtil::Trim_URLOnly(sURL,strURL);
				}
				if(m_bUseScript)
				{
					m_ScMgr.SetData();
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
				if(m_bUseScript)
				{
					if(m_ScMgr.IsChangeSettingFile())
						m_ScMgr.SetData();
				}
				if(IsPassThroughURL(strURL))
					return FALSE;

				if(m_HitURLCache_Manager.IsHItURLCached(strURL,m_strHitReasonAll,arr_RedirectBrowserHit))
					return TRUE;
			}
		}

		//グローバルURL除外に登録されている場合は、即FALSE
		if(Glogal_IsSkipURLfromShareMEM(strURL_A))
		{
#ifdef _DEBUG
			CString logmsg;
			logmsg.Format(_T("ThinBridge:BHO\tGlogal_IsSkipURLfromShareMEM\t%s\n"),strURL);
			//OutputDebugString(logmsg);
#endif
			return FALSE;
		}

		BOOL bRet=FALSE;
		CURLRedirectDataClass* pRedirectData=NULL;
		int imax = (int)m_arr_RedirectBrowser.GetCount();


		BOOL bNoCache=FALSE;
		//Scriptベースの判定
		if(m_bUseScript)
		{
			if(!m_ScMgr.m_strSrc.IsEmpty())
			{
				CScriptHost ScHost;
				CString strRet;
				if(ScHost.ExecScript(sURL,bTopPage,dwZone,m_ScMgr.m_strSrc,bEventBeforeNavigate))
				{
					strRet=ScHost.m_strReturnString;
					if(!strRet.IsEmpty())
					{
						//::MessageBox(NULL,strRet,_T("ThinBridge"),MB_SYSTEMMODAL);
						strRet.TrimLeft();
						strRet.TrimRight();
						CAtlArray<CAtlString> arr_ScriptHitResult;
						SBUtil::Split_TrimBlank(arr_ScriptHitResult,strRet,_T(" "));
						for(int i=0;i<imax;i++)
						{
							pRedirectData = (CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
							if(pRedirectData)
							{
								for(int j=0;j<(int)arr_ScriptHitResult.GetCount();j++)
								{
									CString strHitStr;
									strHitStr = arr_ScriptHitResult.GetAt(j);
									if(strHitStr.CompareNoCase(_T("NoCache"))==0)
									{
										bNoCache=TRUE;
										continue;
									}
									if(pRedirectData->m_strExecType.CompareNoCase(strHitStr)==0)
									{
										arr_RedirectBrowserHit->Add((intptr_t)pRedirectData);
										bRet = TRUE;
										if(m_gbTraceLog)
										{
											CString strReasonLine;
											strReasonLine.Format(_T("HIT [%s] %s / "),pRedirectData->m_strExecType,_T("ThinBridgeScript"));
											m_strHitReasonAll+=strReasonLine;
										}
										break;
									}
								}
							}
						}
					}
				}
			}
		}
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
					BOOL bRe=pRedirectData->IsRedirect(strURL,dwZone);
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
			//キャッシュ有効
			if(!bNoCache)
			{
				//#define MAX_PassThroughURL 100
				int iMaxCache=(int)m_MapPassThroughURLs.GetCount();
#ifdef _DEBUG
				CString logmsg;
				logmsg.Format(_T("ThinBridge:BHO\tm_MapPassThroughURLsCount\t%d\n"),iMaxCache);
				//OutputDebugString(logmsg);
#endif
				CStringA strURL_A(strURL);
				Glogal_SetSkipURL2ShareMEM(strURL_A);
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
						////CString strKey = m_MapPassThroughURLs.GetKeyAt(pos);
						//int iValue = m_MapPassThroughURLs.GetValueAt(pos);
						////ヒット件数が２件以下の物の初めに追加された１0行削除する。
						//if(iValue <=2)
						//{
						//	m_MapPassThroughURLs.RemoveAtPos(pos);
						//	m_MapPassThroughURLs.SetAt(strURL,0);
						//	break;
						//}
						//m_MapPassThroughURLs.GetNext(pos);
					}
				}
			}
		}
		//HIT キャッシュされていない、新規URL
		else //bRet==TRUE
		{
			//キャッシュ有効
			if(!bNoCache)
			{
				m_HitURLCache_Manager.AddData(strURL,m_strHitReasonAll,arr_RedirectBrowserHit);
			}
		}
		return bRet;
	}


	//VOSでの実行は、VirtIE8-11等を想定
	//独自のキャッシュ系は、利用しない。
	//ThinBridge Scriptのみで判定する。結果もキャッシュしない。
	BOOL IsRedirectInVOS(LPCTSTR sURL,BOOL bTopPage,DWORD dwZone,CAtlArray<intptr_t> *arr_RedirectBrowserHit,BOOL bEventBeforeNavigate)
	{
		CString strURL;
		strURL = sURL;
		arr_RedirectBrowserHit->RemoveAll();
		m_strHitReasonAll.Empty();

		//Dataがキャッシュされていない場合は、ここでロード
		if(!m_bDataCached)
		{
			SetArrayData(m_strSettingFilePath);
			if(m_bURLOnly)
			{
				SBUtil::Trim_URLOnly(sURL,strURL);
			}
			m_ScMgr.SetData();
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
				if(m_bURLOnly)
				{
					SBUtil::Trim_URLOnly(sURL,strURL);
				}
				m_ScMgr.SetData();
			}
			//更新されていない、メモリに積んている内容のスルーURLリストをチェックする。
			//Hitキャッシュを確認し、キャッシュしている場合は、それを返す。
			else
			{
				if(m_bURLOnly)
				{
					SBUtil::Trim_URLOnly(sURL,strURL);
				}
				if(m_ScMgr.IsChangeSettingFile())
						m_ScMgr.SetData();
			}
		}
		BOOL bRet=FALSE;
		CURLRedirectDataClass* pRedirectData=NULL;
		int imax = (int)m_arr_RedirectBrowser.GetCount();

		//Scriptベースの判定
		if(!m_ScMgr.m_strSrc.IsEmpty())
		{
			CScriptHost ScHost;
			CString strRet;
			if(ScHost.ExecScript(sURL,bTopPage,dwZone,m_ScMgr.m_strSrc,bEventBeforeNavigate))
			{
				strRet=ScHost.m_strReturnString;
				if(!strRet.IsEmpty())
				{
					//::MessageBox(NULL,strRet,_T("ThinBridge"),MB_SYSTEMMODAL);
					strRet.TrimLeft();
					strRet.TrimRight();
					CAtlArray<CAtlString> arr_ScriptHitResult;
					SBUtil::Split_TrimBlank(arr_ScriptHitResult,strRet,_T(" "));
					for(int i=0;i<imax;i++)
					{
						pRedirectData = (CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
						if(pRedirectData)
						{
							for(int j=0;j<(int)arr_ScriptHitResult.GetCount();j++)
							{
								CString strHitStr;
								strHitStr = arr_ScriptHitResult.GetAt(j);
								if(strHitStr.CompareNoCase(_T("NoCache"))==0)
								{
									continue;
								}
								if(pRedirectData->m_strExecType.CompareNoCase(strHitStr)==0)
								{
									arr_RedirectBrowserHit->Add((intptr_t)pRedirectData);
									bRet = TRUE;
									if(m_gbTraceLog)
									{
										CString strReasonLine;
										strReasonLine.Format(_T("HIT [%s] %s / "),pRedirectData->m_strExecType,_T("ThinBridgeScript"));
										m_strHitReasonAll+=strReasonLine;
									}
									break;
								}
							}
						}
					}
				}
			}
		}
		return bRet;
	}
};

class CResourceCAPData
{
	public:
	CResourceCAPData()
	{
		m_bTabCntCAP=FALSE;
		m_uiTabCntWARM=10;
		m_uiTabCntMAX=15;
		m_strTab_WARM_Msg  =_T("【お願い】\n現在開いているタブ・ウインドウ数が警告値に達しました。\n\nお手数ですが他の不要なタブ・ウインドウを閉じて下さい。\n\nコンピュータ資源の節約に、ご協力ください。");
		m_strTab_MAX_Msg =_T("【お知らせ】\n現在開いているタブ・ウインドウ数が上限値に達しました。\nこれ以上タブ・ウインドウを追加できません。\n\nお手数ですが他の不要なタブ・ウインドウを閉じてから再度実行して下さい。\n\nコンピュータ資源の節約に、ご協力ください。");
		m_uiTab_WARM_ShowTime=5;
		m_uiTab_MAX_ShowTime=5;

		m_bMemUsageCAP=FALSE;
		m_uiMemWARM=600;
		m_uiMemMAX=1024;
		m_strMem_WARM_Msg  =_T("【お願い】\n現在の使用メモリが警告値に達しました。\n\nお手数ですが他の不要なタブ・ウインドウを閉じて下さい。\n\nコンピュータ資源の節約に、ご協力ください。");
		m_strMem_MAX_Msg =_T("【お知らせ】\n現在の使用メモリが上限値に達しました。\n\nお手数ですが他の不要なタブ・ウインドウを閉じてから再度実行して下さい。\n\nコンピュータ資源の節約に、ご協力ください。");
		m_uiMem_WARM_ShowTime=5;
		m_uiMem_MAX_ShowTime=5;

		m_bCPUPriorityCAP=FALSE;
		m_uiCPUPriority=2;
		m_bDataCached=FALSE;
		m_strReadSettingType=_T("(N/A Default)");
	}
	~CResourceCAPData(){}
	void Copy(CResourceCAPData* ptr)
	{
		if(ptr==NULL)return;

		m_bTabCntCAP = ptr->m_bTabCntCAP;
		m_uiTabCntWARM = ptr->m_uiTabCntWARM;
		m_uiTabCntMAX = ptr->m_uiTabCntMAX;
		m_strTab_WARM_Msg = ptr->m_strTab_WARM_Msg;
		m_strTab_MAX_Msg = ptr->m_strTab_MAX_Msg;
		m_uiTab_WARM_ShowTime = ptr->m_uiTab_WARM_ShowTime;
		m_uiTab_MAX_ShowTime = ptr->m_uiTab_MAX_ShowTime;

		m_bMemUsageCAP = ptr->m_bMemUsageCAP;
		m_uiMemWARM = ptr->m_uiMemWARM;
		m_uiMemMAX = ptr->m_uiMemMAX;
		m_strMem_WARM_Msg = ptr->m_strMem_WARM_Msg;
		m_strMem_MAX_Msg = ptr->m_strMem_MAX_Msg;
		m_uiMem_WARM_ShowTime = ptr->m_uiMem_WARM_ShowTime;
		m_uiMem_MAX_ShowTime = ptr->m_uiMem_MAX_ShowTime;

		m_bCPUPriorityCAP = ptr->m_bCPUPriorityCAP;
		m_uiCPUPriority = ptr->m_uiCPUPriority;
		m_bDataCached = ptr->m_bDataCached;
		m_strReadSettingType=ptr->m_strReadSettingType;
	}

	BOOL ReadResourceCAPSettingFromReg()
	{
		TCHAR REG_SETTING_KEY[]=_T("SOFTWARE\\Policies\\ThinBridge\\RCAP");
		BOOL bRet=FALSE;
		DWORD Val=0;
		HKEY  hKey={0};
		LONG lResult=0L;
		DWORD dwType=0;
		DWORD dwSize=0;
		DWORD iSize=0;
		CString strVal;
		TCHAR* pVal=NULL;

		lResult=RegOpenKeyEx(HKEY_CURRENT_USER,REG_SETTING_KEY,0,KEY_READ,&hKey);
		if(lResult == ERROR_SUCCESS)
		{
			bRet=TRUE;
			Val=0;
			lResult=RegQueryValueEx(hKey,_T("EnableIETabLimit"),NULL,&dwType,NULL,&dwSize);
			if(lResult == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey,_T("EnableIETabLimit"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
				if(Val!=0)
					m_bTabCntCAP=TRUE;
				else
					m_bTabCntCAP=FALSE;
			}

			Val=0;
			lResult=RegQueryValueEx(hKey,_T("IETabLimit_WARNING"),NULL,&dwType,NULL,&dwSize);
			if(lResult == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey,_T("IETabLimit_WARNING"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
				int iW=Val;
				if(2<=iW && iW <=99)
					m_uiTabCntWARM=iW;
				else
					m_uiTabCntWARM=10;
			}

			Val=0;
			lResult=RegQueryValueEx(hKey,_T("IETabLimit_MAX"),NULL,&dwType,NULL,&dwSize);
			if(lResult == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey,_T("IETabLimit_MAX"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
				int iW=Val;
				if(3<=iW && iW <=100)
					m_uiTabCntMAX=iW;
				else
					m_uiTabCntMAX=15;
			}
			iSize=0;
			strVal.Empty();
			pVal=NULL;
			RegQueryValueEx(hKey,_T("IETabLimit_WARNING_MSG"),NULL,&dwType,NULL,&iSize);
			if(iSize>0)
			{
				iSize+=1;//+1=null
				pVal = new TCHAR[iSize];
				memset(pVal,0x00,sizeof(TCHAR)*iSize);
				RegQueryValueEx(hKey,_T("IETabLimit_WARNING_MSG"),NULL,&dwType,(LPBYTE)pVal, &iSize);
				strVal = pVal;
				delete [] pVal;
				if(!strVal.IsEmpty())
				{
					strVal.Replace(_T("\r\n"),_T("\n"));
					strVal.Replace(_T("\\n"),_T("\n"));
					m_strTab_WARM_Msg=strVal;
				}
			}

			iSize=0;
			strVal.Empty();
			pVal=NULL;
			RegQueryValueEx(hKey,_T("IETabLimit_MAX_MSG"),NULL,&dwType,NULL,&iSize);
			if(iSize>0)
			{
				iSize+=1;//+1=null
				pVal = new TCHAR[iSize];
				memset(pVal,0x00,sizeof(TCHAR)*iSize);
				RegQueryValueEx(hKey,_T("IETabLimit_MAX_MSG"),NULL,&dwType,(LPBYTE)pVal, &iSize);
				strVal = pVal;
				delete [] pVal;
				if(!strVal.IsEmpty())
				{
					strVal.Replace(_T("\r\n"),_T("\n"));
					strVal.Replace(_T("\\n"),_T("\n"));
					m_strTab_MAX_Msg=strVal;
				}
			}

			Val=0;
			lResult=RegQueryValueEx(hKey,_T("IETabLimit_WARNING_MSG_TIME"),NULL,&dwType,NULL,&dwSize);
			if(lResult == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey,_T("IETabLimit_WARNING_MSG_TIME"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
				int iW=Val;
				if(0<=iW && iW <=60)
					m_uiTab_WARM_ShowTime=iW;
				else
					m_uiTab_WARM_ShowTime=5;
			}

			Val=0;
			lResult=RegQueryValueEx(hKey,_T("IETabLimit_MAX_MSG_TIME"),NULL,&dwType,NULL,&dwSize);
			if(lResult == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey,_T("IETabLimit_MAX_MSG_TIME"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
				int iW=Val;
				if(0<=iW && iW <=60)
					m_uiTab_MAX_ShowTime=iW;
				else
					m_uiTab_MAX_ShowTime=5;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////////
			Val=0;
			lResult=RegQueryValueEx(hKey,_T("EnableIEMemLimit"),NULL,&dwType,NULL,&dwSize);
			if(lResult == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey,_T("EnableIEMemLimit"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
				if(Val!=0)
					m_bMemUsageCAP=TRUE;
				else
					m_bMemUsageCAP=FALSE;
			}

			Val=0;
			lResult=RegQueryValueEx(hKey,_T("IEMemLimit_WARNING"),NULL,&dwType,NULL,&dwSize);
			if(lResult == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey,_T("IEMemLimit_WARNING"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
				int iW=Val;
				if(100<=iW && iW <=1000)
					m_uiMemWARM=iW;
				else
					m_uiMemWARM=600;
			}

			Val=0;
			lResult=RegQueryValueEx(hKey,_T("IEMemLimit_MAX"),NULL,&dwType,NULL,&dwSize);
			if(lResult == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey,_T("IEMemLimit_MAX"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
				int iW=Val;
				if(200<=iW && iW <=2000)
					m_uiMemMAX=iW;
				else
					m_uiMemMAX=1024;
			}

			iSize=0;
			strVal.Empty();
			pVal=NULL;
			RegQueryValueEx(hKey,_T("IEMemLimit_WARNING_MSG"),NULL,&dwType,NULL,&iSize);
			if(iSize>0)
			{
				iSize+=1;//+1=null
				pVal = new TCHAR[iSize];
				memset(pVal,0x00,sizeof(TCHAR)*iSize);
				RegQueryValueEx(hKey,_T("IEMemLimit_WARNING_MSG"),NULL,&dwType,(LPBYTE)pVal, &iSize);
				strVal = pVal;
				delete [] pVal;
				if(!strVal.IsEmpty())
				{
					strVal.Replace(_T("\r\n"),_T("\n"));
					strVal.Replace(_T("\\n"),_T("\n"));
					m_strMem_WARM_Msg=strVal;
				}
			}

			iSize=0;
			strVal.Empty();
			pVal=NULL;
			RegQueryValueEx(hKey,_T("IEMemLimit_MAX_MSG"),NULL,&dwType,NULL,&iSize);
			if(iSize>0)
			{
				iSize+=1;//+1=null
				pVal = new TCHAR[iSize];
				memset(pVal,0x00,sizeof(TCHAR)*iSize);
				RegQueryValueEx(hKey,_T("IEMemLimit_MAX_MSG"),NULL,&dwType,(LPBYTE)pVal, &iSize);
				strVal = pVal;
				delete [] pVal;
				if(!strVal.IsEmpty())
				{
					strVal.Replace(_T("\r\n"),_T("\n"));
					strVal.Replace(_T("\\n"),_T("\n"));
					m_strMem_MAX_Msg=strVal;
				}
			}

			Val=0;
			lResult=RegQueryValueEx(hKey,_T("IEMemLimit_WARNING_MSG_TIME"),NULL,&dwType,NULL,&dwSize);
			if(lResult == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey,_T("IEMemLimit_WARNING_MSG_TIME"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
				int iW=Val;
				if(0<=iW && iW <=60)
					m_uiMem_WARM_ShowTime=iW;
				else
					m_uiMem_WARM_ShowTime=5;
			}

			Val=0;
			lResult=RegQueryValueEx(hKey,_T("IEMemLimit_MAX_MSG_TIME"),NULL,&dwType,NULL,&dwSize);
			if(lResult == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey,_T("IEMemLimit_MAX_MSG_TIME"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
				int iW=Val;
				if(0<=iW && iW <=60)
					m_uiMem_MAX_ShowTime=iW;
				else
					m_uiMem_MAX_ShowTime=5;
			}
			////////////////////////////////////////////////////////////////////////////////////////////////////////
			Val=0;
			lResult=RegQueryValueEx(hKey,_T("EnableIEPriority"),NULL,&dwType,NULL,&dwSize);
			if(lResult == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey,_T("EnableIEPriority"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
				if(Val!=0)
					m_bCPUPriorityCAP=TRUE;
				else
					m_bCPUPriorityCAP=FALSE;
			}

			Val=0;
			lResult=RegQueryValueEx(hKey,_T("IEPriority"),NULL,&dwType,NULL,&dwSize);
			if(lResult == ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey,_T("IEPriority"),NULL,&dwType,(LPBYTE)&Val, &dwSize);
				int iW=Val;
				if(0<=iW && iW <=4)
					m_uiCPUPriority=iW;
				else
					m_uiCPUPriority=2;
			}
			RegCloseKey(hKey);
		}
		return bRet;
	}

	void ReadResourceCAPSetting(LPCTSTR lPath)
	{
		//HKEY_CUの設定を最優先させる。
		//2016-09-30
		if(ReadResourceCAPSettingFromReg())
		{
			m_strReadSettingType=_T("(GPO)");
			return;
		}
		
		if(lPath==NULL)return;
#ifdef _UNICODE
		_wsetlocale(LC_ALL, _T("jpn")); 
#endif
		setlocale( LC_ALL, "Japanese" );  
		CAtlStdioFile in;
		HRESULT		hr={0};
		hr = in.OpenFile(lPath,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
		//hr = in.Create(lPath,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING);
		if(hr==S_OK)
		{
			CString strTemp;
			CString strTempU;
			CString strTemp2;
			CString strTemp3;

			CAtlArray<CAtlString> strArray;
			while(in.ReadLineC(strTemp,CP_ACP))
			{
				strTemp.TrimLeft();
				strTemp.TrimRight();
				strTempU = CString(strTemp);
				strArray.RemoveAll();
				SBUtil::Split(strArray,strTempU,_T("="));
				if(strArray.GetCount() >= 2)
				{
					strTemp2=strArray.GetAt(0);
					strTemp2.TrimLeft();
					strTemp2.TrimRight();

					strTemp3=strArray.GetAt(1);
					strTemp3.TrimLeft();
					strTemp3.TrimRight();

					if(strTemp2.Find(_T(";"))==0)
					{
						;
					}
					else if(strTemp2.Find(_T("#"))==0)
					{
						;
					}
					else
					{
						if(strTemp2.CompareNoCase(_T("EnableIETabLimit"))==0)
						{
							m_strReadSettingType=_T("(INI)");
							if(strTemp3==_T("1"))
								m_bTabCntCAP=TRUE;
							else
								m_bTabCntCAP=FALSE;
						}
						else if(strTemp2.CompareNoCase(_T("IETabLimit_WARNING"))==0)
						{
							int iW=0;
							iW = _ttoi(strTemp3);
							if(2<=iW && iW <=99)
								m_uiTabCntWARM=iW;
							else
								m_uiTabCntWARM=10;
						}
						else if(strTemp2.CompareNoCase(_T("IETabLimit_MAX"))==0)
						{
							int iW=0;
							iW = _ttoi(strTemp3);
							if(3<=iW && iW <=100)
								m_uiTabCntMAX=iW;
							else
								m_uiTabCntMAX=15;
						}
						else if(strTemp2.CompareNoCase(_T("IETabLimit_WARNING_MSG"))==0)
						{
							if(!strTemp3.IsEmpty())
							{
								strTemp3.Replace(_T("\\n"),_T("\n"));
								m_strTab_WARM_Msg=strTemp3;
							}
						}
						else if(strTemp2.CompareNoCase(_T("IETabLimit_MAX_MSG"))==0)
						{
							if(!strTemp3.IsEmpty())
							{
								strTemp3.Replace(_T("\\n"),_T("\n"));
								m_strTab_MAX_Msg=strTemp3;
							}
						}
						else if(strTemp2.CompareNoCase(_T("IETabLimit_WARNING_MSG_TIME"))==0)
						{
							if(!strTemp3.IsEmpty())
							{
								int iW=0;
								iW = _ttoi(strTemp3);
								if(0<=iW && iW <=60)
									m_uiTab_WARM_ShowTime=iW;
								else
									m_uiTab_WARM_ShowTime=5;
							}
						}
						else if(strTemp2.CompareNoCase(_T("IETabLimit_MAX_MSG_TIME"))==0)
						{
							if(!strTemp3.IsEmpty())
							{
								int iW=0;
								iW = _ttoi(strTemp3);
								if(0<=iW && iW <=60)
									m_uiTab_MAX_ShowTime=iW;
								else
									m_uiTab_MAX_ShowTime=5;
							}
						}
						///////////////////////////////////////////////////////////////////////////////////////
						else if(strTemp2.CompareNoCase(_T("EnableIEMemLimit"))==0)
						{
							if(strTemp3==_T("1"))
								m_bMemUsageCAP=TRUE;
							else
								m_bMemUsageCAP=FALSE;
						}
						else if(strTemp2.CompareNoCase(_T("IEMemLimit_WARNING"))==0)
						{
							int iW=0;
							iW = _ttoi(strTemp3);
							if(100<=iW && iW <=1000)
								m_uiMemWARM=iW;
							else
								m_uiMemWARM=600;
						}
						else if(strTemp2.CompareNoCase(_T("IEMemLimit_MAX"))==0)
						{
							int iW=0;
							iW = _ttoi(strTemp3);
							if(200<=iW && iW <=2000)
								m_uiMemMAX=iW;
							else
								m_uiMemMAX=1024;
						}
						else if(strTemp2.CompareNoCase(_T("IEMemLimit_WARNING_MSG"))==0)
						{
							if(!strTemp3.IsEmpty())
							{
								strTemp3.Replace(_T("\\n"),_T("\n"));
								m_strMem_WARM_Msg=strTemp3;
							}
						}
						else if(strTemp2.CompareNoCase(_T("IEMemLimit_MAX_MSG"))==0)
						{
							if(!strTemp3.IsEmpty())
							{
								strTemp3.Replace(_T("\\n"),_T("\n"));
								m_strMem_MAX_Msg=strTemp3;
							}
						}
						else if(strTemp2.CompareNoCase(_T("IEMemLimit_WARNING_MSG_TIME"))==0)
						{
							if(!strTemp3.IsEmpty())
							{
								int iW=0;
								iW = _ttoi(strTemp3);
								if(0<=iW && iW <=60)
									m_uiMem_WARM_ShowTime=iW;
								else
									m_uiMem_WARM_ShowTime=5;
							}
						}
						else if(strTemp2.CompareNoCase(_T("IEMemLimit_MAX_MSG_TIME"))==0)
						{
							if(!strTemp3.IsEmpty())
							{
								int iW=0;
								iW = _ttoi(strTemp3);
								if(0<=iW && iW <=60)
									m_uiMem_MAX_ShowTime=iW;
								else
									m_uiMem_MAX_ShowTime=5;
							}
						}
						///////////////////////////////////////////////////////////////////////////////////////
						else if(strTemp2.CompareNoCase(_T("EnableIEPriority"))==0)
						{
							if(strTemp3==_T("1"))
								m_bCPUPriorityCAP=TRUE;
							else
								m_bCPUPriorityCAP=FALSE;
						}
						else if(strTemp2.CompareNoCase(_T("IEPriority"))==0)
						{
							int iW=0;
							iW = _ttoi(strTemp3);
							if(0<=iW && iW <=4)
								m_uiCPUPriority=iW;
							else
								m_uiCPUPriority=2;
						}
					}
				}
			}
			m_bDataCached=TRUE;
		}
		in.Close();
		setlocale( LC_ALL, "" );
#ifdef _UNICODE
		_wsetlocale(LC_ALL, _T(""));
#endif
	}
	BOOL IsEnableCAP()
	{
		if(m_bTabCntCAP)return TRUE;
		if(m_bMemUsageCAP)return TRUE;
		//if(m_bCPUPriorityCAP)return TRUE;
		return FALSE;
	}
	///////////////////////////////////////////////////
	BOOL m_bTabCntCAP;
	UINT m_uiTabCntWARM;
	UINT m_uiTabCntMAX;
	CString m_strTab_WARM_Msg;
	CString m_strTab_MAX_Msg;
	UINT m_uiTab_WARM_ShowTime;
	UINT m_uiTab_MAX_ShowTime;

	BOOL m_bMemUsageCAP;
	size_t m_uiMemWARM;
	size_t m_uiMemMAX;
	CString m_strMem_WARM_Msg;
	CString m_strMem_MAX_Msg;
	UINT m_uiMem_WARM_ShowTime;
	UINT m_uiMem_MAX_ShowTime;

	BOOL m_bCPUPriorityCAP;
	UINT m_uiCPUPriority;
	BOOL m_bDataCached;
	CString m_strReadSettingType;
	//////////////////////////////////////////////////
};

#pragma once
#include "resource.h"       // ���C�� �V���{��
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

				//���C���h�J�[�h�Ή�
				if(wildcmp(strList_A,strURL_A))
				{
					//Hit�����ꍇ
					bRet = TRUE;
					if(m_gbTraceLog)
					{
						m_strHitReason.Format(_T("�^�[�Q�b�gURL���[���F%s"),strTemp);
					}
					break;
				}
			}
		}
		//���O�̊m�F	
		//���O�Ƀq�b�g�����ꍇ�́AFALSE�ɖ߂��B
		if(bRet==TRUE)
		{
			int imax2 = (int)m_arr_URL_EX.GetCount();
			for(int j=0;j<imax2;j++)
			{
				strTemp.Empty();
				strTemp = m_arr_URL_EX.GetAt(j);
				CStringA strURL_A(pURL);
				CStringA strList_A(strTemp);

				//���C���h�J�[�h�Ή�
				if(wildcmp(strList_A,strURL_A))
				{
					//���O�Ƀq�b�g�����ꍇ�́AFALSE�ɖ߂��B
					//Hit�����ꍇ
					bRet = FALSE;
					if(m_gbTraceLog)
					{
						m_strHitReason.Format(_T("���OURL���[���F%s"),strTemp);
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
		//�L���b�V�����̏ꍇ
		if(iMaxCache >= 64)
		{
			POSITION pos={0};
			pos = m_MapHitURLs.GetStartPosition();
			while (pos)
			{
				//�P���Ɉ�ԌÂ����������B
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
	//���_�C���N�g�Ώۂ�URL��ClassObjects��Map�ɐς�ł�������
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

	//�L���b�V���֘A
	BOOL m_bDataCached;
	DWORD m_dFileSize;
	FILETIME m_FileTime;

	//���_�C���N�g�Ώ�URL��Hit�L���b�V���Ǘ��N���X�I�u�W�F�N�g
	CHitCachedData m_HitURLCache_Manager;

	//���_�C���N�g�ΏۊO��URL��Map�ɐς�ł�������
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
			//�t�@�C���T�C�Y���قȂ�ꍇ
			if(dFileSize != m_dFileSize)
			{
				bRet=TRUE;
			}
			else
			{
				GetFileTime(hFile,NULL,NULL,&FileTime);
				//�X�V�����قȂ�ꍇ
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
				//�󔒂̏ꍇ�́A����
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
						//�����ł́A�����ꍇ(�L���ȂƂ�)
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
					//�����ł́A�����ꍇ(�L���ȂƂ�)
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

		//Data���L���b�V������Ă��Ȃ��ꍇ�́A�����Ń��[�h
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
		//�������ɃL���b�V�����Ă�����̍X�V�`�F�b�N
		else
		{
			//File Size �X�V�����قȂ邩?
			if(IsChangeSettingFile())
			{
				//�L���b�V����j�����ă������ɐςݒ���
				m_bDataCached=FALSE;
				SetArrayData(m_strSettingFilePath);
				m_MapPassThroughURLs.RemoveAll();
				m_HitURLCache_Manager.Clear();

				if(m_bURLOnly)
				{
					SBUtil::Trim_URLOnly(sURL,strURL);
				}
			}
			//�X�V����Ă��Ȃ��A�������ɐς�Ă�����e�̃X���[URL���X�g���`�F�b�N����B
			//Hit�L���b�V�����m�F���A�L���b�V�����Ă���ꍇ�́A�����Ԃ��B
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
				//IE�T�C�g
				if (pRedirectData->m_strExecType == _T("IE"))
				{
					pRedirectData_IE = pRedirectData;
				}
				//Default�T�C�g
				else if (pRedirectData->m_strExecType == _T("Default"))
				{
					pRedirectData_Default = pRedirectData;
				}
				//�j���[�g�����T�C�g
				else if (pRedirectData->m_strExecType == _T(SECTION_DMZ))
				{
					pRedirectData_NeutralSite = pRedirectData;
				}
			}
		}
		//Office365�Ή��A�j���[�g�����T�C�g(���pURL)�𗘗p����
		if (m_bUseNeutralSite)
		{
			//�j���[�g�����T�C�g���n�߂Ƀ`�F�b�N
			//HIT������A���_�C���N�g�����Ȃ��B
			//HIT���Ȃ��ꍇ�́A�j���[�g�����T�C�g���������[���̔�����s���B
			pRedirectData = pRedirectData_NeutralSite;
			if (pRedirectData)
			{
				if (pRedirectData->m_strExecType == _T(SECTION_DMZ))
				{
					//frame�̏ꍇ�ŃI�u�W�F�N�g�̔��������TOP�݂̂̏ꍇ�́A���ցB
					if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
						;
					else
					{
						//���_�C���N�g�𖳌��ɂ��郊�X�g��HIT�����B
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
							//�j���[�g�����T�C�g���[����HIT�����ꍇ�́A���_�C���N�g���Ȃ��B
							return FALSE;
						}
					}
				}
			}
			//IE�T�C�g���`�F�b�N
			//HIT������A���_�C���N�g�����Ȃ��B
			pRedirectData = pRedirectData_IE;
			if (pRedirectData)
			{
				//[IE]
				if (pRedirectData->m_strExecType == _T("IE"))
				{
					//frame�̏ꍇ�ŃI�u�W�F�N�g�̔��������TOP�݂̂̏ꍇ�́A���ցB
					if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
						;
					else
					{
						//���_�C���N�g�𖳌��ɂ��郊�X�g��HIT�����B
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
							//IE�T�C�gHIT�����ꍇ�́A���_�C���N�g���Ȃ��B
							return FALSE;
						}
					}
				}
			}

			//�j���[�g�����T�C�g���������[���𔻒肷��B
			for (int i = 0; i < imax; i++)
			{
				pRedirectData = (CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
				if (pRedirectData)
				{
					//�j���[�g�����T�C�g
					if (pRedirectData->m_strExecType == _T(SECTION_DMZ))
					{
						//�X�L�b�v����B
						continue;
					}

					//IE�T�C�g [IE]
					if (pRedirectData->m_strExecType == _T("IE"))
					{
						//�X�L�b�v����B
						continue;
					}

					//Default�T�C�g [Default]
					if (pRedirectData->m_strExecType == _T("Default"))
					{
						//�X�L�b�v����B
						continue;
					}

					//frame�̏ꍇ�ŃI�u�W�F�N�g�̔��������TOP�݂̂̏ꍇ�́A���ցB
					if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
						continue;
					BOOL bRe = pRedirectData->IsRedirect(strURL);
					CString strReasonLine;
					if (bRe)
					{
						arr_RedirectBrowserHit->Add((intptr_t)pRedirectData);
						bRet = TRUE;
						//���s����B�Ō�܂Ń`�F�b�N�B
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

					//��O�iGoogle Chrome�i�����؂�ւ��j/Chrome Switcher�j�T�C�g�̏ꍇ�́AOffice365�T�C�g���`�F�b�N���Ȃ��B����2�̓Z�b�g�Ƃ݂Ȃ��B
					if(bRet)
					{
						if (pRedirectData->m_strExecType == _T(SECTION_CHROME_SWITCHER))
						{
							//Google Chrome�i�����؂�ւ��j/Chrome Switcher��hit���Ă���̂ŁA����Office365�T�C�g�̓`�F�b�N�s�v
							break;
						}
					}
				}
			}
		}
		//�ȑO�̃o�[�W�����i�ʏ�̏����j
		else
		{
			for(int i=0;i<imax;i++)
			{
				pRedirectData = (CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
				if(pRedirectData)
				{
					//frame�̏ꍇ�ŃI�u�W�F�N�g�̔��������TOP�݂̂̏ꍇ�́A���ցB
					if(bTopPage == FALSE && pRedirectData->m_bTopPageOnly==TRUE)
						continue;
					//IE�T�C�g [IE]
					if (pRedirectData->m_strExecType == _T("IE"))
					{
						//�X�L�b�v����B
						continue;
					}

					//Default�T�C�g [Default]
					if (pRedirectData->m_strExecType == _T("Default"))
					{
						//�X�L�b�v����B
						continue;
					}
					BOOL bRe=pRedirectData->IsRedirect(strURL);
					CString strReasonLine;
					if(bRe)
					{
						arr_RedirectBrowserHit->Add((intptr_t)pRedirectData);
						bRet = TRUE;
						//���s����B�Ō�܂Ń`�F�b�N�B
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
		//HIT�������̂��Ȃ��B
		if(bRet==FALSE)
		{
			//Default�u���E�U�[�փ��_�C���N�g����B
			if (pRedirectData_Default)
			{
				//TopURL�̏ꍇ����
				if (bTopPage == TRUE)
				{
					//��������ݒ肳��Ă���B
					if(!pRedirectData_Default->m_strExecExeFullPath.IsEmpty())
					{
						//IE�ȊO�̏ꍇ�iIE�̏ꍇ�́A�������Ȃ��B�j
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
			//�L���b�V�����̏ꍇ
			else
			{
				POSITION pos={0};
				pos = m_MapPassThroughURLs.GetStartPosition();
				while (pos)
				{
					//�P���Ɉ�ԌÂ����������B
					m_MapPassThroughURLs.RemoveAtPos(pos);
					break;
				}
			}
		}
		//HIT �L���b�V������Ă��Ȃ��A�V�KURL
		else //bRet==TRUE
		{
			m_HitURLCache_Manager.AddData(strURL,m_strHitReasonAll,arr_RedirectBrowserHit);
		}
		return bRet;
	}

	//MS-Edge��IEMode�p
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

		//Data���L���b�V������Ă��Ȃ��ꍇ�́A�����Ń��[�h
		if (!m_bDataCached)
		{
			SetArrayData(m_strSettingFilePath);
			m_MapPassThroughURLs.RemoveAll();
			m_HitURLCache_Manager.Clear();
		}
		//�������ɃL���b�V�����Ă�����̍X�V�`�F�b�N
		else
		{
			//File Size �X�V�����قȂ邩?
			if (IsChangeSettingFile())
			{
				//�L���b�V����j�����ă������ɐςݒ���
				m_bDataCached = FALSE;
				SetArrayData(m_strSettingFilePath);
				m_MapPassThroughURLs.RemoveAll();
				m_HitURLCache_Manager.Clear();
			}
		}
		//�������ɐς�Ă�����e�̃X���[URL���X�g���`�F�b�N����B
		//Hit�L���b�V�����m�F���A�L���b�V�����Ă���ꍇ�́A�����Ԃ��B
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
				//Edge�T�C�g
				if (pRedirectData->m_strExecType == _T("Edge"))
				{
					pRedirectData_Edge = pRedirectData;
				}
				////IE�T�C�g
				//else if (pRedirectData->m_strExecType == _T("IE"))
				//{
				//	pRedirectData_IE = pRedirectData;
				//}
				//Default�T�C�g
				else if (pRedirectData->m_strExecType == _T("Default"))
				{
					pRedirectData_Default = pRedirectData;
				}
				//�j���[�g�����T�C�g
				else if (pRedirectData->m_strExecType == _T(SECTION_DMZ))
				{
					pRedirectData_NeutralSite = pRedirectData;
				}
				//Office365�T�C�g(Custom20)
				else if (pRedirectData->m_strExecType == _T("CUSTOM20"))
				{
					pRedirectData_O365 = pRedirectData;
				}
			}
		}

		//�j���[�g�����T�C�g���n�߂Ƀ`�F�b�N
		//HIT������A���_�C���N�g�����Ȃ��B
		//HIT���Ȃ��ꍇ�́A�j���[�g�����T�C�g���������[���̔�����s���B
		pRedirectData = pRedirectData_NeutralSite;
		if (pRedirectData)
		{
			if (pRedirectData->m_strExecType == _T(SECTION_DMZ))
			{
				//frame�̏ꍇ�ŃI�u�W�F�N�g�̔��������TOP�݂̂̏ꍇ�́A���ցB
				if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
					;
				else
				{
					//���_�C���N�g�𖳌��ɂ��郊�X�g��HIT�����B
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
						//�j���[�g�����T�C�g���[����HIT�����ꍇ�́A���_�C���N�g���Ȃ��B
						return FALSE;
					}
				}
			}
		}

		//IE�T�C�g���`�F�b�N
		//HIT������A���_�C���N�g�����Ȃ��B
		//pRedirectData = pRedirectData_IE;
		//if (pRedirectData)
		//{
		//	//[IE]
		//	if (pRedirectData->m_strExecType == _T("IE"))
		//	{
		//		//frame�̏ꍇ�ŃI�u�W�F�N�g�̔��������TOP�݂̂̏ꍇ�́A���ցB
		//		if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
		//			;
		//		else
		//		{
		//			//���_�C���N�g�𖳌��ɂ��郊�X�g��HIT�����B
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
		//				//IE�T�C�gHIT�����ꍇ�́A���_�C���N�g���Ȃ��B
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
				//frame�̏ꍇ�ŃI�u�W�F�N�g�̔��������TOP�݂̂̏ꍇ�́A���ցB
				if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
					;
				else
				{
					//Edge���X�g��HIT�����B
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
						//���_�C���N�g���Ȃ��B
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
					//frame�̏ꍇ�ŃI�u�W�F�N�g�̔��������TOP�݂̂̏ꍇ�́A���ցB
					if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
						;
					else
					{
						//Edge���X�g��HIT�����B
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
							//���_�C���N�g���Ȃ��B
							return FALSE;
						}
					}
				}
			}
		}

		//�j���[�g�����T�C�g��Edge���������[���𔻒肷��B
		for (int i = 0; i < imax; i++)
		{
			pRedirectData = (CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
			if (pRedirectData)
			{
				//�j���[�g�����T�C�g
				if (pRedirectData->m_strExecType == _T(SECTION_DMZ))
				{
					//�X�L�b�v����B
					continue;
				}

				//Edge[Edge]
				if (pRedirectData->m_strExecType == _T("Edge"))
				{
					//�X�L�b�v����B
					continue;
				}

				//Default�T�C�g [Default]
				if (pRedirectData->m_strExecType == _T("Default"))
				{
					//�X�L�b�v����B
					continue;
				}

				//Edge O365
				if (pRedirectData->m_strExecType == _T("CUSTOM20"))
				{
					if (pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("Edge")) == 0)
					{
						//�X�L�b�v����B
						continue;
					}
				}

				//frame�̏ꍇ�ŃI�u�W�F�N�g�̔��������TOP�݂̂̏ꍇ�́A���ցB
				if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
					continue;
				BOOL bRe = pRedirectData->IsRedirect(strURL);
				CString strReasonLine;
				if (bRe)
				{
					arr_RedirectBrowserHit->Add((intptr_t)pRedirectData);
					bRet = TRUE;
					//���s����B�Ō�܂Ń`�F�b�N�B
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
		//HIT�������̂��Ȃ��B
		if (bRet == FALSE)
		{
			//Default�u���E�U�[�փ��_�C���N�g����B
			if (pRedirectData_Default)
			{
				//TopURL�̏ꍇ����
				if (bTopPage == TRUE)
				{
					//��������ݒ肳��Ă���B
					if (!pRedirectData_Default->m_strExecExeFullPath.IsEmpty())
					{
						//Edge�ȊO�̏ꍇ�iEdge�̏ꍇ�́A�������Ȃ��B�j
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
			//�L���b�V�����̏ꍇ
			else
			{
				POSITION pos = { 0 };
				pos = m_MapPassThroughURLs.GetStartPosition();
				while (pos)
				{
					//�P���Ɉ�ԌÂ����������B
					m_MapPassThroughURLs.RemoveAtPos(pos);
					break;
				}
			}
		}
		//HIT �L���b�V������Ă��Ȃ��A�V�KURL
		else //bRet==TRUE
		{
			m_HitURLCache_Manager.AddData(strURL, m_strHitReasonAll, arr_RedirectBrowserHit);
		}
		return bRet;
	}

	//VirtIE�p(ThinApp Virtual App)
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

		//Data���L���b�V������Ă��Ȃ��ꍇ�́A�����Ń��[�h
		if (!m_bDataCached)
		{
			SetArrayData(m_strSettingFilePath);
			m_MapPassThroughURLs.RemoveAll();
			m_HitURLCache_Manager.Clear();
		}
		//�������ɃL���b�V�����Ă�����̍X�V�`�F�b�N
		else
		{
			//File Size �X�V�����قȂ邩?
			if (IsChangeSettingFile())
			{
				//�L���b�V����j�����ă������ɐςݒ���
				m_bDataCached = FALSE;
				SetArrayData(m_strSettingFilePath);
				m_MapPassThroughURLs.RemoveAll();
				m_HitURLCache_Manager.Clear();
			}
		}
		//�������ɐς�Ă�����e�̃X���[URL���X�g���`�F�b�N����B
		//Hit�L���b�V�����m�F���A�L���b�V�����Ă���ꍇ�́A�����Ԃ��B
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
				//�w��T�C�g
				if (pRedirectData->m_strExecType.CompareNoCase(sLabel)==0)
				{
					pRedirectData_Label = pRedirectData;
				}
				//Default�T�C�g
				else if (pRedirectData->m_strExecType == _T("Default"))
				{
					pRedirectData_Default = pRedirectData;
				}
				//�j���[�g�����T�C�g
				else if (pRedirectData->m_strExecType == _T(SECTION_DMZ))
				{
					pRedirectData_NeutralSite = pRedirectData;
				}
			}
		}

		//�j���[�g�����T�C�g���n�߂Ƀ`�F�b�N
		//HIT������A���_�C���N�g�����Ȃ��B
		//HIT���Ȃ��ꍇ�́A�j���[�g�����T�C�g���������[���̔�����s���B
		pRedirectData = pRedirectData_NeutralSite;
		if (pRedirectData)
		{
			if (pRedirectData->m_strExecType == _T(SECTION_DMZ))
			{
				//frame�̏ꍇ�ŃI�u�W�F�N�g�̔��������TOP�݂̂̏ꍇ�́A���ցB
				if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
					;
				else
				{
					//���_�C���N�g�𖳌��ɂ��郊�X�g��HIT�����B
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
						//�j���[�g�����T�C�g���[����HIT�����ꍇ�́A���_�C���N�g���Ȃ��B
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
				//frame�̏ꍇ�ŃI�u�W�F�N�g�̔��������TOP�݂̂̏ꍇ�́A���ցB
				if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
					;
				else
				{
					//���X�g��HIT�����B
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
						//���_�C���N�g���Ȃ��B
						return FALSE;
					}
				}
			}
		}

		//�j���[�g�����T�C�g��Edge���������[���𔻒肷��B
		for (int i = 0; i < imax; i++)
		{
			pRedirectData = (CURLRedirectDataClass*)m_arr_RedirectBrowser.GetAt(i);
			if (pRedirectData)
			{
				//�j���[�g�����T�C�g
				if (pRedirectData->m_strExecType == _T(SECTION_DMZ))
				{
					//�X�L�b�v����B
					continue;
				}

				//Edge[Edge]
				if (pRedirectData->m_strExecType.CompareNoCase(sLabel)==0)
				{
					//�X�L�b�v����B
					continue;
				}

				//Default�T�C�g [Default]
				if (pRedirectData->m_strExecType == _T("Default"))
				{
					//�X�L�b�v����B
					continue;
				}

				//frame�̏ꍇ�ŃI�u�W�F�N�g�̔��������TOP�݂̂̏ꍇ�́A���ցB
				if (bTopPage == FALSE && pRedirectData->m_bTopPageOnly == TRUE)
					continue;
				BOOL bRe = pRedirectData->IsRedirect(strURL);
				CString strReasonLine;
				if (bRe)
				{
					arr_RedirectBrowserHit->Add((intptr_t)pRedirectData);
					bRet = TRUE;
					//���s����B�Ō�܂Ń`�F�b�N�B
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
		//HIT�������̂��Ȃ��B
		if (bRet == FALSE)
		{
			//Default�u���E�U�[�փ��_�C���N�g����B
			if (pRedirectData_Default)
			{
				//TopURL�̏ꍇ����
				if (bTopPage == TRUE)
				{
					//��������ݒ肳��Ă���B
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
			//�L���b�V�����̏ꍇ
			else
			{
				POSITION pos = { 0 };
				pos = m_MapPassThroughURLs.GetStartPosition();
				while (pos)
				{
					//�P���Ɉ�ԌÂ����������B
					m_MapPassThroughURLs.RemoveAtPos(pos);
					break;
				}
			}
		}
		//HIT �L���b�V������Ă��Ȃ��A�V�KURL
		else //bRet==TRUE
		{
			m_HitURLCache_Manager.AddData(strURL, m_strHitReasonAll, arr_RedirectBrowserHit);
		}
		return bRet;
	}

};


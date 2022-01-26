// BHORedirector.cpp : DLL �G�N�X�|�[�g�̎����ł��B


#include "stdafx.h"
#include "resource.h"
#include "BHORedirector_i.h"
#include "dllmain.h"
#include "BHORedirector.h"
#include "shlobj.h"
#include "shlwapi.h"
#pragma comment(lib, "ShLwApi.Lib")

// DLL �� OLE �ɂ���ăA�����[�h�ł���悤�ɂ��邩�ǂ������w�肵�܂��B
STDAPI DllCanUnloadNow(void){
	return _AtlModule.DllCanUnloadNow();
}
// �v�����ꂽ�^�̃I�u�W�F�N�g���쐬����N���X �t�@�N�g����Ԃ��܂��B
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv){
	return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}
// DllRegisterServer - �G���g�����V�X�e�� ���W�X�g���ɒǉ����܂��B
STDAPI DllRegisterServer(void){
	// �I�u�W�F�N�g�A�^�C�v ���C�u��������у^�C�v ���C�u�������̂��ׂẴC���^�[�t�F�C�X��o�^���܂�
	HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}

// DllUnregisterServer - �G���g�������W�X�g������폜���܂��B
STDAPI DllUnregisterServer(void){
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

// DllInstall - ���[�U�[����уR���s���[�^�[���Ƃ̃V�X�e�� ���W�X�g�� �G���g����ǉ��܂��͍폜���܂��B
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine){
	HRESULT hr = E_FAIL;
	static const wchar_t szUserSwitch[] = L"user";
	if (pszCmdLine != NULL)	{
		if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0){
			ATL::AtlSetPerUserRegistration(true);
		}
	}
	if (bInstall){	
		hr = DllRegisterServer();
		if (FAILED(hr)){
			DllUnregisterServer();
		}
	}
	else{
		hr = DllUnregisterServer();
	}
	return hr;
}

// BHORedirector.cpp : CBHORedirector �̎���
// CBHORedirector

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CBHORedirector::SetSite(IUnknown* pUnkSite)
{
	CString logmsg;
    if (pUnkSite != NULL)
    {
		m_IE_M_Ver=SBUtil::GetTridentVersion();
		if (SBUtil::InThinApp())
		{
			m_bInVOS = TRUE;
			m_strVirtIELabel= SBUtil::GetVirtIELabelStr();
			logmsg.Format(_T("InThinAppVOS:[%s]"), m_strVirtIELabel);
			this->WriteDebugTraceDateTime(logmsg, DEBUG_LOG_TYPE_GE, TRUE);
		}
		else
		{
			m_bInVOS = FALSE;
		}

		logmsg.Format(_T("SetSite [0x%08x] Start"),pUnkSite);
		this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_GE,TRUE);

		//m_strUserDataPath
		TCHAR lpszPath[MAX_PATH*2] = _T("");
		LPITEMIDLIST pidl={0};
		if(::SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &pidl) == NOERROR)
		{
			::SHGetPathFromIDList(pidl, lpszPath);
			::CoTaskMemFree(pidl);
		}
		m_strUserDataPath=lpszPath;
		m_strUserDataPath+=_T("\\ThinBridge\\");

		//2021-02-26 MS-Edge��IEMode����p
		m_bInEdgeIEMode=SBUtil::InEdgeIEMode();
		if(m_bInEdgeIEMode)
		{
			logmsg = _T("EdgeIEMode:TRUE");
			this->WriteDebugTraceDateTime(logmsg, DEBUG_LOG_TYPE_GE, TRUE);
		}

		m_strThinBridge_EXE_FullPath=_AtlModule.m_gSZFullPath;
		m_strThinBridge_EXE_FullPath += _T("ThinBridge.exe");


		m_strRedirectFilePath=_AtlModule.m_gSZFullPath;
		m_strRedirectFilePath+=_T("ThinBridgeBHO.ini");

		CString strUserThinBridgeBHOINIFile;
		strUserThinBridgeBHOINIFile = m_strUserDataPath;
		strUserThinBridgeBHOINIFile += _T("ThinBridgeBHO.ini");
		//���[�U�[�ݒ��D�悳����B
		if(PathFileExists(strUserThinBridgeBHOINIFile))
		{
			m_strRedirectFilePath=strUserThinBridgeBHOINIFile;
		}
		logmsg.Format(_T("ThinBridgeBHO.ini [%s]"),m_strRedirectFilePath);
		this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_EX,TRUE);

		logmsg=_T("SetSite Stage:01");
		this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_GE),TRUE;

		//Delay load
		m_RedirectList.SetArrayDataDelayLoad(m_strRedirectFilePath);
		logmsg=_T("SetSite Stage:02");
		this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_GE,TRUE);

		try
		{
			// IWebBrowser2 �ւ̃|�C���^���L���b�V�����܂��B
			HRESULT hr = pUnkSite->QueryInterface(IID_IWebBrowser2, (void **)&m_spWebBrowser);
			if (SUCCEEDED(hr)&&m_spWebBrowser)
			{
				logmsg=_T("SetSite Stage:03");
				this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_GE,TRUE);

				// DWebBrowserEvents2 ����̃C�x���g���V���N�ɓo�^���܂��B
				hr = DispEventAdvise(m_spWebBrowser);
				if (SUCCEEDED(hr))
				{
					m_fAdvised = TRUE;
				}
			}
			//m_hEventLog = CreateEvent(NULL, FALSE, TRUE, _T("TB_EventCAPChkg"));
			logmsg=_T("SetSite Stage:04 End");
			this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_GE,TRUE);
		}
		catch (...)
		{
			;
		}
    }
	else
    {
		try
		{
			logmsg=_T("UnSetSite");
			this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_GE,TRUE);

			if(m_spWebBrowser)
			{
				logmsg.Format(_T("UnSetSite_WebBrowser[0x%08x]"),m_spWebBrowser);
				this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_GE,TRUE);

				// �C�x���g �V���N�̓o�^�𖕏����܂��B
				if (m_fAdvised)
				{
					DispEventUnadvise(m_spWebBrowser);
					m_fAdvised = FALSE;
				}
				// �����ŁA�L���b�V�������|�C���^�Ȃǂ̃��\�[�X��������܂��B
				m_spWebBrowser.Release();
				m_spWebBrowser=NULL;
			}
		}
		catch (...)
		{
			;
		}
    }
    // ��{�N���X�̎������Ăяo���܂��B
    return IObjectWithSiteImpl<CBHORedirector>::SetSite(pUnkSite);
}
bool CBHORedirector::IsPageIWebBrowser(IDispatch* pDisp)
{//Knowledge Base Q180366
	if(m_spWebBrowser == NULL)return false;

	CComPtr<IDispatch> spDispatch;
	//IDispatchPtr spDispatch;
	HRESULT hr = m_spWebBrowser->QueryInterface(IID_IDispatch, (void**)&spDispatch);
	if (FAILED(hr))
		return false;
	if (pDisp == spDispatch)// Top-level Window object
		return true;
	else
		return false;
}

void STDMETHODCALLTYPE CBHORedirector::BeforeNavigate(LPDISPATCH pDisp, VARIANT* URL,
		VARIANT* Flags, VARIANT* TargetFrameName,
		VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	if (!m_spWebBrowser)
		return;

	CString strURL = V_BSTR(URL);
	if (!SBUtil::IsURL_HTTP(strURL))
		return;

	CString logmsg;
	CString strMsg;
	DWORD dRet=0;

	BOOL bTopPage=FALSE;
	//TOP�y�[�W(Frame�Ȃ�)
	if(IsPageIWebBrowser(pDisp))
	{
		bTopPage=TRUE;
		m_strTopURL=strURL;
		//top�݂̂�Quick���_�C���N�g�̑ΏۂȂ̂ŏ�����FALSE����B
		m_bQuickRedirectExecFlg = FALSE;
	}

	//ESC��������Ă���ꍇ�́A���_�C���N�g�����Ȃ��B
	if(::GetKeyState(VK_ESCAPE)<0)
		return;

	if (m_gbTraceLog)
	{
		CString strTargetFrameName = V_BSTR(TargetFrameName);

		if (strTargetFrameName.IsEmpty())
			strTargetFrameName = _T("N/A");
		else
		{
			//google ad�ł��̂���������(js)�����Ă����@�̈������̂�����̂ŁA�ő�100�����Ő؂�B
			if (strTargetFrameName.GetLength() > 100)
			{
				strTargetFrameName = strTargetFrameName.Mid(0, 100);
				strTargetFrameName += _T("..DIV..");
			}
		}

		logmsg.Format(_T("BeforeNavigate\t%s\tTopPage=%s\tFrameName=%s"), strURL, bTopPage ? _T("Yes") : _T("Frame/iFrame"), strTargetFrameName);
		this->WriteDebugTraceDateTime(logmsg, DEBUG_LOG_TYPE_URL);
	}

	//Top���L���̏ꍇ
	if (m_RedirectList.m_bTopURLOnly)
	{
		//�y�[�W��Top�ł͂Ȃ��ꍇ�́A���_�C���N�g�̕K�v�͂Ȃ��B
		if(!bTopPage)
			return;
	}

	//Hit����Object�R���N�V�����i�[
	CAtlArray<intptr_t> arr_RedirectBrowserHit;
	CURLRedirectDataClass* pRedirectData=NULL;

	BOOL bResultRedirectURL=FALSE;
	//Edge IEMode�̒��̏ꍇ
	if(m_bInEdgeIEMode)
	{
		bResultRedirectURL = m_RedirectList.IsRedirectIEMode(strURL, bTopPage, &arr_RedirectBrowserHit);
	}
	else
	{
		//ThinApp VirtIE�p(ThinApp Virtual App)
		if(m_bInVOS)
		{
			//ThinBridge�Ƃ��Ĕ��f������[CUSTOM01-05]���Ȃ��ꍇ�́AVOS���烊�_�C���N�g�͂��Ȃ��B
			if(m_strVirtIELabel.IsEmpty())
				return;
			bResultRedirectURL = m_RedirectList.IsRedirectVOS(strURL, bTopPage, &arr_RedirectBrowserHit, m_strVirtIELabel);
		}
		else//�ʏ��
		{
			bResultRedirectURL = m_RedirectList.IsRedirect(strURL,bTopPage,&arr_RedirectBrowserHit);
		}
	}

	//���茋�� ���_�C���N�g����B
	if(bResultRedirectURL)
	{
		int iColMax=(int)arr_RedirectBrowserHit.GetCount();
		if(iColMax < 1)
			return;

		//���[�v�h�~
		if(!m_strSafeGuardURL.IsEmpty() && m_strSafeGuardURL==m_strTopURL)
		{
			logmsg.Format(_T("LoopBlocker\t%s"),m_strSafeGuardURL);
			this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_URL);
			return;
		}

		if(!IsSafeGuard(strURL))
		{
			BOOL bJP=TRUE;
			try
			{
				TCHAR szLocalNameBuf[LOCALE_NAME_MAX_LENGTH*2]={0};
				::GetUserDefaultLocaleName(szLocalNameBuf,LOCALE_NAME_MAX_LENGTH);
				TCHAR szNameBuf[3]={0};
				lstrcpyn(szNameBuf,szLocalNameBuf,3);
				CString strLang;
				strLang=szNameBuf;
				strLang.MakeLower();
				if(strLang.Find(_T("ja"))==0)
					bJP=TRUE;
				else
					bJP=FALSE;
			}
			catch (...)
			{
				;
			}
			m_strSafeGuardURL=m_strTopURL;
			CString strSafeGuardMsg;
			strSafeGuardMsg.Format(_T("%s\n%s"),_T("���x����\n�Z���ԂɘA�������u���E�U�[ ���_�C���N�g�����o���܂����B\n\n�u���E�U�[ ���_�C���N�g�������ꎞ�I�ɒ��f���܂����H"),m_strSafeGuardURL);
			if(bJP==FALSE)
				strSafeGuardMsg.Format(_T("%s\n%s"),_T("*WARNING*\nThinBridge detected successive browser redirects in a short time.\n\nWould you like to temporarily suspend browser redirect processing?"),m_strSafeGuardURL);

			if(IDYES!=::MessageBox(NULL,strSafeGuardMsg,_T("ThinBridge Browser Helper"),MB_YESNO|MB_ICONQUESTION))
			{
				m_strSafeGuardURL.Empty();
				m_strSafeGuard_chkURL.Empty();
			}
			return;
		}
		else
		{
			m_strSafeGuardURL.Empty();
		}

		logmsg.Format(_T("#ThinBridge_Redirect(%d)\t%s"),iColMax,m_RedirectList.m_strHitReasonAll);
		this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_DE);

		//TOP�y�[�W(Frame�Ȃ�)
		if(bTopPage)
		{
			//�n�߂�Hit���������g���B
			pRedirectData=(CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(0);
			if(pRedirectData)
			{
				*Cancel = TRUE;
				if (m_RedirectList.m_bQuickRedirect == FALSE)
				{
					this->Navigate_RedirectHTM(m_spWebBrowser,
						pRedirectData->m_strExecType, pRedirectData->m_dRedirectPageAction,
						pRedirectData->m_dwCloseTimeout);
				}
				else//QuickRedirect
				{
					m_bQuickRedirectExecFlg = TRUE;
					if (IsEmptyPage())
					{
						if (m_gbTraceLog)
						{
							logmsg.Format(_T("BeforeNavigate:CloseEmptyTabWindow"));
							this->WriteDebugTraceDateTime(logmsg, DEBUG_LOG_TYPE_URL);
						}
						//m_spWebBrowser->Quit();
						CloseTabWindow(pDisp);
					}
				}

				for(int i=0;i<iColMax;i++)
				{
					pRedirectData=(CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(i);
					if(pRedirectData)
					{
						this->OpenAnotherBrowser(pRedirectData, strURL);
						::Sleep(100);
					}
				}
				return;
			}
		}
		else
		{
			//Frame�̏ꍇ�A���t���[��,iFrame�܂�
			if(pDisp)
			{
				CComPtr<IWebBrowser2>  spWebBrowserFrame;
				HRESULT hr = pDisp->QueryInterface(IID_IWebBrowser2, (void **)&spWebBrowserFrame);
				if (SUCCEEDED(hr))
				{
					if(spWebBrowserFrame)
					{
						*Cancel = TRUE;
						//�n�߂�Hit���������g���B
						pRedirectData=(CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(0);
						if(pRedirectData)
						{
							*Cancel = TRUE;
							if (m_RedirectList.m_bQuickRedirect == FALSE)
							{
								this->Navigate_RedirectHTM(spWebBrowserFrame,
									pRedirectData->m_strExecType, pRedirectData->m_dRedirectPageAction,
									pRedirectData->m_dwCloseTimeout);
							}
							for(int i=0;i<iColMax;i++)
							{
								pRedirectData=(CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(i);
								if(pRedirectData)
								{
									this->OpenAnotherBrowser(pRedirectData, strURL);
									::Sleep(100);
								}
							}
							return;
						}
						return;
					}
				}
				return;
			}
		}
	}
	return;
}

void STDMETHODCALLTYPE CBHORedirector::NavigateComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	if (!m_spWebBrowser)
		return;

	CString strURL = V_BSTR(URL);
	if (!SBUtil::IsURL_HTTP(strURL))
		return;

	CString logmsg;
	if (m_gbTraceLog)
	{
		logmsg.Format(_T("NavigateComplete\t%s"),strURL);
		this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_URL);
	}

	CString strMsg;
	DWORD dRet=0;

	CString strExecPath;
	BOOL bTopPage=FALSE;

	//TOP�y�[�W(Frame�Ȃ�)
	if(IsPageIWebBrowser(pDisp))
	{
		bTopPage=TRUE;
		m_strTopURL=strURL;
	}

	//Quick���_�C���N�g����Ă���ꍇ�́A���̌�̏����͍s��Ȃ��B
	if (m_RedirectList.m_bQuickRedirect)
	{
		if (bTopPage)
		{
			//����BeforeNavigate�Ń��_�C���N�g�ς�
			if (m_bQuickRedirectExecFlg)
				return;
		}
	}

	//Top���L���̏ꍇ
	if (m_RedirectList.m_bTopURLOnly)
	{
		//�y�[�W��Top�ł͂Ȃ��ꍇ�́A���_�C���N�g�̕K�v�͂Ȃ��B
		if (!bTopPage)
			return;
	}

	//ESC��������Ă���ꍇ�́A���_�C���N�g�����Ȃ��B
	if(::GetKeyState(VK_ESCAPE)<0)
		return;

	//Hit����Object�R���N�V�����i�[
	CAtlArray<intptr_t> arr_RedirectBrowserHit;
	CURLRedirectDataClass* pRedirectData=NULL;

	BOOL bResultRedirectURL=FALSE;

	//Edge IEMode�̒��̏ꍇ
	if (m_bInEdgeIEMode)
	{
		bResultRedirectURL = m_RedirectList.IsRedirectIEMode(strURL, bTopPage,&arr_RedirectBrowserHit);
	}
	else
	{
		//ThinApp VirtIE�p(ThinApp Virtual App)
		if (m_bInVOS)
		{
			//ThinBridge�Ƃ��Ĕ��f������[CUSTOM01-05]���Ȃ��ꍇ�́AVOS���烊�_�C���N�g�͂��Ȃ��B
			if (m_strVirtIELabel.IsEmpty())
				return;
			bResultRedirectURL = m_RedirectList.IsRedirectVOS(strURL, bTopPage, &arr_RedirectBrowserHit, m_strVirtIELabel);
		}
		else//�ʏ��
		{
			bResultRedirectURL = m_RedirectList.IsRedirect(strURL, bTopPage, &arr_RedirectBrowserHit);
		}
	}



	//���茋�� ���_�C���N�g����B
	if(bResultRedirectURL)
	{
		int iColMax=(int)arr_RedirectBrowserHit.GetCount();
		if(iColMax < 1)
			return;

		//���[�v�h�~
		if(!m_strSafeGuardURL.IsEmpty() && m_strSafeGuardURL==m_strTopURL)
		{
			logmsg.Format(_T("LoopBlocker\t%s"),m_strSafeGuardURL);
			this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_URL);
			return;
		}
		if(!IsSafeGuard(strURL))
		{
			m_strSafeGuardURL=m_strTopURL;
			CString strSafeGuardMsg;
			strSafeGuardMsg.Format(_T("%s\n%s"),_T("���x����\n�Z���ԂɘA�������u���E�U�[ ���_�C���N�g�����o���܂����B\n\n�u���E�U�[ ���_�C���N�g�������ꎞ�I�ɒ��f���܂����H"),m_strSafeGuardURL);
			if(IDYES!=::MessageBox(NULL,strSafeGuardMsg,_T("ThinBridge Browser Helper"),MB_YESNO|MB_ICONQUESTION))
			{
				m_strSafeGuardURL.Empty();
				m_strSafeGuard_chkURL.Empty();
			}
			return;
		}
		else
		{
			m_strSafeGuardURL.Empty();
		}

		logmsg.Format(_T("#ThinBridge_Redirect(%d)\t%s"),iColMax,m_RedirectList.m_strHitReasonAll);
		this->WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_DE);

		//TOP�y�[�W(Frame�Ȃ�)
		if(bTopPage)
		{
			//�n�߂�Hit���������g���B
			pRedirectData=(CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(0);
			if(pRedirectData)
			{
				this->Navigate_RedirectHTM(m_spWebBrowser,
						pRedirectData->m_strExecType,pRedirectData->m_dRedirectPageAction,
						pRedirectData->m_dwCloseTimeout);
				for(int i=0;i<iColMax;i++)
				{
					pRedirectData=(CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(i);
					if(pRedirectData)
					{
						this->OpenAnotherBrowser(pRedirectData, strURL);
						::Sleep(100);
					}
				}
				return;
			}
		}
		else
		{
			//Frame�̏ꍇ�A���t���[��,iFrame�܂�
			if(pDisp)
			{
				CComPtr<IWebBrowser2>  spWebBrowserFrame;
				HRESULT hr = pDisp->QueryInterface(IID_IWebBrowser2, (void **)&spWebBrowserFrame);
				if (SUCCEEDED(hr))
				{
					if(spWebBrowserFrame)
					{
						//�n�߂�Hit���������g���B
						pRedirectData=(CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(0);
						if(pRedirectData)
						{
							this->Navigate_RedirectHTM(spWebBrowserFrame,
									pRedirectData->m_strExecType,pRedirectData->m_dRedirectPageAction,
									pRedirectData->m_dwCloseTimeout);
							for(int i=0;i<iColMax;i++)
							{
								pRedirectData=(CURLRedirectDataClass*)arr_RedirectBrowserHit.GetAt(i);
								if(pRedirectData)
								{
									this->OpenAnotherBrowser(pRedirectData, strURL);
									::Sleep(100);
								}
							}
							return;
						}
						return;
					}
				}
				return;
			}
		}
	}
	return;
}

//void STDMETHODCALLTYPE CBHORedirector::DocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
//{
//	if (!m_spWebBrowser)
//		return;
//
//	CString strURL = V_BSTR(URL);
//	if (!SBUtil::IsURL_HTTP(strURL))
//		return;
//
//	CString logmsg;
//	if (m_gbTraceLog)
//	{
//		logmsg.Format(_T("DocumentComplete\t%s"), strURL);
//		this->WriteDebugTraceDateTime(logmsg, DEBUG_LOG_TYPE_URL);
//	}
//
//	CString strMsg;
//	DWORD dRet = 0;
//	BOOL bTopPage = FALSE;
//
//	//TOP�y�[�W(Frame�Ȃ�)
//	if (IsPageIWebBrowser(pDisp))
//	{
//		bTopPage = TRUE;
//	}
//	//Quick���_�C���N�g����Ă���ꍇ�́A���̌�̏����͍s��Ȃ��B
//	if (m_RedirectList.m_bQuickRedirect)
//	{
//		if (bTopPage)
//		{
//			//����BeforeNavigate�Ń��_�C���N�g�ς�
//			if (m_bQuickRedirectExecFlg)
//			{
//				//ESC��������Ă���ꍇ�́A���_�C���N�g�����Ȃ��B
//				if (::GetKeyState(VK_ESCAPE) < 0)
//					return;
//				if(strURL.IsEmpty() && IsEmptyPage())
//				{
//					if (m_gbTraceLog)
//					{
//						logmsg.Format(_T("DocumentComplete:CloseEmptyTabWindow"));
//						this->WriteDebugTraceDateTime(logmsg, DEBUG_LOG_TYPE_URL);
//					}
//					CloseTabWindow(pDisp);
//				}
//			}
//		}
//	}
//	return;
//}

BOOL CBHORedirector::IsEmptyPage()
{
	if (!m_spWebBrowser)
		return FALSE;

	CComPtr<IDispatch> pDispDocument;
	m_spWebBrowser->get_Document(&pDispDocument);
	CComQIPtr<IHTMLDocument2> spHTMLDocument2 = pDispDocument;
	HRESULT	hr = { 0 };
	if (spHTMLDocument2)
	{
		CComBSTR	bstrURL;
		CString	strURL;
		hr = spHTMLDocument2->get_URL(&bstrURL);
		if (SUCCEEDED(hr))
		{
			strURL = bstrURL;
		}
		if(strURL.IsEmpty())
		{
			if (m_gbTraceLog)
				this->WriteDebugTraceDateTime(_T("IsEmptyPage:URLEmpty"), DEBUG_LOG_TYPE_URL);
			return TRUE;
		}

		//�i�r�Q�[�V�����L�����Z���n�́A������
		if (strURL.Find(_T("res://")) >= 0)
		{
			if (strURL.Find(_T("/navcancl")) >= 0)
			{
				if (m_gbTraceLog)
					this->WriteDebugTraceDateTime(_T("IsEmptyPage:res:// /navcancel"), DEBUG_LOG_TYPE_URL);
				return TRUE;
			}
		}
		CComPtr<IHTMLElementCollection> lpAllElements = NULL;
		hr = spHTMLDocument2->get_all(&lpAllElements);
		if (SUCCEEDED(hr) && lpAllElements != NULL)
		{
			long nCnt = 0L;
			hr = lpAllElements->get_length(&nCnt);
			if (SUCCEEDED(hr))
			{
				//10�ȉ��͕�����ɂ���BGoogle���������Jump���6��
				if (nCnt <= 10)
				{
					if (m_gbTraceLog)
						this->WriteDebugTraceDateTime(_T("IsEmptyPage:HTMLElements<=10"), DEBUG_LOG_TYPE_URL);
					return TRUE;
				}
			}
		}
	}
	else
	{
		//HTML Document�������ꍇ�́A���S�ɋ�(about:blank�ł��疳��)
		if (m_gbTraceLog)
			this->WriteDebugTraceDateTime(_T("IsEmptyPage:HTMLDocument2 NULL"), DEBUG_LOG_TYPE_URL);
		return TRUE;
	}
	return FALSE;
}


void CBHORedirector::CloseTabWindow(LPDISPATCH pDisp)
{
	if(m_spWebBrowser == NULL)return;

	CComPtr<IDispatch> spDispatch;
	HRESULT hr = m_spWebBrowser->QueryInterface(IID_IDispatch, (void**)&spDispatch);
	if (FAILED(hr))
		return;
	if (pDisp == spDispatch)
	{
		CloseTabWindow();
	}
	return;
}
BOOL CBHORedirector::IsClosedTabWindow()
{
	if(m_spWebBrowser == NULL)return TRUE;

	intptr_t iVal=0;
	HWND hwnd={0};
	HRESULT hr = m_spWebBrowser->get_HWND((LONG_PTR*)&hwnd);
	if(SBUtil::SafeWnd(hwnd))
	{
		if(m_MapCloseWebBrowserPtrs.Lookup((intptr_t)hwnd,iVal))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	return FALSE;
}
void CBHORedirector::CloseTabWindow()
{
	__try
	{
		if(m_spWebBrowser == NULL)return;
		HWND hwnd={0};
		intptr_t iVal=0;
		HRESULT hr = m_spWebBrowser->get_HWND((LONG_PTR*)&hwnd);
		if(SBUtil::SafeWnd(hwnd))
		{
			if(m_MapCloseWebBrowserPtrs.Lookup((intptr_t)hwnd,iVal))
			{
				//����Close���������s���Ă���B2�d��Close���邱�Ƃ��������B��O�ŗ�����\��������̂ŁB
				;
			}
			else
			{
				m_MapCloseWebBrowserPtrs.SetAt((intptr_t)hwnd,(intptr_t)hwnd);
				//if(!(m_IE_M_Ver==8 && m_bInVOS==TRUE))
				//	m_spWebBrowser->Stop();
				m_spWebBrowser->ExecWB(OLECMDID_CLOSE, OLECMDEXECOPT_DONTPROMPTUSER, 0, 0);
				//m_spWebBrowser->put_Visible(VARIANT_FALSE);
				//m_spWebBrowser->Quit();
				if (m_gbTraceLog)
				{
					this->WriteDebugTraceDateTime(_T("CloseTabWindow"), DEBUG_LOG_TYPE_URL);
				}
			}
		}
		else
		{
			if(hwnd)
			{
				if(m_MapCloseWebBrowserPtrs.Lookup((intptr_t)hwnd,iVal))
				{
					m_MapCloseWebBrowserPtrs.RemoveKey((intptr_t)hwnd);
				}
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		;
	}
}

void CBHORedirector::WriteDebugTraceDateTime(LPCTSTR msg,int iLogType,BOOL bForce/*=FALSE*/)
{
	if(!bForce)
	{
		if(!m_gbTraceLog)return;
	}
	if(msg==NULL)return;
	if(_tcslen(msg)==0)return;
	CString strWriteLine;
	CTime time = CTime::GetCurrentTime();
	strWriteLine.Format(_T("ThinBridge:BHO\t%s\t%s\t%s\n"),time.Format(_T("%Y-%m-%d %H:%M:%S")),sDEBUG_LOG_TYPE[iLogType],msg);
	if(strWriteLine.GetLength()>4000)
	{
		strWriteLine = strWriteLine.Mid(0,4000);
		strWriteLine+=_T("..DIV..");
	}
	_wsetlocale(LC_ALL, _T("jpn")); 
	OutputDebugString(strWriteLine);
	//BOOL bFileWriteFlg=FALSE;
	//switch(m_iAdvancedLogLevel)
	//{
	//	//�S�Ẵ��O���o��
	//	case DEBUG_LOG_LEVEL_OUTPUT_ALL:
	//	{
	//		bFileWriteFlg=TRUE;
	//		break;
	//	}
	//	//�t�@�C���������ݖ����B
	//	case DEBUG_LOG_LEVEL_OUTPUT_NO_FILE:
	//	{
	//		bFileWriteFlg=FALSE;
	//		break;
	//	}
	//	//URL�֘A�̂�
	//	case DEBUG_LOG_LEVEL_OUTPUT_URL:
	//	{
	//		//���O�̎�ނ���ʁAURL�̏ꍇ
	//		if(iLogType==DEBUG_LOG_TYPE_GE || iLogType==DEBUG_LOG_TYPE_URL)
	//		{
	//			bFileWriteFlg=TRUE;
	//		}
	//		break;
	//	}
	//	default:
	//		break;
	//}
	//if(bFileWriteFlg)
	//{
	//	FILE *fp=NULL;
	//	if(_wfopen_s(&fp,m_strLogFileFullPath,_T("a"))==0)
	//	{
	//		fputws(strWriteLine,fp);
	//		fclose(fp);
	//	}
	//}
	_wsetlocale(LC_ALL, _T(""));
}

void CBHORedirector::OpenAnotherBrowser(CURLRedirectDataClass* pRedirectData,CString strURL)
{
	if(pRedirectData==NULL)return;

	if(SBUtil::IsURL_HTTP(strURL))
	{
		CString strExecTypeUpper;
		strExecTypeUpper=pRedirectData->m_strExecType;
		strExecTypeUpper.MakeUpper();
		CString strCommand;
		CString strCommand2;
		CString logmsg;

		if(strExecTypeUpper.CompareNoCase(_T("RDP"))==0
		||strExecTypeUpper.CompareNoCase(_T("VMWARE"))==0
		||strExecTypeUpper.CompareNoCase(_T("CITRIX"))==0
		||strExecTypeUpper.CompareNoCase(_T("FIREFOX"))==0
		||strExecTypeUpper.CompareNoCase(_T("CHROME"))==0
		||strExecTypeUpper.CompareNoCase(_T("EDGE"))==0
		||strExecTypeUpper.CompareNoCase(_T("IE"))==0)
		{
			strCommand.Format(_T("\"%s\" \"%s\" \"%s\""),m_strThinBridge_EXE_FullPath,strURL,pRedirectData->m_strExecType);
			strCommand2.Format(_T("%s \"%s\""),strURL,pRedirectData->m_strExecType);
		}
		//Default
		else if (strExecTypeUpper.CompareNoCase(_T("Default")) == 0)
		{
			if (pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("RDP")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("VMWARE")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("CITRIX")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("FIREFOX")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("CHROME")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("IE")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("EDGE")) == 0)
			{
				strCommand.Format(_T("\"%s\" \"%s\" \"%s\""), m_strThinBridge_EXE_FullPath, strURL, pRedirectData->m_strExecExeFullPath);
				strCommand2.Format(_T("%s \"%s\""), strURL, pRedirectData->m_strExecExeFullPath);
			}
			else
			{
				strCommand.Format(_T("\"%s\" \"%s\" \"%s\""), m_strThinBridge_EXE_FullPath, strURL, pRedirectData->m_strExecExeFullPath);
				strCommand2.Format(_T("%s \"%s\""), strURL, pRedirectData->m_strExecExeFullPath);
			}
		}
		//Office365�Ή�
		else if (strExecTypeUpper.CompareNoCase(_T(SECTION_O365)) == 0)
		{
			if (pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("RDP")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("VMWARE")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("CITRIX")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("FIREFOX")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("CHROME")) == 0
				|| pRedirectData->m_strExecExeFullPath.CompareNoCase(_T("EDGE")) == 0)
			{
				strCommand.Format(_T("\"%s\" \"%s\" \"%s\""), m_strThinBridge_EXE_FullPath, strURL, pRedirectData->m_strExecExeFullPath);
				strCommand2.Format(_T("%s \"%s\""), strURL, pRedirectData->m_strExecExeFullPath);
			}
			else
			{
				strCommand.Format(_T("\"%s\" \"%s\" \"%s\""), m_strThinBridge_EXE_FullPath, strURL, pRedirectData->m_strExecExeFullPath);
				strCommand2.Format(_T("%s \"%s\""), strURL, pRedirectData->m_strExecExeFullPath);
			}
		}
		//Custom�n�̏ꍇ�́Am_strExecExeFullPath
		else if(strExecTypeUpper.Find(_T("CUSTOM"))==0)
		{
			strCommand.Format(_T("\"%s\" \"%s\" \"%s\""),m_strThinBridge_EXE_FullPath,strURL,pRedirectData->m_strExecExeFullPath);
			strCommand2.Format(_T("%s \"%s\""),strURL,pRedirectData->m_strExecExeFullPath);
		}
		else
		{
			strCommand.Format(_T("\"%s\" \"%s\" \"%s\""),m_strThinBridge_EXE_FullPath,strURL,pRedirectData->m_strExecType);
			strCommand2.Format(_T("%s \"%s\""),strURL,pRedirectData->m_strExecType);
		}


		logmsg.Format(_T("#ExecuteThinBridge\t%s"),strCommand);
		WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_DE);

		//::ShellExecute(NULL,NULL,strCommand,NULL,NULL, SW_SHOW);
		STARTUPINFO si={0};
		PROCESS_INFORMATION pi={0};
		si.cb = sizeof( si );
		unsigned long ecode = 0;
		::Sleep(100);
		if(!CreateProcess( NULL,(LPTSTR)(LPCTSTR)strCommand,NULL, NULL, TRUE, 0, NULL,NULL, &si, &pi ))
		{
			::Sleep(100);
			logmsg.Format(_T("ExecuteThinBridge:CreateProcess Retry1 0x%08x"),::GetLastError());
			WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_DE);
			SetLastError(NO_ERROR);
			//Retry
			if(!CreateProcess( m_strThinBridge_EXE_FullPath,(LPTSTR)(LPCTSTR)strCommand2,NULL, NULL, TRUE, 0, NULL,NULL, &si, &pi ))
			{
				::Sleep(100);
				logmsg.Format(_T("ExecuteThinBridge:ShellExecute Retry2 0x%08x"),::GetLastError());
				WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_DE);
				SetLastError(NO_ERROR);
				if(::ShellExecute(NULL,_T("open"),m_strThinBridge_EXE_FullPath,strCommand2,NULL, SW_SHOW) <= HINSTANCE(32))
				{
					::Sleep(100);
					logmsg.Format(_T("ExecuteThinBridge:ShellExecute Retry3 0x%08x"),::GetLastError());
					WriteDebugTraceDateTime(logmsg,DEBUG_LOG_TYPE_DE);
					SetLastError(NO_ERROR);
					::ShellExecute(NULL,NULL,strCommand,NULL,NULL, SW_SHOW);
				}
			}
		}
		if(pi.hThread)
		{
			CloseHandle( pi.hThread );  // �X���b�h�̃n���h���͎g��Ȃ��̂ł����j��
			pi.hThread=0;
		}
		if(pi.hProcess)
		{
			CloseHandle( pi.hProcess ); // �����v���Z�X�̃n���h���͎g��Ȃ��̂Ŕj��
			pi.hProcess=0;
		}
	}
}

void CBHORedirector::Navigate_RedirectHTM(CComPtr<IWebBrowser2>  spWebBrowse,CString OptionParam,DWORD dRedirectPageAction,DWORD dwCloseTimeout)
{
	if(spWebBrowse==NULL)
		return;
	//https://kb.vmware.com/selfservice/microsites/search.do?language=en_US&cmd=displayKC&externalId=2073845
	if(!(m_IE_M_Ver==8))
		spWebBrowse->Stop();
	CString strURL;
	strURL.Format(_T("res://%s/205?q=1"),_AtlModule.m_gSZModuleName);

	if(OptionParam.CompareNoCase(_T("Firefox"))==0
	||OptionParam.CompareNoCase(_T("Chrome"))==0
	||OptionParam.CompareNoCase(_T("Edge"))==0
	||OptionParam.CompareNoCase(_T("RDP"))==0
	||OptionParam.CompareNoCase(_T("VMWARE"))==0
	||OptionParam.CompareNoCase(_T("CITRIX"))==0
	)
	{
		strURL+=_T("&type=");
		strURL+=OptionParam.MakeLower();
	}
//	else if(OptionParam.CompareNoCase(_T("Custom"))==0)
//	else if(OptionParam.CompareNoCase(_T("IE"))==0)
//	else

	//res://ThinBridgeBHO.dll/PNG/202 Default
	//res://ThinBridgeBHO.dll/PNG/206 Chrome
	//res://ThinBridgeBHO.dll/PNG/207 Citrix
	//res://ThinBridgeBHO.dll/PNG/208 Edge
	//res://ThinBridgeBHO.dll/PNG/209 Firefox
	//res://ThinBridgeBHO.dll/PNG/210 RDP
	//res://ThinBridgeBHO.dll/PNG/211 VMware

	if(dRedirectPageAction==1)
	{
		strURL+=_T("&hb=1");
	}
	else if(dRedirectPageAction==2)
	{
		strURL+=_T("&hb=2");
	}

	if(dwCloseTimeout!=3)
	{
		CString strCloseTimeout;
		strCloseTimeout.Format(_T("&cto=%d"),dwCloseTimeout);
		strURL+=strCloseTimeout;
	}

	DWORD		dwFlags 			= 0;
	LPCTSTR 	lpszTargetFrameName = NULL;
	LPCTSTR 	lpszHeaders 		= NULL;
	LPVOID		lpvPostData 		= NULL;
	DWORD		dwPostDataLen		= 0;

	CComVariant   vURL;
	CComVariant   vHeaders;
	CComVariant   vTargetFrameName;
	CComVariant   vFlags( (long) dwFlags );
	CComVariant   vPostData;

	vURL = strURL;
	try
	{
		HRESULT hr = spWebBrowse->Navigate2(&vURL, &vFlags, &vTargetFrameName,NULL, &vHeaders);
		//ATLASSERT(hr == S_OK);
	}
	catch (...)
	{
		ATLASSERT(0);
	}
}

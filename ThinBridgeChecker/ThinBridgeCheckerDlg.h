
// ThinBridgeCheckerDlg.h : ヘッダー ファイル
//

#pragma once
#pragma comment(lib, "Psapi.lib") 
#define PSAPI_VERSION 1
#include "Psapi.h"
#include <Tlhelp32.h>
#include "locale.h"
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
static BOOL Is64BitWindows()
{
#if defined(_WIN64)
	return TRUE;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
	// 32-bit programs run on both 32-bit and 64-bit Windows
	// so must sniff
	BOOL f64 = FALSE;
	LPFN_ISWOW64PROCESS fnIsWow64Process={0};

	fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(_T("kernel32.dll")),"IsWow64Process");
	if(NULL != fnIsWow64Process)
	{
		return fnIsWow64Process(GetCurrentProcess(),&f64) && f64;
	}
	return FALSE;
#else
	return FALSE; // Win64 does not support Win16
#endif
}
#include <winternl.h>

struct RTL_USER_PROCESS_PARAMETERS_I
{
	BYTE Reserved1[16];
	PVOID Reserved2[10];
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
};
struct PEB_INTERNAL
{
	BYTE Reserved1[2];
	BYTE BeingDebugged;
	BYTE Reserved2[1];
	PVOID Reserved3[2];
	struct PEB_LDR_DATA* Ldr;
	RTL_USER_PROCESS_PARAMETERS_I* ProcessParameters;
	BYTE Reserved4[104];
	PVOID Reserved5[52];
	struct PS_POST_PROCESS_INIT_ROUTINE* PostProcessInitRoutine;
	BYTE Reserved6[128];
	PVOID Reserved7[1];
	ULONG SessionId;
};
typedef NTSTATUS (NTAPI* NtQueryInformationProcessPtr)(
	IN HANDLE ProcessHandle,
	IN PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL);

typedef ULONG (NTAPI* RtlNtStatusToDosErrorPtr)(NTSTATUS Status);

static SIZE_T GetRemoteCommandLineW(HANDLE hProcess, LPWSTR pszBuffer, UINT bufferLength)
{
	// Locating functions
	HINSTANCE hNtDll = GetModuleHandleW(L"ntdll.dll");
	NtQueryInformationProcessPtr NtQueryInformationProcess = (NtQueryInformationProcessPtr)GetProcAddress(hNtDll, "NtQueryInformationProcess");
	RtlNtStatusToDosErrorPtr RtlNtStatusToDosError = (RtlNtStatusToDosErrorPtr)GetProcAddress(hNtDll, "RtlNtStatusToDosError");

	if(!NtQueryInformationProcess || !RtlNtStatusToDosError)
	{
		//printf("Functions cannot be located.\n");
		return 0;
	}
  
	// Get PROCESS_BASIC_INFORMATION
	PROCESS_BASIC_INFORMATION pbi={0};
	ULONG len=0;
	NTSTATUS status = NtQueryInformationProcess(
	hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &len);
	SetLastError(RtlNtStatusToDosError(status));

	if(NT_ERROR(status) || !pbi.PebBaseAddress)
	{
		//printf("NtQueryInformationProcess(ProcessBasicInformation) failed.\n");
		return 0;
	}

	// Read PEB memory block
	SIZE_T bytesRead = 0;
	PEB_INTERNAL peb={0};
	if(!ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(peb), &bytesRead))
	{
		//printf("Reading PEB failed.\n");
		return 0;
	}
  
	// Obtain size of commandline string
	RTL_USER_PROCESS_PARAMETERS_I upp={0};
	if(!ReadProcessMemory(hProcess, peb.ProcessParameters, &upp, sizeof(upp), &bytesRead))
	{
		//printf("Reading USER_PROCESS_PARAMETERS failed.\n");
		return 0;
	}
  
	if(!upp.CommandLine.Length)
	{
		//printf("Command line length is 0.\n");
		return 0;
	}
  
	// Check the buffer size
	DWORD dwNeedLength = (upp.CommandLine.Length+1) / sizeof(wchar_t) +1;
	if(bufferLength < dwNeedLength)
	{
		//printf("Not enough buffer.\n");
		return dwNeedLength;
	}
  
	// Get the actual command line
	pszBuffer[dwNeedLength - 1] =L'\0';
	if(!ReadProcessMemory(hProcess, upp.CommandLine.Buffer, pszBuffer, upp.CommandLine.Length, &bytesRead))
	{
		//printf("Reading command line failed.\n");
		return 0;
	}
	return bytesRead / sizeof(wchar_t);
}

struct RESULT_DATA
{
	DWORD m_Error;
	DWORD m_Warning;
	DWORD m_Success;
};
#include "DlgMsg.h"

// CThinBridgeCheckerDlg ダイアログ
class CThinBridgeCheckerDlg : public CDialogEx
{
// コンストラクション
public:
	CThinBridgeCheckerDlg(CWnd* pParent = NULL);	// 標準コンストラクター
	virtual ~CThinBridgeCheckerDlg()
	{
		if(m_pMsgDlg)
		{
			delete m_pMsgDlg;
			m_pMsgDlg=NULL;
		}
	}

	CString m_strExeFolderPath;
// ダイアログ データ
	enum { IDD = IDD_ThinBridgeChecker_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

	RESULT_DATA m_stResultData;

	DlgMsg* m_pMsgDlg;
// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void SetDlgText(LPCTSTR lp)
	{
		CString str(lp);
		if(str.IsEmpty())
		{
			SetDlgItemText(IDC_EDIT1,_T(""));
			return;
		}
		CString strData;

		if(str==_T("\n"))
			strData.Format(_T("%s\r\n"),str);
		else
			strData.Format(_T("%s\r\n"),str);
		CString strPrevData;
		GetDlgItemText(IDC_EDIT1,strPrevData);
		strPrevData+=strData;
		SetDlgItemText(IDC_EDIT1,strPrevData);
	}
	afx_msg void OnBnClickedOk();
	CEdit m_Edit;
	afx_msg void OnBnClickedButton1();

	typedef int (__stdcall * TMessageBoxTimeout)(HWND, LPCTSTR, LPCTSTR, UINT, WORD, DWORD);
	void ShowTOMessageBox(LPCTSTR strMsg,int iType,int iTimeOut)
	{
		BOOL bFreeLibrary = FALSE;
		HMODULE hModule = {0};
		hModule = ::GetModuleHandle(_T("user32.dll"));
		if(!hModule)
		{
			hModule = ::LoadLibrary(_T("user32.dll"));
			if(hModule)
				bFreeLibrary = TRUE;
		}
		CString szAppName;
		szAppName = theApp.m_pszAppName;

		if(hModule)
		{
			TMessageBoxTimeout  MessageBoxTimeout;
	#ifdef _UNICODE
			MessageBoxTimeout = (TMessageBoxTimeout) GetProcAddress(hModule, "MessageBoxTimeoutW");
	#else
			MessageBoxTimeout = (TMessageBoxTimeout) GetProcAddress(hModule, "MessageBoxTimeoutA");
	#endif
			if(MessageBoxTimeout)
			{
				MessageBoxTimeout(this->m_hWnd, strMsg,
							szAppName, iType, LANG_NEUTRAL,iTimeOut);
			}
			else
			{
				::MessageBox(this->m_hWnd,strMsg,szAppName,iType);
			}
			if(bFreeLibrary)
			{
				FreeLibrary(hModule);
				bFreeLibrary=FALSE;
				hModule=NULL;
			}
		}
	}

	CString GetTridentVersionStr()
	{
		CString strRet;
		DWORD dwDummy=0;
		DWORD dwSize=0;
		DWORD dwMajar=0;
		DWORD dwMinor=0;
		DWORD dwBuild=0;
		DWORD dwPrivate=0;

		strRet = _T("MSHTML Trident Version N/A");

		dwSize = ::GetFileVersionInfoSize(_T("MSHTML.DLL"), &dwDummy);
		if(dwSize>0)
		{
			PBYTE pData = new BYTE[dwSize];
			memset(pData,0x00,dwSize);
			if(::GetFileVersionInfo(_T("MSHTML.DLL"), 0, dwSize, pData))
			{ 
				VS_FIXEDFILEINFO* pFileInfo={0};
				UINT nLen=0;
				if(::VerQueryValue(pData, _T("\\"),(PVOID*)&pFileInfo, &nLen))
				{
					dwMajar = HIWORD(pFileInfo->dwFileVersionMS);
					dwMinor = LOWORD(pFileInfo->dwFileVersionMS);
					dwBuild = HIWORD(pFileInfo->dwFileVersionLS);
					dwPrivate = LOWORD(pFileInfo->dwFileVersionLS);
					strRet.Format(_T("MSHTML Trident Version %d.%d.%d.%d"),dwMajar,dwMinor,dwBuild,dwPrivate);
				}
			}
			delete[] pData;
		}
		return strRet;
	}
	CString GetOSInfo(void)
	{
#if 1600<=_MSC_VER   //      Visual C++ 2010以上
#pragma warning(push)
#pragma warning(disable : 4996) // GetVersionExが古い形式として宣言されましたを回避
#endif
		OSVERSIONINFOEX ovi = {0};
		ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		GetVersionEx((OSVERSIONINFO*)&ovi);
		DWORD dwBuildNumber = ovi.dwBuildNumber;
		CString strOS = _T("Windows");

		CString strBuff;

		if(ovi.wProductType==VER_NT_WORKSTATION)
			strOS+=_T(" Client");
		else if(ovi.wProductType==VER_NT_DOMAIN_CONTROLLER||ovi.wProductType==VER_NT_SERVER)
			strOS+=_T(" Server");

		if(Is64BitWindows())
		{
			strBuff.Format(_T("%s x64"), strOS);
		}
		else
		{
			strBuff.Format(_T("%s x86"), strOS);
		}

		if((ovi.wSuiteMask&VER_SUITE_TERMINAL)==VER_SUITE_TERMINAL)
		{
			if(!((ovi.wSuiteMask&VER_SUITE_SINGLEUSERTS)==VER_SUITE_SINGLEUSERTS))
			{
				strBuff+=_T(" (RDS Session)");
			}
		}
#if 1600<=_MSC_VER   //      Visual C++ 2010以上
#pragma warning(pop)
#endif
		return strBuff;
	}
	CString GetOSKernelVersion()
	{
		CString strRet;
		DWORD dwDummy=0;
		DWORD dwSize=0;
		DWORD dwMajar=0;
		DWORD dwMinor=0;
		DWORD dwBuild=0;
		DWORD dwPrivate=0;

		strRet = _T("Kernel Version N/A");

		dwSize = ::GetFileVersionInfoSize(_T("KERNEL32.DLL"), &dwDummy);
		if(dwSize>0)
		{
			PBYTE pData = new BYTE[dwSize];
			memset(pData,0x00,dwSize);
			if(::GetFileVersionInfo(_T("KERNEL32.DLL"), 0, dwSize, pData))
			{ 
				VS_FIXEDFILEINFO* pFileInfo={0};
				UINT nLen=0;
				if(::VerQueryValue(pData, _T("\\"),(PVOID*)&pFileInfo, &nLen))
				{
					dwMajar = HIWORD(pFileInfo->dwFileVersionMS);
					dwMinor = LOWORD(pFileInfo->dwFileVersionMS);
					dwBuild = HIWORD(pFileInfo->dwFileVersionLS);
					dwPrivate = LOWORD(pFileInfo->dwFileVersionLS);
					strRet.Format(_T("Kernel Version %d.%d.%d.%d"),dwMajar,dwMinor,dwBuild,dwPrivate);
				}
			}
			delete[] pData;
		}
		return strRet;
	}

	CString ThinBridgeBHOCreate()
	{
		CString logmsg;
		HRESULT	hr={0};
		CString strThinBridgePath;
		strThinBridgePath=GetThinBridgeBHOPath();
		if(strThinBridgePath.IsEmpty())
		{
			logmsg.Format(_T("*ERROR* ThinBridgeBHO Create Failed"));
			m_stResultData.m_Error+=1;
			return logmsg;
		}

		GUID	rclsidTD={0};
		CComPtr<IObjectWithSite>	pIObjectWithSiteThinBridge;
		//ThinBridge
		WCHAR	pszGUIDTD[] = {L"{3A56619B-37AC-40DA-833E-410F3BEDCBDC}"};
		::CLSIDFromString(pszGUIDTD,&rclsidTD);
		hr = ::CoCreateInstance(rclsidTD,NULL,CLSCTX_INPROC_SERVER,IID_IObjectWithSite,(void**)&pIObjectWithSiteThinBridge);
		if(pIObjectWithSiteThinBridge)
		{
			logmsg.Format(_T("*SUCCESS* ThinBridgeBHO Create Success %s"),pszGUIDTD);
			m_stResultData.m_Success+=1;
			pIObjectWithSiteThinBridge.Release();
		}
		else
		{
			logmsg.Format(_T("*ERROR* ThinBridgeBHO Create Failed %s"),pszGUIDTD);
			m_stResultData.m_Error+=1;
		}
		return logmsg;
	}
	inline CString _BaseFunc_String(LPCTSTR lpKey,LPCTSTR lpRegSub=NULL,BOOL b64=FALSE)
	{
		CString strVal;
		HKEY  hKey={0};
		LONG lResult=0L;
		DWORD dwType=0;
		DWORD dwOption=0;
		dwOption= KEY_READ;
		if(b64)
			dwOption= dwOption| KEY_WOW64_64KEY;

		lResult=RegOpenKeyEx(HKEY_LOCAL_MACHINE,lpKey,
			0, dwOption,&hKey);
		if(lResult == ERROR_SUCCESS)
		{
			DWORD iSize=0;
			TCHAR* pVal=NULL;

			RegQueryValueEx(hKey,lpRegSub,NULL,&dwType,NULL,&iSize);
			if(iSize>0)
			{
				iSize+=1;//+1=null
				pVal = new TCHAR[iSize];
				memset(pVal,0x00,sizeof(TCHAR)*iSize);
				RegQueryValueEx(hKey,lpRegSub,NULL,&dwType,(LPBYTE)pVal, &iSize);
				strVal = pVal;
				delete [] pVal;
			}
			RegCloseKey(hKey);
		}
		return strVal;
	}
	inline CString _BaseFunc_String_CU(LPCTSTR lpKey,LPCTSTR lpRegSub=NULL)
	{
		CString strVal;
		HKEY  hKey={0};
		LONG lResult=0L;
		DWORD dwType=0;
		lResult=RegOpenKeyEx(HKEY_CURRENT_USER,lpKey,
			0,KEY_READ,&hKey);
		if(lResult == ERROR_SUCCESS)
		{
			DWORD iSize=0;
			TCHAR* pVal=NULL;

			RegQueryValueEx(hKey,lpRegSub,NULL,&dwType,NULL,&iSize);
			if(iSize>0)
			{
				iSize+=1;//+1=null
				pVal = new TCHAR[iSize];
				memset(pVal,0x00,sizeof(TCHAR)*iSize);
				RegQueryValueEx(hKey,lpRegSub,NULL,&dwType,(LPBYTE)pVal, &iSize);
				strVal = pVal;
				delete [] pVal;
			}
			RegCloseKey(hKey);
		}
		return strVal;
	}
	inline int _BaseFunc_DWORD(LPCTSTR lpKey,LPCTSTR lpRegSub,int initVal=0, BOOL b64 = FALSE)
	{
		DWORD Val=initVal;
		HKEY  hKey={0};
		LONG lResult=0L;
		DWORD dwType=0;
		DWORD dwSize=0;
		DWORD dwOption = 0;
		dwOption = KEY_READ;
		if (b64)
			dwOption = dwOption | KEY_WOW64_64KEY;

		lResult=RegOpenKeyEx(HKEY_LOCAL_MACHINE,lpKey,0, dwOption,&hKey);
		if(lResult == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey,lpRegSub,NULL,&dwType,NULL,&dwSize);
			RegQueryValueEx(hKey,lpRegSub,NULL,&dwType,(LPBYTE)&Val, &dwSize);
			RegCloseKey(hKey);
		}
		return Val;
	}
	inline int _BaseFunc_DWORD_CU(LPCTSTR lpKey,LPCTSTR lpRegSub,int initVal=0)
	{
		DWORD Val=initVal;
		HKEY  hKey={0};
		LONG lResult=0L;
		DWORD dwType=0;
		DWORD dwSize=0;
		lResult=RegOpenKeyEx(HKEY_CURRENT_USER,lpKey,0,KEY_READ,&hKey);
		if(lResult == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey,lpRegSub,NULL,&dwType,NULL,&dwSize);
			RegQueryValueEx(hKey,lpRegSub,NULL,&dwType,(LPBYTE)&Val, &dwSize);
			RegCloseKey(hKey);
		}
		return Val;
	}

	CString GetThinBridgeBasePath()
	{
		CString strRet;
		CString strReg;
		TCHAR REG_SETTING_KEY[]=_T("SOFTWARE\\Classes\\CLSID\\{3A56619B-37AC-40DA-833E-410F3BEDCBDC}\\InprocServer32");
		strReg=_BaseFunc_String(REG_SETTING_KEY,NULL);
		if(!strReg.IsEmpty())
		{
			CString strPath;
			strPath = strReg;
			strPath.Replace(_T("\""),_T(""));
			strPath.TrimLeft();
			strPath.TrimRight();
			strPath.MakeLower();
			strPath.Replace(_T("\\thinbridgebho.dll"),_T("\\"));
			strPath.Replace(_T("\\thinbridgebho64.dll"),_T("\\"));
			strRet=strPath;
		}

		if (strRet.IsEmpty()||!PathFileExists(strRet))
		{
			CString REG_SETTING_KEY=_T("SOFTWARE\\ThinBridge");
			strRet=_BaseFunc_String(REG_SETTING_KEY,_T("Path"));
			if(!strRet.IsEmpty())
			{
				if(PathFileExists(strRet))
					return strRet;
			}
			else
				strRet.Empty();
		}
		return strRet;
	}

	CString GetThinBridgeBHOPath()
	{
		CString strRet;
		CString strReg;
		TCHAR REG_SETTING_KEY[]=_T("SOFTWARE\\Classes\\CLSID\\{3A56619B-37AC-40DA-833E-410F3BEDCBDC}\\InprocServer32");
		strReg=_BaseFunc_String(REG_SETTING_KEY,NULL);
		if(!strReg.IsEmpty())
		{
			CString strPath;
			strPath = strReg;
			strPath.Replace(_T("\""),_T(""));
			strPath.TrimLeft();
			strPath.TrimRight();
			return strPath;
		}
		return strRet;
	}

	CString ThinBridgeBHOReg()
	{
		CString strRet;
		CString strReg;
		CString strTemp;
		TCHAR REG_SETTING_KEY[]=_T("SOFTWARE\\Classes\\CLSID\\{3A56619B-37AC-40DA-833E-410F3BEDCBDC}\\InprocServer32");
		strReg=_BaseFunc_String(REG_SETTING_KEY,NULL);
		if(!strReg.IsEmpty())
		{
			CString strPath;
			strPath = strReg;
			strPath.Replace(_T("\""),_T(""));
			strPath.TrimLeft();
			strPath.TrimRight();
			if (PathFileExists(strPath))
			{
				strRet.Format(_T("*SUCCESS* ThinBridgeBHO_REG Success (File Found) [%s] [%s]"),strReg,strPath);
				m_stResultData.m_Success+=1;
				strRet+= _T("\r\n");
				strRet += GetThinBridgeBHOVersion(strPath);
				CString strZonePath;
				strZonePath = strPath;
				strZonePath += _T(":Zone.Identifier");
			    HANDLE hRead = ::CreateFile(strZonePath, 
                     GENERIC_READ, 
                     FILE_SHARE_READ, 
                     NULL, 
                     OPEN_EXISTING, 
                     0, 
                     NULL);
			    if(hRead != INVALID_HANDLE_VALUE)
				{
				    ::CloseHandle(hRead); 
					strRet += _T("\r\n");
					strTemp.Format(_T("*WARNING* ThinBridgeBHO Block file attribute [%s]"),strZonePath);
					m_stResultData.m_Warning+=1;
					strRet += strTemp;
				}
			}
			else
			{
				strRet.Format(_T("*ERROR* ThinBridgeBHO_REG Failed(File not found) [%s]"),strReg);
				m_stResultData.m_Error+=1;
			}
		}
		else
		{
			strRet=_T("*ERROR* ThinBridgeBHO_REG Failed(REG_EMPTY_Value)");
			m_stResultData.m_Error+=1;
		}
		//////////////////////////////////////////////////////////////////////
		CString strVal;
		
		TCHAR szKey[]=_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects\\{3A56619B-37AC-40DA-833E-410F3BEDCBDC}");
		strVal=_BaseFunc_String(szKey,NULL);
		if(!strVal.IsEmpty())
		{
			strRet+= _T("\r\n");
			strTemp.Format(_T("*SUCCESS* ThinBridgeBHO_REG Browser Helper Objects Success [%s]"),szKey);
			m_stResultData.m_Success+=1;
			strRet+=strTemp;
		}
		else
		{
			strRet+= _T("\r\n");
			strTemp.Format(_T("*ERROR* ThinBridgeBHO_REG Browser Helper Objects Failed(REG_EMPTY_Value) [%s]"),szKey);
			m_stResultData.m_Error+=1;
			strRet+=strTemp;
		}
		return strRet;
	}
	CString GetThinBridgeBHOVersion(LPCTSTR lpPath)
	{
		CString strRet;
		if(lpPath==NULL)return strRet;
		DWORD dwDummy=0;
		DWORD dwSize=0;
		DWORD dwMajar=0;
		DWORD dwMinor=0;
		DWORD dwBuild=0;
		DWORD dwPrivate=0;

		strRet = _T("*ERROR* ThinBridgeBHO Version N/A");

		dwSize = ::GetFileVersionInfoSize(lpPath, &dwDummy);
		if(dwSize>0)
		{
			PBYTE pData = new BYTE[dwSize];
			memset(pData,0x00,dwSize);
			if(::GetFileVersionInfo(lpPath, 0, dwSize, pData))
			{ 
				VS_FIXEDFILEINFO* pFileInfo={0};
				UINT nLen=0;
				if(::VerQueryValue(pData, _T("\\"),(PVOID*)&pFileInfo, &nLen))
				{
					dwMajar = HIWORD(pFileInfo->dwFileVersionMS);
					dwMinor = LOWORD(pFileInfo->dwFileVersionMS);
					dwBuild = HIWORD(pFileInfo->dwFileVersionLS);
					dwPrivate = LOWORD(pFileInfo->dwFileVersionLS);
					strRet.Format(_T("*SUCCESS* ThinBridgeBHO Version %d.%d.%d.%d [%s]"),dwMajar,dwMinor,dwBuild,dwPrivate,lpPath);
					m_stResultData.m_Success+=1;
				}
			}
			delete[] pData;
		}
		else
			m_stResultData.m_Error+=1;

		return strRet;
	}

	CString ThinBridgeBHO64Reg()
	{
		CString strRet;
		CString strReg;
		CString strTemp;
		TCHAR REG_SETTING_KEY[]=_T("SOFTWARE\\Classes\\CLSID\\{3A56619B-37AC-40DA-833E-410F3BEDCBDC}\\InprocServer32");
		strReg=_BaseFunc_String(REG_SETTING_KEY,NULL);
		if(!strReg.IsEmpty())
		{
			CString strPath;
			strPath = strReg;
			strPath.Replace(_T("\""),_T(""));
			strPath.TrimLeft();
			strPath.TrimRight();
			strPath.MakeLower();
			strPath.Replace(_T("\\thinbridgebho.dll"),_T("\\ThinBridgeBHO64.dll"));
			if (PathFileExists(strPath))
			{
				strRet.Format(_T("*SUCCESS* ThinBridgeBHO64_REG Success (File Found) [%s]"),strPath);
				m_stResultData.m_Success+=1;
				strRet+= _T("\r\n");
				strRet += GetThinBridgeBHO64Version(strPath);
				CString strZonePath;
				strZonePath = strPath;
				strZonePath += _T(":Zone.Identifier");
			    HANDLE hRead = ::CreateFile(strZonePath, 
                     GENERIC_READ, 
                     FILE_SHARE_READ, 
                     NULL, 
                     OPEN_EXISTING, 
                     0, 
                     NULL);
			    if(hRead != INVALID_HANDLE_VALUE)
				{
				    ::CloseHandle(hRead); 
					strRet += _T("\r\n");
					strTemp.Format(_T("*WARNING* ThinBridgeBHO64 Block file attribute [%s]"),strZonePath);
					m_stResultData.m_Warning+=1;
					strRet += strTemp;
				}
			}
			else
			{
				strRet.Format(_T("*ERROR* ThinBridgeBHO64_REG Failed(File not found) [%s]"),strReg);
				m_stResultData.m_Error+=1;
			}
		}
		else
		{
			strRet=_T("*ERROR* ThinBridgeBHO64_REG Failed(REG_EMPTY_Value)");
			m_stResultData.m_Error+=1;
		}
		return strRet;
	}
	CString GetThinBridgeBHO64Version(LPCTSTR lpPath)
	{
		CString strRet;
		if(lpPath==NULL)return strRet;
		DWORD dwDummy=0;
		DWORD dwSize=0;
		DWORD dwMajar=0;
		DWORD dwMinor=0;
		DWORD dwBuild=0;
		DWORD dwPrivate=0;

		strRet = _T("*ERROR* ThinBridgeBHO64 Version N/A");

		dwSize = ::GetFileVersionInfoSize(lpPath, &dwDummy);
		if(dwSize>0)
		{
			PBYTE pData = new BYTE[dwSize];
			memset(pData,0x00,dwSize);
			if(::GetFileVersionInfo(lpPath, 0, dwSize, pData))
			{ 
				VS_FIXEDFILEINFO* pFileInfo={0};
				UINT nLen=0;
				if(::VerQueryValue(pData, _T("\\"),(PVOID*)&pFileInfo, &nLen))
				{
					dwMajar = HIWORD(pFileInfo->dwFileVersionMS);
					dwMinor = LOWORD(pFileInfo->dwFileVersionMS);
					dwBuild = HIWORD(pFileInfo->dwFileVersionLS);
					dwPrivate = LOWORD(pFileInfo->dwFileVersionLS);
					strRet.Format(_T("*SUCCESS* ThinBridgeBHO64 Version %d.%d.%d.%d [%s]"),dwMajar,dwMinor,dwBuild,dwPrivate,lpPath);
					m_stResultData.m_Success+=1;
				}
			}
			delete[] pData;
		}
		else
			m_stResultData.m_Error+=1;

		return strRet;
	}
	CString IE3rdBHO()
	{
		CString strRet;
		CString strVal;

		TCHAR REG_SETTING_KEY[]=_T("SOFTWARE\\Microsoft\\Internet Explorer\\Main");
		strVal=_BaseFunc_String_CU(REG_SETTING_KEY,_T("Enable Browser Extensions"));
		if(strVal.CompareNoCase(_T("yes"))==0)
		{
			strRet.Format(_T("*SUCCESS* Internet Explorer Enable Browser Extensions [%s]"),strVal);
			m_stResultData.m_Success+=1;
		}
		else
		{
			strRet.Format(_T("*ERROR*  Internet Explorer Enable Browser Extensions [%s]"),strVal);
			m_stResultData.m_Error+=1;
		}
		return strRet;
	}
	CString IE_BHO_PolicieOLD()
	{
		CString strRet;
		DWORD dwVal = 0;


		TCHAR REG_SETTING_KEY[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Ext");
		strRet.Format(_T("HKLM\\%s\r\n"),REG_SETTING_KEY);
		dwVal = _BaseFunc_DWORD(REG_SETTING_KEY, _T("IgnoreFrameApprovalCheck"));
		CString strTemp;
		if (dwVal==1)
		{
			strTemp.Format(_T("*INFORMATION* GPO[新たにインストールされたアドオンを自動的にアクティブ化する] [有効](%d)\r\n"),dwVal);
			strRet += strTemp;
			//m_stResultData.m_Success += 1;
		}
		else
		{
			strTemp.Format(_T("*INFORMATION* GPO[新たにインストールされたアドオンを自動的にアクティブ化する] [無効 or 未構成](%d)\r\n"), dwVal);
			strRet += strTemp;
			//m_stResultData.m_Warning += 1;
		}
		dwVal = _BaseFunc_DWORD(REG_SETTING_KEY, _T("DisableAddonLoadTimePerformanceNotifications"));
		if (dwVal == 1)
		{
			strTemp.Format(_T("*INFORMATION* GPO[アドオンのパフォーマンスの通知を無効にする] [有効](%d)\r\n"), dwVal);
			strRet += strTemp;
			//m_stResultData.m_Success += 1;
		}
		else
		{
			strRet.Format(_T("*INFORMATION* GPO[アドオンのパフォーマンスの通知を無効にする] [無効 or 未構成](%d)\r\n"), dwVal);
			strRet += strTemp;
			//m_stResultData.m_Warning += 1;
		}
		return strRet;
	}
	CString IE_BHO_PolicieNEW()
	{
		CString strRet;
		DWORD dwVal = 0;

/*
HK_LM\Software\Microsoft\Windows\CurrentVersion\Policies\Ext
ListBox_Support_CLSID=1
HK_LM\Software\Microsoft\Windows\CurrentVersion\Policies\Ext\CLSID
{3A56619B-37AC-40DA-833E-410F3BEDCBDC}=1
*/
		TCHAR REG_SETTING_KEY[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Ext");
		strRet.Format(_T("HKLM\\%s\r\n"), REG_SETTING_KEY);
		dwVal = _BaseFunc_DWORD(REG_SETTING_KEY, _T("ListBox_Support_CLSID"));
		CString strTemp;
		if (dwVal == 1)
		{
			strTemp.Format(_T("*INFORMATION* GPO [Internet Explorer]-[セキュリティの機能]-[アドオン管理]-[アドオンの一覧] [有効](%d)\r\n"), dwVal);
			strRet += strTemp;
		}
		else
		{
			strTemp.Format(_T("*INFORMATION* GPO [Internet Explorer]-[セキュリティの機能]-[アドオン管理]-[アドオンの一覧] [無効 or 未構成](%d)\r\n"), dwVal);
			strRet += strTemp;
		}
		dwVal = _BaseFunc_DWORD(REG_SETTING_KEY, _T("{3A56619B-37AC-40DA-833E-410F3BEDCBDC}"),-1);
		if (dwVal == 1)
		{
			strTemp.Format(_T("*INFORMATION* GPO [Internet Explorer]-[セキュリティの機能]-[アドオン管理]-[アドオンの一覧]-[ThinBridgeBHO] [有効](%d)\r\n"), dwVal);
			strRet += strTemp;
		}
		else if (dwVal == -1)
		{
			strTemp.Format(_T("*INFORMATION* GPO [Internet Explorer]-[セキュリティの機能]-[アドオン管理]-[アドオンの一覧]-[ThinBridgeBHO] [未構成](%d)\r\n"), dwVal);
			strRet += strTemp;
		}
		else if (dwVal == 0)
		{
			strTemp.Format(_T("*INFORMATION* GPO [Internet Explorer]-[セキュリティの機能]-[アドオン管理]-[アドオンの一覧]-[ThinBridgeBHO] [無効](%d)\r\n"), dwVal);
			strRet += strTemp;
		}
		else if (dwVal == 2)
		{
			strTemp.Format(_T("*INFORMATION* GPO [Internet Explorer]-[セキュリティの機能]-[アドオン管理]-[アドオンの一覧]-[ThinBridgeBHO] [ユーザーによる有効・無効設定可](%d)\r\n"), dwVal);
			strRet += strTemp;
		}
		return strRet;
	}
	CString IEThinBridgeBlock()
	{
		CString strRet;
		DWORD dwVal=0;

		TCHAR REG_SETTING_KEY[]=_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Ext\\Settings\\{3A56619B-37AC-40DA-833E-410F3BEDCBDC}");
		dwVal=_BaseFunc_DWORD_CU(REG_SETTING_KEY,_T("Flags"));
		if(dwVal==1)
		{
			strRet.Format(_T("*ERROR*  ThinBridgeBHO Disabled [%d]"), dwVal);
			m_stResultData.m_Error += 1;
		}
		else
		{
			strRet.Format(_T("*SUCCESS* ThinBridgeBHO Enabled [%d]"), dwVal);
			m_stResultData.m_Success += 1;
		}
		return strRet;
	}

	CString ThinBridgeInfomationNormal()
	{
		CString strRet;
		CString strPath;
		CString strTemp;
		CString REG_SETTING_KEY = _T("SOFTWARE\\ThinBridge");
		strPath = _BaseFunc_String(REG_SETTING_KEY, _T("Path"));
		if (strPath.IsEmpty())
		{
			strRet += _T("*ERROR* ThinBridge Install Failed(Path not found)\r\n");
			m_stResultData.m_Error += 1;
		}
		else
		{
			strTemp.Format(_T("*SUCCESS* ThinBridge Install Success (Path Found)[%s]\r\n"), strPath);
			strRet += strTemp;
			m_stResultData.m_Success += 1;
		}

		strPath = _BaseFunc_String(REG_SETTING_KEY, _T("Version"));
		if (strPath.IsEmpty())
		{
			strRet += _T("*ERROR* ThinBridge Install Failed(Version not found)\r\n");
			m_stResultData.m_Error += 1;
		}
		else
		{
			strTemp.Format(_T("*SUCCESS* ThinBridge Install Success (Version Found)[%s]\r\n"), strPath);
			strRet += strTemp;
			m_stResultData.m_Success += 1;
		}

		strPath = _BaseFunc_String(REG_SETTING_KEY, _T("Rulefile"));
		if (strPath.IsEmpty())
		{
			strRet += _T("*ERROR* ThinBridge Install Failed(Rulefile not found)\r\n");
			m_stResultData.m_Error += 1;
		}
		else
		{
			strTemp.Format(_T("*SUCCESS* ThinBridge Install Success (Rulefile Found)[%s]\r\n"), strPath);
			strRet += strTemp;
			m_stResultData.m_Success += 1;
		}
		strPath = _BaseFunc_String(REG_SETTING_KEY, _T("ExtensionExecfile"));
		if (strPath.IsEmpty())
		{
			strRet += _T("*ERROR* ThinBridge Install Failed(ExtensionExecfile not found)\r\n");
			m_stResultData.m_Error += 1;
		}
		else
		{
			strTemp.Format(_T("*SUCCESS* ThinBridge Install Success (ExtensionExecfile Found)[%s]\r\n"), strPath);
			strRet += strTemp;
			m_stResultData.m_Success += 1;
		}

		strRet += GetThinBridgeModuleVersion(_T("TBo365URLSyncSetting.exe"), FALSE);
		strRet += GetThinBridgeModuleVersion(_T("ThinBridgeChecker.exe"), FALSE);
		strRet += GetThinBridgeModuleVersion(_T("ThinBridgeRuleUpdater.exe"), FALSE);
		strRet += GetThinBridgeModuleVersion(_T("ThinBridgeRuleUpdaterSetting.exe"), FALSE);
		strRet += GetThinBridgeModuleVersion(_T("TBRedirector.exe"), FALSE);

		strRet += _T("\r\n--リダイレクト定義自動更新情報--\r\n");
		strRet += _T("#GPO\r\n");
		REG_SETTING_KEY = _T("SOFTWARE\\WOW6432Node\\Policies\\ThinBridge\\Rule");
		strPath = _BaseFunc_String(REG_SETTING_KEY, _T("ConfigServerURL"));
		strTemp.Format(_T("%s\r\nConfigServerURL=%s\r\n"), REG_SETTING_KEY, strPath);
		strRet += strTemp;
		strPath = _BaseFunc_String(REG_SETTING_KEY, _T("ConfigServerURL2"));
		strTemp.Format(_T("ConfigServerURL2=%s\r\n"), strPath);
		strRet += strTemp;
		strPath = _BaseFunc_String(REG_SETTING_KEY, _T("ConfigServerURL3"));
		strTemp.Format(_T("ConfigServerURL3=%s\r\n"), strPath);
		strRet += strTemp;
		strPath = _BaseFunc_String(REG_SETTING_KEY, _T("ConfigServerURL4"));
		strTemp.Format(_T("ConfigServerURL4=%s\r\n"), strPath);
		strRet += strTemp;
		strPath = _BaseFunc_String(REG_SETTING_KEY, _T("ConfigServerURL5"));
		strTemp.Format(_T("ConfigServerURL5=%s\r\n"), strPath);
		strRet += strTemp;
		strPath = _BaseFunc_String(REG_SETTING_KEY, _T("ConfigServerURL6"));
		strTemp.Format(_T("ConfigServerURL6=%s\r\n"), strPath);
		strRet += strTemp;
		int dwVal = _BaseFunc_DWORD(REG_SETTING_KEY, _T("Interval"));
		strTemp.Format(_T("Interval=%d\r\n"), dwVal);
		strRet += strTemp;
		dwVal = _BaseFunc_DWORD(REG_SETTING_KEY, _T("ServerConnectionRule"));
		strTemp.Format(_T("ServerConnectionRule=%d\r\n"), dwVal);
		strRet += strTemp;

		strPath.Format(_T("%s%s"), GetThinBridgeBasePath(), _T("ThinBridgeRuleUpdater.conf"));
		strTemp.Format(_T("%s\r\n"), strPath);
		strRet += _T("#File\r\n");
		strRet += strTemp;

		CStdioFile in;
		if (in.Open(strPath, CFile::modeRead | CFile::shareDenyNone))
		{
			CString strTemp;
			while (in.ReadString(strTemp))
			{
				strRet += strTemp;
				strRet += _T("\r\n");
			}
			in.Close();
		}
		else
		{
			strTemp.Format(_T("%s File not found\r\n"), strPath);
			strRet += strTemp;
		}
		return strRet;
	}

	CString GetModuleVersion(LPCTSTR lpFile, BOOL bMust)
	{
		DWORD dwDummy = 0;
		DWORD dwSize = 0;
		DWORD dwMajar = 0;
		DWORD dwMinor = 0;
		DWORD dwBuild = 0;
		DWORD dwPrivate = 0;
		CString strPath;
		CString strRet;
		CString strTemp;
		strPath=lpFile;
		if (!PathFileExists(strPath))
		{
			if (bMust)
			{
				strTemp.Format(_T("*ERROR* %s Failed(File not found)"), strPath);
				strRet = strTemp;
				m_stResultData.m_Error += 1;
			}
			else
			{
				strTemp.Format(_T("*WARNING* %s (File not found)"), strPath);
				strRet = strTemp;
				m_stResultData.m_Warning += 1;
			}
			return strRet;
		}
		strRet.Format(_T("*SUCCESS* %s Success (File Found)"), lpFile);
		m_stResultData.m_Success += 1;
		strRet += _T("\r\n");
		dwSize = ::GetFileVersionInfoSize(strPath, &dwDummy);
		if (dwSize > 0)
		{
			PBYTE pData = new BYTE[dwSize];
			memset(pData, 0x00, dwSize);
			if (::GetFileVersionInfo(strPath, 0, dwSize, pData))
			{
				VS_FIXEDFILEINFO* pFileInfo = { 0 };
				UINT nLen = 0;
				if (::VerQueryValue(pData, _T("\\"), (PVOID*)&pFileInfo, &nLen))
				{
					dwMajar = HIWORD(pFileInfo->dwFileVersionMS);
					dwMinor = LOWORD(pFileInfo->dwFileVersionMS);
					dwBuild = HIWORD(pFileInfo->dwFileVersionLS);
					dwPrivate = LOWORD(pFileInfo->dwFileVersionLS);
					strTemp.Format(_T("*SUCCESS* %s Version %d.%d.%d.%d"), lpFile, dwMajar, dwMinor, dwBuild, dwPrivate);
					m_stResultData.m_Success += 1;
					strRet += strTemp;
				}
			}
			else
			{
				strTemp.Format(_T("*WARNING* %s Version N/A"), lpFile);
				strRet += strTemp;
				m_stResultData.m_Warning += 1;
			}
			delete[] pData;
		}
		else
		{
			strTemp.Format(_T("*WARNING* %s Version N/A"), lpFile);
			strRet += strTemp;
			m_stResultData.m_Warning += 1;
		}
		strRet += _T("\r\n");
		return strRet;
	}


	CString GetThinBridgeModuleVersion(LPCTSTR lpFile,BOOL bMust)
	{
		DWORD dwDummy=0;
		DWORD dwSize=0;
		DWORD dwMajar=0;
		DWORD dwMinor=0;
		DWORD dwBuild=0;
		DWORD dwPrivate=0;
		CString strPath;
		CString strRet;
		CString strTemp;
		strPath.Format(_T("%s%s"),GetThinBridgeBasePath(),lpFile);
		if(!PathFileExists(strPath))
		{
			if(bMust)
			{
				strTemp.Format(_T("*ERROR* %s Failed(File not found)\r\n"),strPath);
				strRet=strTemp;
				m_stResultData.m_Error+=1;
			}
			else
			{
				strTemp.Format(_T("*WARNING* %s (File not found)\r\n"),strPath);
				strRet=strTemp;
				m_stResultData.m_Warning+=1;
			}
			return strRet;
		}
		strRet.Format(_T("*SUCCESS* %s Success (File Found) [%s]\r\n"),lpFile, strPath);
		m_stResultData.m_Success+=1;
		dwSize = ::GetFileVersionInfoSize(strPath, &dwDummy);
		if(dwSize>0)
		{
			PBYTE pData = new BYTE[dwSize];
			memset(pData,0x00,dwSize);
			if(::GetFileVersionInfo(strPath, 0, dwSize, pData))
			{ 
				VS_FIXEDFILEINFO* pFileInfo={0};
				UINT nLen=0;
				if(::VerQueryValue(pData, _T("\\"),(PVOID*)&pFileInfo, &nLen))
				{
					dwMajar = HIWORD(pFileInfo->dwFileVersionMS);
					dwMinor = LOWORD(pFileInfo->dwFileVersionMS);
					dwBuild = HIWORD(pFileInfo->dwFileVersionLS);
					dwPrivate = LOWORD(pFileInfo->dwFileVersionLS);
					strTemp.Format(_T("*SUCCESS* %s Version %d.%d.%d.%d [%s]\r\n"),lpFile,dwMajar,dwMinor,dwBuild,dwPrivate,strPath);
					m_stResultData.m_Success+=1;
					strRet+=strTemp;
				}
			}
			else
			{
//				strTemp.Format(_T("*WARNING* %s Version N/A\r\n"),lpFile);
//				strRet += strTemp;
//				m_stResultData.m_Warning+=1;
				CFileStatus status;
				CFile cfile;
				if (cfile.GetStatus(strPath, status))
				{
					strTemp.Format(_T("*SUCCESS* %s FileSize %I64u\r\n"), lpFile,status.m_size);
					strRet += strTemp;
				}
			}
			delete[] pData;
		}
		else
		{
//			strTemp.Format(_T("*WARNING* %s Version N/A\r\n"),lpFile);
//			strRet += strTemp;
//			m_stResultData.m_Warning+=1;
			CFileStatus status;
			CFile cfile;
			if (cfile.GetStatus(strPath, status))
			{
				strTemp.Format(_T("*SUCCESS* %s FileSize %I64u\r\n"), lpFile, status.m_size);
				strRet += strTemp;
			}
		}
		CString strZonePath;
		strZonePath = strPath;
		strZonePath += _T(":Zone.Identifier");
		HANDLE hRead = ::CreateFile(strZonePath, 
                   GENERIC_READ, 
                   FILE_SHARE_READ, 
                   NULL, 
                   OPEN_EXISTING, 
                   0, 
                   NULL);
		if(hRead != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(hRead); 
			strRet += _T("\r\n");
			strTemp.Format(_T("*WARNING* %s Block file attribute [%s]\r\n"),lpFile,strZonePath);
			m_stResultData.m_Warning+=1;
			strRet += strTemp;
		}
		return strRet;
	}


	CString GetThinBridgeEXEVersion()
	{
		CString strRet;
		strRet = GetThinBridgeModuleVersion(_T("ThinBridge.exe"),TRUE);
		return strRet;
	}
	CString GetThinBridgeEXEReg()
	{
		CString strRet;
		CString strDefVal;
		CString strAppNameVal;
		CString strAppPathVal;
		DWORD dwPolicyVal=0;

		CString strReg;
		CString strTemp;
		TCHAR REG_SETTING_KEY[]=_T("SOFTWARE\\Microsoft\\Internet Explorer\\Low Rights\\ElevationPolicy\\{3A56619B-37AC-40DA-833E-410F3BEDCBDC}");

		strDefVal=_BaseFunc_String(REG_SETTING_KEY,NULL);
		strAppNameVal=_BaseFunc_String(REG_SETTING_KEY,_T("AppName"));
		strAppPathVal=_BaseFunc_String(REG_SETTING_KEY,_T("AppPath"));
		dwPolicyVal=_BaseFunc_DWORD(REG_SETTING_KEY,_T("Policy"),-1);

		if(strDefVal.IsEmpty()
		||strAppNameVal.IsEmpty()
		||strAppPathVal.IsEmpty()
		||dwPolicyVal==-1
		)
		{
			strRet.Format(_T("*ERROR* ThinBridge.EXE REG_Value Failed(REG_EMPTY_Value) Default:[%s] AppName:[%s] AppPath:[%s] Policy:[%d]"),strDefVal,strAppNameVal,strAppPathVal,dwPolicyVal);
			m_stResultData.m_Error+=1;
		}
		else
		{
			strRet.Format(_T("*SUCCESS* ThinBridge.EXE REG_Value Success Default:[%s] AppName:[%s] AppPath:[%s] Policy:[%d]"),strDefVal,strAppNameVal,strAppPathVal,dwPolicyVal);
			m_stResultData.m_Success+=1;
		}
		return strRet;
	}

	CString GetAllModules(DWORD processID)
	{
		CString strRet;
		strRet = _T(" ");
		try
		{
			if(processID==0)
			{
				return strRet;
			}
			if(IsX64Process(processID))
			{
				strRet = _T("x64 Process");
				return strRet;
			}
			HMODULE hMods[1024]={0};
			HANDLE hProcess={0};
			DWORD cbNeeded=0;
			unsigned int i=0;

			hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
									PROCESS_VM_READ,
									FALSE, processID );
			if (NULL == hProcess)
				return strRet;

			TCHAR szModName[MAX_PATH*2]={0};
			CString strTemp;
			CString strModDetail;
			// Get a list of all the modules in this process.
			if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
			{
				for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
				{
					DoMessage();
					memset(szModName,0x00,sizeof(szModName));
					// Get the full path to the module's file.
					if ( GetModuleFileNameEx( hProcess, hMods[i], szModName,
												sizeof(szModName) / sizeof(TCHAR)))
					{
						// Print the module name and handle value.
						strModDetail.Empty();
						strModDetail = GetModDetail(szModName);
						if(strModDetail.Find(_T("\tMicrosoft Corporation\t"))>0)
							strTemp.Format(_T("%s (0x%08x) %s\r\n"), szModName, hMods[i],strModDetail );
						else
							strTemp.Format(_T("[*]%s (0x%08x) %s\r\n"), szModName, hMods[i],strModDetail );
						strRet += strTemp;
					}
				}
			}
			// Release the handle to the process.
			CloseHandle( hProcess );
		}
		catch (...)
		{
			ATLASSERT(0);
		}
		return strRet;
	}
struct LANGANDCODEPAGE
{
	WORD wLanguage;
	WORD wCodePage;
} *lpTranslate;

	CString GetModDetail(LPCTSTR lpModPath)
	{
		CString strRet =_T("N/A");
		CString strVersion;
		CString strCompanyName;
		CString strProductName;
		CString strFileDescription;
		CString strLegalCopyright;

		if(lpModPath==NULL)return strRet;
		DWORD dwDummy=0;
		DWORD dwSize=0;
		DWORD dwMajar=0;
		DWORD dwMinor=0;
		DWORD dwBuild=0;
		DWORD dwPrivate=0;

		dwSize = ::GetFileVersionInfoSize(lpModPath, &dwDummy);
		if(dwSize>0)
		{
			PBYTE pData = new BYTE[dwSize];
			memset(pData,0x00,dwSize);
			if(::GetFileVersionInfo(lpModPath, 0, dwSize, pData))
			{ 
				VS_FIXEDFILEINFO* pFileInfo={0};
				UINT nLen=0;
				if(::VerQueryValue(pData, _T("\\"),(PVOID*)&pFileInfo, &nLen))
				{
					dwMajar = HIWORD(pFileInfo->dwFileVersionMS);
					dwMinor = LOWORD(pFileInfo->dwFileVersionMS);
					dwBuild = HIWORD(pFileInfo->dwFileVersionLS);
					dwPrivate = LOWORD(pFileInfo->dwFileVersionLS);
					strVersion.Format(_T("%d.%d.%d.%d"),dwMajar,dwMinor,dwBuild,dwPrivate);
				}
				UINT TranslateLen=0;
				void *pvVersion={0};
				UINT VersionLen=0;
				VerQueryValue(pData,_T("\\VarFileInfo\\Translation"),
				(LPVOID*)&lpTranslate, &TranslateLen);
				for(UINT i=0;i<TranslateLen/sizeof(*lpTranslate);i++)
				{
					//コードページを指定
					CString name;
					name.Format(_T("\\StringFileInfo\\%04x%04x\\%s"),
					lpTranslate[i].wLanguage,
					lpTranslate[i].wCodePage,_T("CompanyName"));
					if(VerQueryValue(pData, name, &pvVersion, &VersionLen))
					{
						strCompanyName=(LPCTSTR)pvVersion;
						strCompanyName.TrimLeft();
						strCompanyName.TrimRight();
						strCompanyName.Replace(_T("\r"),_T(""));
						strCompanyName.Replace(_T("\n"),_T(""));
					}

					name.Format(_T("\\StringFileInfo\\%04x%04x\\%s"),
					lpTranslate[i].wLanguage,
					lpTranslate[i].wCodePage,_T("ProductName"));
					if(VerQueryValue(pData, name, &pvVersion, &VersionLen))
					{
						strProductName=(LPCTSTR)pvVersion;
						strProductName.TrimLeft();
						strProductName.TrimRight();
						strProductName.Replace(_T("\r"),_T(""));
						strProductName.Replace(_T("\n"),_T(""));
					}

					name.Format(_T("\\StringFileInfo\\%04x%04x\\%s"),
					lpTranslate[i].wLanguage,
					lpTranslate[i].wCodePage,_T("FileDescription"));
					if(VerQueryValue(pData, name, &pvVersion, &VersionLen))
					{
						strFileDescription=(LPCTSTR)pvVersion;
						strFileDescription.TrimLeft();
						strFileDescription.TrimRight();
						strFileDescription.Replace(_T("\r"),_T(""));
						strFileDescription.Replace(_T("\n"),_T(""));
					}

					name.Format(_T("\\StringFileInfo\\%04x%04x\\%s"),
					lpTranslate[i].wLanguage,
					lpTranslate[i].wCodePage,_T("LegalCopyright"));
					if(VerQueryValue(pData, name, &pvVersion, &VersionLen))
					{
						strLegalCopyright=(LPCTSTR)pvVersion;
						strLegalCopyright.TrimLeft();
						strLegalCopyright.TrimRight();
						strLegalCopyright.Replace(_T("\r"),_T(""));
						strLegalCopyright.Replace(_T("\n"),_T(""));
					}
					break;
				}
			}
			delete[] pData;
			//strRet.Format(_T("Version[%s] CompanyName[%s] ProductName[%s] FileDescription[%s] LegalCopyright[%s]"),strVersion,strCompanyName,strProductName,strFileDescription,strLegalCopyright);
			strRet.Format(_T("\tver%s\t%s\t%s\t%s\t%s"),strVersion,strCompanyName,strProductName,strFileDescription,strLegalCopyright);
		}
		return strRet;
	}
	int GetNativeIEProcCnt()
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		CPtrArray pptPIDArray;
		if(!hSnapshot)
			return -1;

		PROCESSENTRY32 pe32={0};
		pe32.dwSize = sizeof(PROCESSENTRY32);
		CString strExeFile;
		if(Process32First(hSnapshot, &pe32))
		{
			do
			{
				strExeFile = pe32.szExeFile;
				if(strExeFile.CompareNoCase(_T("iexplore.exe"))==0)
				{
					pptPIDArray.Add((void*)pe32.th32ProcessID);
				}
			}while(Process32Next(hSnapshot, &pe32));
		}
		CloseHandle(hSnapshot);

		return pptPIDArray.GetCount();

	}
	int GetNativeChromeProcCnt()
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		CPtrArray pptPIDArray;
		if (!hSnapshot)
			return -1;

		PROCESSENTRY32 pe32 = { 0 };
		pe32.dwSize = sizeof(PROCESSENTRY32);
		CString strExeFile;
		if (Process32First(hSnapshot, &pe32))
		{
			do
			{
				strExeFile = pe32.szExeFile;
				if (strExeFile.CompareNoCase(_T("chrome.exe")) == 0)
				{
					pptPIDArray.Add((void*)pe32.th32ProcessID);
				}
			} while (Process32Next(hSnapshot, &pe32));
		}
		CloseHandle(hSnapshot);

		return pptPIDArray.GetCount();

	}
	///////////////////////////////////////////////////////////////////////////////////
	BOOL SafeTerminateProcess(HANDLE hProcess, UINT uExitCode)
	{
		DWORD dwTID=0;
		DWORD dwCode=0;
		DWORD dwErr=0;
		HANDLE hProcessDup = INVALID_HANDLE_VALUE;
		HANDLE hRT = NULL;
		HINSTANCE hKernel = GetModuleHandle(_T("Kernel32"));
		BOOL bSuccess = FALSE;
		BOOL bDup = FALSE;
		bDup = DuplicateHandle(GetCurrentProcess(),hProcess,GetCurrentProcess(),&hProcessDup,PROCESS_ALL_ACCESS,FALSE,0);
		// Detect the special case where the process is 
		// already dead...
		if(GetExitCodeProcess((bDup) ? hProcessDup : hProcess, &dwCode) && (dwCode == STILL_ACTIVE) ) 
		{
			FARPROC pfnExitProc={0};
			pfnExitProc = GetProcAddress(hKernel, "ExitProcess");
			hRT = CreateRemoteThread((bDup) ? hProcessDup : hProcess,NULL,0,(LPTHREAD_START_ROUTINE)pfnExitProc,(PVOID)uExitCode, 0, &dwTID);
			if( hRT == NULL )
				dwErr = GetLastError();
		}
		else
		{
			dwErr = ERROR_PROCESS_ABORTED;
		}

		if(hRT)
		{
			// Must wait process to terminate to 
			// guarantee that it has exited...
			DWORD waitRes=0;
			waitRes = WaitForSingleObject((bDup) ? hProcessDup : hProcess,5*1000);
			if(waitRes == WAIT_TIMEOUT)
			{
				::TerminateProcess((bDup) ? hProcessDup : hProcess,0);
			}
			CloseHandle(hRT);
			bSuccess = TRUE;
		}
		if(bDup)
			CloseHandle(hProcessDup);
		if(!bSuccess)
			SetLastError(dwErr);
		return bSuccess;
	}

	CString GetIE_FullPath()
	{
		CString strRet;
		HKEY  hKey={0};
		DWORD dwDisposition=0;
		LONG lResult=0L;
		CString strVal;
		DWORD dwType=0;

		lResult=RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\iexplore.exe"),
			0,KEY_READ,&hKey);
		if(lResult == ERROR_SUCCESS)
		{
			DWORD iSize=0;
			TCHAR* pVal=NULL;

			RegQueryValueEx(hKey,_T(""),NULL,&dwType,NULL,&iSize);
			if(iSize>0)
			{
				iSize+=1;//+1=null
				pVal = new TCHAR[iSize];
				memset(pVal,0x00,sizeof(TCHAR)*iSize);
				RegQueryValueEx(hKey,_T(""),NULL,&dwType,(LPBYTE)pVal, &iSize);
				strVal = pVal;
				delete [] pVal;
			}
			RegCloseKey(hKey);
			strRet=strVal;
		}
		//HK_CUをチェック
		if(strRet.IsEmpty())
		{
			lResult=RegOpenKeyEx(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\iexplore.exe"),
				0,KEY_READ,&hKey);
			if(lResult == ERROR_SUCCESS)
			{
				DWORD iSize=0;
				TCHAR* pVal=NULL;

				RegQueryValueEx(hKey,_T(""),NULL,&dwType,NULL,&iSize);
				if(iSize>0)
				{
					iSize+=1;//+1=null
					pVal = new TCHAR[iSize];
					memset(pVal,0x00,sizeof(TCHAR)*iSize);
					RegQueryValueEx(hKey,_T(""),NULL,&dwType,(LPBYTE)pVal, &iSize);
					strVal = pVal;
					delete [] pVal;
				}
				RegCloseKey(hKey);
				strRet=strVal;
			}
		}
		if(strRet.IsEmpty())
		{
			strRet = _T("C:\\Program Files\\Internet Explorer\\iexplore.exe");
			if(Is64BitWindows())
			{
				strRet=_T("C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe");
			}
		}
		return strRet;
	}
	void ChromeStart(LPCTSTR lpParam = NULL)
	{
		CString strURL(lpParam);
		if (lpParam == NULL)
			strURL=_T("about:blank");
		if (::ShellExecute(NULL, _T("open"), _T("chrome.exe"), strURL, NULL, SW_SHOW) <= HINSTANCE(32))
		{
			CString IEcmd;
			IEcmd.Format(_T("chrome.exe %s"), strURL);
			if (::ShellExecute(NULL, NULL, IEcmd, NULL, NULL, SW_SHOW) <= HINSTANCE(32))
			{
				AfxMessageBox(_T("Chromeの起動に失敗しました。しばらく時間をおいてから再実行してください。"), MB_OK | MB_ICONSTOP);
			}
		}
	}

	void IEStart(LPCTSTR lpParam=NULL)
	{
		CString IEcmd;
		if(lpParam==NULL)
			IEcmd.Format(_T("\"%s\" about:blank"),GetIE_FullPath());
		else
			IEcmd.Format(_T("\"%s\" \"%s\""),GetIE_FullPath(),lpParam);

		STARTUPINFO si={0};
		PROCESS_INFORMATION pi={0};
		si.cb = sizeof( si );
		if(!CreateProcess( NULL,(LPTSTR)(LPCTSTR)IEcmd,NULL, NULL, TRUE, 0, NULL,NULL, &si, &pi ))
		{
			AfxMessageBox(_T("IEの起動に失敗しました。しばらく時間をおいてから再実行してください。"),MB_OK|MB_ICONSTOP);
		}
		else
		{
			if(lpParam!=NULL)
			{
				if(pi.hThread)
				{
					CloseHandle( pi.hThread );  // スレッドのハンドルは使わないのですぐ破棄
					pi.hThread=0;
				}
				if(pi.hProcess)
				{
					CloseHandle( pi.hProcess ); // もうプロセスのハンドルは使わないので破棄
					pi.hProcess=0;
				}
				return;
			}
			::Sleep(5000);
			if(GetNativeIEProcCnt()<1)
			::Sleep(5000);
			if(GetNativeIEProcCnt()<1)
			::Sleep(5000);
			CPtrArray pptPIDArray;
			if(GetNativeIEProcCnt()>=2)
			{
				DoMessage();

				HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
				if(!hSnapshot)
					return;

				PROCESSENTRY32 pe32={0};
				pe32.dwSize = sizeof(PROCESSENTRY32);
				CString strExeFile;
				if(Process32First(hSnapshot, &pe32))
				{
					do
					{
						strExeFile = pe32.szExeFile;
						if(strExeFile.CompareNoCase(_T("iexplore.exe"))==0)
						{
							pptPIDArray.Add((void*)pe32.th32ProcessID);
						}
					}while(Process32Next(hSnapshot, &pe32));
				}
				CloseHandle(hSnapshot);
				CString strTemp;
				for(int ii=0;ii<pptPIDArray.GetCount();ii++)
				{
					DoMessage();
					DWORD processID=0;
					processID = (DWORD)pptPIDArray.GetAt(ii);
					SetDlgText(_T("--IE詳細情報--"));
					strTemp.Format(_T("iexplore.exe PID:[%d]"),processID);
					strTemp += GetAllModules(processID);
					if(!strTemp.IsEmpty())
						SetDlgText(strTemp);
				}
			}
			if(pi.hThread)
			{
				CloseHandle( pi.hThread );  // スレッドのハンドルは使わないのですぐ破棄
				pi.hThread=0;
			}
			if(pi.hProcess)
			{
				CloseHandle( pi.hProcess ); // もうプロセスのハンドルは使わないので破棄
				pi.hProcess=0;
			}
			TermIE();
		}
	}
	BOOL IsX64Process(DWORD dwPID)
	{
		if(dwPID==0)return FALSE;

		if(!Is64BitWindows())
			return FALSE;

		BOOL bRet = FALSE;
		HANDLE processHandle = {0};
		processHandle = OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_INFORMATION, FALSE, dwPID);
		if(processHandle)
		{
			BOOL bWow64Onx86=FALSE;
			typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
			LPFN_ISWOW64PROCESS fnIsWow64Process={0};

			fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(_T("kernel32.dll")),"IsWow64Process");
			if(NULL != fnIsWow64Process)
			{
				fnIsWow64Process(processHandle,&bWow64Onx86);
				//64OSで32アプリが動いている
				if(bWow64Onx86)
					bRet=FALSE;
				else
					bRet=TRUE;
			}
			CloseHandle(processHandle);
		}
		return bRet;
	}


	void TermIE()
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		CPtrArray pptPIDArray;
		if(!hSnapshot)
			return;

		PROCESSENTRY32 pe32={0};
		pe32.dwSize = sizeof(PROCESSENTRY32);
		CString strExeFile;
		if(Process32First(hSnapshot, &pe32))
		{
			do
			{
				strExeFile = pe32.szExeFile;
				if(strExeFile.CompareNoCase(_T("iexplore.exe"))==0)
				{
					pptPIDArray.Add((void*)pe32.th32ProcessID);
				}
			}while(Process32Next(hSnapshot, &pe32));
		}
		CloseHandle(hSnapshot);
		for(int ii=0;ii<pptPIDArray.GetCount();ii++)
		{
			DWORD processID=0;
			processID = (DWORD)pptPIDArray.GetAt(ii);
			HANDLE processHandle = {0};
			processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
			if(processHandle)
			{
				if(!SafeTerminateProcess(processHandle,0))
					::TerminateProcess(processHandle,0);
				::Sleep(1000);
			}
		}
	}
	CString ExecChk();

	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton6();

	void Command_WINTOPMOST_T(int top)
	{
		if(!IsWindow(this->m_hWnd))return;
		if( 0 == top )
		{
			DWORD dwExstyle = (DWORD)::GetWindowLongPtr(this->m_hWnd, GWL_EXSTYLE );
			if( dwExstyle & WS_EX_TOPMOST )
			{
				top = 2; // 最前面である -> 解除
			}
			else
			{
				top = 1;
			}
		}

		HWND hwndInsertAfter=NULL;
		switch( top )
		{
			case 1:
				hwndInsertAfter = HWND_TOPMOST;
				break;
			case 2:
				hwndInsertAfter = HWND_NOTOPMOST;
				break;
			default:
				return;
		}
		::SetWindowPos( this->m_hWnd, hwndInsertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
	}
	void DoMessage()
	{
		MSG    msg={0};
		HWND hWnd={0};
		if(::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))//|PM_QS_PAINT))
		{
			hWnd=msg.hwnd;
			if(hWnd)
			{
				if(	hWnd == this->m_hWnd
					||::IsChild(this->m_hWnd,hWnd)
				)
				AfxGetThread()->PumpMessage();
				if(m_pMsgDlg)
				{
					m_pMsgDlg->StepIt();
				}
				::Sleep(1);
			}
		}
	}
	CString GetNativeProc()
	{
		CString strRet;
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if(!hSnapshot)
			return strRet;

		PROCESSENTRY32 pe32={0};
		pe32.dwSize = sizeof(PROCESSENTRY32);
		CString strExeFile;
		if(Process32First(hSnapshot, &pe32))
		{
			CString strLine;
			do
			{
				DoMessage();
				strExeFile = pe32.szExeFile;
				CString strProcPath;
				CString strProcCommandP;
				CString strModDetail;
				if(pe32.th32ProcessID>0)
				{
					strProcPath.Empty();
					strProcPath = GetProcessName(pe32.th32ProcessID);
					if(!strProcPath.IsEmpty())
					{
						strProcCommandP.Empty();
						strProcCommandP=GetCommandLineData(pe32.th32ProcessID);
						strModDetail.Empty();
						strModDetail = GetModDetail(strProcPath);
						if(strModDetail.Find(_T("\tMicrosoft Corporation\t"))>0)
							strLine.Format(_T("%d\t%s\t%s\t%s\t%s\r\n"),pe32.th32ProcessID,strExeFile,strProcPath,strProcCommandP,strModDetail);
						else
							strLine.Format(_T("[*]%d\t%s\t%s\t%s\t%s\r\n"),pe32.th32ProcessID,strExeFile,strProcPath,strProcCommandP,strModDetail);
					
						strRet +=strLine;
					}
				}
			}while(Process32Next(hSnapshot, &pe32));
		}
		CloseHandle(hSnapshot);
		return strRet;
	}
	#define MAX_COMMAND_LINE 2048
	CString GetCommandLineData(DWORD dwPID)
	{
		CString strRet;
		WCHAR szCommandLine[MAX_COMMAND_LINE]={0};
		if(dwPID==0)return strRet;
		try
		{
			HANDLE processHandle = {0};
			processHandle = OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_INFORMATION, FALSE, dwPID);
			if(processHandle)
			{
				GetRemoteCommandLineW(processHandle,szCommandLine, MAX_COMMAND_LINE);
				CloseHandle(processHandle);
				CStringW strW(szCommandLine);
				strW.TrimLeft();
				strW.TrimRight();
				if(!strW.IsEmpty())
					strRet = strW;
			}
		}
		catch (...)
		{
			ATLASSERT(0);
		}
		return strRet;
	}

	CString GetProcessName(DWORD dwPID)
	{
		CString strRet;
		if(dwPID==0)return strRet;
		try
		{
			HANDLE processHandle = {0};
			processHandle = OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_INFORMATION, FALSE, dwPID);
			if(processHandle)
			{
				WCHAR szModName[MAX_PATH+1]={0};
				if(GetModuleFileNameExW(processHandle, NULL, szModName, MAX_PATH))
				{
					CStringW strW((LPCWSTR)szModName);
					strRet = strW;
				}
				CloseHandle(processHandle);
			}
		}
		catch (...)
		{
			ATLASSERT(0);
		}
		return strRet;
	}

	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
};

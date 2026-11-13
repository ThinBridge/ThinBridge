#pragma once

static int launchAttempt;
static int successfulAttempt;

static LSTATUS WINAPI TestOpenLaunchKey(HKEY, LPCWSTR, DWORD, REGSAM, PHKEY)
{
    return ERROR_FILE_NOT_FOUND;
}

static void PrintLaunch(const char* api, LPCWSTR verb, LPCWSTR file, LPCWSTR arguments)
{
    std::printf("%s|%s|%s|%s\n", api,
        static_cast<LPCSTR>(CW2A(verb ? verb : L"", CP_UTF8)),
        static_cast<LPCSTR>(CW2A(file ? file : L"", CP_UTF8)),
        static_cast<LPCSTR>(CW2A(arguments ? arguments : L"", CP_UTF8)));
}

static HINSTANCE WINAPI TestShellExecute(HWND, LPCWSTR verb, LPCWSTR file,
    LPCWSTR arguments, LPCWSTR, INT)
{
    PrintLaunch("ShellExecute", verb, file, arguments);
    return reinterpret_cast<HINSTANCE>(++launchAttempt == successfulAttempt ? 33 : 32);
}

static BOOL WINAPI TestCreateProcess(LPCWSTR file, LPWSTR arguments, LPSECURITY_ATTRIBUTES,
    LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION)
{
    PrintLaunch("CreateProcess", nullptr, file, arguments);
    return ++launchAttempt == successfulAttempt;
}

static void RunLaunchTest(CRedirectApp& app)
{
    if (__argc != 5)
        std::exit(2);
    successfulAttempt = _wtoi(__wargv[4]);
    CCRre worker;
    const CString browser = __wargv[2];
    worker.m_strURL = __wargv[3];
    worker.m_strCustomBrowserPath = L"C:\\Browser Test\\custom.exe";
    app.m_strChrome_FullPath = L"C:\\Browser Test\\chrome.exe";
    app.m_strFirefox_FullPath = L"C:\\Browser Test\\firefox.exe";
    app.m_strIE_FullPath = L"C:\\Browser Test\\iexplore.exe";
    app.m_RedirectList.m_bDisableIE_DDE = TRUE;
    if (browser == L"chrome")
        worker.m_iSolutionType = PROC_LChrome;
    else if (browser == L"firefox")
        worker.m_iSolutionType = PROC_LFirefox;
    else if (browser == L"edge")
        worker.m_iSolutionType = PROC_LEdge;
    else if (browser == L"default")
        worker.m_iSolutionType = PROC_LDefaultBrowser;
    else if (browser == L"custom")
        worker.m_iSolutionType = PROC_LCustom;
    else if (browser == L"ie")
        worker.m_iSolutionType = PROC_LIE;
    else if (browser == L"rdp") {
        worker.m_iSolutionType = PROC_RDP;
        worker.m_strFilePath = worker.m_strURL;
    }
    else if (browser == L"vmware") {
        worker.m_iSolutionType = PROC_VMWARE;
        worker.m_strHorizon_ConnectionServerName = L"horizon.example";
        worker.m_strHorizon_AppName = L"Remote App";
    }
    else if (browser == L"citrix") {
        worker.m_iSolutionType = PROC_CITRIX;
        worker.m_strCitrix_AppName = L"Published App";
    }
    else
        std::exit(2);
    worker.Exec();
}

#undef ShellExecute
#define ShellExecute TestShellExecute
#undef CreateProcess
#define CreateProcess TestCreateProcess
#undef RegOpenKeyEx
#define RegOpenKeyEx TestOpenLaunchKey

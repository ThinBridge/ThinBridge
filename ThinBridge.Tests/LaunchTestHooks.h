#pragma once

static int launchAttempt;
static int successfulAttempt;

static bool testDdeConnection;
static bool testRegisteredClient;
static int registryKey;
static LPCWSTR settingPolicy;

static LSTATUS WINAPI TestReadSettingPolicy(HKEY root, LPCWSTR path, LPCWSTR name,
    DWORD flags, LPDWORD, PVOID data, LPDWORD size)
{
    if (root != HKEY_LOCAL_MACHINE || wcscmp(path, L"SOFTWARE\\Policies\\ThinBridge") ||
        wcscmp(name, L"DisableSetting") || flags != (RRF_RT_DWORD | RRF_SUBKEY_WOW6464KEY) ||
        *size != sizeof(DWORD))
        std::exit(2);
    if (!settingPolicy || wcscmp(settingPolicy, L"missing") == 0)
        return ERROR_FILE_NOT_FOUND;
    *static_cast<DWORD*>(data) = wcstoul(settingPolicy, nullptr, 10);
    return ERROR_SUCCESS;
}

static LSTATUS WINAPI TestOpenLaunchKey(HKEY root, LPCWSTR path, DWORD, REGSAM, PHKEY key)
{
    if (testRegisteredClient &&
        ((root == HKEY_CLASSES_ROOT && wcscmp(path, L"vmware-view\\shell\\open\\command") == 0) ||
         (root == HKEY_LOCAL_MACHINE && wcscmp(path, L"SOFTWARE\\Citrix\\Dazzle") == 0))) {
        registryKey = root == HKEY_CLASSES_ROOT ? 1 : 2;
        *key = reinterpret_cast<HKEY>(&registryKey);
        return ERROR_SUCCESS;
    }
    return ERROR_FILE_NOT_FOUND;
}

static LSTATUS WINAPI TestQueryLaunchValue(HKEY key, LPCWSTR name, LPDWORD, LPDWORD type,
    LPBYTE data, LPDWORD size)
{
    if (key != reinterpret_cast<HKEY>(&registryKey) ||
        wcscmp(name, registryKey == 1 ? L"" : L"Launcher") != 0)
        return ERROR_FILE_NOT_FOUND;
    LPCWSTR value = registryKey == 1 ? L"\"C:\\Remote Test\\vmware.exe\" \"%1\"" :
        L"C:\\Remote Test\\SelfService.exe";
    const DWORD required = static_cast<DWORD>((wcslen(value) + 1) * sizeof(wchar_t));
    if (data && *size < required)
        return ERROR_MORE_DATA;
    if (data)
        memcpy(data, value, required);
    *size = required;
    *type = REG_SZ;
    return ERROR_SUCCESS;
}

static LSTATUS WINAPI TestCloseLaunchKey(HKEY)
{
    return ERROR_SUCCESS;
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

static UINT WINAPI TestDdeInitialize(LPDWORD instance, PFNCALLBACK, DWORD, DWORD)
{
    *instance = 1;
    return DMLERR_NO_ERROR;
}

static HSZ WINAPI TestDdeCreateStringHandle(DWORD, LPCWSTR name, int)
{
    return reinterpret_cast<HSZ>(wcscmp(name, L"IEXPLORE") == 0 ? 1 :
        wcscmp(name, L"WWW_OpenURL") == 0 ? 2 : 0);
}

static HCONV WINAPI TestDdeConnect(DWORD instance, HSZ service, HSZ topic, PCONVCONTEXT)
{
    if (instance != 1 || service != reinterpret_cast<HSZ>(1) || topic != reinterpret_cast<HSZ>(2))
        std::exit(2);
    return testDdeConnection ? reinterpret_cast<HCONV>(1) : nullptr;
}

static BOOL WINAPI TestDdeFreeStringHandle(DWORD, HSZ) { return TRUE; }
static BOOL WINAPI TestDdeDisconnect(HCONV) { return TRUE; }
static BOOL WINAPI TestDdeUninitialize(DWORD) { return TRUE; }

static HDDEDATA WINAPI TestDdeClientTransaction(LPBYTE data, DWORD size, HCONV conversation,
    HSZ, UINT, UINT type, DWORD timeout, LPDWORD)
{
    if (conversation != reinterpret_cast<HCONV>(1) || type != XTYP_EXECUTE || timeout != 30000 ||
        size < sizeof(wchar_t) || size % sizeof(wchar_t))
        std::exit(2);
    const auto text = reinterpret_cast<LPCWSTR>(data);
    const auto characters = size / sizeof(wchar_t);
    if (text[characters - 1] != L'\0' || wcsnlen_s(text, characters) != characters - 1)
        std::exit(2);
    PrintLaunch("DdeClientTransaction", L"XTYP_EXECUTE", nullptr, text);
    return reinterpret_cast<HDDEDATA>(1);
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
    else if (browser == L"ie" || browser == L"ie-dde" || browser == L"ie-dde-unavailable") {
        worker.m_iSolutionType = PROC_LIE;
        app.m_RedirectList.m_bDisableIE_DDE = browser == L"ie";
        testDdeConnection = browser == L"ie-dde";
    }
    else if (browser == L"rdp") {
        worker.m_iSolutionType = PROC_RDP;
        worker.m_strFilePath = worker.m_strURL;
    }
    else if (browser == L"vmware" || browser == L"vmware-registered") {
        testRegisteredClient = browser == L"vmware-registered";
        worker.m_iSolutionType = PROC_VMWARE;
        worker.m_strHorizon_ConnectionServerName = L"horizon.example";
        worker.m_strHorizon_AppName = L"Remote App";
    }
    else if (browser == L"citrix" || browser == L"citrix-registered") {
        testRegisteredClient = browser == L"citrix-registered";
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
#undef RegQueryValueEx
#define RegQueryValueEx TestQueryLaunchValue
#define RegCloseKey TestCloseLaunchKey
#undef RegGetValue
#define RegGetValue TestReadSettingPolicy
#undef DdeInitialize
#define DdeInitialize TestDdeInitialize
#undef DdeCreateStringHandle
#define DdeCreateStringHandle TestDdeCreateStringHandle
#define DdeConnect TestDdeConnect
#define DdeFreeStringHandle TestDdeFreeStringHandle
#define DdeClientTransaction TestDdeClientTransaction
#define DdeDisconnect TestDdeDisconnect
#define DdeUninitialize TestDdeUninitialize

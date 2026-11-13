#pragma once

#include <cstdio>
#include <cstdlib>
#include "LaunchTestHooks.h"

static bool IsRoutingTest()
{
    return GetEnvironmentVariableW(L"THINBRIDGE_TEST_ROUTE", nullptr, 0) != 0;
}

static void PrepareRoutingTest(CRedirectApp& app)
{
    app.SettingConf.m_iSolutionType = PROC_RDP;
    app.SettingConf.m_strCustomBrowserPath.Empty();
    app.m_strExeFolderPath = L"C:\\ThinBridge Test\\";
}

static void PrintRoutingTest(CRedirectApp& app)
{
    std::printf("%u\n%s\n", static_cast<unsigned>(app.SettingConf.m_iSolutionType),
        static_cast<LPCSTR>(CW2A(app.SettingConf.m_strCustomBrowserPath, CP_UTF8)));
}

static bool IsRdpFileTest()
{
    return __argc == 5 && wcscmp(__wargv[1], L"--test-rdp-file") == 0;
}

static void RunThinBridgeTest(CRedirectApp& app)
{
    if (__argc == 3 && wcscmp(__wargv[1], L"--test-setting-policy") == 0) {
        settingPolicy = __wargv[2];
        std::printf("%d\n", app.IsSettingDisabled());
        return;
    }
    if (IsRdpFileTest()) {
        app.m_strRDP_FileFullPath = __wargv[2];
        app.m_RedirectList.m_strRDP_ServerName = L"rdp.example";
        app.m_RedirectList.m_strRemoteAppName = L"Remote App";
        app.m_RedirectList.m_strRemoteAppPath = L"C:\\Remote App\\app.exe";
        const CString mode = __wargv[4];
        if (mode != L"desktop" && mode != L"remote-app" && mode != L"no-default-command")
            std::exit(2);
        app.m_RedirectList.m_bRemoteAppMode = mode != L"desktop";
        if (mode == L"remote-app")
            app.m_RedirectList.m_strRemoteAppCommandLine = L"/open";
        app.m_RedirectList.m_bRedirect_Clipboard = TRUE;
        app.m_RedirectList.m_bRedirect_Drive = TRUE;
        app.m_strRdp_Template_File_Data = L"screen mode id:i:2\n";
        app.m_strRdp_ReadOnce_File_Data = L"prompt for credentials:i:1\n";
        CString command = __wargv[3];
        app.CreateRDPFile(command);
        std::puts("RDP file generated");
        return;
    }
    if (__argc > 1 && wcscmp(__wargv[1], L"--test-launch") == 0) {
        RunLaunchTest(app);
        return;
    }
    if (const auto* logPath = _wgetenv(L"THINBRIDGE_TEST_LOOP_LOG")) {
        if (__argc != 2)
            std::exit(2);
        app.m_LoopBlock.SetFilePath(logPath);
        std::printf("%u\n", app.m_LoopBlock.CheckLoop(__wargv[1]));
        return;
    }

    app.InitCommandParamOptionParam();
    std::printf("%s\n%s\n%s\n",
        static_cast<LPCSTR>(CW2A(app.m_CommandParam, CP_UTF8)),
        static_cast<LPCSTR>(CW2A(app.m_OptionParam, CP_UTF8)),
        static_cast<LPCSTR>(CW2A(app.m_EtcParam, CP_UTF8)));
}

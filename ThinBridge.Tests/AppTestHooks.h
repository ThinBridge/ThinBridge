#pragma once

#include <cstdio>
#include <cstdlib>

static void RunThinBridgeTest(CRedirectApp& app)
{
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

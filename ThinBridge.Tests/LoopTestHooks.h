#pragma once

inline HANDLE WINAPI TestOpenLoopEvent(DWORD, BOOL, LPCWSTR)
{
    if (GetEnvironmentVariableW(L"THINBRIDGE_TEST_LOOP_EVENT", nullptr, 0))
        return CreateEventW(nullptr, TRUE, TRUE, nullptr);
    return nullptr;
}

inline DWORD WINAPI TestLoopTickCount()
{
    return 60000;
}

#pragma push_macro("OpenEvent")
#pragma push_macro("GetTickCount")
#undef OpenEvent
#define OpenEvent TestOpenLoopEvent
#define GetTickCount TestLoopTickCount

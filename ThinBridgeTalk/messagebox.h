/*
 * This header file allows to call MessageBoxTimeout() Win32 API.
 * If the function is not available, it falls back to MessageBox().
 */
#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <Windows.h>
#include <tchar.h>

#ifdef _UNICODE
#define MBT_NAME "MessageBoxTimeoutW"
#else
#define MBT_NAME "MessageBoxTimeoutA"
#endif

typedef int (__stdcall *TMessageBoxTimeout)(HWND, LPCTSTR, LPCTSTR, UINT, WORD, DWORD);

/*
 * Show a message box, and automatically close it after N seconds.
 * Return a MessageBox status code (e.g. MB_OK).
 */
static int ShowMessageBoxTimeout(LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, int iTimeOutSec)
{
	int ret;
	int msec = iTimeOutSec * 1000;
	TMessageBoxTimeout MessageBoxTimeout;
	HMODULE hModule;

	/*
	 * MessageBoxTimeout is officially exposed by Windows.
         * Peek user32.dll to get the function symbol.
	 */
	hModule = GetModuleHandle(_T("user32.dll"));
	if (hModule) {
		MessageBoxTimeout = (TMessageBoxTimeout) GetProcAddress(hModule, MBT_NAME);
		if (MessageBoxTimeout) {
			return MessageBoxTimeout(NULL, lpText, lpCaption, uType, LANG_NEUTRAL, msec);
		}
	}

	/*
	 * In an unlikely event where user32.dll is not loaded,
	 * try LoadLibrary().
	 */
	hModule = LoadLibrary(_T("user32.dll"));
	if (hModule) {
		MessageBoxTimeout = (TMessageBoxTimeout) GetProcAddress(hModule, MBT_NAME);
		if (MessageBoxTimeout) {
			ret = MessageBoxTimeout(NULL, lpText, lpCaption, uType, LANG_NEUTRAL, msec);
			FreeLibrary(hModule);
			return ret;
		}
		FreeLibrary(hModule);
	}

	return MessageBox(NULL, lpText, lpCaption, uType);  /* fallback! */
}
#endif

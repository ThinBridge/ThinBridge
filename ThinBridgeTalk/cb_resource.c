/*
 * Implements `Resource` command.
 *
 * (1) Receive messages in the format of "R [BROWSER] [NTABS]"
 * (2) Check the system limit in ResourceCap.ini. Return `closeTab: 1`
 *　　 if the maximum limit has been exceeded.
 *
 * Example:
 *
 * >>> cb_resource("R edge 10")
 * {"status":"OK", "closeTab": 1}
 */

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "internal.h"
#include "messagebox.h"

#define HKCU HKEY_CURRENT_USER
#define TB_RCAP _T("SOFTWARE\\Policies\\ThinBridge\\RCAP")

struct config {
	DWORD tab_enabled;
	DWORD tab_max;
	DWORD tab_warn;
	LPTSTR tab_max_msg;
	DWORD tab_max_msg_time;
	LPTSTR tab_warn_msg;
	DWORD tab_warn_msg_time;
};

static LPTSTR decode_str(LPTSTR str)
{
       TCHAR c;
       int isescape = 0;
       int len = _tcslen(str);
       int i = 0;
       int j = 0;

       for (i = 0; i < len; i++) {
               c = str[i];
               if (c == _T('\\')) {
                       isescape = 1;
               } else if (isescape && c == _T('n')) {
                       str[j++] =_T('\n');
                       isescape = 0;
               } else if (isescape && c == _T('r')) {
                       // Ignore '\r' to convert '\\r\\n' to '\n'
                       isescape = 0;
               } else {
                       str[j++] = c;
                       isescape = 0;
               }
       }
       str[j] = _T('\0');
       return str;
}

/*
 * Read a string from Windows registry. This function allocates a
 * new buffer (so you must free it later).
 *
 * Note: `out` gets modified if (and only if) on success.
 */
static int get_winreg_str(HKEY hkey, LPCTSTR key, LPCTSTR subkey, LPTSTR *out)
{
	LSTATUS ret;
	LPTSTR str;
	DWORD size;

	ret = RegGetValue(hkey, key, subkey, RRF_RT_REG_SZ, NULL, NULL, &size);
	if (ret != ERROR_SUCCESS) {
		return -1;
	}

	str = malloc(sizeof(TCHAR) * (size + 1));
	if (str == NULL) {
		return -1;
	}

	ret = RegGetValue(hkey, key, subkey, RRF_RT_REG_SZ, NULL, str, &size);
	if (ret != ERROR_SUCCESS) {
		free(str);
		return -1;
	}
	str[size] = _T('\0');
	*out = decode_str(str);
	return 0;
}

/*
 * Read a dword from Windows registry.
 */
static int get_winreg_dword(HKEY hkey, LPCSTR key, LPCSTR subkey, DWORD *out)
{
	LSTATUS ret;
	DWORD val;
	DWORD size = sizeof(DWORD);

	ret = RegGetValue(hkey, key, subkey, RRF_RT_REG_DWORD, NULL, &val, &size);
	if (ret != ERROR_SUCCESS) {
		return -1;
	}
	*out = val;
	return 0;
}

static void init_config(struct config *conf)
{
	memset(conf, 0, sizeof(struct config));

	get_winreg_dword(HKCU, TB_RCAP,
		_T("EnableTabLimit"), &conf->tab_enabled);

	get_winreg_dword(HKCU, TB_RCAP,
		_T("TabLimit_MAX"), &conf->tab_max);

	get_winreg_str(HKCU, TB_RCAP,
		_T("TabLimit_MAX_MSG"), &conf->tab_max_msg);

	get_winreg_dword(HKCU, TB_RCAP,
		_T("TabLimit_MAX_MSG_TIME"), &conf->tab_max_msg_time);

	get_winreg_dword(HKCU, TB_RCAP,
		_T("TabLimit_WARNING"), &conf->tab_warn);

	get_winreg_str(HKCU, TB_RCAP,
		_T("TabLimit_WARNING_MSG"), &conf->tab_warn_msg);

	get_winreg_dword(HKCU, TB_RCAP,
		_T("TabLimit_WARNING_MSG_TIME"), &conf->tab_warn_msg_time);
}

static void dump_config(struct config *conf)
{
	_tprintf(_T("===============================\n"));
	_tprintf(_T("EnableTabLimit           : %i\n"), conf->tab_enabled);
	_tprintf(_T("TabLimit_MAX             : %i\n"), conf->tab_max);
	_tprintf(_T("TabLimit_MAX_MSG         : %s\n"), conf->tab_max_msg);
	_tprintf(_T("TabLimit_MAX_MSG_TIME    : %i\n"), conf->tab_max_msg_time);
	_tprintf(_T("TabLimit_WARNING         : %i\n"), conf->tab_warn);
	_tprintf(_T("TabLimit_WARNING_MSG     : %s\n"), conf->tab_warn_msg);
	_tprintf(_T("TabLimit_WARNING_MSG_TIME: %i\n"), conf->tab_warn_msg_time);
	_tprintf(_T("===============================\n"));
}

static void destroy_config(struct config *conf)
{
	free(conf->tab_warn_msg);
	conf->tab_warn_msg = NULL;

	free(conf->tab_max_msg);
	conf->tab_max_msg = NULL;
}

static void show_warning(LPCTSTR message, int sec)
{
	HANDLE hMutex;
	DWORD ret;

	/* Use mutex to show a single MessageBox at one time */
	hMutex = CreateMutex(NULL, FALSE, _T("Local\\TB_RCAP_ALERT"));
	if (hMutex) {
		ret = WaitForSingleObject(hMutex, 300);   /* 300 msec */
		if (ret != WAIT_TIMEOUT) {
			ShowMessageBoxTimeout(message, _T("ThinBridge"),
						MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL, sec);
		}
		CloseHandle(hMutex);
	}
}

int cb_resource(char *cmd)
{
	int ntabs;
	char *space;
	char *browser;
	int closeTab = 0;
	struct config conf;

	/*
	 * R edge 12
	 *   ----
	 */
	browser = cmd + 2;
	space = strchr(browser, ' ');
	if (space == NULL) {
		fprintf(stderr, "invalid query request '%s'", cmd);
		return -1;
	}
	*space = '\0';

	/*
	 *  R edge 12
	 *         --
	 */
	ntabs = atoi(space + 1);

	/*
	 * Perform resource check
	 */
	init_config(&conf);

	/* Easy debugging via "R debug 1" */
	if (strcmp(browser, "debug") == 0) {
		dump_config(&conf);
	}

	if (conf.tab_enabled) {
		if (conf.tab_max < ntabs) {
			show_warning(conf.tab_max_msg, conf.tab_max_msg_time);
			closeTab = 1;
		}
		else if (conf.tab_warn <= ntabs) {
			show_warning(conf.tab_warn_msg, conf.tab_warn_msg_time);
		}
	}

	talk_response("{\"status\":\"OK\",\"closeTab\":%i}", closeTab);

	destroy_config(&conf);
	return 0;
}

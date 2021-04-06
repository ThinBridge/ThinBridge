#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "internal.h"

static int get_ThinBridgeExtensionExecfile(char *buf, DWORD size)
{
	int ret;
	DWORD len = size;
	memset(buf, 0, size);

	ret = RegGetValueA(HKEY_LOCAL_MACHINE, "SOFTWARE\\ThinBridge", "ExtensionExecfile", RRF_RT_REG_SZ,
	                   NULL, buf, &size);
	if (ret != ERROR_SUCCESS) {
	    fprintf(stderr, "cannot read %s (%i)", "SOFTWARE\\ThinBridge", ret);
	    return -1;
	}
	buf[len - 1] = '\0';
	return 0;
}

static int open_url(char *browser, char *path, char *url)
{
	int ret;
	struct strbuf sb = {0};
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	DWORD dwCreationFlags;

	memset(&pi, 0, sizeof(pi));
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	strbuf_concat(&sb, " \"");
	strbuf_concat(&sb, url);
	strbuf_putchar(&sb, '"');
	strbuf_putchar(&sb, '\0');

	/*
	 * We need this on Firefox to prevent the new process from
	 * getting killed. This behaviour is documented on MDN:
	 *
	 * https://developer.mozilla.org/en-US/docs/Mozilla/Add-ons/WebExtensions/Native_messaging
	 */
	if (strcmp(browser, "firefox") == 0) {
		dwCreationFlags = CREATE_BREAKAWAY_FROM_JOB;
	}
	else {
		dwCreationFlags = CREATE_NEW_PROCESS_GROUP;
	}

	ret = CreateProcess(path,   /* lpApplicationName */
	                    sb.buf, /* lpCommandLine */
	                    NULL,   /* lpProcessAttributes  */
	                    NULL,   /* lpThreadAttributes */
	                    FALSE,  /* bInheritHandles */
	                    dwCreationFlags,
	                    NULL,   /* lpEnvironment */
	                    NULL,   /* lpCurrentDirectory */
	                    &si,    /* lpStartupInfo */
	                    &pi);   /* lpProcessInformation */
	if (ret == 0) {
	    fprintf(stderr, "cannot exec '%s %s' (%d)", path, sb.buf, GetLastError());
	    free(sb.buf);
	    return -1;
	}

	free(sb.buf);
	return 0;
}

int cb_query(char *cmd)
{
	char *url;
	char path[MAX_PATH];
	char *space;
	char *browser;

	if (strlen(cmd) < 3) {
		fprintf(stderr, "command too short '%s'", cmd);
		return -1;
	}

	/*
	 *  Q edge https://example.com
	 *    ----
	 */
	browser = cmd + 2;

	space = strchr(browser, ' ');
	if (space == NULL) {
		fprintf(stderr, "invalid query request '%s'", cmd);
		return -1;
	}
	*space = '\0';

	/*
	 *  Q edge https://example.com
	 *         -------------------
	 */
	url = space + 1;

	if (get_ThinBridgeExtensionExecfile(path, MAX_PATH) < 0)
		return -1;

	if (open_url(browser, path, url) < 0)
		return -1;

	talk_response("{\"status\":\"OK\"}");
	return 0;
}

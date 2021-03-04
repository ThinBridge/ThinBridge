#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "host.h"

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

static int open_url(char *path, char *url)
{
    int ret;
    struct strbuf sb = {0};
    PROCESS_INFORMATION pi;
    STARTUPINFO si;

    memset(&pi, 0, sizeof(pi));
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    strbuf_concat(&sb, " \"");
    strbuf_concat(&sb, url);
    strbuf_putchar(&sb, '"');
    strbuf_putchar(&sb, '\0');

    ret = CreateProcess(path,   /* lpApplicationName */
                        sb.buf, /* lpCommandLine */
                        NULL,   /* lpProcessAttributes  */
                        NULL,   /* lpThreadAttributes */
                        FALSE,  /* bInheritHandles */
                        CREATE_NEW_PROCESS_GROUP,
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

#define PREFIX "Q edge "
#define PREFIX_LEN 7

int cb_query(char *cmd)
{
    char *url;
    char path[MAX_PATH];

    if (strncmp(cmd, PREFIX, PREFIX_LEN) != 0) {
        fprintf(stderr, "invalid command: '%s'", cmd);
        return -1;
    }
    url = cmd + PREFIX_LEN;


    if (get_ThinBridgeExtensionExecfile(path, MAX_PATH) < 0)
        return -1;
    if (open_url(path, url) < 0)
        return -1;

    talk_response("{\"status\":\"OK\"}");
    return 0;
}

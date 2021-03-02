#include <Windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <io.h>
#include <fcntl.h>

/*
 * Return a response to the Chrome extension.
 *
 * Host programs are expected to return the following data.
 * (4 byte header + JSON payload)
 *
 *   +--+--+--+--+=========================+
 *   |  LENGTH   |  JSON paylod ...        |
 *   +--+--+--+--+=========================+
 */
void talk_response(const char *msg, ...)
{
    va_list args;
    int len = 0;

    /* Prevent Windows from translating CRLF */
    setmode(_fileno(stdout), O_BINARY);

    /* Print 4-byte header */
    va_start(args, msg);
    len = vsnprintf(NULL, 0, msg, args);
    fwrite(&len, sizeof(len), 1, stdout);
    va_end(args);

    /* Push the remaining body to stdout */
    va_start(args, msg);
    vfprintf(stdout, msg, args);
    fflush(stdout);
    va_end(args);
}

int cb_config(const char *cmd)
{
    talk_response("{\"status\":\"OK\",\"config\":{"
                  "  \"URLPatterns\":[[\"http*://*.example.com/*\", \"edge\"]],"
                  "  \"URLExcludePatterns\":[],"
                  "  \"CloseEmptyTab\":1"
                  "}}");
    return 0;
}

int cb_query(const char *cmd)
{
    const char *url;
    int ret;

    url = strchr(cmd + 2, ' ');
    if (url == NULL) {
        return -1;
    }

    ret = (int) ShellExecuteA(NULL, "open", "iexplore.exe", url, NULL, SW_SHOW);
    if (ret > 32) {
        return -1;
    }

    talk_response("{\"status\":\"OK\"}");
    return 0;
}


int main(int argc, char *argv[])
{
    char *cmd;
    char *buf;
    int len;
    int ret;

    ret = fread(&len, sizeof(len), 1, stdin);
    if (ret < 1) {
        fprintf(stderr, "cannot read %i bytes", sizeof(len));
        return -1;
    }

    if (len < 3) {
        fprintf(stderr, "command too short (%i bytes)", len);
        return -1;
    }

    buf = (char *) calloc(1, len);
    if (buf == NULL) {
        perror("calloc");
        return -1;
    }

    ret = fread(buf, len, 1, stdin);
    if (ret < 1) {
        fprintf(stderr, "cannot read %i bytes", len);
        free(buf);
        return -1;
    }

    /* Trim double-quotes from input */
    buf[len - 1] = '\0';
    cmd = buf + 1;

    /*
     * Talk protocol defines two operations.
     *
     *  "C edge"
     *    ... Read and return ThinBridgeBHO.ini.
     *  "Q edge https://google.com"
     *    ... Open URL in another browser.
     */
    switch (cmd[0]) {
    case L'C':
        ret = cb_config(cmd);
        break;
    case L'Q':
        ret = cb_query(cmd);
        break;
    default:
        fprintf(stderr, "unknown command '%s'", cmd);
        break;
    }
    free(buf);
    return 0;
}

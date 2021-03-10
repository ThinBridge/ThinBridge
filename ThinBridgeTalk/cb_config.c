#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "host.h"

struct config {
    int only_main_frame;
    int ignore_query_string;
    struct strbuf urls;
};

static int get_ThinBridgeRulefile(char *buf, DWORD size)
{
    int ret;
    DWORD len = size;
    memset(buf, 0, size);

    ret = RegGetValueA(HKEY_LOCAL_MACHINE,"SOFTWARE\\ThinBridge","Rulefile", RRF_RT_REG_SZ,
                       NULL, buf, &size);
    if (ret != ERROR_SUCCESS) {
        fprintf(stderr, "cannot read %s (%i)","SOFTWARE\\ThinBridge", ret);
        return -1;
    }
    buf[len - 1] = '\0';
    return 0;
}

static char *read_file(const char *path)
{
    int ret;
    FILE *fp;
    char *buf;
    struct stat st;

    fp = fopen(path, "rb");
    if (fp == NULL)
        return NULL;

    ret = fstat(_fileno(fp), &st);
    if (ret == -1) {
        fclose(fp);
        return NULL;
    }

    if (!(st.st_mode & S_IFREG)) {
        fclose(fp);
        return NULL;
    }

    buf = calloc(st.st_size + 1, 1);
    if (!buf) {
        fclose(fp);
        return NULL;
    }

    ret = fread(buf, st.st_size, 1, fp);
    if (ret != 1) {
        free(buf);
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    return buf;
}

static void parse_conf(char *data, struct config *conf)
{
    char *line;
    int active = 0;
    int global = 0;
    int mark = -1;
    int custom20 = 0;


    line = strtok(data, "\r\n");
    while (line) {
        switch (line[0]) {
        case ';':
        case '#':
            break;
        case '[':
            active = 0;
            global = 0;
            mark = -1;
            custom20 = 0;

            if (strcmp(line, "[Edge]") == 0) {
                mark = conf->urls.count;
                active = 1;
            }
            else if (strcmp(line, "[CUSTOM18]") == 0) {
                mark = conf->urls.count;
                active = 1;
            }
            else if (strcmp(line, "[CUSTOM20]") == 0) {
                mark = conf->urls.count;
                active = 1;
            	custom20 = 1;
            }
            else if (strcmp(line, "[GLOBAL]") == 0) {
                global = 1;
            }
            break;
        case '@':
            if (active && strcmp(line, "@DISABLED") == 0) {
                conf->urls.count = mark;
                active = 0;
            }
            if(active && custom20){
              if (strstr(line,"@BROWSER_PATH:")) {
                 if(strcmp(line, "@BROWSER_PATH:Edge") == 0)
                   active = 1;
                 else{
                   conf->urls.count = mark;
                   active = 0;
                 }
               }
            }
            else if (global && strcmp(line, "@TOP_PAGE_ONLY") == 0) {
                conf->ignore_query_string = 1;
            }
            else if (global && strcmp(line, "@UNTRUSTED_ZONE") == 0) {
                conf->only_main_frame = 1;
            }
            break;
        case '-':
            if (strstr(line,"-#")==line) {
               break;
            }
        default:
            if (active) {
                strbuf_concat(&conf->urls, line);
                strbuf_putchar(&conf->urls, '\n');
            }
            break;
        }
        line = strtok(NULL, "\r\n");
    }
    strbuf_putchar(&conf->urls, '\0');
}


static char *dump_json(struct config *conf)
{
    struct strbuf sb = {0};
    char *ptr;
    char *end;
    char buf[64];
    int need_comma;

    strbuf_concat(&sb, "{\"CloseEmptyTab\":1,");

    /*
     * OnlyMainFrame
     */
    strbuf_concat(&sb, "\"OnlyMainFrame\":");
    strbuf_concat(&sb, _itoa(conf->only_main_frame, buf, 10));
    strbuf_putchar(&sb, ',');

    /*
     * IgnoreQueryString
     */
    strbuf_concat(&sb, "\"IgnoreQueryString\":");
    strbuf_concat(&sb, _itoa(conf->ignore_query_string, buf, 10));
    strbuf_putchar(&sb, ',');

    /*
     * URLPatterns
     */
    strbuf_concat(&sb, "\"URLPatterns\":[");
    ptr = conf->urls.buf;
    need_comma = 0;
    while (*ptr) {
        end = strchr(ptr, '\n');
        if (ptr[0] != '-') {
            if (need_comma)
                strbuf_putchar(&sb, ',');
            strbuf_concat(&sb, "[");
            strbuf_concat_jsonstr(&sb, ptr, end - ptr);
            strbuf_concat(&sb, ",\"edge\"]");
            need_comma = 1;
        }
        ptr = end + 1;
    }
    strbuf_concat(&sb, "],");

    /*
     * URLExcludePatterns
     */
    strbuf_concat(&sb, "\"URLExcludePatterns\":[");
    ptr = conf->urls.buf;
    need_comma = 0;
    while (*ptr) {
        end = strchr(ptr, '\n');
        if (ptr[0] == '-') {
            if (need_comma)
                strbuf_putchar(&sb, ',');
            strbuf_concat(&sb, "[");
            strbuf_concat_jsonstr(&sb, ptr + 1, end - ptr - 1);
            strbuf_concat(&sb, ",\"edge\"]");
            need_comma = 1;
        }
        ptr = end + 1;
    }
    strbuf_concat(&sb, "]}");
    strbuf_putchar(&sb, '\0');
    return sb.buf;
}

int cb_config(char *cmd)
{
    char *data;
    char *json;
    struct config conf = {0};

    char path[MAX_PATH] = {0};

    if (get_ThinBridgeRulefile(path, MAX_PATH) < 0)
        return -1;

    data = read_file(path);
    if (data == NULL) {
        fprintf(stderr, "cannot read %s", path);
        return -1;
    }

    parse_conf(data, &conf);

    json = dump_json(&conf);

    talk_response("{\"status\":\"OK\",\"config\":%s}", json);

    free(data);
    free(json);
    free(conf.urls.buf);
    return 0;
}

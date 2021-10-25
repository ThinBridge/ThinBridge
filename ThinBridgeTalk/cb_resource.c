/*
 * Implements `Resource` command. Returns two information:
 *
 * (1) system limit defined in ResourceCap.ini
 * (2) memory amount used by the given browser (in MB)
 *
 * Example:
 *
 * >>> cb_resource("R edge")
 * {"status":"OK", "config": {"MemLimitMax":1024, ...}, "MemUsedMB": 282}
 */

#include <Windows.h>
#include <Dbghelp.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "internal.h"

#pragma warning(disable : 4996)
#define startswith(s, t) (strstr((s), (t)) == (s))
#define CONFIG_PATH "C:\\Program Files\\ThinBridge\\ResourceCap.ini"

struct config {
	int tab_enabled;
	int tab_max;
	int tab_warn;
	int mem_enabled;
	int mem_max;
	int mem_warn;
};

static int get_config_path(char *buf, DWORD size)
{
	strncpy(buf, CONFIG_PATH, size);
	/*
	int ret;
	ret = RegGetValueA(HKEY_LOCAL_MACHINE, "SOFTWARE\\ThinBridge", "RCAPFile", RRF_RT_REG_SZ,
			   NULL, buf, &size);
	if (ret != ERROR_SUCCESS) {
		fprintf(stderr, "cannot read %s (%i)", "SOFTWARE\\ThinBridge", ret);
		return -1;
	}
	*/
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

	line = strtok(data, "\r\n");
	while (line) {
		if (startswith(line, "EnableIETabLimit=")) {
			conf->tab_enabled = atoi(line + strlen("EnableIETabLimit="));
		} else if (startswith(line, "IETabLimit_WARNING=")) {
			conf->tab_warn = atoi(line + strlen("IETabLimit_WARNING="));
		} else if (startswith(line, "IETabLimit_MAX=")) {
			conf->tab_max = atoi(line + strlen("IETabLimit_MAX="));
		} else if (startswith(line, "EnableIEMemLimit=")) {
			conf->mem_enabled = atoi(line + strlen("EnableIEMemLimit="));
		} else if (startswith(line, "IEMemLimit_WARNING=")) {
			conf->mem_warn = atoi(line + strlen("IEMemLimit_WARNING="));
		} else if (startswith(line, "IEMemLimit_MAX=")) {
			conf->mem_max = atoi(line + strlen("IEMemLimit_MAX="));
		}
		line = strtok(NULL, "\r\n");
	}
}

static char* dump_json(struct config *conf)
{
	struct strbuf sb = {0};
	char buf[64];

	/* TabLimitEnabled */
	strbuf_concat(&sb, "{\"TabLimitEnabled\":");
	strbuf_concat(&sb, _itoa(conf->tab_enabled, buf, 10));

	/* TabLimitMax */
	strbuf_concat(&sb, ",\"TabLimitMax\":");
	strbuf_concat(&sb, _itoa(conf->tab_max, buf, 10));

	/* TabLimitWarn */
	strbuf_concat(&sb, ",\"TabLimitWarn\":");
	strbuf_concat(&sb, _itoa(conf->tab_warn, buf, 10));

	/* MemLimitEnabled */
	strbuf_concat(&sb, ",\"MemLimitEnabled\":");
	strbuf_concat(&sb, _itoa(conf->mem_enabled, buf, 10));

	/* MemLimitEnabled */
	strbuf_concat(&sb, ",\"MemLimitMax\":");
	strbuf_concat(&sb, _itoa(conf->mem_max, buf, 10));

	/* MemLimitEnabled */
	strbuf_concat(&sb, ",\"MemLimitWarn\":");
	strbuf_concat(&sb, _itoa(conf->mem_warn, buf, 10));
	strbuf_concat(&sb, "}");

	return sb.buf;
}

/*
 * Return the amount of memory used by the program (in MB).
 *
 * >>> find_memory_usage("msedge.exe")
 * 10224  // mb
 */
int find_memory_usage(char *procname)
{
	long long bytes = 0;
	HANDLE hs;
	HANDLE hp;
	PROCESS_MEMORY_COUNTERS_EX pmc = {0};
	PROCESSENTRY32 entry = {0};
	BOOL found;

	pmc.cb = sizeof(pmc);
	entry.dwSize = sizeof(entry);

	hs = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hs == INVALID_HANDLE_VALUE) {
		return -1;
	}

	found = Process32First(hs, &entry);
	while (found) {
		if (strcmp(procname, entry.szExeFile) == 0) {
			hp = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, entry.th32ProcessID);
			if (hp != INVALID_HANDLE_VALUE) {
				if (GetProcessMemoryInfo(hp, (PROCESS_MEMORY_COUNTERS *) &pmc, sizeof(pmc))) {
					bytes  += pmc.PrivateUsage;
				}
				CloseHandle(hp);
			}
		}
		found = Process32Next(hs, &entry);
	}
	CloseHandle(hs);
	return (int) (bytes / 1024 / 1024);
}

int cb_resource(char *cmd)
{
	char *data;
	char *json;
	int mb;
	struct config conf = {0};
	char path[MAX_PATH] = {0};
	char *browser;

	/*
	 * Load config from Resource.ini
	 *
	 * If the file being non-existent, it means that the resource cap
         * feature is not enabled, so we just return an empty object.
	 */
	if (get_config_path(path, MAX_PATH)) {
		talk_response("{\"status\": \"OK\"}");
		return 0;
	}

	data = read_file(path);
	if (data == NULL) {
		talk_response("{\"status\": \"OK\"}");
		return 0;
	}

	parse_conf(data, &conf);
	json = dump_json(&conf);

	/*
	 * "R edge"
	 *    ----
	 */
	browser = cmd + 2;
	if (strcmp(browser, "edge") == 0) {
		mb = find_memory_usage("msedge.exe");
	} else if (strcmp(browser, "chrome") == 0) {
		mb = find_memory_usage("chrome.exe");
	} else if (strcmp(browser, "firefox") == 0) {
		mb = find_memory_usage("firefox.exe");
	} else {
		return -1;
	}

	talk_response("{\"status\":\"OK\",\"config\":%s,\"MemUsed\":%i}", json, mb);

	free(data);
	free(json);
	return 0;
}

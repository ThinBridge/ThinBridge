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

struct config {
	int tab_enabled;
	int tab_max;
	int tab_warn;
	char *tab_max_msg;
	char *tab_warn_msg;
	int mem_enabled;
	int mem_max;
	int mem_warn;
	char *mem_max_msg;
	char *mem_warn_msg;
};

static int get_config_path(char *buf, DWORD size)
{
	int ret;
	ret = RegGetValueA(HKEY_LOCAL_MACHINE, "SOFTWARE\\ThinBridge", "RCAPfile", RRF_RT_REG_SZ,
			   NULL, buf, &size);
	if (ret != ERROR_SUCCESS) {
		fprintf(stderr, "cannot read %s (%i)", "SOFTWARE\\ThinBridge", ret);
		return -1;
	}
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

static char *decode_str(char *str)
{
	char c;
	int isescape = 0;
	int len = strlen(str);
	int i = 0;
	int j = 0;

	for (i = 0; i < len; i++) {
		c = str[i];
		if (c == '\\') {
			isescape = 1;
		} else if (isescape && c == 'n') {
			str[j++] = '\n';
			isescape = 0;
		} else if (isescape && c == 'r') {
			// Ignore '\r' to convert '\\r\\n' to '\n'
			isescape = 0;
		} else {
			str[j++] = c;
			isescape = 0;
		}
	}
	str[j] = '\0';
	return str;
}

/*
 * Parse ResourceCap.ini into "config"
 */
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
		} else if (startswith(line, "IETabLimit_WARNING_MSG=")) {
			conf->tab_warn_msg = decode_str(line + strlen("IETabLimit_WARNING_MSG="));
		} else if (startswith(line, "IETabLimit_MAX_MSG=")) {
			conf->tab_max_msg = decode_str(line + strlen("IETabLimit_MAX_MSG="));
		} else if (startswith(line, "IEMemLimit_WARNING_MSG=")) {
			conf->mem_warn_msg = decode_str(line + strlen("IEMemLimit_WARNING_MSG="));
		} else if (startswith(line, "IEMemLimit_MAX_MSG=")) {
			conf->mem_max_msg = decode_str(line + strlen("IEMemLimit_MAX_MSG="));
		}
		line = strtok(NULL, "\r\n");
	}
}

/*
 * Return the executable name for the browser.
 */
static char *get_process_name(const char *browser)
{
	if (strcmp(browser, "edge") == 0) {
		return "msedge.exe";
	} else if (strcmp(browser, "chrome") == 0) {
		return "chrome.exe";
	} else if (strcmp(browser, "firefox") == 0) {
		return "firefox.exe";
	}
	return NULL;
}

/*
 * Return the amount of memory used by the program (in MB).
 *
 * >>> find_memory_usage("msedge.exe")
 * 10224  // mb
 */
static int find_memory_usage(char *browser)
{
	long long bytes = 0;
	HANDLE hs;
	HANDLE hp;
	PROCESS_MEMORY_COUNTERS_EX pmc = {0};
	PROCESSENTRY32 entry = {0};
	char *procname;
	BOOL found;

	pmc.cb = sizeof(pmc);
	entry.dwSize = sizeof(entry);

	procname = get_process_name(browser);
	if (procname == NULL) {
		return -1;
	}

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

static int show_warning(const char *msg)
{
	return MessageBox(NULL, msg, "ThinBridge", MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL);
}

/*
 * Check if the current resource usage is within the limit.
 */
static int check_resource(char *browser, int ntabs, int *closeTab)
{
	struct config conf = {0};
	char path[MAX_PATH] = {0};
	char *data = NULL;
	int memused;

	if (get_config_path(path, MAX_PATH))
		return -1;

	data = read_file(path);
	if (data == NULL)
		return -1;

	parse_conf(data, &conf);

	/* Continue on failure ... (memused = -1) */
	memused = find_memory_usage(browser);

	if (conf.tab_enabled && conf.tab_max <= ntabs) {
		show_warning(conf.tab_max_msg);
		*closeTab = 1;
	}
	else if (conf.mem_enabled && conf.mem_max <= memused) {
		show_warning(conf.mem_max_msg);
		*closeTab = 1;
	}
	else if (conf.tab_enabled && conf.tab_warn <= ntabs) {
		show_warning(conf.tab_warn_msg);
	}
	else if (conf.mem_enabled && conf.mem_warn <= memused) {
		show_warning(conf.tab_max_msg);
	}

	free(data);
	return 0;
}

int cb_resource(char *cmd)
{
	int ntabs;
	char *space;
	char *browser;
	int closeTab = 0;

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

	if (check_resource(browser, ntabs, &closeTab)) {
		closeTab = 0;
	}

	talk_response("{\"status\":\"OK\",\"closeTab\":%i}", closeTab);
	return 0;
}

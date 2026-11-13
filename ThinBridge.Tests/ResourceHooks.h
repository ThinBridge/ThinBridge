static LSTATUS test_read_policy(HKEY key, LPCTSTR subkey, LPCTSTR name,
	DWORD flags, PVOID data, LPDWORD size)
{
	DWORD value;
	LPCTSTR message;
	DWORD required;
	char policy[2];

	if (key != HKEY_CURRENT_USER || _tcscmp(subkey, _T("SOFTWARE\\Policies\\ThinBridge\\RCAP")))
		return ERROR_INVALID_PARAMETER;
	if (GetEnvironmentVariableA("THINBRIDGE_TEST_POLICY", policy, sizeof(policy)) != 1)
		return ERROR_FILE_NOT_FOUND;
	if (flags == RRF_RT_REG_DWORD) {
		if (!_tcscmp(name, _T("EnableTabLimit")))
			value = policy[0] == '1';
		else if (!_tcscmp(name, _T("TabLimit_MAX")))
			value = 10;
		else if (!_tcscmp(name, _T("TabLimit_WARNING")))
			value = 8;
		else if (!_tcscmp(name, _T("TabLimit_MAX_MSG_TIME")))
			value = 7;
		else if (!_tcscmp(name, _T("TabLimit_WARNING_MSG_TIME")))
			value = 3;
		else
			return ERROR_FILE_NOT_FOUND;
		message = NULL;
		required = sizeof(value);
	} else if (flags == RRF_RT_REG_SZ) {
		if (!_tcscmp(name, _T("TabLimit_MAX_MSG")))
			message = _T("Limit reached");
		else if (!_tcscmp(name, _T("TabLimit_WARNING_MSG")))
			message = _T("Warning\\r\\nTab count");
		else
			return ERROR_FILE_NOT_FOUND;
		required = (DWORD)((_tcslen(message) + 1) * sizeof(TCHAR));
	} else {
		return ERROR_INVALID_PARAMETER;
	}
	if (data && *size < required)
		return ERROR_MORE_DATA;
	if (data)
		memcpy(data, message ? (const void *)message : (const void *)&value, required);
	*size = required;
	return ERROR_SUCCESS;
}

static DWORD WINAPI test_wait_for_warning(HANDLE mutex, DWORD timeout)
{
	if (timeout != 300)
		fprintf(stderr, "unexpected warning wait: %lu\n", timeout);
	return GetEnvironmentVariableA("THINBRIDGE_TEST_WARNING_BUSY", NULL, 0)
		? WAIT_TIMEOUT : WAIT_OBJECT_0;
}

static int test_show_warning(LPCTSTR message, LPCTSTR caption, UINT flags, int seconds)
{
	_ftprintf(stderr, _T("warning=%s\ncaption=%s\nflags=%u\nseconds=%d\n"),
		message, caption, flags, seconds);
	return IDOK;
}

#undef RegGetValue
#define RegGetValue(key, subkey, name, flags, type, data, size) \
	test_read_policy(key, (LPCTSTR)(subkey), (LPCTSTR)(name), flags, data, size)
/* Use an unnamed mutex so concurrent tests and running products stay independent. */
#undef CreateMutex
#define CreateMutex(attributes, owner, name) CreateMutexW(attributes, owner, NULL)
#define WaitForSingleObject test_wait_for_warning
#define ShowMessageBoxTimeout test_show_warning

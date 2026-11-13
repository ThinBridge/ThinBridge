static LSTATUS WINAPI test_read_rulefile(HKEY key, LPCSTR subkey, LPCSTR name,
	DWORD flags, LPDWORD type, PVOID data, LPDWORD size)
{
	DWORD length;
	if (key != HKEY_LOCAL_MACHINE || strcmp(subkey, "SOFTWARE\\ThinBridge") ||
		strcmp(name, "Rulefile") || flags != RRF_RT_REG_SZ)
		return ERROR_INVALID_PARAMETER;
	length = GetEnvironmentVariableA("THINBRIDGE_TEST_RULEFILE", data, *size);
	if (!length)
		return ERROR_FILE_NOT_FOUND;
	if (length >= *size)
		return ERROR_MORE_DATA;
	*size = length + 1;
	return ERROR_SUCCESS;
}

#define RegGetValueA test_read_rulefile

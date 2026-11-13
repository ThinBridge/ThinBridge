/* Included by the instrumented cb_query.c build after the Win32 declarations. */
static LSTATUS WINAPI test_read_value(HKEY key, LPCSTR subkey, LPCSTR name,
	DWORD flags, LPDWORD type, PVOID data, LPDWORD size)
{
	static const char path[] = "C:\\ThinBridge Test\\ThinBridge.exe";
	if (key != HKEY_LOCAL_MACHINE || strcmp(subkey, "SOFTWARE\\ThinBridge") ||
		strcmp(name, "ExtensionExecfile") || flags != RRF_RT_REG_SZ)
		return ERROR_INVALID_PARAMETER;
	if (GetEnvironmentVariableA("THINBRIDGE_TEST_NO_EXECFILE", NULL, 0))
		return ERROR_FILE_NOT_FOUND;
	if (*size < sizeof(path))
		return ERROR_MORE_DATA;
	memcpy(data, path, sizeof(path));
	*size = sizeof(path);
	return ERROR_SUCCESS;
}

static BOOL WINAPI test_create_process(LPCSTR path, LPSTR command,
	LPSECURITY_ATTRIBUTES process_security, LPSECURITY_ATTRIBUTES thread_security,
	BOOL inherit, DWORD flags, LPVOID environment, LPCSTR directory,
	LPSTARTUPINFO startup, LPPROCESS_INFORMATION process)
{
	char fail[2];
	fprintf(stderr, "path=%s\ncommand=%s\nflags=%lu\ninherit=%d\n", path, command, flags, inherit);
	if (GetEnvironmentVariableA("THINBRIDGE_TEST_FAIL_LAUNCH", fail, sizeof(fail)) == 1 &&
		fail[0] == '1') {
		SetLastError(ERROR_FILE_NOT_FOUND);
		return FALSE;
	}
	return TRUE;
}

#define RegGetValueA test_read_value
#define CreateProcessA test_create_process

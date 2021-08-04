#pragma once
#include "resource.h"       // メイン シンボル
#include <shlguid.h>
#include <exdispid.h>
#include <atlstr.h>
#include <atlcoll.h>
#include <atlfile.h>
#include "locale.h"
#include "atltime.h"
#include <activscp.h>
#include "TBGlobal.h"
#include "SBUtil.h"
#include <atlfile.h>
#include <string>

static std::wstring mutil2unicode(std::string mut_str, UINT code_page)
{
	std::wstring outstr;

	int nWideByteLength = MultiByteToWideChar(code_page,0, mut_str.c_str(), -1, NULL,0);

	if(nWideByteLength <= 0)
		nWideByteLength = (int)mut_str.length();

	wchar_t * buff = new wchar_t[nWideByteLength];
	memset(buff, 0, sizeof(wchar_t) * nWideByteLength);

	MultiByteToWideChar(code_page, 0, mut_str.c_str(), -1, buff, nWideByteLength);

	outstr = buff;
	delete buff;

	return outstr;
};

class CAtlStdioFile : public CAtlFile
{
private:
	static const int buffer_size = 512;
	TCHAR *buffer;
	char *buffera;

public:
	CAtlStdioFile()
	{
		buffer = new TCHAR[buffer_size];
		buffera = new char[buffer_size];
	}
	~CAtlStdioFile()
	{
		delete buffer;
		delete buffera;
	}

	HRESULT OpenFile(LPCTSTR szFilename, DWORD dwDesiredAccess, DWORD dwShareMode,
		DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL)
	{
		memset(buffer,0x00,sizeof(TCHAR)*buffer_size);
		memset(buffera,0x00,sizeof(char)*buffer_size);

		BYTE buffer_byte[2]={0};
		DWORD readsize = 0;
		if(SUCCEEDED(Create(szFilename, dwDesiredAccess, dwShareMode, dwCreationDisposition, dwFlagsAndAttributes))) {
			if((dwDesiredAccess & GENERIC_READ) && (dwCreationDisposition & OPEN_EXISTING)) {
				if(FAILED(Read(buffer_byte, 2, readsize))){
					Close();
				} else {
					if(readsize == 2) {
						if((buffer_byte[0] == 0xff && buffer_byte[1] == 0xfe) ||
							(buffer_byte[0] == 0xfe && buffer_byte[1] == 0xff))
							return S_OK;
					}
					Seek(0, FILE_BEGIN);
					return S_OK;
				}
			} else {
				return S_OK;
			}
		}
		return S_FALSE;
	}

	void WiteUnicodeHeader()
	{
		BYTE buffer_byte[2];
		buffer_byte[0] = 0xff;
		buffer_byte[1] = 0xfe;
		Seek(0, FILE_BEGIN);
		Write(buffer_byte, 2);
	}

	BOOL ReadLine(CString &str)
	{
		int sp, pos;
		CString tmp;
		DWORD readsize = 0, size;
		str = _T("");

		ZeroMemory(buffer, buffer_size*sizeof(TCHAR));
		while(Read(buffer, buffer_size, size) == S_OK) {
			if(size <= 0)
				break;
			readsize += size;
			tmp += buffer;
			sp = tmp.Find(_T('\n'));
			if(sp >= 0) {
				++sp;
				str = tmp.Left(sp);
				pos = sp*sizeof(TCHAR) - size;
				readsize+=pos;
				Seek(pos, FILE_CURRENT);
				break;
			}
			//改行無しの最終行対応
			else if(readsize==size)
			{
				str=tmp;
			}
			ZeroMemory(buffer, buffer_size*sizeof(TCHAR));
		}

		if(readsize <= 0)
			return FALSE;

		str.TrimRight(_T('\n'));
		str.TrimRight(_T('\r'));

		return TRUE;
	}

	BOOL ReadLineA(CStringA &str)
	{
		int sp, pos;
		CStringA tmp;
		DWORD readsize = 0, size;
		str = "";

		ZeroMemory(buffera, buffer_size*sizeof(char));
		while(Read(buffera, buffer_size, size) == S_OK) {
			if(size <= 0)
				break;
			readsize += size;
			tmp += buffera;
			sp = tmp.Find('\n');
			if(sp >= 0) {
				++sp;
				str = tmp.Left(sp);
				pos = sp*sizeof(char) - size;
				readsize+=pos;
				Seek(pos, FILE_CURRENT);
				break;
			}
			//改行無しの最終行対応
			else if(readsize==size)
			{
				str=tmp;
			}
			ZeroMemory(buffera, buffer_size*sizeof(char));
		}

		if(readsize <= 0)
			return FALSE;

		str.TrimRight('\n');
		str.TrimRight('\r');

		return TRUE;
	}

	BOOL ReadLineC(CString &str, int codepage)
	{
		int sp, pos;
		CString tmp;
		DWORD readsize = 0, size;
		str = _T("");

		ZeroMemory(buffera, buffer_size);
		while(Read(buffera, buffer_size, size) == S_OK) {
			if(size <= 0)
				break;
			readsize += size;
			tmp += mutil2unicode(buffera, codepage).c_str();
			sp = tmp.Find(_T('\n'));
			if(sp >= 0) {
				++sp;
				str = tmp.Left(sp);
				pos = sp - size;
				readsize+=pos;
				Seek(pos, FILE_CURRENT);
				break;
			}
			//改行無しの最終行対応
			else if(readsize==size)
			{
				str=tmp;
			}
			ZeroMemory(buffera, buffer_size);
		}

		if(readsize <= 0)
			return FALSE;

		str.TrimRight(_T('\r'));
		str.TrimRight(_T('\n'));

		return TRUE;
	}

	HRESULT WriteLine(CString str)
	{
		HRESULT hr = S_OK;
		str += _T("\r\n");
		int len = str.GetLength();
		if(len > 1) {
			hr = Write(str.GetBuffer(), len*sizeof(TCHAR));
			str.ReleaseBuffer();
		}
		return hr;
	}

	HRESULT WriteLineA(CStringA str)
	{
		HRESULT hr = S_OK;
		str += "\r\n";
		int len = str.GetLength();
		if(len > 1) {
			hr = Write(str.GetBuffer(), len*sizeof(char));
			str.ReleaseBuffer();
		}
		return hr;
	}
};

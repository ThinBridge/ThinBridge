#pragma once
#include "resource.h"       // メイン シンボル
#include <shlguid.h>
#include <exdispid.h>
#include <atlstr.h>
#include <atlcoll.h>
#include <atlfile.h>
#include "locale.h"
#include "atltime.h"

using namespace ATL;

//////////////////////////////////////////////////////////////////////////////////////////////////
//define
#define ZONE_NA        0x0000
#define INTRANET_ZONE  0x0001
#define TRUSTED_ZONE   0x0002
#define INTERNET_ZONE  0x0004
#define UNTRUSTED_ZONE 0x0008



//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//Global members

static TCHAR DEF_URLS[][12]=
{
	_T("http://"),
	_T("https://"),
	_T("ftp://"),
	_T("file://"),
	_T("mailto:"),
	_T("news:"),
	_T("nntp://"),
	_T("prospero://"),
	_T("telnet://"),
	_T("wais://"),
	_T("ms-help://"),
	_T("gopher://"),
	_T("about:"),
	_T("mk:@"),
	_T("\\\\"),
};
static TCHAR DEF_SCRIPT[][11]=
{
	_T("javascript"),
	_T("script"),
	_T("vbscript"),
	_T("jscript"),
};
//////////////////////////////////////////////////////////////////////////////////////////////////

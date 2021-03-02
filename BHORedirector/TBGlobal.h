#pragma once
#include "resource.h"       // メイン シンボル
#include <shlguid.h> 
#include <exdispid.h> 
#include "BHORedirector_i.h"
#include <atlstr.h>
#include <atlcoll.h>
#include <atlfile.h>
#include "locale.h"
#include "atltime.h"

using namespace ATL;

//////////////////////////////////////////////////////////////////////////////////////////////////
//define
#define DEBUG_LOG_LEVEL_OUTPUT_ALL 0
#define DEBUG_LOG_LEVEL_OUTPUT_NO_FILE 1
#define DEBUG_LOG_LEVEL_OUTPUT_URL 2 //DEBUG_LOG_TYPE_GE | DEBUG_LOG_TYPE_URL

#define DEBUG_LOG_TYPE_GE	 0 //一般情報
#define DEBUG_LOG_TYPE_DE	 1 //詳細情報
#define DEBUG_LOG_TYPE_URL	 2 //URL情報
#define DEBUG_LOG_TYPE_TR	 3 //トライデント動作情報
#define DEBUG_LOG_TYPE_CL	 4 //Close処理関連情報
#define DEBUG_LOG_TYPE_JS	 5 //Javascript関連情報
#define DEBUG_LOG_TYPE_EX	 6 //例外処理関連情報
#define DEBUG_LOG_TYPE_AC	 7 //操作アクション情報
#define DEBUG_LOG_TYPE_DDE	 8 //DDE情報


//////////////////////////////////////////////////////////////////////////////////////////////////
//Global members
BOOL m_gbTraceLog=FALSE;

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
static TCHAR sDEBUG_LOG_TYPE[][8]=
{
	_T("GE"),
	_T("DE"),
	_T("URL"),
	_T("TR"),
	_T("CL"),
	_T("JS"),
	_T("EX"),
	_T("AC"),
	_T("DDE"),
};

//////////////////////////////////////////////////////////////////////////////////////////////////
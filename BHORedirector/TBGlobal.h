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
#define ZONE_NA        0x0000
#define INTRANET_ZONE  0x0001
#define TRUSTED_ZONE   0x0002
#define INTERNET_ZONE  0x0004
#define UNTRUSTED_ZONE 0x0008

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
#define MAX_MEM_EX_URL 128
#define MAX_URL_SIZE 2088
#pragma comment(linker, "/SECTION:.SHAREDMEM,RWS")
#pragma data_seg(".SHAREDMEM")
LONG g_iCacheIndex = 0;                            //OK
char g_szExURLs[MAX_MEM_EX_URL][MAX_URL_SIZE]={0};
DWORD g_iPrevMS_Sum=0;
DWORD g_iPrevMS=0;
DWORD g_iBreakerCnt=0;
#pragma data_seg()
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
//16
//32
//void Global_WindowTabCnt_Increment(DWORD hWND)
//{
//	DWORD dwInsPos=0;
//	BOOL bInsPos=FALSE;
//	DWORD dwHitPos=0;
//	BOOL bHitPos=FALSE;
//	for(DWORD i=0;i<MAX_TAB_WND_COL;i++)
//	{
//		DWORD hWndTemp=0;
//		hWndTemp=g_iWindowTabHWNCollection[i];
//		if(hWndTemp==NULL)
//		{
//			if(bInsPos==FALSE)
//			{
//				bInsPos = TRUE;	
//				dwInsPos=i;
//			}
//		}
//		else
//		{
//			//同一WND
//			if(hWndTemp==hWND)
//			{
//				bHitPos = TRUE;
//				dwHitPos=i;
//				break;
//			}
//			else
//			{
//				//見つかったがWindowが無効 消す。
//				if(!::IsWindow((HWND)hWndTemp))
//				{
//					g_iWindowTabHWNCollection[i] = NULL;
//					//ここでカウンターをデクリメント
//					InterlockedDecrement(&g_iWindowTabCnt);
//
//				}
//
//			}
//		}
//	}
//	//登録されていない、新規
//	if(bHitPos==FALSE)
//	{
//		//挿入するIndex
//		if(bInsPos)
//		{
//			g_iWindowTabHWNCollection[dwInsPos] = hWND;
//			//ここでカウンターをインクリメント
//			InterlockedIncrement(&g_iWindowTabCnt);
//		}
//	}
////	else
////	{
////	}
//}
//void Global_WindowTabCnt_Decrement(DWORD hWND)
//{
//	InterlockedDecrement(&g_iWindowTabCnt);
//}
//DWORD Global_GetWindowTabCnt()
//{
//	return g_iWindowTabCnt;
//}

void Glogal_SetSkipURL_Clear()
{
	memset(g_szExURLs,0x00,sizeof(char)*(MAX_MEM_EX_URL*MAX_URL_SIZE));
}
void Glogal_SetSkipURL2ShareMEM(LPCSTR sURL)
{
	if(sURL)
	{
		if(g_iCacheIndex>=MAX_MEM_EX_URL)
			g_iCacheIndex=0;
		lstrcpynA(g_szExURLs[g_iCacheIndex],sURL,MAX_URL_SIZE);

		InterlockedIncrement(&g_iCacheIndex);
		if(g_iCacheIndex>=MAX_MEM_EX_URL)
			g_iCacheIndex=0;
//#ifdef _DEBUG
//		CStringA logmsg;
//		logmsg.Format(("ThinBridge:BHO\tGlogal_SetSkipURL2ShareMEM\t%s (%d)\n"),sURL,g_iCacheIndex);
//		OutputDebugStringA(logmsg);
//#endif
	}
}

BOOL Glogal_IsSkipURLfromShareMEM(LPCSTR sURL)
{
	char* ptr={0};

	//直近の1つ目を判定しておく。
	int iPreChk1=g_iCacheIndex-1;
	if(iPreChk1>=0)
	{
		ptr=g_szExURLs[iPreChk1];
		if(ptr[0])
		{
			if(lstrcmpA(ptr,sURL)==0)
				return TRUE;
		}
		else
			iPreChk1=MAX_MEM_EX_URL;
	}
	else
		iPreChk1=MAX_MEM_EX_URL;

	//直近の2つ目を判定しておく。
	int iPreChk2=g_iCacheIndex-2;
	if(iPreChk2>=0)
	{
		ptr=g_szExURLs[iPreChk2];
		if(ptr[0])
		{
			if(lstrcmpA(ptr,sURL)==0)
				return TRUE;
		}
		else
			iPreChk2=MAX_MEM_EX_URL;
	}
	else
		iPreChk2=MAX_MEM_EX_URL;

	//直近の3つ目を判定しておく。
	int iPreChk3=g_iCacheIndex-3;
	if(iPreChk3>=0)
	{
		ptr=g_szExURLs[iPreChk3];
		if(ptr[0])
		{
			if(lstrcmpA(ptr,sURL)==0)
				return TRUE;
		}
		else
			iPreChk3=MAX_MEM_EX_URL;
	}
	else
		iPreChk3=MAX_MEM_EX_URL;

	//パフォーマンスの調整がやりのくいので、ベタで羅列する。
	//直近の4つ目を判定しておく。
	int iPreChk4=g_iCacheIndex-4;
	if(iPreChk4>=0)
	{
		ptr=g_szExURLs[iPreChk4];
		if(ptr[0])
		{
			if(lstrcmpA(ptr,sURL)==0)
				return TRUE;
		}
		else
			iPreChk4=MAX_MEM_EX_URL;
	}
	else
		iPreChk4=MAX_MEM_EX_URL;

	//直近の5つ目を判定しておく。
	int iPreChk5=g_iCacheIndex-5;
	if(iPreChk5>=0)
	{
		ptr=g_szExURLs[iPreChk5];
		if(ptr[0])
		{
			if(lstrcmpA(ptr,sURL)==0)
				return TRUE;
		}
		else
			iPreChk5=MAX_MEM_EX_URL;
	}
	else
		iPreChk5=MAX_MEM_EX_URL;

	for(int i=0;i<MAX_MEM_EX_URL;i++)
	{
		if(iPreChk1==i)continue;
		if(iPreChk2==i)continue;
		if(iPreChk3==i)continue;
		if(iPreChk4==i)continue;
		if(iPreChk5==i)continue;

		ptr=NULL;
		ptr=g_szExURLs[i];
		if(ptr[0])
		{
			if(lstrcmpA(ptr,sURL)==0)
			{
#ifdef _DEBUG
		CStringA logmsg;
		logmsg.Format(("ThinBridge:BHO\tGlogal_IsSkipURLfromShareMEM\t%s (%d)\n"),ptr,i);
		OutputDebugStringA(logmsg);
#endif
				return TRUE;
			}
		}
		else
			return FALSE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
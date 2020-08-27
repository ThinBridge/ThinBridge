// dllmain.cpp : DllMain の実装

#include "stdafx.h"
#include "resource.h"
#include "BHORedirector_i.h"
#include "dllmain.h"
#include <atlstr.h>
#include <atlcoll.h>
#pragma comment(linker, "/base:\"0x13240000\"")

CBHORedirectorModule _AtlModule;
// DLL エントリ ポイント
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	DWORD pid = GetCurrentProcessId();
	//return _AtlModule.DllMain(dwReason, lpReserved); 
	if(dwReason == DLL_PROCESS_ATTACH)
    {
        //DisableThreadLibraryCalls(hInstance);
		TCHAR szPath[MAX_PATH+1]={0};
		TCHAR szDrive[_MAX_DRIVE]={0};
		TCHAR szDir[_MAX_DIR]={0};
		TCHAR szFileName[_MAX_FNAME]={0};
		TCHAR szExt[_MAX_EXT]={0};
		GetModuleFileName(hInstance,szPath,MAX_PATH);
		_tsplitpath_s(szPath, szDrive,_MAX_DRIVE, szDir,_MAX_DIR, szFileName,_MAX_FNAME, szExt,_MAX_EXT); 
		ATL::CString str;
		str = szDrive;
		str += szDir;
		memset(_AtlModule.m_gSZFullPath,0x00,sizeof(TCHAR)*MAX_PATH);
		lstrcpyn(_AtlModule.m_gSZFullPath,str,MAX_PATH);

		str = szFileName;
		str += szExt;
		memset(_AtlModule.m_gSZModuleName,0x00,sizeof(TCHAR)*MAX_PATH);
		lstrcpyn(_AtlModule.m_gSZModuleName,str,MAX_PATH);

		GetModuleFileName(NULL,szPath,MAX_PATH);
		memset(_AtlModule.m_gSZParentProcessFullPath,0x00,sizeof(TCHAR)*MAX_PATH);
		lstrcpyn(_AtlModule.m_gSZParentProcessFullPath,szPath,MAX_PATH);

		_tsplitpath_s(szPath, szDrive,_MAX_DRIVE, szDir,_MAX_DIR, szFileName,_MAX_FNAME, szExt,_MAX_EXT); 
		str = szFileName;
		str += szExt;

		memset(_AtlModule.m_gSZParentProcessName,0x00,sizeof(TCHAR)*MAX_PATH);
		lstrcpyn(_AtlModule.m_gSZParentProcessName,str,MAX_PATH);

		ATL::CString strLog;
		strLog.Format(_T("ThinBridge:BHO DllMain ProcessAttach Dll:[%s] EXE:[%s] PID:[%d]"),_AtlModule.m_gSZModuleName,_AtlModule.m_gSZParentProcessName,pid);
		OutputDebugString(strLog);
    }
	else if(dwReason == DLL_PROCESS_DETACH)
	{
		ATL::CString strLog;
		strLog.Format(_T("ThinBridge:BHO DllMain ProcessDetach Dll:[%s] EXE:[%s] PID:[%d]"),_AtlModule.m_gSZModuleName,_AtlModule.m_gSZParentProcessName,pid);
		OutputDebugString(strLog);

	}
    return _AtlModule.DllMain(dwReason, lpReserved); 
}

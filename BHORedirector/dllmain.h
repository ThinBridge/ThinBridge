// dllmain.h : モジュール クラスの宣言
class CBHORedirectorModule : public ATL::CAtlDllModuleT< CBHORedirectorModule >
{
public :
	CBHORedirectorModule()
	{
		memset(m_gSZFullPath,0x00,sizeof(TCHAR)*MAX_PATH);
		memset(m_gSZModuleName,0x00,sizeof(TCHAR)*MAX_PATH);
		memset(m_gSZParentProcessFullPath,0x00,sizeof(TCHAR)*MAX_PATH);
		memset(m_gSZParentProcessName,0x00,sizeof(TCHAR)*MAX_PATH);
	}
	DECLARE_LIBID(LIBID_BHORedirectorLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_BHOREDIRECTOR, "{3278FA3E-416E-4F32-9EF6-F9334692FA75}")
	TCHAR m_gSZFullPath[MAX_PATH];
	TCHAR m_gSZModuleName[MAX_PATH];
	TCHAR m_gSZParentProcessFullPath[MAX_PATH];
	TCHAR m_gSZParentProcessName[MAX_PATH];

	HRESULT AddCommonRGSReplacements(IRegistrarBase* pRegistrar) throw()
	{
		ATLENSURE_SUCCEEDED(__super::AddCommonRGSReplacements(pRegistrar));
		ATLASSERT(!IsEqualGUID(m_libid, GUID_NULL));
		pRegistrar->AddReplacement(OLESTR("MODULE_FOLDER"),T2OLE(m_gSZFullPath));
		return S_OK;
	}
};

extern class CBHORedirectorModule _AtlModule;

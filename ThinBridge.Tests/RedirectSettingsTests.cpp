#include "../ThinBridge/stdafx.h"
#include "../ThinBridge/ThinBridge.h"
#include "CppUnitTest.h"
#include <cstring>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ThinBridgeTests
{
    TEST_CLASS(RedirectSettingsTests)
    {
        wchar_t settingsPath[MAX_PATH] = {};

        void WriteSettings(const char* contents)
        {
            CFile file;
            Assert::IsTrue(file.Open(settingsPath, CFile::modeWrite | CFile::modeCreate) != FALSE);
            file.Write(contents, static_cast<UINT>(std::strlen(contents)));
            file.Close();
        }

        void LoadSettings(CURLRedirectList& settings, const char* contents)
        {
            WriteSettings(contents);
            settings.SetArrayData(settingsPath);
        }

    public:
        TEST_METHOD_INITIALIZE(CreateSettingsFile)
        {
            wchar_t tempDirectory[MAX_PATH];
            const DWORD length = GetTempPathW(_countof(tempDirectory), tempDirectory);
            Assert::IsTrue(length > 0 && length < _countof(tempDirectory));
            Assert::IsTrue(GetTempFileNameW(tempDirectory, L"tbr", 0, settingsPath) != 0);
        }

        TEST_METHOD_CLEANUP(DeleteSettingsFile)
        {
            if (settingsPath[0] != L'\0')
                DeleteFileW(settingsPath);
        }

        TEST_METHOD(GlobalTopPageOnlySetsUrlOnly)
        {
            CURLRedirectList settings;
            LoadSettings(settings, "[GLOBAL]\r\n@TOP_PAGE_ONLY\r\n[EDGE]\r\nhttps://example.com/*\r\n");
            Assert::IsTrue(settings.m_bURLOnly != FALSE);
            Assert::IsFalse(settings.m_bTopURLOnly != FALSE);
            Assert::IsFalse(settings.m_pEdge->m_bTopPageOnly != FALSE);
        }

        TEST_METHOD(GlobalUntrustedZoneSetsTopUrlOnly)
        {
            CURLRedirectList settings;
            LoadSettings(settings, "[GLOBAL]\r\n@UNTRUSTED_ZONE\r\n[EDGE]\r\nhttps://example.com/*\r\n");
            Assert::IsTrue(settings.m_bTopURLOnly != FALSE);
            Assert::IsFalse(settings.m_bURLOnly != FALSE);
        }

        TEST_METHOD(BrowserTopPageOnlyStaysOnBrowserRule)
        {
            CURLRedirectList settings;
            LoadSettings(settings, "[GLOBAL]\r\n[EDGE]\r\n@TOP_PAGE_ONLY\r\nhttps://example.com/*\r\n");
            Assert::IsTrue(settings.m_pEdge->m_bTopPageOnly != FALSE);
            Assert::IsFalse(settings.m_bURLOnly != FALSE);
        }

        TEST_METHOD(GlobalTraceAndQuickRedirectAreLoaded)
        {
            CURLRedirectList settings;
            LoadSettings(settings, "[global]\r\n@redirect_page_action:1\r\n@intranet_zone");
            Assert::IsTrue(settings.m_bTraceLog != FALSE);
            Assert::IsTrue(settings.m_bQuickRedirect != FALSE);
            Assert::IsFalse(settings.m_bTopURLOnly != FALSE);
        }

        TEST_METHOD(BrowserRulePreservesPatternOrderAndCommentedExclusions)
        {
            CURLRedirectList settings;
            LoadSettings(settings,
                "https://ignored.example/*\r\n"
                "[edge]\r\n;[ignored]\r\n#@DISABLED\r\n@UNKNOWN\r\n"
                "  https://example.com/First*  \r\nhttps://example.com/second*\r\n"
                "-https://example.com/excluded*\r\n;-https://example.com/commented*\r\n"
                "@exclude_group:  Group A  \r\n");
            const auto& edge = *settings.m_pEdge;
            Assert::IsFalse(edge.m_bDisabled != FALSE);
            Assert::IsTrue(edge.m_arr_URL.GetCount() == 2);
            Assert::AreEqual(L"https://example.com/First*", edge.m_arr_URL[0].GetString());
            Assert::AreEqual(L"https://example.com/second*", edge.m_arr_URL[1].GetString());
            Assert::IsTrue(edge.m_arr_URL_EX.GetCount() == 2);
            Assert::AreEqual(L"https://example.com/excluded*", edge.m_arr_URL_EX[0].GetString());
            Assert::AreEqual(L"#https://example.com/commented*", edge.m_arr_URL_EX[1].GetString());
            Assert::IsTrue(edge.m_arr_ExcludeGroup.GetCount() == 1);
            Assert::AreEqual(L"Group A", edge.m_arr_ExcludeGroup[0].GetString());
        }

        TEST_METHOD(BrowserOptionsAreScopedToTheirSection)
        {
            CURLRedirectList settings;
            LoadSettings(settings,
                "[EDGE]\r\n@DISABLED\r\n@BROWSER_PATH:  C:\\Program Files\\Browser.exe  \r\n"
                "@REDIRECT_PAGE_ACTION:9\r\n@CLOSE_TIMEOUT:99\r\n"
                "[CHROME]\r\n@CLOSE_TIMEOUT:0\r\n"
                "[FIREFOX]\r\n@REDIRECT_PAGE_ACTION:1\r\n@CLOSE_TIMEOUT:12\r\n");
            Assert::IsTrue(settings.m_pEdge->m_bDisabled != FALSE);
            Assert::AreEqual(L"C:\\Program Files\\Browser.exe", settings.m_pEdge->m_strExecExeFullPath.GetString());
            Assert::AreEqual<DWORD>(2, settings.m_pEdge->m_dRedirectPageAction);
            Assert::AreEqual<DWORD>(60, settings.m_pEdge->m_dwCloseTimeout);
            Assert::IsFalse(settings.m_pChrome->m_bDisabled != FALSE);
            Assert::IsTrue(settings.m_pChrome->m_strExecExeFullPath.IsEmpty());
            Assert::AreEqual<DWORD>(0, settings.m_pChrome->m_dRedirectPageAction);
            Assert::AreEqual<DWORD>(3, settings.m_pChrome->m_dwCloseTimeout);
            Assert::AreEqual<DWORD>(1, settings.m_pFirefox->m_dRedirectPageAction);
            Assert::AreEqual<DWORD>(12, settings.m_pFirefox->m_dwCloseTimeout);
        }

        TEST_METHOD(LastSectionForSameBrowserReplacesEarlierRule)
        {
            CURLRedirectList settings;
            LoadSettings(settings, "[EDGE]\r\n@DISABLED\r\nhttps://old.example/*\r\n"
                "[edge]\r\nhttps://new.example/*");
            Assert::IsFalse(settings.m_pEdge->m_bDisabled != FALSE);
            Assert::IsTrue(settings.m_pEdge->m_arr_URL.GetCount() == 1);
            Assert::AreEqual(L"https://new.example/*", settings.m_pEdge->m_arr_URL[0].GetString());
        }

        TEST_METHOD(ReloadClearsPreviousSettings)
        {
            CURLRedirectList settings;
            LoadSettings(settings, "[GLOBAL]\r\n@UNTRUSTED_ZONE\r\n@RDP_CLIPBOARD\r\n"
                "[EDGE]\r\nhttps://old.example/*\r\n");
            Assert::IsTrue(settings.m_bTopURLOnly != FALSE);
            Assert::IsTrue(settings.m_bRedirect_Clipboard != FALSE);
            Assert::IsTrue(settings.m_pEdge->m_arr_URL.GetCount() == 1);
            LoadSettings(settings, "[GLOBAL]\r\n[CHROME]\r\nhttps://new.example/*\r\n");
            Assert::IsFalse(settings.m_bTopURLOnly != FALSE);
            Assert::IsFalse(settings.m_bRedirect_Clipboard != FALSE);
            Assert::IsTrue(settings.m_pEdge->m_arr_URL.IsEmpty());
            Assert::IsTrue(settings.m_pChrome->m_arr_URL.GetCount() == 1);
        }

        TEST_METHOD(GlobalReaderLoadsRemoteSettingsBeforeRdpSection)
        {
            CURLRedirectList settings;
            WriteSettings("[global]\r\n@VMW_SERVERNAME: Horizon.Example\r\n@VMW_APPNAME: Horizon App\r\n"
                "@CTX_APPNAME: Citrix App\r\n@RDP_SERVERNAME: Rdp.Example\r\n"
                "@RDP_APPNAME: Remote App\r\n@RDP_APPPATH: C:\\Apps\\Remote.exe\r\n"
                "@RDP_COMMANDLINE: /open %URL%\r\n@RDP_CLIPBOARD\r\n@RDP_PRINTER\r\n"
                "@RDP_DRIVE\r\n@DISABLE_IE_DDE\r\n"
                "[RDP]\r\n@RDP_SERVERNAME: ignored.example\r\n");
            settings.SetGlobalData(settingsPath);
            Assert::AreEqual(L"Horizon.Example", settings.m_strHorizon_ConnectionServerName.GetString());
            Assert::AreEqual(L"Horizon App", settings.m_strHorizon_AppName.GetString());
            Assert::AreEqual(L"Citrix App", settings.m_strCitrix_AppName.GetString());
            Assert::AreEqual(L"Rdp.Example", settings.m_strRDP_ServerName.GetString());
            Assert::AreEqual(L"Remote App", settings.m_strRemoteAppName.GetString());
            Assert::AreEqual(L"C:\\Apps\\Remote.exe", settings.m_strRemoteAppPath.GetString());
            Assert::AreEqual(L"/open %URL%", settings.m_strRemoteAppCommandLine.GetString());
            Assert::IsTrue(settings.m_bRedirect_Clipboard != FALSE);
            Assert::IsTrue(settings.m_bRedirect_Printer != FALSE);
            Assert::IsTrue(settings.m_bRedirect_Drive != FALSE);
            Assert::IsTrue(settings.m_bDisableIE_DDE != FALSE);
        }

        TEST_METHOD(SaveAndReloadPreservesGlobalAndBrowserSettings)
        {
            CURLRedirectList settings;
            LoadSettings(settings, "[GLOBAL]\r\n@TOP_PAGE_ONLY\r\n@UNTRUSTED_ZONE\r\n"
                "[EDGE]\r\n@DISABLED\r\n@BROWSER_PATH:C:\\Browser.exe\r\n"
                "@CLOSE_TIMEOUT:12\r\nhttps://example.com/*\r\n-https://example.com/local/*\r\n");
            CString serialized;
            Assert::IsTrue(settings.SaveData(settingsPath, serialized) != FALSE);
            CURLRedirectList reloaded;
            reloaded.SetArrayData(settingsPath);
            Assert::IsTrue(reloaded.m_bURLOnly != FALSE);
            Assert::IsTrue(reloaded.m_bTopURLOnly != FALSE);
            Assert::IsTrue(reloaded.m_pEdge->m_bDisabled != FALSE);
            Assert::AreEqual<DWORD>(12, reloaded.m_pEdge->m_dwCloseTimeout);
            Assert::IsTrue(reloaded.m_pEdge->m_arr_URL.GetCount() == 1);
            Assert::IsTrue(reloaded.m_pEdge->m_arr_URL_EX.GetCount() == 1);
            Assert::AreEqual(L"https://example.com/*", reloaded.m_pEdge->m_arr_URL[0].GetString());
            Assert::AreEqual(L"https://example.com/local/*", reloaded.m_pEdge->m_arr_URL_EX[0].GetString());
        }

        TEST_METHOD(EmptySettingsFileUsesProductDefaults)
        {
            CURLRedirectList settings;
            LoadSettings(settings, "");
            Assert::IsTrue(settings.m_bURLOnly != FALSE);
            Assert::IsTrue(settings.m_bTopURLOnly != FALSE);
            Assert::IsTrue(settings.m_bQuickRedirect != FALSE);
            Assert::IsNotNull(settings.m_pEdge);
            Assert::IsTrue(settings.m_pEdge->m_arr_URL.IsEmpty());
            Assert::AreEqual<DWORD>(3, settings.m_pEdge->m_dwCloseTimeout);
        }

        TEST_METHOD(CitrixCustomEnvironmentLoadsOnlyItsSupportedBrowserSections)
        {
            CURLRedirectList settings;
            WriteSettings("[GLOBAL]\r\n[EDGE]\r\nhttps://edge.example/*\r\n"
                "[CHROME]\r\nhttps://chrome.example/*\r\n[CUSTOM18]\r\nhttps://neutral.example/*\r\n");
            settings.SetArrayData(settingsPath, TRUE);
            Assert::IsNull(settings.m_pEdge);
            Assert::IsNotNull(settings.m_pChrome);
            Assert::IsNotNull(settings.m_pCustom18);
            Assert::AreEqual(1, static_cast<int>(settings.m_pChrome->m_arr_URL.GetCount()));
            Assert::AreEqual(1, static_cast<int>(settings.m_pCustom18->m_arr_URL.GetCount()));
            Assert::AreEqual(L"https://chrome.example/*", settings.m_pChrome->m_arr_URL[0].GetString());
            Assert::AreEqual(L"https://neutral.example/*", settings.m_pCustom18->m_arr_URL[0].GetString());
        }

        TEST_METHOD(SaveAndReloadPreservesDisabledExcludeGroups)
        {
            CURLRedirectList settings;
            LoadSettings(settings, "[EDGE]\r\nhttps://example.com/*\r\n"
                "@EXCLUDE_GROUP:Active\r\n@EXCLUDE_GROUP:#Disabled\r\n"
                "@EXCLUDE_GROUP:;AlsoDisabled\r\n#@EXCLUDE_GROUP:Comment\r\n");
            CString serialized;
            Assert::IsTrue(settings.SaveData(settingsPath, serialized) != FALSE);
            Assert::IsTrue(serialized.Find(L"@EXCLUDE_GROUP:#Disabled\n") >= 0);
            Assert::IsTrue(serialized.Find(L"@EXCLUDE_GROUP:;AlsoDisabled\n") >= 0);

            CURLRedirectList reloaded;
            reloaded.SetArrayData(settingsPath);
            const auto& groups = reloaded.m_pEdge->m_arr_ExcludeGroup;
            Assert::AreEqual(3, static_cast<int>(groups.GetCount()));
            Assert::AreEqual(L"Active", groups[0].GetString());
            Assert::AreEqual(L"#Disabled", groups[1].GetString());
            Assert::AreEqual(L";AlsoDisabled", groups[2].GetString());
        }

        TEST_METHOD(SaveUsesGlobalTopUrlOnlyForBrowserDirective)
        {
            CURLRedirectList settings;
            LoadSettings(settings, "[GLOBAL]\r\n@UNTRUSTED_ZONE\r\n[EDGE]\r\nhttps://example.com/*\r\n");
            Assert::IsFalse(settings.m_pEdge->m_bTopPageOnly != FALSE);
            CString serialized;
            Assert::IsTrue(settings.SaveData(settingsPath, serialized) != FALSE);
            CURLRedirectList reloaded;
            reloaded.SetArrayData(settingsPath);
            Assert::IsTrue(reloaded.m_pEdge->m_bTopPageOnly != FALSE);
        }
    };

    TEST_CLASS(UrlHelpersTests)
    {
    public:
        TEST_METHOD(UrlRecognitionUsesSchemePrefixes)
        {
            Assert::IsTrue(SBUtil::IsURL(L"HTTPS://example.com") != FALSE);
            Assert::IsTrue(SBUtil::IsURL(L"file:///C:/page.html") != FALSE);
            Assert::IsFalse(SBUtil::IsURL(L"C:\\page.html") != FALSE);
            Assert::IsFalse(SBUtil::IsURL(nullptr) != FALSE);
        }

        TEST_METHOD(HttpRecognitionIsCaseSensitive)
        {
            Assert::IsTrue(SBUtil::IsURL_HTTP(L"http://example.com") != FALSE);
            Assert::IsTrue(SBUtil::IsURL_HTTP(L"https://example.com") != FALSE);
            Assert::IsFalse(SBUtil::IsURL_HTTP(L"HTTPS://example.com") != FALSE);
            Assert::IsFalse(SBUtil::IsURL_HTTP(L"file:///C:/page.html") != FALSE);
        }

        TEST_METHOD(TrimUrlKeepsPathAndRemovesQueryAndFragment)
        {
            const auto unchanged = SBUtil::Trim_URLOnly(L"https://example.com/a%3Fb%23c");
            const auto queryFirst = SBUtil::Trim_URLOnly(L"https://example.com/path?query#fragment");
            const auto fragmentFirst = SBUtil::Trim_URLOnly(L"https://example.com/path#fragment?text");
            Assert::AreEqual(L"https://example.com/a%3Fb%23c", unchanged.GetString());
            Assert::AreEqual(L"https://example.com/path", queryFirst.GetString());
            Assert::AreEqual(L"https://example.com/path", fragmentFirst.GetString());
        }
    };
}

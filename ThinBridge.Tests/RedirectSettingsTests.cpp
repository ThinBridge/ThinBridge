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

        void LoadSettings(CURLRedirectList& settings, const char* contents)
        {
            CFile file;
            Assert::IsTrue(file.Open(settingsPath, CFile::modeWrite | CFile::modeCreate) != FALSE);
            file.Write(contents, static_cast<UINT>(std::strlen(contents)));
            file.Close();
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
    };
}

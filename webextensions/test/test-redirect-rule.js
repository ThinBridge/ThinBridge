const assert = require('assert');
const sinon = require('sinon');
const chromestub = require('./chrome-stub.js')

describe('Redirect rule', () => {
  const redirectors = [
    { browser: 'edge',   module: require('../testee/edge.js')   },
  ].forEach(({browser, module}) => {
    describe(browser, () => {
      const isClosableTab = true;
      const tabId = 123;
      const shouldCloseTab = true;
      const baseConfig = {
        CloseEmptyTab: 1,
        DefaultBrowser: "Chrome",
        IgnoreQueryString: 1,
        OnlyMainFrame : 1,
        Sections: [],
      }
      const edgeSection = { // should not redirect since it's me
        "Name": "edge",
        "Patterns": ["https://www.clear-code.com/*"],
        "Excludes": [],
      };
      const chromeSection = {
        "Name": "chrome",
        "Patterns": ["*://*"],
        "Excludes": [],
      };
      const custom18Section = {  // a.k.a DMZ section, should not redirect
        "Name": "custom18",
        "Patterns": ["https://www.example.com/*"],
        "Excludes": [],
      };
      const queryTestSection = {
        "Name": "firefox",
        "Patterns": ["https://www.google.com/search"],
        "Excludes": [],
      };

      function config(sections = [], additionals = {}) {
        const config = {...baseConfig, ...additionals};
        config.Sections = [...config.Sections, ...sections];
        config.NamedSections = Object.fromEntries(config.Sections.map(section => [section.Name, section]));
        return config;
      }


      const thinbridge = module.client;
      let thinbridge_mock;

      beforeEach(() => {
        thinbridge_mock = sinon.mock(thinbridge);
      });

      afterEach(() => {
        thinbridge_mock.restore();
      });


      //
      // * redirect rule: compatible mode
      //   This mode is for common usage and should be compatible with v1.x
      //   especially v1.4.0 which is ManifestV2 version of this add-on.
      //   v1.4.0 was the latest version of public v1.x releases, v1.5.0 isn't
      //   published.
      //
      describe('compatible mode', () => {

        it('redirect no match URL', () => {
          const url = "https://www.google.com/";
          const conf = config();
          thinbridge_mock.expects("redirect").once().withArgs(url, tabId, shouldCloseTab);
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, true);
        });

        it('load no match URL with no default browser', () => {
          const url = "https://www.google.com/";
          const conf = config([], { DefaultBrowser: "" });
          thinbridge_mock.expects("redirect").never();
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, false);
        });

        it('redirect matched URL', () => {
          const url = "https://www.google.com/";
          const conf = config([chromeSection])
          thinbridge_mock.expects("redirect").once().withArgs(url, tabId, shouldCloseTab);
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, true);
        });

        it('redirect URL matched to one of patterns', () => {
          const url = "https://www.example.com/";
          const conf = config(
            [{
              "Name": "chrome",
              "Patterns": [
                "https://www.google.com/*",
                "https://www.clear-code.com/*",
                "https://www.example.com/*",
              ],
              "Excludes": [],
            }]
          )
          thinbridge_mock.expects("redirect").once().withArgs(url, tabId, shouldCloseTab);
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, true);
        });

        it('do not close tab with CloseEmptyTab option', () => {
          const url = "https://www.google.com/";
          const conf = config([chromeSection], { CloseEmptyTab: false })
          thinbridge_mock.expects("redirect").once().withArgs(url, tabId, !shouldCloseTab);
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, true);
        });

        it('do not close non-closable tab', () => {
          const url = "https://www.google.com/";
          const conf = config([chromeSection])
          thinbridge_mock.expects("redirect").once().withArgs(url, tabId, !shouldCloseTab);
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, !isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, true);
        });

        it('load URL matched to custom18', () => {
          const url = "https://www.example.com/";
          const conf = config([custom18Section])
          thinbridge_mock.expects("redirect").never()
          const shouldBlock = thinbridge.handleURLAndBlock(conf, 0, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, false);
        });

        it('treat custom18 prior than others', () => {
          const url = "https://www.example.com/";
          const conf = config([chromeSection, custom18Section])
          thinbridge_mock.expects("redirect").never()
          const shouldBlock = thinbridge.handleURLAndBlock(conf, 0, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, false);
        });

        it(`load URL matched to ${browser}`, () => {
          const url = "https://www.clear-code.com/";
          const conf = config([edgeSection])
          thinbridge_mock.expects("redirect").never()
          const shouldBlock = thinbridge.handleURLAndBlock(conf, 0, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, false);
        });

        it(`treat ${browser} prior than others`, () => {
          const url = "https://www.clear-code.com/";
          const conf = config([chromeSection, edgeSection])
          thinbridge_mock.expects("redirect").never()
          const shouldBlock = thinbridge.handleURLAndBlock(conf, 0, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, false);
        });

        it('treat URL as unmatched to custom18, when it matched to exclude pattern', () => {
          const url = "https://www.example.com/index.html";
          const conf = config([custom18Section])
          conf.Sections[0].Excludes = [url]
          thinbridge_mock.expects("redirect").once().withArgs(url, tabId, shouldCloseTab);
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, true);
        });

        it('treat URL as unmatched to custom18, when it matched to one of multiple exclude patterns', () => {
          const url = "https://www.example.com/";
          const conf = config(
            [{
              "Name": "custom18",
              "Patterns": ["*"],
              "Excludes": [
                "https://www.google.com/*",
                "https://www.clear-code.com/*",
                "https://www.example.com/*",
              ],
            }]
          );
          thinbridge_mock.expects("redirect").once().withArgs(url, tabId, shouldCloseTab);
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, true);
        });

        it('preserve query for redirection', () => {
          const conf = config([queryTestSection]);
          const url = "https://www.google.com/search?q=foobar";
          thinbridge_mock.expects("redirect").once().withArgs(url, tabId, shouldCloseTab);
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, true);
        });

        it('ignore URL query when matching with IgnoreQueryString=1', () => {
          const url = "https://www.google.com/search?q=hoge";
          const conf = config([queryTestSection], { DefaultBrowser: "" })
          thinbridge_mock.expects("redirect").once().withArgs(url, tabId, shouldCloseTab);
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, true);
        });

        it('regard URL query when matching with IgnoreQueryString=0', () => {
          const url = "https://www.google.com/search?q=hoge";
          const conf = config([queryTestSection], { DefaultBrowser: "", IgnoreQueryString: 0 })
          thinbridge_mock.expects("redirect").never();
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, false);
        });
      });


      //
      // * redirect rule: action mode
      //   This mode is for a perticular usage, not for public.
      //
      describe('action mode', () => {

        it('override default redirect action to load', () => {
          const url = "https://www.google.com/";
          const conf = config([{...chromeSection}]);
          conf.Sections[0].Action = "load";
          thinbridge_mock.expects("redirect").never();
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, false);
        });

        it('override default load action to redirect', () => {
          const url = "https://www.clear-code.com/";
          const conf = config([{...edgeSection}]);
          conf.Sections[0].Action = "redirect";
          thinbridge_mock.expects("redirect").once().withArgs(url, tabId, true);
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, true);
        });

        it('set default redirect action explicitly', () => {
          const url = "https://www.google.com/";
          const conf = config([{...chromeSection}]);
          conf.Sections[0].Action = "redirect";
          thinbridge_mock.expects("redirect").once().withArgs(url, tabId, true);
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, true);
        });

        it('set default load action explicitly', () => {
          const url = "https://www.clear-code.com/";
          const conf = config([{...edgeSection}]);
          conf.Sections[0].Action = "load";
          thinbridge_mock.expects("redirect").never();
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, false);
        });
      });
    });
  });
});
const assert = require('assert');
const sinon = require('sinon');
const chromestub = require('./chrome-stub.js')

describe('Redirect rule', () => {
  const redirectors = [
    { browser: 'chrome', format: 'thinbridge', module: require('../testee/chrome.js') },
    { browser: 'edge',   format: 'thinbridge', module: require('../testee/edge.js')   },
    { browser: 'chrome', format: 'ie-view-we', module: require('../testee/chrome.js')   },
    { browser: 'edge',   format: 'ie-view-we', module: require('../testee/edge.js')   },
  ].forEach(({browser, format, module}) => {
    function isEdge() {
      return browser == "edge";
    }
    describe(`Browser: ${browser}, Format: ${format}`, () => {
      const isClosableTab = true;
      const tabId = 123;
      const shouldCloseTab = true;
      const baseConfig = {
        CloseEmptyTab: 1,
        DefaultBrowser: isEdge() ? "Chrome" : "Edge",
        IgnoreQueryString: 1,
        OnlyMainFrame : 1,
        Sections: [],
      }
      const edgeSection = {
        "Name": "edge",
        "Patterns": [isEdge() ? "https://www.clear-code.com/*" : "*://*"],
        "Excludes": [],
      };
      const chromeSection = {
        "Name": "chrome",
        "Patterns": [isEdge() ? "*://*" : "https://www.clear-code.com/*"],
        "Excludes": [],
      };
      const mySection = isEdge() ? edgeSection : chromeSection;
      const counterSection = isEdge() ? chromeSection : edgeSection;
      const citrixSection = {
        "Name": "citrix",
        "Patterns": ["https://www.citrix.com/*"],
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
        if (format === 'ie-view-we') {
          config.Sections = config.Sections.map(section => {
            let destSection = { Name: section.Name }
            if (section.Patterns) {
              destSection.URLPatterns = section.Patterns.map(pattern =>
                // The second column is just remained for compatibility with
                // IE View WE, not used in actual.
                [pattern, 'chrome']);
            }
            if (section.Excludes) {
              destSection.URLExcludePatterns = section.Excludes.map(pattern =>
                // Ditto
                [pattern, 'chrome']);
            }
            if (section.ExcludeGroups) {
              destSection.ExcludeGroups = section.ExcludeGroups;
            }
            return destSection;
          })
        }
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

        it('section with no patterns', () => {
          const url = "https://www.google.com/";
          const conf = config([{"Name": "firefox"}]);
          thinbridge_mock.expects("redirect").once().withArgs(url, tabId, shouldCloseTab);
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, true);
        });

        it('redirect matched URL', () => {
          const url = "https://www.google.com/";
          const conf = config([counterSection])
          thinbridge_mock.expects("redirect").once().withArgs(url, tabId, shouldCloseTab);
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, true);
        });

        it('redirect URL matched to one of patterns', () => {
          const url = "https://www.example.com/";
          const conf = config(
            [{
              "Name": isEdge() ? "chrome" : "edge",
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
          const conf = config([counterSection], { CloseEmptyTab: false })
          thinbridge_mock.expects("redirect").once().withArgs(url, tabId, !shouldCloseTab);
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, true);
        });

        it('do not close non-closable tab', () => {
          const url = "https://www.google.com/";
          const conf = config([counterSection])
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
          const conf = config([counterSection, custom18Section])
          thinbridge_mock.expects("redirect").never()
          const shouldBlock = thinbridge.handleURLAndBlock(conf, 0, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, false);
        });

        it(`load URL matched to ${browser}`, () => {
          const url = "https://www.clear-code.com/";
          const conf = config([mySection])
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
          const conf = config([{...custom18Section, Excludes: [url]}])
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

        it('treat URL as unmatched to custom18, when it matched to an exclude group', () => {
          const url = "https://www.citrix.com/";
          const conf = config(
            [
              {
                "Name": "custom18",
                "Patterns": ["*"],
                "Excludes": [],
                "ExcludeGroups": ["Citrix"],
              },
              citrixSection,
            ]
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
          const url = "https://www.citrix.com/";
          const conf = config([{...citrixSection}]);
          conf.Sections[0].Action = "load";
          thinbridge_mock.expects("redirect").never();
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, false);
        });

        it('override default load action to redirect', () => {
          const url = "https://www.clear-code.com/";
          const conf = config([mySection]);
          conf.Sections[0].Action = "redirect";
          thinbridge_mock.expects("redirect").once().withArgs(url, tabId, true);
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, true);
        });

        it('set default redirect action explicitly', () => {
          const url = "https://www.citrix.com/";
          const conf = config([{...citrixSection}]);
          conf.Sections[0].Action = "redirect";
          thinbridge_mock.expects("redirect").once().withArgs(url, tabId, true);
          const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
          thinbridge_mock.verify();
          assert.equal(shouldBlock, true);
        });

        it('set default load action explicitly', () => {
          const url = "https://www.clear-code.com/";
          const conf = config([mySection]);
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

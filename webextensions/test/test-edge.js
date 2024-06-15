const assert = require('assert');
const sinon = require('sinon');
const chromestub = require('./chrome-stub.js')
const edge = require('../testee/edge.js');

describe('Microsoft Edge Add-on', () => {
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
  const citrixSection = {
    "Name": "citrix",
    "Patterns": ["https://www.google.com/*"],
    "Excludes": [],
  };
  const edgeSection = { // should not redirect since it's me
    "Name": "edge",
    "Patterns": ["https://www.microsoft.com/*"],
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


  const thinbridge = edge.client;
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
  describe('redirect rule: compatible mode', () => {

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
            "https://www.microsoft.com/*",
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

    it('load URL matched to edge', () => {
      const url = "https://www.microsoft.com/";
      const conf = config([edgeSection])
      thinbridge_mock.expects("redirect").never()
      const shouldBlock = thinbridge.handleURLAndBlock(conf, 0, url, isClosableTab);
      thinbridge_mock.verify();
      assert.equal(shouldBlock, false);
    });

    it('treat edge prior than others', () => {
      const url = "https://www.microsoft.com/";
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
            "https://www.microsoft.com/*",
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
      const conf = config([citrixSection]);
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
  describe('redirect rule: action mode', () => {

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
      const url = "https://www.microsoft.com/";
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
      const url = "https://www.microsoft.com/";
      const conf = config([{...edgeSection}]);
      conf.Sections[0].Action = "load";
      thinbridge_mock.expects("redirect").never();
      const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
      thinbridge_mock.verify();
      assert.equal(shouldBlock, false);
    });
  });

  describe('checkRedirectIntervalLimit', () => {
    let clock;

    beforeEach(() => {
      clock = sinon.useFakeTimers();
      thinbridge.recentRequests = {};
    });

    afterEach(() => {
      delete thinbridge.recentRequests;
      clock.restore();
    });

    it('allow first request', () => {
      const url = "https://www.google.com/";
      const ret = thinbridge.checkRedirectIntervalLimit(tabId, url);
      assert.equal(ret, false);
    });

    it('skip frequent same request', () => {
      const url = "https://www.google.com/";
      const ret1 = thinbridge.checkRedirectIntervalLimit(tabId, url);
      clock.tick(300);
      const ret2 = thinbridge.checkRedirectIntervalLimit(tabId, url);
      assert.deepEqual({ ret1: !!ret1, ret2: !!ret2 }, { ret1: false, ret2: true });
    });

    it('allow same request after 1 sec', () => {
      const url = "https://www.google.com/";
      const ret1 = thinbridge.checkRedirectIntervalLimit(tabId, url);
      clock.tick(1001);
      const ret2 = thinbridge.checkRedirectIntervalLimit(tabId, url);
      assert.deepEqual({ ret1: !!ret1, ret2: !!ret2 }, { ret1: false, ret2: false });
    });

    it('allow same request in different tab', () => {
      const url = "https://www.google.com/";
      const ret1 = thinbridge.checkRedirectIntervalLimit(tabId, url);
      clock.tick(300);
      const ret2 = thinbridge.checkRedirectIntervalLimit(tabId + 1, url);
      assert.deepEqual({ ret1: !!ret1, ret2: !!ret2 }, { ret1: false, ret2: false });
    });
  });

});

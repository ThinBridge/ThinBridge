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


  describe('isRedirectURL', () => {

    it('no match should be redirected', () => {
      const url = "https://www.google.com/";
      const conf = config();
      const shouldBlock = thinbridge.isRedirectURL(conf, url);
      assert.equal(shouldBlock, true);
    });

    it('no match and no default should be loaded', () => {
      const url = "https://www.google.com/";
      const conf = config([], { DefaultBrowser: "" });
      const shouldBlock = thinbridge.isRedirectURL(conf, url);
      assert.equal(shouldBlock, false);
    });

    it('match redirect', () => {
      const url = "https://www.google.com/";
      const conf = config([chromeSection])
      const shouldBlock = thinbridge.isRedirectURL(conf, url);
      assert.equal(shouldBlock, true);
    });

    it('multiple patterns', () => {
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
      const shouldBlock = thinbridge.isRedirectURL(conf, url);
      assert.equal(shouldBlock, true);
    });

    it('custom18 should be loaded', () => {
      const url = "https://www.example.com/";
      const conf = config([custom18Section])
      const shouldBlock = thinbridge.isRedirectURL(conf, url);
      assert.equal(shouldBlock, false);
    });

    it('custom18 should be prior than others', () => {
      const url = "https://www.example.com/";
      const conf = config([chromeSection, custom18Section])
      const shouldBlock = thinbridge.isRedirectURL(conf, url);
      assert.equal(shouldBlock, false);
    });

    it('edge section should be loaded', () => {
      const url = "https://www.microsoft.com/";
      const conf = config([edgeSection])
      const shouldBlock = thinbridge.isRedirectURL(conf, url);
      assert.equal(shouldBlock, false);
    });

    it('edge section should be prior than others', () => {
      const url = "https://www.microsoft.com/";
      const conf = config([chromeSection, edgeSection])
      const shouldBlock = thinbridge.isRedirectURL(conf, url);
      assert.equal(shouldBlock, false);
    });

    it('match exclude pattern', () => {
      const url = "https://www.example.com/index.html";
      const conf = config([custom18Section])
      conf.Sections[0].Excludes = [url]
      const shouldBlock = thinbridge.isRedirectURL(conf, url);
      assert.equal(shouldBlock, true);
    });

    it('match multiple exclude pattern', () => {
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
      const shouldBlock = thinbridge.isRedirectURL(conf, url);
      assert.equal(shouldBlock, true);
    });

    it('ignore query', () => {
      const url = "https://www.google.com/search?q=hoge";
      const conf = config([queryTestSection], { DefaultBrowser: "" })
      const shouldBlock = thinbridge.isRedirectURL(conf, url);
      assert.equal(shouldBlock, true);
    });

    it('match with query', () => {
      const url = "https://www.google.com/search?q=hoge";
      const conf = config([queryTestSection], { DefaultBrowser: "", IgnoreQueryString: 0 })
      const shouldBlock = thinbridge.isRedirectURL(conf, url);
      assert.equal(shouldBlock, false);
    });
  });
});

const assert = require('assert');
const sinon = require('sinon');
const chromestub = require('./chrome-stub.js')
const edge = require('../testee/edge.js');

describe('Microsoft Edge Add-on', function() {
  const isClosableTab = true;
  const tabId = 123;
  const shouldCloseTab = true;

  describe('redirect rule: compatible mode', function() {
    const thinbridge = edge.client;
    let mock;
    const baseConfig = {
      CloseEmptyTab: 1,
      DefaultBrowser: "Chrome",
      IgnoreQueryString: 1,
      OnlyMainFrame : 1,
      Sections: [{
        "Name": "custom18",
        "Patterns": [],
        "Excludes": [],
      }],
    }

    function config(sections, additionals){
      const config = {...baseConfig, ...additionals};
      config.Sections = [...config.Sections, ...sections];
      config.NamedSections = Object.fromEntries(config.Sections.map(section => [section.Name, section]));
      return config;
    }

    beforeEach(function() {
      mock = sinon.mock(thinbridge);
    });

    afterEach(function() {
      mock.restore();
    });

    it('query is preserved', function() {
      const conf = config(
        [
          {
            "Name": "citrix",
            "Patterns": ["https://www.google.com/*"],
            "Excludes": [],
          }
        ],
      );
      const url = "https://www.google.com/search?q=foobar";
      mock.expects("redirect").once().withArgs(url, tabId, shouldCloseTab);
      const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
      mock.verify();
      assert.equal(shouldBlock, true);
    });

    it('no match should be redirected', function() {
      const url = "https://www.google.com/";
      mock.expects("redirect").once().withArgs(url, tabId, !shouldCloseTab);
      const shouldBlock = thinbridge.handleURLAndBlock(baseConfig, tabId, url, isClosableTab);
      mock.verify();
      assert.equal(shouldBlock, true);
    });

    it('custom18 should be loaded', function() {
      const url = "https://www.google.com/";
      const conf = config(
        [
          {
            "Name": "custom18",
            "Patterns": [url],
            "Excludes": [],
          }
        ]);
      mock.expects("redirect").never()
      const shouldBlock = thinbridge.handleURLAndBlock(conf, 0, url, isClosableTab);
      mock.verify();
      assert.equal(shouldBlock, false);
    });
  });

  describe('redirect rule: action mode', function() {
    const thinbridge = edge.client;
    let mock;
    const baseConfig = {
      CloseEmptyTab: 1,
      DefaultBrowser: "Chrome",
      IgnoreQueryString: 1,
      OnlyMainFrame : 1,
      Sections: [],
    }

    function config(sections, additionals){
      const config = {...baseConfig, ...additionals};
      config.Sections = [...config.Sections, ...sections];
      config.NamedSections = Object.fromEntries(config.Sections.map(section => [section.Name, section]));
      return config;
    }

    beforeEach(function() {
      mock = sinon.mock(thinbridge);
    });

    afterEach(function() {
      mock.restore();
    });

    it('load action', function() {
      const url = "https://www.google.com/";
      const conf = config(
	[
	  {
            "Name": "chrome",
            "Patterns": ["https://www.google.com/*"],
            "Excludes": [],
            "Action": "load",
          }
	]);
      mock.expects("redirect").never();
      const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
      mock.verify();
      assert.equal(shouldBlock, false);
    });

    it('redirect action', function() {
      const url = "https://www.google.com/";
      const conf = config(
	[
	  {
            "Name": "chrome",
            "Patterns": ["https://www.google.com/*"],
            "Excludes": [],
            "Action": "redirect",
          }
	]);
      mock.expects("redirect").once().withArgs(url, tabId, true);
      const shouldBlock = thinbridge.handleURLAndBlock(conf, tabId, url, isClosableTab);
      mock.verify();
      assert.equal(shouldBlock, true);
    });
  });
});

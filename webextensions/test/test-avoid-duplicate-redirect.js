const assert = require('assert');
const sinon = require('sinon');
const chromestub = require('./chrome-stub.js')

describe('avoidDuplicateRedirect', () => {
  const redirectors = [
    { browser: 'chrome', module: require('../testee/chrome.js') },
    { browser: 'edge',   module: require('../testee/edge.js')   },
  ].forEach(({browser, module}) => {
    function isEdge() {
      return browser == "edge";
    }
    describe(browser, () => {
      const thinbridge = module.client;
      const tabId = 123;
      let clock;

      beforeEach(() => {
        clock = sinon.useFakeTimers();
        thinbridge.recentRequests = {};
      });

      afterEach(() => {
        delete thinbridge.recentRequests;
        clock.restore();
        sinon.restore();
      });

      const config = {
        CloseEmptyTab: 1,
        DefaultBrowser: isEdge() ? "Chrome" : "Edge",
        IgnoreQueryString: 1,
        OnlyMainFrame : 1,
        Sections: [
          {
            "Name": "edge",
            "Patterns": [isEdge() ? "https://www.clear-code.com/*" : "*://*"],
            "Excludes": [],
          },
          {
            "Name": "chrome",
            "Patterns": [isEdge() ? "*://*" : "https://www.clear-code.com/*"],
            "Excludes": [],
          },
        ],
      };

      const tab = {
        id: tabId,
        windowId: 1,
        url: 'https://example.com/',
        status: 'complete',
      };

      const SERVER_NAME = 'com.clear_code.thinbridge';

      it('handleStartup => onTabUpdated', async () => {
        const sendNativeMessageStub = sinon.stub(chrome.runtime, 'sendNativeMessage')
          .resolves({ config });

        sinon.stub(chrome.tabs, 'query').resolves([tab]);
        sinon.stub(chrome.tabs, 'get')
          .resolves(null)
          .onCall(0).resolves(tab)
          .onCall(1).resolves(tab);
        sinon.stub(chrome.tabs, 'remove').resolves(true);
        sinon.stub(chrome.storage.session, 'get').resolves({ newTabIds: null, knownTabIds: null });

        thinbridge.cached = config;
        await thinbridge.handleStartup(config);
        await thinbridge.onTabUpdated(tabId, { url: tab.url }, { ...tab });
        await Promise.resolve(true); // We need to wait until all async operations are finished

        const redirectMessage = new String(`Q ${browser} ${tab.url}`);
        assert.equal(sendNativeMessageStub.withArgs(SERVER_NAME, redirectMessage).callCount, 1);
     });

      it('onTabUpdated => handleStartup', async () => {
        const sendNativeMessageStub = sinon.stub(chrome.runtime, 'sendNativeMessage')
          .resolves({ config });

        sinon.stub(chrome.tabs, 'query').resolves([tab]);
        sinon.stub(chrome.tabs, 'get')
          .resolves(null)
          .onCall(0).resolves(tab)
          .onCall(1).resolves(tab);
        sinon.stub(chrome.tabs, 'remove').resolves(true);
        sinon.stub(chrome.storage.session, 'get').resolves({ newTabIds: null, knownTabIds: null });

        thinbridge.cached = config;
        await thinbridge.onTabUpdated(tabId, { url: tab.url }, { ...tab });
        await thinbridge.handleStartup(config);
        await Promise.resolve(true); // We need to wait until all async operations are finished

        const redirectMessage = new String(`Q ${browser} ${tab.url}`);
        assert.equal(sendNativeMessageStub.withArgs(SERVER_NAME, redirectMessage).callCount, 1);
     });
    });
  });
});

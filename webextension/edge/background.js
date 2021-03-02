/*
 * This program is a browser module for ThinBridge that allows users
 * to redirect URLs to another browser (e.g. Edge -> IE).
 */
var BROWSER = 'edge'
var SERVER_NAME = 'com.clear_code.thinbridge';
var ALARM_MINUTES = 1;

var BROWSER_BACK = `data:text/html,${escape('<script type="application/javascript">history.back()</script>')}`;

var ThinBridgeTalkClient = {

  NAME: 'ThinBridgeTalkClient',

  init: function() {
    if (this.running) {
        return;
    }
    this.running = true;
    this.cached = null;
    this.callback = this.onBeforeRequest.bind(this);
    this.isNewTab = {};
    this.configure();
    this.listen();
    console.log('Running as Thinbridge Talk client');
  },

  configure: function() {
    var server = SERVER_NAME;
    var query = new String('C ' + BROWSER);

    chrome.runtime.sendNativeMessage(server, query, (resp) => {
      this.cached = resp.config;
      console.log('[Talk] configure', JSON.stringify(resp.config));
    });
  },

  listen: function() {
    chrome.webRequest.onBeforeRequest.addListener(
      this.callback,
      {
        urls: ['<all_urls>'],
        types: ['main_frame','sub_frame']
      },
      ['blocking']
    );

    /* Refresh config for every N minute */
    console.log('[Talk] poll config for every', ALARM_MINUTES , 'minutes');
    chrome.alarms.create(this.NAME, {'periodInMinutes': ALARM_MINUTES});

    chrome.alarms.onAlarm.addListener((alarm) => {
      if (alarm.name === this.NAME) {
        this.configure();
      }
    });

    /* Tab book-keeping for intelligent tab handlings */
    chrome.tabs.onCreated.addListener(tab => {
      this.isNewTab[tab.id] = 1;
    });

    chrome.tabs.onUpdated.addListener((id, info, tab) => {
      if (info.status === 'complete') {
        delete this.isNewTab[tab.id];
      }
    });
  },

  /* ThinBridge-compatible match() function
   * See 'CURLRedirectDataClass.wildcmp()' for details.
   */
  match: function(wild, string) {
    var i = 0;
    var j = 0;
    var mp, cp;

    while ((j < string.length) && (wild[i] != '*')) {
      if ((wild[i] != string[j]) && (wild[i] != '?')) {
        return 0;
      }
      i += 1;
      j += 1;
    }
    while (j < string.length) {
      if (wild[i] == '*') {
        i += 1;

        if (i == wild.length) {
          return 1;
        }
        mp = i;
        cp = j + 1
      } else if ((wild[i] == string[j]) || (wild[i] == '?')) {
        i += 1;
        j += 1;
      } else {
        i = mp;
        j = cp;
        cp += 1;
      }
    }
    while (wild[i] == '*' && i < wild.length) {
      i += 1;
    }
    return i >= wild.length;
  },

  redirect: function(bs, details) {
    var server = SERVER_NAME;
    var query = new String('Q ' + BROWSER + ' ' + details.url);

    if (details.tabId < 0) {
        return;
    }

    chrome.tabs.get(details.tabId, (tab) => {
      if (chrome.runtime.lastError) return;

      /* This is required to handle "preload" tabs */
      if (!tab) return;

      /* Open another browser via Query */
      chrome.runtime.sendNativeMessage(server, query);

      /* Close the opening tab automatically (if required) */
      if (details.type == 'main_frame') {
        if (bs.CloseEmptyTab && this.isNewTab[details.tabId]) {
          chrome.tabs.remove(details.tabId);
        }
      }
    });
    return { redirectUrl: BROWSER_BACK }
  },

  onBeforeRequest: function(details) {
    var bs = this.cached;
    var url = details.url;

    if (!bs) {
      console.log('[Talk] config cache is empty. Fetching...');
      this.configure();
      return;
    }

    if (bs.OnlyMainFrame && details.type != "main_frame") {
      console.log(`[Talk] ignore subframe request ${url}`);
      return;
    }

    if (bs.IgnoreQueryString) {
      url = url.replace(/\?.*/, '');
    }

    /* URLExcludePatterns */
    for (var i = 0; i < bs.URLExcludePatterns.length; i++) {
      var pattern = bs.URLExcludePatterns[i][0];
      var browser = bs.URLExcludePatterns[i][1].toLowerCase();

      if (browser != BROWSER)
        continue;

      if (this.match(pattern, url)) {
        console.log(`[Talk] Match Exclude ${pattern} ${url}`);
        return this.redirect(bs, details);
      }
    }

    /* URLPatterns */
    for (var i = 0; i < bs.URLPatterns.length; i++) {
      var pattern = bs.URLPatterns[i][0];
      var browser = bs.URLPatterns[i][1].toLowerCase();

      if (this.match(pattern, url)) {
        console.log(`[Talk] Match ${pattern} ${url}`);
        if (browser == BROWSER)
          return;
        return this.redirect(bs, details);
      }
    }

    /* No pattern matched */
    console.log(`[Talk] No pattern matched ${url}`);
    return this.redirect(bs, details);
  }
};

ThinBridgeTalkClient.init();

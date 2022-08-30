"use strict";

/*
 * Basic settings for modern browsers
 *
 * Programming Note: Just tweak these constants for each browser.
 * It should work fine across Edge, Chrome and Firefox without any
 * further modifications.
 */
const BROWSER = 'chrome';
const CUSTOM18 = 'custom18';
const SERVER_NAME = 'com.clear_code.thinbridge';
const ALARM_MINUTES = 1;
const CANCEL_REQUEST = {redirectUrl:`data:text/html,${escape('<script type="application/javascript">history.back()</script>')}`};

/*
 * ThinBridge's matching function (See BHORedirector/URLRedirectCore.h)
 *
 *  1. `?` represents a single character.
 *  2. `*` represents an arbitrary substring.
 *
 * >>> wildcmp("http?://*.example.com/*", "https://www.example.com/")
 * true
 */
function wildcmp(wild, string) {
  let i = 0;
  let j = 0;
  let mp, cp;

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
};

/*
 * Observe WebRequests with config fetched from ThinBridge.
 *
 * A typical configuration looks like this:
 *
 * {
 *   CloseEmptyTab:1, OnlyMainFrame:1, IgnoreQueryString:1, DefaultBrowser:"IE",
 *   Sections: [
 *     {Name:"ie", Path:"", Patterns:["*://example.com/*"], Excludes:[]},
 *     ...
 *   ]
 * }
 */
const ThinBridgeTalkClient = {

  init: function() {
    this.cached = null;
    this.callback = this.onBeforeRequest.bind(this);
    this.isNewTab = {};
    this.configure();
    this.listen();
    console.log('Running as Thinbridge Talk client');
  },

  configure: function() {
    const query = new String('C ' + BROWSER);

    chrome.runtime.sendNativeMessage(SERVER_NAME, query, (resp) => {
      if (chrome.runtime.lastError) {
        console.log('Cannot fetch config', JSON.stringify(chrome.runtime.lastError));
        return;
      }
      const isStartup = (this.cached == null);
      this.cached = resp.config;
      this.cached.NamedSections = Object.fromEntries(resp.config.Sections.map(section => [section.Name, section])));
      console.log('Fetch config', JSON.stringify(this.cached));

      if (isStartup) {
        this.handleStartup(this.cached);
      }
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
    console.log('Poll config for every', ALARM_MINUTES , 'minutes');
    chrome.alarms.create("poll-config", {'periodInMinutes': ALARM_MINUTES});

    chrome.alarms.onAlarm.addListener((alarm) => {
      if (alarm.name === "poll-config") {
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

  /*
   * Request redirection to Native Messaging Hosts.
   *
   * * chrome.tabs.get() is to confirm that the URL is originated from
   *   an actual tab (= not an internal prefetch request).
   *
   * * Request Example: "Q edge https://example.com/".
   */
  redirect: function(url, tabId, closeTab) {
    chrome.tabs.get(tabId, (tab) => {
      if (chrome.runtime.lastError) {
        console.log(`* Ignore prefetch request`);
        return;
      }
      if (!tab) {
        console.log(`* URL is not coming from an actual tab`);
        return;
      }

      const query = new String('Q ' + BROWSER + ' ' + url);
      chrome.runtime.sendNativeMessage(SERVER_NAME, query, (resp) => {
        if (closeTab) {
          chrome.tabs.remove(tabId);
        }
      });
    });
  },

  match: function(section, url, namedSections) {
    for (const name of section.ExcludeGroups) {
      const foreignSection = namedSections[name];
      if (!foreignSection)
        continue;
      for (const pattern of foreignSection.Patterns) {
        if (wildcmp(pattern, url)) {
          console.log(`* Match Exclude ${section.Name} (referring ${name}) [${pattern}]`);
          return false;
        }
      }
    }

    for (const pattern of section.Excludes) {
      if (wildcmp(pattern, url)) {
        console.log(`* Match Exclude ${section.Name} [${pattern}]`);
        return false;
      }
    }

    for (const pattern of section.Patterns) {
      if (wildcmp(pattern, url)) {
        console.log(`* Match ${section.Name} [${pattern}]`);
        return true;
      }
    }
    return false;
  },

  getBrowserName: function(section) {
    const name = section.Name.toLowerCase();

    /* CUSTOM18 means "common" URL */
    if (name == CUSTOM18)
      return name;

    /* Guess the browser name from the executable path */
    if (name.match(/^custom/i)) {
      if (section.Path.match(RegExp(BROWSER, "i")))
        return BROWSER;
    }
    return name;
  },

  isRedirectURL: function(config, url) {
    let section;
    const matches = [];

    if (!url) {
      console.log(`* Empty URL found`);
      return false;
    }

    if (!/^https?:/.test(url)) {
      console.log(`* Ignore non-HTTP/HTTPS URL`);
      return false;
    }

    if (config.IgnoreQueryString) {
      url = url.replace(/\?.*/, '');
    }

    console.log(`* Lookup sections for ${url}`);

    for (let i = 0; i < config.Sections.length; i++) {
      section = config.Sections[i];

      if (this.match(section, url, config.NamedSections)) {
        matches.push(this.getBrowserName(section))
      }
    }
    console.log(`* Result: [${matches.join(", ")}]`);

    if (matches.length > 0) {
      return !(matches.includes(CUSTOM18) || matches.includes(BROWSER));
    } else if (config.DefaultBrowser) {
      console.log(`* Use DefaultBrowser: ${config.DefaultBrowser}`);
      return !config.DefaultBrowser.match(RegExp(BROWSER, 'i'));
    } else {
      console.log(`* DefaultBrowser is blank`);
      return false;
    }
  },

  /* Handle startup tabs preceding to onBeforeRequest */
  handleStartup: function(config) {
    chrome.tabs.query({}, (tabs) => {
      tabs.forEach((tab) => {
        const url = tab.url || tab.pendingUrl;
        console.log(`handleStartup ${url} (tab=${tab.id})`);
        if (this.isRedirectURL(config, url)) {
          console.log(`* Redirect to another browser`);
          this.redirect(url, tab.id, config.CloseEmptyTab);
        }
      });
    });
  },

  /* Callback for webRequest.onBeforeRequest */
  onBeforeRequest: function(details) {
    const config = this.cached;
    let closeTab = false;
    const isMainFrame = (details.type == 'main_frame');

    console.log(`onBeforeRequest ${details.url} (tab=${details.tabId})`);

    if (!config) {
      console.log('* Config cache is empty. Fetching...');
      this.configure();
      return;
    }

    if (details.tabId < 0) {
      console.log(`* Ignore internal request`);
      return;
    }

    if (config.OnlyMainFrame && !isMainFrame) {
      console.log(`* Ignore subframe request`);
      return;
    }

    if (config.CloseEmptyTab && isMainFrame && this.isNewTab[details.tabId]) {
      closeTab = true;
    }

    if (this.isRedirectURL(config, details.url)) {
      console.log(`* Redirect to another browser`);
      this.redirect(details.url, details.tabId, closeTab);
      return CANCEL_REQUEST;
    }
  }
};

ThinBridgeTalkClient.init();

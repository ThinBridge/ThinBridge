'use strict';

/*
 * Basic settings for modern browsers
 *
 * Programming Note: Just tweak these constants for each browser.
 * It should work fine across Edge, Chrome and Firefox without any
 * further modifications.
 */
const BROWSER = 'edge';
const DMZ_SECTION = 'custom18';
const CONTINUOUS_SECTION = 'custom19';
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
  newTabIds: new Set(),
  knownTabIds: new Set(),
  resumed: false,

  init() {
    this.cached = null;
    this.ensureLoadedAndConfigured();
    console.log('Running as Thinbridge Talk client');
  },

  async ensureLoadedAndConfigured() {
    return Promise.all([
      !this.cached && this.configure(),
      this.load(),
    ]);
  },

  async configure() {
    const query = new String('C ' + BROWSER);

    const resp = await chrome.runtime.sendNativeMessage(SERVER_NAME, query);
    if (chrome.runtime.lastError) {
      console.log('Cannot fetch config', JSON.stringify(chrome.runtime.lastError));
      return;
    }
    const isStartup = (this.cached == null);
    this.cached = resp.config;
    this.cached.NamedSections = Object.fromEntries(resp.config.Sections.map(section => [section.Name, section]));
    console.log('Fetch config', JSON.stringify(this.cached));

    if (isStartup && !this.resumed) {
      this.handleStartup(this.cached);
    }
  },

  save() {
    chrome.storage.session.set({
      newTabIds: [...this.newTabIds],
      knownTabIds: [...this.knownTabIds],
    });
  },

  async load() {
    if (this.$promisedLoaded)
      return this.$promisedLoaded;

    console.log(`Redirector: loading previous state`);
    return this.$promisedLoaded = new Promise(async (resolve, _reject) => {
      try {
        const { newTabIds, knownTabIds } = await chrome.storage.session.get({ newTabIds: null, knownTabIds: null });
        console.log(`ThinBridgeTalkClient: loaded newTabIds, knownTabIds => `, JSON.stringify(newTabIds), JSON.stringify(knownTabIds));
        this.resumed = !!(newTabIds || knownTabIds);
        if (newTabIds) {
          for (const tabId of newTabIds) {
            this.newTabIds.add(tabId);
          }
        }
        if (knownTabIds) {
          for (const tabId of knownTabIds) {
            this.knownTabIds.add(tabId);
          }
        }
      }
      catch(error) {
        console.log('ThinBridgeTalkClient: failed to load previous state: ', error.name, error.message);
      }
      resolve();
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
  redirect(url, tabId, closeTab) {
    chrome.tabs.get(tabId).then(async tab => {
      if (chrome.runtime.lastError) {
        console.log(`* Ignore prefetch request`);
        return;
      }
      if (!tab) {
        console.log(`* URL is not coming from an actual tab`);
        return;
      }

      const query = new String('Q ' + BROWSER + ' ' + url);
      chrome.runtime.sendNativeMessage(SERVER_NAME, query);

      if (!closeTab)
        return;

      let existingTab = tab;
      let counter = 0;
      do {
        if (!existingTab)
          break;
        if (counter > 100) {
          console.log(`couldn't close tab ${tabId} within ${counter} times retry.`);
          break;
        }
        if (counter++ > 0)
          console.log(`tab ${tabId} still exists: trying to close (${counter})`);
        chrome.tabs.remove(tabId);
      } while (existingTab = await chrome.tabs.get(tabId).catch(_error => null));
    });
  },

  match(section, url, namedSections) {
    for (const name of (section.ExcludeGroups || [])) {
      const foreignSection = namedSections[name.toLowerCase()];
      //console.log(`* Referring exclude group ${name}: ${JSON.stringify(foreignSection && foreignSection.Patterns)}`);
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

  getBrowserName(section) {
    const name = section.Name.toLowerCase();

    if (name == DMZ_SECTION)
      return name;

    /* Guess the browser name from the executable path */
    if (name.match(/^custom/i)) {
      if (section.Path.match(RegExp(BROWSER, 'i')))
        return BROWSER;
    }
    return name;
  },

  handleURLAndBlock(config, tabId, url, isClosableTab) {
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

    let loadCount     = 0;
    let redirectCount = 0;
    let closeTabCount = 0;
    const matchedSectionNames = [];
    sectionsLoop:
    for (const section of config.Sections) {
      console.log(`handleURLAndBlock: check for section ${section.Name} (${JSON.stringify(section)})`);
      if (!this.match(section, url, config.NamedSections)) {
        console.log(` => unmatched`);
        continue;
      }

      const sectionName = (section.Name || '').toLowerCase();
      matchedSectionNames.push(sectionName);

      if (config.CloseEmptyTab && isClosableTab)
        closeTabCount++;

      console.log(` => matched, action = ${config.Action}`);
      if (section.Action) {
        switch(section.Action.toLowerCase()) {
          case 'redirect':
            redirectCount++;
            break;

          case 'load':
          default:
            loadCount++;
            break;
        }
        if (sectionName == DMZ_SECTION || sectionName == CONTINUOUS_SECTION)
          break sectionsLoop;
      }
      else {
        switch (this.getBrowserName(section)) {
          case DMZ_SECTION:
            console.log(` => action not defined, default action for CUSTOM18: load`);
            loadCount++;
            break sectionsLoop;

          case BROWSER.toLowerCase():
            console.log(` => action not defined, default action for ${BROWSER}: load`);
            loadCount++;
            break;

          default:
            console.log(` => action not defined, default action: redirect`);
            redirectCount++;
            if (sectionName == CONTINUOUS_SECTION)
              break sectionsLoop;
            break;
        }
      }
    }
    console.log(`* Result: [${matchedSectionNames.join(', ')}]`);

    if (redirectCount == 0) {
      console.log(`* No redirection: fallback to default`);
      if (config.DefaultBrowser == '' ||
          String(config.DefaultBrowser).toLowerCase() == BROWSER.toLowerCase()) {
        console.log(`* Continue to load as the default reaction`);
        loadCount++;
      }
      else {
        console.log(`* Redirect to the default browser ${config.DefaultBrowser}`);
        redirectCount++;
      }
    }

    if (redirectCount > 0 || loadCount == 0) {
      console.log(`* Redirect to another browser`);
      this.redirect(url, tabId, closeTabCount > 0);
    }
    console.log(`* Continue to load: ${loadCount > 0}`);
    return loadCount == 0;
  },

  /* Handle startup tabs preceding to onBeforeRequest */
  handleStartup(config) {
    chrome.tabs.query({}).then(tabs => {
      tabs.forEach((tab) => {
        const url = tab.url || tab.pendingUrl;
        console.log(`handleStartup ${url} (tab=${tab.id})`);
        this.handleURLAndBlock(config, tab.id, url, true);
      });
    });
  },

  async onTabCreated(tab) {
    await this.ensureLoadedAndConfigured();
    this.newTabIds.add(tab.id);
    this.save();
  },

  async onTabRemoved(tabId, _removeInfo) {
    await this.ensureLoadedAndConfigured();
    this.newTabIds.delete(tabId);
    this.knownTabIds.delete(tabId);
    this.save();
  },

  async onTabUpdated(tabId, info, tab) {
    await this.ensureLoadedAndConfigured();

    this.knownTabIds.add(tabId);

    if (info.status === 'complete' ||
        (info.status !== 'loading' && info.url)) {
      this.newTabIds.delete(tabId);
    }

    if (BROWSER !== 'edge') {
      this.save();
      return;
    }

    /*
     * Edge won't call webRequest.onBeforeRequest() when navigating
     * from Edge-IE to Edge, so we need to handle requests on this timing.
     * On such case, info.status is always undefined and only URL changes
     * are notified.
     */

    const config = this.cached;
    const url = tab.pendingUrl || tab.url;

    if (!config) {
      this.save();
      return;
    }

    console.log(`onTabUpdated ${url} (tab=${tabId}, windowId=${tab.windowId}, status=${info.status}/${tab.status})`);

    if (!this.handleURLAndBlock(config, tabId, url))
      return;

    console.log(`* Redirect to another browser`);
    /* Call executeScript() to stop the page loading immediately.
     * Then let the tab go back to the previous page.
     */
    chrome.scripting.executeScript({
      target: { tabId },
      func: function goBack(tabId) {
        window.stop();
        chrome.tabs.goBack(tabId);
      },
      args: [tabId],
    });
  },

  /* Callback for webRequest.onBeforeRequest */
  onBeforeRequest(details) {
    const config = this.cached;
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

    const isClosableTab = isMainFrame && (this.newTabIds.has(details.tabId) || !this.knownTabIds.has(details.tabId));

    if (this.handleURLAndBlock(config, details.tabId, details.url, isClosableTab))
      return CANCEL_REQUEST;
  },
};

chrome.webRequest.onBeforeRequest.addListener(
  ThinBridgeTalkClient.onBeforeRequest.bind(ThinBridgeTalkClient),
  {
    urls: ['<all_urls>'],
    types: ['main_frame','sub_frame']
  },
  ['blocking']
);

/* Refresh config for every N minute */
console.log('Poll config for every', ALARM_MINUTES , 'minutes');
chrome.alarms.create('poll-config', {'periodInMinutes': ALARM_MINUTES});

chrome.alarms.onAlarm.addListener((alarm) => {
  if (alarm.name === 'poll-config') {
    ThinBridgeTalkClient.configure();
  }
});

/* Tab book-keeping for intelligent tab handlings */
chrome.tabs.onCreated.addListener(ThinBridgeTalkClient.onTabCreated.bind(ThinBridgeTalkClient));
chrome.tabs.onUpdated.addListener(ThinBridgeTalkClient.onTabUpdated.bind(ThinBridgeTalkClient));


/*
 * Support ThinBridge's resource cap feature
 */
const ResourceCap = {

  init() {
    console.log('Running Resource Cap client');
  },

  /*
   * On each navigation, we ask the host program to check the
   * current resource usage.
   */
  onNavigationCommitted(details) {
    console.log(`onNavigationCommitted: ${details.url}`);

    /* frameId != 0 indicates iframe requests */
    if (details.frameId) {
      console.log(`* Ignore subframe requests`);
      return;
    }

    chrome.tabs.query({}).then(tabs => {
      const ntabs = this.count(tabs);
      console.log(`* Perform resource check (ntabs=${ntabs})`);
      this.check(details.tabId, ntabs);
    });
  },

  check(tabId, ntabs) {
    const query = new String(`R ${BROWSER} ${ntabs}`);
    chrome.runtime.sendNativeMessage(SERVER_NAME, query).then(resp => {
      // Need this to support ThinBridge v4.0.2.3 (or before)
      if (chrome.runtime.lastError) {
        return;
      }

      if (resp.closeTab) {
        chrome.tabs.remove(tabId).then(() => {
          if (chrome.runtime.lastError) {
            console.log(`* ${chrome.runtime.lastError}`);
            return;
          }
          console.log(`* Close Tab#${tabId}`)
        });
      }
    });
  },

  count(tabs) {
    /* Exclude the internal pages such as "edge://blank" */
    tabs = tabs.filter((tab) => {
      const url = tab.url || tab.pendingUrl;
      return /^https?:/.test(url);
    });
    return tabs.length;
  }
};

chrome.webNavigation.onCommitted.addListener(ResourceCap.onNavigationCommitted.bind(ResourceCap));

ThinBridgeTalkClient.init();
ResourceCap.init();

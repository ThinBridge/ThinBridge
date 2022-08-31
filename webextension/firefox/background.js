'use strict';

/*
 * Basic settings for modern browsers
 *
 * Programming Note: Just tweak these constants for each browser.
 * It should work fine across Edge, Chrome and Firefox without any
 * further modifications.
 */
const BROWSER = 'firefox';
const CUSTOM18 = 'custom18';
const SERVER_NAME = 'com.clear_code.thinbridge';
const ALARM_MINUTES = 1;
const CANCEL_REQUEST = {cancel: 1}

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

  init() {
    this.cached = null;
    this.callback = this.onBeforeRequest.bind(this);
    this.isNewTab = {};
    this.configure();
    this.listen();
    console.log('Running as Thinbridge Talk client');
  },

  configure() {
    const query = new String('C ' + BROWSER);

    browser.runtime.sendNativeMessage(SERVER_NAME, query).then(resp => {
      if (browser.runtime.lastError) {
        console.log('Cannot fetch config', JSON.stringify(browser.runtime.lastError));
        return;
      }
      const isStartup = (this.cached == null);
      this.cached = resp.config;
      this.cached.NamedSections = Object.fromEntries(resp.config.Sections.map(section => [section.Name, section]));
      console.log('Fetch config', JSON.stringify(this.cached));

      if (isStartup) {
        this.handleStartup(this.cached);
      }
    });
  },

  listen() {
    browser.webRequest.onBeforeRequest.addListener(
      this.callback,
      {
        urls: ['<all_urls>'],
        types: ['main_frame','sub_frame']
      },
      ['blocking']
    );

    /* Refresh config for every N minute */
    console.log('Poll config for every', ALARM_MINUTES , 'minutes');
    browser.alarms.create('poll-config', {'periodInMinutes': ALARM_MINUTES});

    browser.alarms.onAlarm.addListener((alarm) => {
      if (alarm.name === 'poll-config') {
        this.configure();
      }
    });

    /* Tab book-keeping for intelligent tab handlings */
    browser.tabs.onCreated.addListener(tab => {
      this.isNewTab[tab.id] = 1;
    });

    browser.tabs.onUpdated.addListener((id, info, tab) => {
      if (info.status === 'complete') {
        delete this.isNewTab[tab.id];
      }
    });

    if (BROWSER == 'edge') {
      /*
       * Edge won't call webRequest.onBeforeRequest() when navigating
       * from Edge-IE to Edge (GitLab#11).
       */
      browser.tabs.onUpdated.addListener(this.onTabUpdated.bind(this));
    }
  },

  /*
   * Request redirection to Native Messaging Hosts.
   *
   * * browser.tabs.get() is to confirm that the URL is originated from
   *   an actual tab (= not an internal prefetch request).
   *
   * * Request Example: "Q edge https://example.com/".
   */
  redirect(url, tabId, closeTab) {
    browser.tabs.get(tabId).then(tab => {
      if (browser.runtime.lastError) {
        console.log(`* Ignore prefetch request`);
        return;
      }
      if (!tab) {
        console.log(`* URL is not coming from an actual tab`);
        return;
      }

      const query = new String('Q ' + BROWSER + ' ' + url);
      browser.runtime.sendNativeMessage(SERVER_NAME, query).then(_resp => {
        if (closeTab) {
          browser.tabs.remove(tabId);
        }
      });
    });
  },

  match(section, url, namedSections) {
    for (const name of section.ExcludeGroups) {
      const foreignSection = namedSections[name.toLowerCase()];
      //console.log(`* Referring exlude group ${name}: ${JSON.stringify(foreignSection && foreignSection.Patterns)}`);
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

    /* CUSTOM18 means "common" URL */
    if (name == CUSTOM18)
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
        console.log(` => unmached`);
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
        if (sectionName == 'custom18' || sectionName == 'custom19')
          break sectionsLoop;
      }
      else {
        switch (this.getBrowserName(section)) {
          case 'custom18':
            console.log(` => action not defined, default action for CUSTMO18: load`);
            loadCount++;
            break sectionsLoop;

          case BROWSER.toLowerCase():
            console.log(` => action not defined, default action for ${BROWSER}: load`);
            loadCount++;
            break;

          default:
            console.log(` => action not defined, default action: redirect`);
            redirectCount++;
            if (sectionName == 'custom19')
              break sectionsLoop;
            break;
        }
      }
    }
    console.log(`* Result: [${matchedSectionNames.join(', ')}]`);

    if (redirectCount > 0 || loadCount == 0) {
      console.log(`* Redirect to another browser`);
      this.redirect(url, tabId, closeTabCount > 0);
    }
    console.log(`* Continue to load: ${loadCount > 0}`);
    return loadCount == 0;
  },

  /* Handle startup tabs preceding to onBeforeRequest */
  handleStartup(config) {
    browser.tabs.query({}).then(tabs => {
      tabs.forEach((tab) => {
        const url = tab.url || tab.pendingUrl;
        console.log(`handleStartup ${url} (tab=${tab.id})`);
        this.handleURLAndBlock(config, tab.id, url, true);
      });
    });
  },

  onTabUpdated(tabId, info, tab) {
    const config = this.cached;

    if (info.status !== "loading" ||
        !config)
      return;

    const url = tab.pendingUrl || tab.url;
    console.log(`onTabUpdated ${url} (tab=${tabId})`);

    if (!this.handleURLAndBlock(config, url))
      return;

    console.log(`* Redirect to another browser`);
    /* Call executeScript() to stop the page loading immediately.
     * Then let the tab go back to the previous page.
     */
    browser.tabs.executeScript(tabId, {code: 'window.stop()', runAt: 'document_start'}).then(() => {
      browser.tabs.goBack(tabId);
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

    const isClosableTab = isMainFrame && this.isNewTab[details.tabId];

    if (this.handleURLAndBlock(config, details.tabId, details.url, isClosableTab))
      return CANCEL_REQUEST;
  }
};

/*
 * Support ThinBridge's resource cap feature
 */
const ResourceCap = {

  init() {
    browser.webNavigation.onCommitted.addListener(ResourceCap.onNavigationCommitted);
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

    browser.tabs.query({}).then(tabs => {
      const ntabs = ResourceCap.count(tabs);
      console.log(`* Perform resource check (ntabs=${ntabs})`);
      ResourceCap.check(details.tabId, ntabs);
    });
  },

  check(tabId, ntabs) {
    const query = new String(`R ${BROWSER} ${ntabs}`);
    browser.runtime.sendNativeMessage(SERVER_NAME, query).then(resp => {
      // Need this to support ThinBridge v4.0.2.3 (or before)
      if (browser.runtime.lastError) {
        return;
      }

      if (resp.closeTab) {
        browser.tabs.remove(tabId).then(() => {
          if (browser.runtime.lastError) {
            console.log(`* ${browser.runtime.lastError}`);
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

ThinBridgeTalkClient.init();
ResourceCap.init();

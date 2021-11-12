"use strict";

/*
 * Basic settings for modern browsers
 *
 * Programming Note: Just tweak these constants for each browser.
 * It should work fine across Edge, Chrome and Firefox without any
 * further modifications.
 */
var BROWSER = 'edge';
var CUSTOM18 = 'custom18';
var SERVER_NAME = 'com.clear_code.thinbridge';
var ALARM_MINUTES = 1;
var CANCEL_REQUEST = {redirectUrl:`data:text/html,${escape('<script type="application/javascript">history.back()</script>')}`};

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
var ThinBridgeTalkClient = {

	init: function() {
		this.cached = null;
		this.callback = this.onBeforeRequest.bind(this);
		this.isNewTab = {};
		this.configure();
		this.listen();
		console.log('Running as Thinbridge Talk client');
	},

	configure: function() {
		var query = new String('C ' + BROWSER);

		chrome.runtime.sendNativeMessage(SERVER_NAME, query, (resp) => {
			if (chrome.runtime.lastError) {
				console.log('Cannot fetch config', JSON.stringify(chrome.runtime.lastError));
				return;
			}
			var isStartup = (this.cached == null);
			this.cached = resp.config;
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

		/*
		 * Edge won't call webRequest.onBeforeRequest() when navigating
		 * from Edge-IE to Edge (GitLab#11).
		 */
		chrome.tabs.onUpdated.addListener(this.onTabUpdated.bind(this));
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

			var query = new String('Q ' + BROWSER + ' ' + url);
			chrome.runtime.sendNativeMessage(SERVER_NAME, query, (resp) => {
				if (closeTab) {
					chrome.tabs.remove(tabId);
				}
			});
		});
	},

	match: function(section, url) {
		var i;
		for (i = 0; i < section.Excludes.length; i++) {
			if (wildcmp(section.Excludes[i], url)) {
				console.log(`* Match Exclude ${section.Name} [${section.Excludes[i]}]`);
				return false;
			}
		}

		for (i = 0; i < section.Patterns.length; i++) {
			if (wildcmp(section.Patterns[i], url)) {
				console.log(`* Match ${section.Name} [${section.Patterns[i]}]`);
				return true;
			}
		}
		return false;
	},

	getBrowserName: function(section) {
		var name = section.Name.toLowerCase();

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
		var section;
		var matches = [];

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

		for (var i = 0; i < config.Sections.length; i++) {
			section = config.Sections[i];

			if (this.match(section, url)) {
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
				var url = tab.url || tab.pendingUrl;
				console.log(`handleStartup ${url} (tab=${tab.id})`);
				if (this.isRedirectURL(config, url)) {
					console.log(`* Redirect to another browser`);
					this.redirect(url, tab.id, config.CloseEmptyTab);
				}
			});
		});
	},

	onTabUpdated: function(tabId, info, tab) {
		var config = this.cached;
		var url = tab.pendingUrl || tab.url;

		if (info.status !== "loading") {
			return;
		}
		if (!config) {
			return;
		}

		console.log(`onTabUpdated ${url} (tab=${tabId})`);

		if (this.isRedirectURL(config, url)) {
			console.log(`* Redirect to another browser`);
			this.redirect(url, tabId, false);

			/* Call executeScript() to stop the page loading immediately.
			 * Then let the tab go back to the previous page.
			 */
			chrome.tabs.executeScript(tabId, {code: "window.stop()", runAt: "document_start"}, () => {
				chrome.tabs.goBack(tabId);
			});
		}
	},

	/* Callback for webRequest.onBeforeRequest */
	onBeforeRequest: function(details) {
		var config = this.cached;
		var closeTab = false;
		var isMainFrame = (details.type == 'main_frame');

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

/*
 * Support ThinBridge's resource cap feature
 */
var ResourceCap = {

	init: function() {
		chrome.webNavigation.onCommitted.addListener(ResourceCap.onNavigationCommitted);
		console.log('Running Resource Cap client');
	},

	/*
	 * On each navigation, we ask the host program to check the
	 * current resource usage.
	 */
	onNavigationCommitted: function(details) {
		console.log(`onNavigationCommitted: ${details.url}`);

		/* frameId != 0 indicates iframe requests */
		if (details.frameId) {
			console.log(`* Ignore subframe requests`);
			return;
		}

		chrome.tabs.query({}, (tabs) => {
			var ntabs = ResourceCap.count(tabs);
			console.log(`* Perform resource check (ntabs=${ntabs})`);
			ResourceCap.check(details.tabId, ntabs);
		});
	},

	check: function(tabId, ntabs) {
		var query = new String(`R ${BROWSER} ${ntabs}`);
		chrome.runtime.sendNativeMessage(SERVER_NAME, query, (resp) => {
			// Need this to support ThinBridge v4.0.2.3 (or before)
			if (chrome.runtime.lastError) {
				return;
			}

			if (resp.closeTab) {
				chrome.tabs.remove(tabId, () => {
					if (chrome.runtime.lastError) {
						console.log(`* ${chrome.runtime.lastError}`);
						return;
					}
					console.log(`* Close Tab#${tabId}`)
				});
			}
		});
	},

	count: function(tabs) {
		/* Exclude the internal pages such as "edge://blank" */
		tabs = tabs.filter((tab) => {
			var url = tab.url || tab.pendingUrl;
			return /^https?:/.test(url);
		});
		return tabs.length;
	}
};

ThinBridgeTalkClient.init();
ResourceCap.init();

"use strict";

/*
 * This is an addon to redirect URLs from Chrome to ThinBridge.
 *
 * * Poll redirect rules every 1 minute via sendNativeMessage().
 *
 * * Keep watch on requests using webRequest.onBeforeRequest().
 *
 * * Redirection is done by sending the target URL with 'Q' prefix
 *   to the native host program.
 */

var BROWSER = 'chrome';
var CUSTOM18 = 'custom18';
var SERVER_NAME = 'com.clear_code.thinbridge';
var ALARM_MINUTES = 1;

/*
 * Chrome does not support {cancel: 1} as part of BlockingResponse.
 * This is a cludgy alternative that uses Data URL.
 */
var BROWSER_BACK = `data:text/html,${escape('<script type="application/javascript">history.back()</script>')}`;

var ThinBridgeTalkClient = {

	NAME: 'ThinBridgeTalkClient',

	init: function() {
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
			console.log('Configure', JSON.stringify(resp.config));
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

	/*
	 * This is the JavaScript-ported `wildcmp()` function, coming
	 * from CURLRedirectDataClass in URLRedirectCore.h.
	 */
	wildcmp: function(wild, string) {
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

	redirect: function(config, details) {
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
				if (config.CloseEmptyTab && this.isNewTab[details.tabId]) {
					chrome.tabs.remove(details.tabId);
				}
			}
		});
		return { redirectUrl: BROWSER_BACK }
	},

	match: function(url, section) {
		var i;
		var pattern;

		for (i = 0; i < section.Excludes.length; i++) {
			pattern = section.Excludes[i];

			if (this.wildcmp(pattern, url)) {
				console.log(`* Match Exclude ${section.Name} [${pattern}]`);
				return false;
			}
		}

		for (i = 0; i < section.Patterns.length; i++) {
			pattern = section.Patterns[i];

			if (this.wildcmp(pattern, url)) {
				console.log(`* Match ${section.Name} [${pattern}]`);
				return true;
			}
		}
		return false;
	},

	getName: function(section) {
		var name = section.Name.toLowerCase();

		if (name == CUSTOM18)
			return name;

		/*
		 * For custom sections, we need to guess the target browser
		 * from the executable path.
		 */
		if (name.match(/^custom/i)) {
			if (section.Path.match(RegExp(BROWSER, "i")))
				return BROWSER;
		}
		return name;
	},

	onBeforeRequest: function(details) {
		var config = this.cached;
		var matches = [];
		var section;
		var url = details.url;
		var i;

		if (!config) {
			console.log('Config cache is empty. Fetching...');
			this.configure();
			return;
		}

		if (config.OnlyMainFrame && details.type != "main_frame") {
			console.log(`Ignore subframe request ${url}`);
			return;
		}

		if (config.IgnoreQueryString) {
			url = url.replace(/\?.*/, '');
		}

		console.log(`Lookup sections for ${url}`);

		for (i = 0; i < config.Sections.length; i++) {
			section = config.Sections[i];

			if (this.match(url, section)) {
				matches.push(this.getName(section))
			}
		}
		console.log(`Result: ${matches.join(", ")}`);

		if (matches.length > 0) {
			/*
			 * CUSTOM18 contains a list of "common" URLs that we should
			 * never perform redirection.
			 */
			if (matches.includes(CUSTOM18))
				return;

			if (matches.includes(BROWSER))
				return;

			return this.redirect(config, details);
		}

		/* No pattern matched */
		console.log(`Use default browser ${config.DefaultBrowser}`);

		if (config.DefaultBrowser.match(RegExp(BROWSER, 'i')))
			return;

		return this.redirect(config, details);
	}
};

ThinBridgeTalkClient.init();

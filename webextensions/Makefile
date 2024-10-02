.PHONY: clean packages install_dependency lint format install_hook

NPM_MOD_DIR := $(CURDIR)/node_modules
NPM_BIN_DIR := $(NPM_MOD_DIR)/.bin

all: packages

testee:
	mkdir $@

testee/chrome.js: edge/background.js testee
	sed -e "s/ThinBridgeTalkClient.init();/exports.client = ThinBridgeTalkClient/g" -e "s/ResourceCap.init();//g" chrome/background.js > $@

testee/edge.js: edge/background.js testee
	sed -e "s/ThinBridgeTalkClient.init();/exports.client = ThinBridgeTalkClient/g" -e "s/ResourceCap.init();//g" edge/background.js > $@

unittest: install_dependency testee/chrome.js testee/edge.js
	npx mocha --require mocha-suppress-logs test

test: unittest

test-verbose: install_dependency testee/chrome.js testee/edge.js
	npx mocha test

clean:
	rm -f *.zip
	rm -f *.xpi
	rm -rf testee

packages: test clean lint
	cd chrome && make && make dev && mv *.zip ../
	cd edge && make && make dev && mv *.zip ../
	cd firefox && make && mv *.xpi ../

install_dependency:
	[ -e "$(NPM_BIN_DIR)/eslint" -a -e "$(NPM_BIN_DIR)/jsonlint-cli" ] || npm install --save-dev

lint: install_dependency
	"$(NPM_BIN_DIR)/eslint" . --ext=.js --report-unused-disable-directives
	find . -type d -name node_modules -prune -o -type f -name '*.json' -print | xargs "$(NPM_BIN_DIR)/jsonlint-cli"

format: install_dependency
	"$(NPM_BIN_DIR)/eslint" . --ext=.js --report-unused-disable-directives --fix

install_hook:
	echo '#!/bin/sh\nmake lint' > "$(CURDIR)/../.git/hooks/pre-commit" && chmod +x "$(CURDIR)/../.git/hooks/pre-commit"

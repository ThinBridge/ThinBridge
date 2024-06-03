.PHONY: clean install_dependency

NPM_MOD_DIR := $(CURDIR)/node_modules
NPM_BIN_DIR := $(NPM_MOD_DIR)/.bin

all: test

testee:
	mkdir $@

testee/edge.js: edge/background.js testee
	sed -e "s/ThinBridgeTalkClient.init();/exports.client = ThinBridgeTalkClient/g" -e "s/ResourceCap.init();//g" edge/background.js > $@

unittest: install_dependency testee/edge.js
	npx mocha --require mocha-suppress-logs test

test: unittest

test-verbose: install_dependency testee/edge.js
	npx mocha test

clean:
	rm -rf testee

install_dependency:
	npm install --save-dev

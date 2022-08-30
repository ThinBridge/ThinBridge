.PHONY: clean xpi

FILES = manifest.json \
        misc/128x128.png \
        background.js

all: xpi

clean:
	rm -f ThinBridgeFirefox.xpi

xpi: $(FILES)
	zip -9 - $(FILES) > ThinBridgeFirefox.xpi

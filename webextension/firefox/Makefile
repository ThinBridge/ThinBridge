FILES = manifest.json \
        misc/128x128.png \
        background.js

all: ThinBridgeFirefox.zip

clean:
	rm -f ThinBridgeFirefox.zip

ThinBridgeFirefox.zip: $(FILES)
	zip -9 - $(FILES) > $@

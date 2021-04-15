FILES = manifest.json \
        misc/128x128.png \
        background.js

all: ThinBridgeChrome.zip

clean:
	rm -f ThinBridgeChrome.zip

ThinBridgeChrome.zip: $(FILES)
	zip -9 - $(FILES) > $@

FILES = manifest.json \
        misc/128x128.png \
        background.js

all: ThinBridgeChrome.zip ThinBridgeChromeDev.zip

clean:
	rm -f ThinBridgeChrome.zip
	rm -f ThinBridgeChromeDev.zip

ThinBridgeChrome.zip: $(FILES)
	zip -9 - $(FILES) > $@

ThinBridgeChromeDev.zip: $(FILES)
	rm -rf dev
	mkdir -p dev
	cp -R manifest.json misc background.js dev/
	sed -i -E -e 's/ThinBridge/ThinBridge Enterrpise Developer Edition/g' dev/manifest.json
	cd dev && zip -9 - $(FILES) > ../$@

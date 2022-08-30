FILES = manifest.json \
        misc/128x128.png \
        _locales/ja/messages.json \
        background.js

all: ThinBridgeEdge.zip ThinBridgeEdgeDev.zip

clean:
	rm -f ThinBridgeEdge.zip
	rm -f ThinBridgeEdgeDev.zip

ThinBridgeEdge.zip: $(FILES)
	zip -9 - $(FILES) > $@

ThinBridgeEdgeDev.zip: $(FILES)
	rm -rf dev
	mkdir -p dev
	cp -R manifest.json misc _locales background.js dev/
	sed -i -E -e 's/ThinBridge/ThinBridge Enterrpise Developer Edition/g' dev/_locales/*/messages.json
	cp $(FILES)
	cd dev && zip -9 - $(FILES) > ../$@

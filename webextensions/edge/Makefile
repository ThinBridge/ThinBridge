.PHONY: clean zip dev

FILES = manifest.json \
        misc/128x128.png \
        _locales/ja/messages.json \
        background.js

all: zip dev

clean:
	rm -f *.zip

zip: $(FILES)
	zip -9 - $(FILES) > ThinBridgeEdge.zip

dev: $(FILES)
	rm -rf dev
	mkdir -p dev
	cp -R manifest.json misc _locales background.js dev/
	sed -i -E -e 's/ThinBridge/ThinBridge Enterprise Developer Edition/g' dev/_locales/*/messages.json
	cd dev && zip -9 - $(FILES) > ../ThinBridgeEdgeDev.zip

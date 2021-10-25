FILES = manifest.json \
        misc/128x128.png \
        misc/alert.png \
        background.js

all: ThinBridgeEdge.zip

clean:
	rm -f ThinBridgeEdge.zip

ThinBridgeEdge.zip: $(FILES)
	zip -9 - $(FILES) > $@

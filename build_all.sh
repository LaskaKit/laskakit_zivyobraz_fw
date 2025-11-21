#!/bin/bash

# space separated values
BOARDS=$(grep '\[env:' platformio.ini | tr -d '][' | cut -d ':' -f 2 | tr '\n' ' ')
DISPLAYS=$(grep '^#.*DISPLAY_' include/displays.hpp | cut -d '_' -f 2 | tr '\n' ' ')NONE


for BOARD in $BOARDS; do
    for DISPLAY in $DISPLAYS; do
        echo "Building Zivyobraz for $DISPLAY $BOARD"
        DISPLAY_TYPE="DISPLAY_$DISPLAY" pio run -e "$BOARD"
		if [ $? -ne 0 ]; then
			exit 1
		fi
    done
done

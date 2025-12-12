#!/bin/bash

# space separated values
# BOARDS=$(grep '\[env:' platformio.ini | tr -d '][' | cut -d ':' -f 2 | tr '\n' ' ')
BOARDS=espink-v3
# DISPLAYS=$(grep '^#.*DISPLAY_' include/displays.hpp | cut -d '_' -f 2 | tr '\n' ' ')NONE
DISPLAYS="GDEY075T7 GDEM075F52 GDEM102F91"

for BOARD in $BOARDS; do
    for DISPLAY in $DISPLAYS; do
        if [ "$BOARD" == "espink-v2" ] && [ "$DISPLAY" == "GDEY1248F51" ]; then
            echo "Skipping $BOARD $DISPLAY"
            continue
        fi
        echo "Building Zivyobraz for $BOARD $DISPLAY"
        DISPLAY_TYPE="DISPLAY_$DISPLAY" pio run -e "$BOARD"
		if [ $? -ne 0 ]; then
			exit 1
		fi
    done
done

#!/bin/bash

# space separated values
BOARDS="espink-v3 uespink-v1"
DISPLAYS=$(grep '^#.*DISPLAY_' include/displays.hpp | cut -d '_' -f 2 | tr '\n' ' ')NONE
# DISPLAYS="GDEY075T7 GDEY075Z08 GDEY1248F51 GDEM102F91 GDEM075F52"

for BOARD in $BOARDS; do
    for DISPLAY in $DISPLAYS; do
        echo "###############################################"
        echo "Building Zivyobraz for $BOARD $DISPLAY"
        echo "###############################################"
        DISPLAY_TYPE="DISPLAY_$DISPLAY" pio run -e "$BOARD"
		if [ $? -ne 0 ]; then
			exit 1
		fi
    done
done

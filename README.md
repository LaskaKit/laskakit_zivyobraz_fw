# Supported Hardware

## Boards
- ESPink v3.5
- ESPink v2.7

Should work on other espink_v2.x and espink_v3.x but it is not tested.

## Displays
- GDEY075T7
<!--- GDEY1248F51-->
- E2741FS081
- E2741CS0B2
- NONE (headless)


# How to build

There are 2 environments:

- espink_v2
- espink_v3


## Build examples

Build fw for espink_v2 and GDEY075T7 display:

`DISPLAY_TYPE=DISPLAY_GDEY075T7 pio run -e espink_v2`

Build fw for espink_v3 without display:

`pio run -e espink_v3`


## Flashing

Build creates 2 binaries:

- firmware.bin
- firmware_merged.bin

firmware_merged.bin is complete binary and be flashed with esptool from address 0x0.

Or you can flash directly from platformio by appending `-t upload` to the build command.

`DISPLAY_TYPE=DISPLAY_GDEY075T7 pio run -e espink_v3 -t upload`

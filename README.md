# Supported Hardware

## Boards
- ESPink v3.5
- ESPink v2.7

Should work on other espink_v2.x and espink_v3.x but it is not tested.

## Displays
- GDEY075T7
- GDEY075Z08
- GDEY1248F51 (espink-v3 only)
- E2741FS081
- E2741CS0B2
- NONE

# How to build

Make sure you have synced submodules:

`git submodule update --init`

There are 2 environments:

- espink-v2
- espink-v3


## Build examples

Build all fw at once:

`./build_all.sh`

Build fw for espink_v2 and GDEY075T7 display:

`DISPLAY_TYPE=DISPLAY_GDEY075T7 pio run -e espink_v3`

Build fw for espink_v3 without display:

`pio run -e espink_v3`


## Flashing

Build creates binaries and puts them into artifacts directory. The binaries contain all partitions needed (bootloader, partitions, app0, fw) to flash it from 0x0.

Example with esptool:

`esptool --chip esp32s3 --port /dev/ttyACM0 --baud 921600 write_flash 0x0 artifacts/zo_espink-v3_GDEY075T7_0-3-0.bin`

| Board       | Chip    |
| ----------- | ------- |
| ESPink v3.5 | esp32s3 |
| ESPink v2.7 | esp32   |

Or you can flash directly from platformio by appending `-t upload` to the build command.

`DISPLAY_TYPE=DISPLAY_GDEY075T7 pio run -e espink_v3 -t upload`

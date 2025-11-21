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

## Sensors

All sensors are detected automatically. It is even possible to daisy chain them but for use with zivyobraz use just one. `temp`, `hum` and `pres` url params are used for exporting values to zivyobraz.

- [SHT40](https://www.laskakit.cz/laskakit-sht40-senzor-teploty-a-vlhkosti-vzduchu/)
- [BME280](https://www.laskakit.cz/arduino-senzor-tlaku--teploty-a-vlhkosti-bme280/)
- [SCD41](https://www.laskakit.cz/laskakit-scd41-senzor-co2--teploty-a-vlhkosti-vzduchu/) [pres=co2]
- [STCC4](https://www.laskakit.cz/laskakit-stcc4-senzor-co2--teploty-a-vlhkosti-vzduchu/) [pres=co2]
- [SGP41](https://www.laskakit.cz/laskakit-sgp41-voc-a-nox-senzor-kvality-ovzdusi/) [temp=nox, hum=voc]
- [BH1750](https://www.laskakit.cz/laskakit-bh1750-snimac-intenzity-osvetleni/) [temp=lux]

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

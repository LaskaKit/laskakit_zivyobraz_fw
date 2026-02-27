# Supported Hardware

## Boards
- ESPink  v3.5
- ESPink  v2.7
- uESPink v1.2

Should work on other espink_v2.x and espink_v3.x but it is not tested.

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

## Build commands examples

Build all fw at once:

`./build_all.sh`

Build and flash fw for espink-v3 and GDEY075T7 display:

`DISPLAY_TYPE=DISPLAY_GDEY075T7 pio run -e espink-v3 -t upload`

Build and flash fw for espink-v3 and display that uses GxEPD driver:

`DISPLAY_TYPE=DISPLAY_GDEY116F51 pio run -e espink-v3-gxepd -t upload`

Build fw for espink_v3 without display:

`pio run -e espink_v3`


## Flashing

| Board        | Chip    |
| ------------ | ------- |
| ESPink v3.5  | esp32s3 |
| ESPink v2.7  | esp32   |
| uESPink v1.2 | esp32s3 |

Build creates binaries and puts them into `./artifacts` directory. The binaries contain all partitions needed (bootloader, partitions, app0, fw) to flash it from 0x0.

Example with esptool:

`esptool --chip esp32s3 --port /dev/ttyACM0 --baud 921600 write-flash 0x0 artifacts/zo_espink-v3_GDEY075T7_0-3-0.bin`

Or you can flash directly from platformio by appending `-t upload` to the build command.

`DISPLAY_TYPE=DISPLAY_GDEY075T7 pio run -e espink-v3 -t upload`


# Supported displays

| size | colorspace | name | driver | espink-v3 | espink-v2 | uespink-v1 | url |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 1.54 | C4 | GDEM0154F51H | GXEPD | 1 | 0 | 1 | [eshop](https://www.laskakit.cz/good-display-gdem0154f51h-1-54--200x200-epaper-displej-4-barvy/) |
| 2.13 | BW | GDEY0213B74 | GXEPD | 0 | 1 | 0 | [eshop](https://www.laskakit.cz/good-display-gdey0213b74-2-13--250x122-epaper-displej/) |
| 2.9 | BW | GDEY029T71H | GXEPD | 0 | 1 | 0 | [eshop](https://www.laskakit.cz/good-display-gdey029t71h-2-9--384x168-epaper-displej/) |
| 4.2 | C4 | GDEM042F52 | GXEPD | 1 | 0 | 0 | [eshop](https://www.laskakit.cz/good-display-gdem042f52-4-2--400x300-epaper-displej-4-barvy/) |
| 4.26 | BW | GDEQ0426T82 | RAW | 1 | 0 | 0 | [eshop](https://www.laskakit.cz/good-display-gdeq0426t82-4-26--800x480-epaper-displej-grayscale/) |
| 5.83 | C4 | GDEY0583F41 | RAW | 1 | 0 | 0 | [eshop](https://www.laskakit.cz/good-display-gdey0583f41-5-83--648x480-epaper-displej-4-barvy/) |
| 7.4 | BW | E2741CS0B2 | RAW | 1 | 0 | 0 | [eshop](https://www.laskakit.cz/pervasive-displays-e2741cs0b2-7-4--800x480-epaper-displej/) |
| 7.4 | RBW | E2741FS081 | RAW | 1 | 0 | 0 | [eshop](https://www.laskakit.cz/pervasive-displays-e2741fs081-7-4--800x480-epaper-cerveny-displej/) |
| 7.5 | G4 | GDEY075T7 | RAW | 1 | 0 | 0 | [eshop](https://www.laskakit.cz/good-display-gdey075t7-7-5--800x480-epaper-displej-grayscale/) |
| 7.5 | RBW | GDEY075Z08 | RAW | 1 | 0 | 0 | [eshop](https://www.laskakit.cz/good-display-gdey075z08-7-5--800x480-epaper-cerveny-displej/) |
| 7.5 | C4 | GDEM075F52 | RAW | 1 | 0 | 0 | [eshop](https://www.laskakit.cz/good-display-gdem075f52-7-5--800x480-epaper-displej-4-barvy/) |
| 10.2 | C4 | GDEM102F91 | GXEPD | 1 | 0 | 0 | [eshop](https://www.laskakit.cz/good-display-gdem102f91-10-2--960x640-epaper-displej-4-barvy/) |
| 11.6 | C4 | GDEY116F51 | GXEPD | 1 | 0 | 0 | [eshop](https://www.laskakit.cz/good-display-gdey116f51-11-6--960x640-epaper-displej-4-barvy/) |
| 12.48 | C4 | GDEY1248F51 | RAW | 1 | 0 | 0 | [eshop](https://www.laskakit.cz/good-display-gdey1248z51-12-48--1304x984-epaper-cerveny-displej/) |

# Supported Hardware

## Boards
- ESPink v3.5
- ESPink v2.7

Should work on other espink_v2.x and espink_v3.x but it is not tested.

## Sensors

All sensors are detected automatically. It is even possible to daisy chain them but for use with zivyobraz use just one. `temp`, `hum` and `pres` url params are used for exporting values to zivyobraz.

- [SHT40](https://www.laskakit.cz/laskakit-sht40-senzor-teploty-a-vlhkosti-vzduchu/)
- [BME280](https://www.laskakit.cz/arduino-senzor-tlaku--teploty-a-vlhkosti-bme280/)
- [SCD41](https://www.laskakit.cz/laskakit-scd41-senzor-co2--teploty-a-vlhkosti-vzduchu/) [pres=co2]
- [STCC4](https://www.laskakit.cz/laskakit-stcc4-senzor-co2--teploty-a-vlhkosti-vzduchu/) [pres=co2]
- [SGP41](https://www.laskakit.cz/laskakit-sgp41-voc-a-nox-senzor-kvality-ovzdusi/) [temp=nox, hum=voc]
- [BH1750](https://www.laskakit.cz/laskakit-bh1750-snimac-intenzity-osvetleni/) [temp=lux]


## Displays

Here we list all displays that are supported/planned to be supported by this fw. All displays are available on LaskaKit store.

### 4 Color
| Display | Size (inch) | Resolution | Aspect Ratio | ESPink V2 | ESPink V3 |
| ------- | ----------- | ---------- | ------------ | --------- | --------- |
| [GDEM042F52](https://www.laskakit.cz/good-display-gdem042f52-4-2--400x300-epaper-displej-4-barvy/) | 4.5 | 400x300 | 4:3 | :x: | :x: |
| [GDEM075F52](https://www.laskakit.cz/good-display-gdem075f52-7-5--800x480-epaper-displej-4-barvy/) | 7.5 | 800x480 | 5:3 | :white_check_mark: | :white_check_mark: |
| [GDEM102F91](https://www.laskakit.cz/good-display-gdem102f91-10-2--960x640-epaper-displej-4-barvy/) | 10.2 | 960x640 | 3:2 | :white_check_mark: | :white_check_mark: |
| [GDEY0213F51](https://www.laskakit.cz/good-display-gdey0213f51-2-13--250x122--epaper-displej-4-barvy/) | 2.13 | 250x122 | ~2:1 | :x: | :x: |
| [GDEY0266F51H](https://www.laskakit.cz/good-display-gdey0266f51h-2-66--360x184-epaper-displej-4-barvy/) | 2.66 | 360x184 | ~2:1 | :x: | :x: |
| [GDEY029F51H](https://www.laskakit.cz/good-display-gdey029f51h-2-9--384x168-epaper-displej-4-barvy/) | 2.9 | 384x168 | 16:7 | :x: | :x: |
| [GDEY116F51](https://www.laskakit.cz/good-display-gdey116f51-11-6--960x640-epaper-displej-4-barvy/) | 11.6 | 960x640 | 3:2 | :x: | :x: |
| [GDEY1248F51](https://www.laskakit.cz/good-display-gdey1248z51-12-48--1304x984-epaper-cerveny-displej/) | 12.48 | 1304x984 | ~4:3 | :x: | :white_check_mark: |

### Black/White

| Display | Size (inch) | Resolution | Aspect Ratio | ESPink V2 | ESPink V3 |
| ------- | ----------- | ---------- | ------------ | --------- | --------- |
| [GDEM102T91](https://www.laskakit.cz/good-display-gdeq102t90-10-2--960x640-epaper-displej/) | 10.2 | 960x640 | 3:2 | :x: | :x: |
| [GDEM1085T51](https://www.laskakit.cz/good-display-gdem1085t51-10-85--1360x480-epaper-displej/) | 10.85 | 1360x480 | 17:6 | :x: | :x: |
| [GDEM133T91](https://www.laskakit.cz/good-display-gdem133t91-13-3--960x680-epaper-displej/) | 13.3 | 960x680 | 24:17 | :x: | :x: |
| [GDEQ031T10](https://www.laskakit.cz/good-display-gdeq031t10-3-1--296x128-epaper-displej/) | 3.1 | 296x128 | 37:16 | :x: | :x: |
| [GDEY042T81](https://www.laskakit.cz/good-display-gdeq042t81-4-2--400x300-epaper-displej/) | 4.2 | 400x300 | 4:3 | :x: | :x: |
| [E2741CS0B2](https://www.laskakit.cz/pervasive-displays-e2741cs0b2-7-4--800x480-epaper-displej/) | 7.4 | 800x480 | 5:3 | :white_check_mark: | :white_check_mark: |


### 4 Gray

<!-- spatny colorspace v popisku grayscale displeju-->
| Display | Size (inch) | Resolution | Aspect Ratio | ESPink V2 | ESPink V3 |
| ------- | ----------- | ---------- | ------------ | --------- | --------- |
| [GDEQ0426T82](https://www.laskakit.cz/good-display-gdeq0426t82-4-26--800x480-epaper-displej-grayscale/) | 4.26 | 800x480 | 5:3 | :x: | :x: |
| [GDEQ0583T81](https://www.laskakit.cz/good-display-gdeq0583t31-5-83--648x480-epaper-displej-grayscale/) | 5.83 | 648x480 | ~4:3 | :x: | :x: |
| [GDEW0154T8D](https://www.laskakit.cz/good-display-gdew0154t8d-1-54--152x152-epaper-displej-grayscale/) | 1.54 | 152x152 | 1:1 | :x: | :x: |
| [GDEY0154D67](https://www.laskakit.cz/good-display-gdey0154d67-1-54--200x200-epaper-displej/) | 1.54 | 200x200 | 1:1 | :x: | :x: |
| [GDEY029T94](https://www.laskakit.cz/good-display-gdey029t94-2-9--296x128-epaper-displej-grayscale/) | 2.9 | 296x128 | 37x16 | :x: | :x: |
| [GDEY116T91](https://www.laskakit.cz/good-display-gdey116t91-11-6--960x640-epaper-displej-grayscale/) | 11.6 | 960x640 | 3:2 | :x: | :x: |
| [GDEY075T7](https://www.laskakit.cz/good-display-gdey075t7-7-5--800x480-epaper-displej-grayscale/) | 7.5 | 800x480 | 5:3 | :white_check_mark: | :white_check_mark: |


### 16 Gray

<!-- spatny colorspace v popisku -->
| Display | Size (inch) | Resolution | Aspect Ratio | ESPink V2 | ESPink V3 |
| ------- | ----------- | ---------- | ------------ | --------- | --------- |
| [GDEP133UT3](https://www.laskakit.cz/good-display-gdep133ut3-13-3--1600x1200-parallel-epaper-displej/) | 13.3 | 1600x1200 | 4:3 | :x: | :x: |


### Black/White/Red

| Display | Size (inch) | Resolution | Aspect Ratio | ESPink V2 | ESPink V3 |
| ------- | ----------- | ---------- | ------------ | --------- | --------- |
| [GDEM133Z91](https://www.laskakit.cz/good-display-gdem133z91-13-3--960x680-epaper-cerveny-displej/) | 13.3 | 960x680 | 24:17 | :x: | :x: |
| [GDEY075Z08](https://www.laskakit.cz/good-display-gdey075z08-7-5--800x480-epaper-cerveny-displej/) | 7.5 | 800x480 | 5:3 | :white_check_mark: | :white_check_mark: |
| [GDEY116Z91](https://www.laskakit.cz/good-display-gdey116z91-11-6--960x640-epaper-cerveny-displej/) | 11.6 | 960x640 | 3:2 | :x: | :x: |
| [E2741FS081](https://www.laskakit.cz/pervasive-displays-e2741fs081-7-4--800x480-epaper-cerveny-displej/) | 7.4 | 800:480 | 5:3 | :white_check_mark: | :white_check_mark: |


### 6 Colors

| Display | Size (inch) | Resolution | Aspect Ratio | ESPink V2 | ESPink V3 |
| ------- | ----------- | ---------- | ------------ | --------- | --------- |
| [GDEP040E01](https://www.laskakit.cz/good-display-gdep040e01-4-0--600x400-epaper-barevny-displej/) | 4.0 | 600x400 | 3:2 | :x: | :x: |
| [GDEP073E01](https://www.laskakit.cz/good-display-gdep073e01-7-3--800x480-epaper-barevny-displej/) | 7.3 | 800x480 | 5:3 | :x: | :x: |
| [GDEP133C02](https://www.laskakit.cz/good-display-gdep133c02-13-3--1600x1200-qspi-epaper-barevny-displej/) | 13.3 | 1600x1200 | 4:3 | :x: | :x: |

<!-- celkem 29 displeju -->


# How to build

Make sure you have synced submodules:

`git submodule update --init`

There are 2 environments:

- espink-v2
- espink-v3


## Build commands examples

Build all fw at once:

`./build_all.sh`

Build fw for espink_v2 and GDEY075T7 display:

`DISPLAY_TYPE=DISPLAY_GDEY075T7 pio run -e espink_v3`

Build fw for espink_v3 without display:

`pio run -e espink_v3`


## Flashing

Build creates binaries and puts them into artifacts directory. The binaries contain all partitions needed (bootloader, partitions, app0, fw) to flash it from 0x0.

Example with esptool:

`esptool --chip esp32s3 --port /dev/ttyACM0 --baud 921600 write-flash 0x0 artifacts/zo_espink-v3_GDEY075T7_0-3-0.bin`

| Board       | Chip    |
| ----------- | ------- |
| ESPink v3.5 | esp32s3 |
| ESPink v2.7 | esp32   |

Or you can flash directly from platformio by appending `-t upload` to the build command.

`DISPLAY_TYPE=DISPLAY_GDEY075T7 pio run -e espink_v3 -t upload`
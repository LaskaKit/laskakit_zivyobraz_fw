# ESPink Firmware

ESP32-based firmware for LaskaKit e-paper displays with Zivyobraz integration.

## Features

- **LaskaKit E-Paper Library**: Unified interface for various e-paper panels
- **Zivyobraz Client**: Fetches and displays images from zivyobraz.eu
- **Multiple Board Support**: Compatible with various ESPink board versions
- **PlatformIO Integration**: Easy build and deployment

## Supported Hardware

### Boards
- ESPink v3.3
- ESPink v3.5 (with PSRAM)
- ESPink-4.26 (ESP32 Dev)
- EPDiy v7
- Raw EPDiy v7 (FastEPD)

### Displays
- GDEQ0426T82 (4.26" tri-color)
- GDEY042T81 (4.2" black/white)
- GDEY075T7 (7.5" - in development)

## Getting Started

### Prerequisites
- [PlatformIO](https://platformio.org/) installed
- ESP32 development environment

### Building

1. Clone the repository
2. Open in PlatformIO
3. Select your target environment:
   - `espink` - ESPink v3.3/v3.5 (ESP32-S3)
   - `espink42` - ESPink-4.26 (ESP32 Dev)
   - `espink_v3-5` - ESPink v3.5 with PSRAM

4. Build and upload:
```bash
pio run -e <your_environment>
pio run -e <your_environment> -t upload
```

### Configuration

Update pin definitions in `src/main.cpp` for your board:

```cpp
// ESPink v3.5 pinout (default)
#define SDA 11
#define SCL 12
#define CS 10
#define DC 48
#define RST 45
#define BUSY 38
#define PWR 47
```

## Project Structure

```
├── src/                    # Main source code
├── include/               # Header files and fonts
├── lib/
│   ├── laskakit_epaper/   # E-paper display library
│   └── laskakit_zivyobraz_client/  # Zivyobraz client library
└── pctest/               # PC testing utilities
```

## Libraries

### LaskakitEpaper (v0.1.0)
Unified interface for e-paper panels available on LaskaKit boards.

### LaskakitZivyobrazClient (v0.1.0)
Client library for fetching images from zivyobraz.eu service.

## Dependencies

- GxEPD2 (^1.6.4)
- Adafruit GFX Library (^1.12.1)
- bb_epaper (^1.1.2)

## Status

🚧 Work in progress - Active development

## License

See project files for license information.

## Author

Patrik Novak


#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <esp_log.h>

#include "boards.hpp"
#include "displays.hpp"
#include "laskakit_epaper.hpp"
#include "zivyobrazclient.hpp"
#include "sensor.hpp"
#include "epdbus.hpp"
#include "zdecoder.h"
#include "gfx.hpp"

// ZIVYOBRAZ CLIENT PARAMS
namespace {
    constexpr const char* ZIVYOBRAZ_HOST = "https://cdn.zivyobraz.eu";
    constexpr const char* ZIVYOBRAZ_FIRMWARE_VERSION = "2.4";
    constexpr const char* ZIVYOBRAZ_FIRMWARE_TYPE = VERSION;

    constexpr const char* AP_SSID = "ESPink";
    constexpr const char* AP_PASS = "zivyobraz";
    constexpr const char* AP_CONN_STR = "WIFI:S:ESPink;T:WPA;P:zivyobraz;";

    constexpr size_t DEEP_SLEEP_TIME_S = 120;
}

// ---------------


using namespace LaskaKit::ZivyObraz;
using namespace LaskaKit::Epaper;

EPDBusSettings epdBusSettings = {
    PIN_EPD_SCL,
    PIN_EPD_SDA,
    PIN_EPD_CS,
    PIN_EPD_DC,
    PIN_EPD_BUSY,
    PIN_EPD_RST,
    PIN_PWR,
};
DISPLAY_T display = DISPLAY_T(epdBusSettings);
GFX<DISPLAY_T> gfxDisplay(&display);
WiFiManager wm;
uint8_t rowBuffer[DISPLAY_T::WIDTH];

void zdecRowCallback(const struct ZDecoder* decoder);  // forward declaration
ZDec decoder(DISPLAY_T::WIDTH, DISPLAY_T::HEIGHT, rowBuffer, zdecRowCallback);

void downloadCallback(ContentType contentType, const uint8_t* data, size_t datalen);  // forward declaration
ZivyObrazClient client(ZIVYOBRAZ_HOST, downloadCallback);

SensorReading sensorReading;

// power on peripherals (I2C bus, display, ...)
void powerOn()
{
    if (!PIN_PWR) return;
    pinMode(PIN_PWR, OUTPUT);
    digitalWrite(PIN_PWR, HIGH);
}

// power off peripherals (I2C bus, display, ...)
void powerOff()
{
    if (!PIN_PWR) return;
    pinMode(PIN_PWR, OUTPUT);
    digitalWrite(PIN_PWR, LOW);
}


double readBattery()
{
    pinMode(PIN_VBAT, INPUT);
    uint32_t milliVolts = analogReadMilliVolts(PIN_VBAT);
    double voltage = (milliVolts / 1000.0) * VBAT_DIVIDER_RATIO;
    return voltage;
}


// universal callback
void zdecRowCallback(const struct ZDecoder* decoder) {
    for (uint16_t col = 0; col < decoder->width; col++) {
        switch (DISPLAY_T::COLORTYPE) {
            case ColorType::BW:
            case ColorType::G4:
                gfxDisplay.drawPixel(col, decoder->currentRow, ZtoRGB565(decoder->rowBuffer[col], z2GrayscaleToRGB565Lut, 4));
                break;
            case ColorType::C4:
            case ColorType::RBW:
            case ColorType::YBW:
                gfxDisplay.drawPixel(col, decoder->currentRow, ZtoRGB565(decoder->rowBuffer[col], z2ColorToRGB565Lut, 4));
                break;
            case ColorType::G8:
                gfxDisplay.drawPixel(col, decoder->currentRow, ZtoRGB565(decoder->rowBuffer[col], z3GrayscaleToRGB565Lut, 8));
                break;
            case ColorType::C7:
                gfxDisplay.drawPixel(col, decoder->currentRow, ZtoRGB565(decoder->rowBuffer[col], z3ColorToRGB565Lut, 8));
                break;
        }
    }
}

void downloadCallback(ContentType contentType, const uint8_t* data, size_t datalen)
{
    Serial.printf("Download callback %ld bytes\n", datalen);
    switch (contentType) {
        case ContentType::APPLICATION_JSON:
            Serial.println("Received JSON");
            break;
        case ContentType::IMAGE_Z2:
        case ContentType::IMAGE_Z3:
            Serial.println("Received image/z2-3");
            decoder.decode(data, datalen);
            break;
        case ContentType::IMAGE_BMP:
            Serial.println("Received image/bmp");
            break;
        case ContentType::IMAGE_PNG:
            Serial.println("Received image/png");
            break;
        case ContentType::TEXT_PLAIN:
            Serial.println("Received text/plain");
            break;
        case ContentType::TEXT_HTML:
            Serial.println("Received text/html");
            break;
        case ContentType::UNKNOWN:
            Serial.println("Received unsuported content type");
            break;
    }
}

void displaySensors(GFX<DISPLAY_T>& gfxDisplay, SensorReading& reading)
{
    if (reading.flag & Sensor::_SHT4x) {
        gfxDisplay.printf(" SHT4x:T%.1fH%.1f\n", reading.sht.temperature, reading.sht.humidity);
    }
    if (reading.flag & Sensor::_BME280) {
        gfxDisplay.printf(" BME280:T%.1fH%.1fP%.1f\n", reading.bme.temperature, reading.bme.humidity, reading.bme.pressure);
    }
    if (reading.flag & Sensor::_SCD4x) {
        gfxDisplay.printf(" SCD4xT%.1fH%.1fCO2%.1f\n", reading.scd.temperature, reading.scd.humidity, reading.scd.co2);
    }
    if (reading.flag & Sensor::_STCC4) {
        gfxDisplay.printf(" STCC4:T%.1fH%.2fCO2%.1f\n", reading.stcc4.temperature, reading.stcc4.humidity, reading.stcc4.co2);
    }
    if (reading.flag & Sensor::_SGP41) {
        gfxDisplay.printf(" SGP41:VOC%dNOx%d\n", reading.sgp41.voc, reading.sgp41.nox);
    }
    if (reading.flag & Sensor::_BH1750) {
        gfxDisplay.printf(" BH1750:LUX%.1f\n", reading.bh1750.lux);
    }
}

void screenConfigPortal(GFX<DISPLAY_T>& gfxDisplay)
{
    gfxDisplay.fillScreen(static_cast<uint16_t>(RGB565::WHITE));

    uint8_t scale = 4;
    if (gfxDisplay.height() < 300 || gfxDisplay.width() < 300) {
        gfxDisplay.setTextSize(1);
        scale = 3;
    } else if (gfxDisplay.height() < 480 || gfxDisplay.width() < 480) {
        gfxDisplay.setTextSize(1);
        scale = 4;
    } else {
        gfxDisplay.setTextSize(2);
        scale = 6;
    }
    gfxDisplay.setCursor(0, 0);

    gfxDisplay.setTextColor(static_cast<uint16_t>(RGB565::BLACK));
    gfxDisplay.printf("    Board: ");
    #if defined ESPINK_V3
        gfxDisplay.printf(" ESPink v3\n");
    #elif defined ESPINK_V2
        gfxDisplay.printf(" ESPink v2\n");
    #elif defined UESPINK_V1
        gfxDisplay.printf(" MicroESPink v1\n");
    #elif defined EPDIY_V7
        gfxDisplay.printf(" EPDIY v7");
    #else
        gfxDisplay.printf("unknown\n");
    #endif
    gfxDisplay.printf("  Display: %s\n", DISPLAY_T::NAME);
    gfxDisplay.printf("      Res: %lux%lu\n", DISPLAY_T::WIDTH, DISPLAY_T::HEIGHT);
    gfxDisplay.printf("    Color: %s\n", colorTypeToCStr(DISPLAY_T::COLORTYPE));
    gfxDisplay.printf("       IP: %s\n", WiFi.softAPIP().toString().c_str());
    gfxDisplay.printf("      MAC: %s\n", WiFi.macAddress().c_str());
    gfxDisplay.printf("  AP SSID: %s\n", AP_SSID);
    gfxDisplay.printf("  AP PASS: %s\n", AP_PASS);
    gfxDisplay.printf("  Battery: %4.2f V\n", readBattery());
    // gfxDisplay.printf("    PSRAM: %d bytes\n", psramFound() ? ESP.getPsramSize() : 0);
    gfxDisplay.printf("   Sensor:\n");
    displaySensors(gfxDisplay, sensorReading);
    gfxDisplay.drawColorSwatch();

    uint16_t qrsize = 25 * scale;
    gfxDisplay.drawQRCodeText(2 * scale, gfxDisplay.height() - qrsize - 2 * scale, AP_CONN_STR, static_cast<uint16_t>(RGB565::BLACK), static_cast<uint16_t>(RGB565::WHITE), scale);
    gfxDisplay.fullUpdate();
}

void screenConfigPortalTimeout(GFX<DISPLAY_T>& gfxDisplay)
{
    gfxDisplay.fillScreen(static_cast<uint16_t>(RGB565::WHITE));
    gfxDisplay.setTextSize(2);
    gfxDisplay.setCursor(0, 0);
    gfxDisplay.printf("Configuration portal has timed out.\n");
    gfxDisplay.printf("Press reset to retry.");
    gfxDisplay.fullUpdate();
}

#ifdef ESPINK_V3
void setupButton()
{
    pinMode(PIN_BUTTON, INPUT);
}

bool buttonPressed()
{
    return digitalRead(PIN_BUTTON) == 0;
}

void handleButtonPress()
{
    if (buttonPressed()) {
        log_i("Button pressed. Clear white and reset wifi settings.");
        wm.erase();
        gfxDisplay.fillScreen(static_cast<uint16_t>(RGB565::WHITE));  // Clear the screen
        gfxDisplay.fullUpdate();
        wm.startConfigPortal(AP_SSID, AP_PASS);
        // uint64_t sleepTimeSeconds = DEEP_SLEEP_TIME_S * 30 * 24 * 30;
        // log_i("Sleep for %lluh %llum %llus", sleepTimeSeconds / 3600, (sleepTimeSeconds % 3600) / 60, sleepTimeSeconds % 60);
        // esp_sleep_enable_timer_wakeup(sleepTimeSeconds * 1000000);
        // WiFi.mode(WIFI_OFF);
        // esp_deep_sleep_start();
    }
}
#endif  // ESPINK_V3

String assembleUrl()
{
    String path;
    path += "/?mac=" + WiFi.macAddress();
    path += "&x=" + String(DISPLAY_T::WIDTH);
    path += "&y=" + String(DISPLAY_T::HEIGHT);
    path += "&c=" + String(colorTypeToCStr(DISPLAY_T::COLORTYPE));
    path += "&fw=" + String(ZIVYOBRAZ_FIRMWARE_VERSION);
    path += "&fwType=" + String(ZIVYOBRAZ_FIRMWARE_TYPE);
    path += "&timestamp_check=1";
    path += "&ssid=" + WiFi.SSID();
    path += "&rssi=" + String(WiFi.RSSI());
    path += "&v=" + String(readBattery());
    if (sensorReading.flag & Sensor::_SHT4x) {
        path += "&temp=" + String(sensorReading.sht.temperature);
        path += "&hum=", String(sensorReading.sht.humidity);
    } else if (sensorReading.flag & Sensor::_BME280) {
        path += "&temp=" + String(sensorReading.bme.temperature);
        path += "&hum=" + String(sensorReading.bme.humidity);
        path += "&pres=" + String(sensorReading.bme.pressure);
    } else if (sensorReading.flag & Sensor::_SCD4x) {
        path += "&temp=" + String(sensorReading.scd.temperature);
        path += "&hum=" + String(sensorReading.scd.humidity);
        path += "&pres=" + String(sensorReading.scd.co2);
    } else if (sensorReading.flag & Sensor::_STCC4) {
        path += "&temp=" + String(sensorReading.stcc4.temperature);
        path += "&hum=" + String(sensorReading.stcc4.humidity);
        path += "&pres=" + String(sensorReading.stcc4.co2);
    } else if (sensorReading.flag & Sensor::_SGP41) {
        path += "&temp=" + String(sensorReading.sgp41.nox);
        path += "&hum=" + String(sensorReading.sgp41.voc);
    } else if (sensorReading.flag & Sensor::_BH1750) {
        path += "&temp=" + String(sensorReading.bh1750.lux);
    }
    return path;
}

// return sleep time in seconds
uint64_t parseSleepTime()
{
    char headerValue[20];  // tmp buffer
    if (client.getHeader(headerValue, "PreciseSleep")) {
        log_i("Use PreciseSleep");
        return String(headerValue).toInt();
    } else if (client.getHeader(headerValue, "SleepSeconds")) {
        log_i("Use SleepSeconds");
        return String(headerValue).toInt();
    } else if (client.getHeader(headerValue, "Sleep")) {
        log_i("Using Sleep");
        return String(headerValue).toInt() * 60;
    } else {
        log_i("No sleep in headers. Using default");
        return DEEP_SLEEP_TIME_S;
    }
}

void setup()
{
    Wire.setPins(PIN_I2C_SDA, PIN_I2C_SCL);
    Serial.begin(115200);
#ifdef ESPINK_V3
    setupButton();
#endif
    wm.setConfigPortalTimeout(300);
    wm.setConnectTimeout(30);
    wm.setHostname("ESPink");
    wm.setAPCallback([](WiFiManager* myWiFiManager) {
        screenConfigPortal(gfxDisplay);
    });
    wm.setConfigPortalTimeoutCallback([]() {
        screenConfigPortalTimeout(gfxDisplay);
        uint64_t sleepTimeSeconds = DEEP_SLEEP_TIME_S * 30 * 24 * 30;
        log_i("Sleep for %lluh %llum %llus", sleepTimeSeconds / 3600, (sleepTimeSeconds % 3600) / 60, sleepTimeSeconds % 60);
        esp_sleep_enable_timer_wakeup(sleepTimeSeconds * 1000000);
        WiFi.mode(WIFI_OFF);
        esp_deep_sleep_start();
    });
    log_i("Start");

    // power on peripherals
    log_i("Peripherals ON");
    powerOn();
#ifdef ESPINK_V3
    handleButtonPress();
#endif

    // read sensors
    log_i("Reading sensors.");
    readSensors();

    // connect to wifi
    bool wmStatus = wm.autoConnect(AP_SSID, AP_PASS);
    if (!wmStatus) {
        log_i("Could not connect");
        return;
    }
    log_i("Connected to WiFi!");
    log_i("Local IP: %s", WiFi.localIP().toString());
    log_i("MAC: %s", WiFi.macAddress());

    String path = assembleUrl();
    log_i("Downloading image.");
    client.get(path.c_str());

    // shutdown wifi
    log_i("Shutting down WiFi");
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);

    // update display
    if (decoder.state() == ZERROR) {
        log_e("Decoder encountered an error.");
        log_e("Reason: %d", decoder.error());
    } else {
        log_i("Redrawing display.");
        display.fullUpdate();
    }

    // power off peripherals
    log_i("Peripherals OFF");
    powerOff();

    // deep sleep
    uint64_t sleepTimeSeconds = parseSleepTime();
    log_i("Sleep for %lluh %llum %llus", sleepTimeSeconds / 3600, (sleepTimeSeconds % 3600) / 60, sleepTimeSeconds % 60);
    esp_sleep_enable_timer_wakeup(sleepTimeSeconds * 1000000);
    esp_deep_sleep_start();
}


void loop()
{
    // Serial.println("test");
    log_i("test");
    delay(1500);
}

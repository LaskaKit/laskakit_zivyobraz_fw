#include <memory>
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <qrcode.h>
#include <WiFiManager.h>


#include "boards.hpp"
#include "displays.hpp"
#include "espclient.hpp"
#include "sensor.hpp"
#include "epdbus.hpp"
#include "gfx.hpp"
#include "zdecoder.h"

// ZIVYOBRAZ CLIENT PARAMS
namespace {
    constexpr const char* ZIVYOBRAZ_HOST = "https://cdn.zivyobraz.eu";
    constexpr const char* ZIVYOBRAZ_FIRMWARE_VERSION = "2.4";
    constexpr const char* ZIVYOBRAZ_FIRMWARE_TYPE = VERSION;

    constexpr const char* AP_SSID = "ESPINK-Setup";
    constexpr const char* AP_PASS = "zivyobraz";

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
    PIN_EPD_RST
};
DISPLAY_T display = DISPLAY_T(epdBusSettings);
WiFiManager wm;

extern const uint16_t z2ColorToRGB565Lut[4];
extern const uint16_t z2GrayscaleToRGB565Lut[4];
extern const uint16_t z3ColorToRGB565Lut[8];
extern const uint16_t z3GrayscaleToRGB565Lut[8];

// power on peripherals (I2C bus, display, ...)
void powerOn()
{
    pinMode(PIN_PWR, OUTPUT);
    digitalWrite(PIN_PWR, HIGH);
}

// power off peripherals (I2C bus, display, ...)
void powerOff()
{
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
void cb(const struct ZDecoder* decoder) {
    for (uint16_t col = 0; col < decoder->width; col++) {
        switch (DISPLAY_T::COLORTYPE) {
            case ColorType::BW:
            case ColorType::G4:
                display.drawPixel(col, decoder->currentRow, ZtoRGB565(decoder->rowBuffer[col], z2GrayscaleToRGB565Lut, 4));
            case ColorType::C4:
            case ColorType::RBW:
            case ColorType::YBW:
                display.drawPixel(col, decoder->currentRow, ZtoRGB565(decoder->rowBuffer[col], z2ColorToRGB565Lut, 4));
            case ColorType::G8:
                display.drawPixel(col, decoder->currentRow, ZtoRGB565(decoder->rowBuffer[col], z3GrayscaleToRGB565Lut, 8));
            case ColorType::C7:
                display.drawPixel(col, decoder->currentRow, ZtoRGB565(decoder->rowBuffer[col], z3ColorToRGB565Lut, 8));
        }
    }
}


void _drawColorSwatch(GFX<DISPLAY_T>& gfx, const uint16_t* colorLut, uint8_t numColors, uint16_t posX, uint16_t swatchWidth = 20) {
    uint16_t swatchPartHeight = gfx.height() / numColors;
    for (size_t i = 0; i < 4; i++) {
        uint16_t posY = swatchPartHeight * i;
        gfx.fillRect(posX, posY, swatchWidth, swatchPartHeight, colorLut[i]);
    }
    gfx.fillRect(posX, 0, 2, gfx.height(), colorLut[1]);
    gfx.fillRect(posX + swatchWidth -2, 0, 2, gfx.height(), colorLut[1]);
    gfx.fillRect(posX, 0, swatchWidth, 2, colorLut[1]);
    gfx.fillRect(posX, gfx.height() - 2, swatchWidth, 2, colorLut[1]);
}

void drawColorSwatch(GFX<DISPLAY_T>& gfx)
{
    switch (DISPLAY_T::COLORTYPE) {
        case ColorType::G4:
            _drawColorSwatch(gfx, z2GrayscaleToRGB565Lut, 4, gfx.width() - 20);
            break;
        case ColorType::C4:
            _drawColorSwatch(gfx, z2ColorToRGB565Lut, 4, gfx.width() - 20);
            break;
    }
}


void setup()
{
    powerOn();
    // set I2C pins
    Wire.setPins(PIN_I2C_SDA, PIN_I2C_SCL);
    Serial.begin(115200);

    // testGfx(&display);
    // return;

    wm.setConfigPortalTimeout(300);
    wm.setConnectTimeout(30);
    wm.setHostname("ESPINK");

    wm.setAPCallback([](WiFiManager* myWiFiManager) {
        Serial.println("Entered config mode");
        Serial.println(WiFi.softAPIP());
        Serial.println("WiFi Manager");
        Serial.println("Connect to: " + myWiFiManager->getConfigPortalSSID());
        // display_qr(display, 150, 80, 10);
        display.fullUpdate();
    });

    if (!wm.autoConnect(AP_SSID, AP_PASS)) {
        Serial.println("Failed to connect, starting configuration portal");
        if (!wm.startConfigPortal(AP_SSID, AP_PASS)) {
            Serial.println("Failed to connect and hit timeout.");
            esp_sleep_enable_timer_wakeup(DEEP_SLEEP_TIME_S * 1000000);
            delay(100);
            esp_deep_sleep_start();
        }
    }

    Serial.println("\nConnected to WiFi!");
    Serial.println("Local IP: " + WiFi.localIP().toString());
    Serial.println(WiFi.macAddress());

    SensorReading sensorReading = readSensors();
    printSensors(sensorReading);

    static uint8_t rowBuffer[DISPLAY_T::WIDTH];
    ZDec decoder(DISPLAY_T::WIDTH, DISPLAY_T::HEIGHT, rowBuffer, cb);

    static EspClient<ZDec> client(ZIVYOBRAZ_HOST, &decoder);
    client.addParam("mac", WiFi.macAddress().c_str());
    client.addParam("x", String(DISPLAY_T::WIDTH).c_str());
    client.addParam("y", String(DISPLAY_T::HEIGHT).c_str());
    client.addParam("c", colorTypeToCStr(DISPLAY_T::COLORTYPE));
    client.addParam("fw", ZIVYOBRAZ_FIRMWARE_VERSION);
    client.addParam("fwType", ZIVYOBRAZ_FIRMWARE_TYPE);
    client.addParam("timestamp_check", "1");
    client.addParam("ssid", WiFi.SSID().c_str());
    client.addParam("rssi", std::to_string(WiFi.RSSI()).c_str());
    client.addParam("v", std::to_string(readBattery()).c_str());

    if (sensorReading.flag & Sensor::_SHT4x) {
        client.addParam("temp", std::to_string(sensorReading.sht.temperature).c_str());
        client.addParam("hum", std::to_string(sensorReading.sht.humidity).c_str());
    } else if (sensorReading.flag & Sensor::_BME280) {
        client.addParam("temp", std::to_string(sensorReading.bme.temperature).c_str());
        client.addParam("hum", std::to_string(sensorReading.bme.humidity).c_str());
        client.addParam("pres", std::to_string(sensorReading.bme.pressure).c_str());
    } else if (sensorReading.flag & Sensor::_SCD4x) {
        client.addParam("temp", std::to_string(sensorReading.scd.temperature).c_str());
        client.addParam("hum", std::to_string(sensorReading.scd.humidity).c_str());
        client.addParam("pres", std::to_string(sensorReading.scd.co2).c_str());
    } else if (sensorReading.flag & Sensor::_STCC4) {
        client.addParam("temp", std::to_string(sensorReading.stcc4.temperature).c_str());
        client.addParam("hum", std::to_string(sensorReading.stcc4.humidity).c_str());
        client.addParam("pres", std::to_string(sensorReading.stcc4.co2).c_str());
    } else if (sensorReading.flag & Sensor::_SGP41) {
        client.addParam("temp", std::to_string(sensorReading.sgp41.nox).c_str());
        client.addParam("hum", std::to_string(sensorReading.sgp41.voc).c_str());
    } else if (sensorReading.flag & Sensor::_BH1750) {
        client.addParam("temp", std::to_string(sensorReading.bh1750.lux).c_str());
    }


    unsigned long start = millis();

    client.get();
    // TODO shutdowm WiFI
    Serial.printf("Download time: %lu ms\n", millis() - start);

    start = millis();
    display.fullUpdate();
    powerOff();
    Serial.printf("Update time: %lu ms\n", millis() - start);

    // Process headers
    char headerValue[20];  // tmp buffer
    uint64_t sleepTimeMinutes = 0;
    uint64_t sleepTimeSeconds = 0;
    client.getHeader(headerValue, "Sleep");
    sleepTimeMinutes = String(headerValue).toInt();
    client.getHeader(headerValue, "SleepSeconds");
    sleepTimeSeconds = String(headerValue).toInt();


    if (sleepTimeSeconds > 0) {
        Serial.printf("Deep sleep: Using SleepSeconds: %d s\n", sleepTimeSeconds);
        esp_sleep_enable_timer_wakeup(sleepTimeSeconds * 1000000);
    } else if (sleepTimeMinutes > 0) {
        Serial.printf("Deep sleep: Using Sleep: %d m\n", sleepTimeMinutes);
        esp_sleep_enable_timer_wakeup(sleepTimeMinutes * 60 * 1000000);
    } else {
        Serial.printf("Deep sleep: Using Default: %d s\n", DEEP_SLEEP_TIME_S);
        esp_sleep_enable_timer_wakeup(DEEP_SLEEP_TIME_S * 1000000);
    }
    delay(100);
    esp_deep_sleep_start();
}

void loop()
{
    Serial.println("display test");
    delay(15000);
}

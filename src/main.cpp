#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <esp_log.h>
#include <ArduinoJson.h>

#include "boards.hpp"
#include "displays.hpp"
#include "laskakit_epaper.hpp"
#include "zivyobrazclient.hpp"
#include "sensor.hpp"
#include "epdbus.hpp"
#include "zdecoder.h"
#include "bmpdecoder.h"
#include "gfx.hpp"
#include "apsettings.hpp"

// ZIVYOBRAZ CLIENT PARAMS
#define ZIVYOBRAZ_HOST = "https://cdn.zivyobraz.eu";
#define ZIVYOBRAZ_API_VERSION = "3.0";
#define ZIVYOBRAZ_FIRMWARE_VERSION = VERSION;
#define DEEP_SLEEP_TIME_S = 120;


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

APSettings apSettings;
ZDec zDecoder;
BMPDec bmpDecoder;
ZivyObrazClient zoClient;

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


// universal row callback
template <class DECODER_T>
void rowCallback(const DECODER_T* decoder) {
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

bool handleZ(const uint8_t* data, size_t len)
{
    zDecoder.decode(data, len);
    return zDecoder.state() != ZERROR;
}

bool handleBMP(const uint8_t* data, size_t len)
{
    bmpDecoder.decode(data, len);
    return bmpDecoder.state() != BMP_ERROR;
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

    uint16_t qrsize = 29 * scale;
    gfxDisplay.drawQRCodeText(2 * scale, gfxDisplay.height() - qrsize - 2 * scale, apSettings.getConnstr().c_str(), static_cast<uint16_t>(RGB565::BLACK), static_cast<uint16_t>(RGB565::WHITE), scale);
    gfxDisplay.fullUpdate();
    log_i("Displaying config portal display.");
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
        wm.startConfigPortal(apSettings.ssid, apSettings.password);
        // uint64_t sleepTimeSeconds = DEEP_SLEEP_TIME_S * 30 * 24 * 30;
        // log_i("Sleep for %lluh %llum %llus", sleepTimeSeconds / 3600, (sleepTimeSeconds % 3600) / 60, sleepTimeSeconds % 60);
        // esp_sleep_enable_timer_wakeup(sleepTimeSeconds * 1000000);
        // WiFi.mode(WIFI_OFF);
        // esp_deep_sleep_start();
    }
}
#endif  // ESPINK_V3


// return sleep time in seconds
uint64_t parseSleepTime()
{
    char headerValue[20];  // tmp buffer
    if (zoClient.getHeader(headerValue, 20, "PreciseSleep")) {
        log_i("Use PreciseSleep");
        return String(headerValue).toInt();
    } else if (zoClient.getHeader(headerValue, 20, "SleepSeconds")) {
        log_i("Use SleepSeconds");
        return String(headerValue).toInt();
    } else if (zoClient.getHeader(headerValue, 20, "Sleep")) {
        log_i("Using Sleep");
        return String(headerValue).toInt() * 60;
    } else {
        log_i("No sleep in headers. Using default");
        return DEEP_SLEEP_TIME_S;
    }
}

String buildJsonPayload()
{
    JsonDocument doc;
    doc["apiVersion"] = ZIVYOBRAZ_API_VERSION;
    #if defined ESPINK_V3
        doc["board"] = "ESPink_V3";
    #elif defined ESPINK_V2
        doc["board"] = "ESPink_V2";
    #elif defined UESPINK_V1
        doc["board"] = "Micro ESPink_V1";
    #elif defined EPDIY_V7
        doc["board"] = "EPDIY_V7";
    #else
        doc["board"] = "unknown";
    #endif
    doc["fwVersion"] = ZIVYOBRAZ_FIRMWARE_VERSION;

    JsonObject system = doc["system"].to<JsonObject>();
    system["vccVoltage"] = readBattery();

    JsonObject network = doc["network"].to<JsonObject>();
    network["ssid"] = WiFi.SSID();
    network["rssi"] = WiFi.RSSI();
    network["mac"] = WiFi.macAddress();
    network["ipAddress"] = WiFi.localIP();

    JsonObject display = doc["display"].to<JsonObject>();
    display["type"] = DISPLAY_T::NAME;
    display["width"] = DISPLAY_T::WIDTH;
    display["height"] = DISPLAY_T::HEIGHT;
    display["colorType"] = colorTypeToCStr(DISPLAY_T::COLORTYPE);

    String out;
    serializeJsonPretty(doc, out);
    return out;
}


void setup()
{
    // power on peripherals
    powerOn();

    // setup interfaces
    Wire.setPins(PIN_I2C_SDA, PIN_I2C_SCL);
    Serial.begin(115200);

    // setup display
    display.init();

    // setup decoders
    zDecoder.init(DISPLAY_T::WIDTH, DISPLAY_T::HEIGHT, rowBuffer, rowCallback);
    bmpDecoder.init(DISPLAY_T::WIDTH, DISPLAY_T::HEIGHT, rowBuffer, z2GrayscaleToRGB565Lut, 4, rowCallback);

    // setup zivyobraz client
    zoClient.setBaseUrl(ZIVYOBRAZ_HOST);
    zoClient.setApiKey("12345678");
    zoClient.registerHandler(ContentType::IMAGE_Z2, handleZ);
    zoClient.registerHandler(ContentType::IMAGE_Z3, handleZ);
    zoClient.registerHandler(ContentType::IMAGE_BMP, handleBMP);

    // setup wifi manager
    apSettings.init();
    wm.setConfigPortalTimeout(300);
    wm.setConnectTimeout(30);
    wm.setHostname("ESPink");
    wm.setAPCallback([](WiFiManager* myWiFiManager) {
        EPDBus::UseSleep(false);
        screenConfigPortal(gfxDisplay);
        EPDBus::UseSleep(true);
    });
    wm.setConfigPortalTimeoutCallback([]() {
        WiFi.mode(WIFI_OFF);
        screenConfigPortalTimeout(gfxDisplay);
        uint64_t sleepTimeSeconds = DEEP_SLEEP_TIME_S * 30 * 24 * 30;
        log_i("Sleep for %lluh %llum %llus", sleepTimeSeconds / 3600, (sleepTimeSeconds % 3600) / 60, sleepTimeSeconds % 60);
        esp_sleep_enable_timer_wakeup(sleepTimeSeconds * 1000000);
        esp_deep_sleep_start();
    });

#ifdef ESPINK_V3
    setupButton();
    handleButtonPress();
#endif

    // read sensors
    log_i("Reading sensors.");
    readSensors();

    log_i("Connecting to wifi.");
    // connect to wifi
    bool wmStatus = wm.autoConnect(apSettings.ssid, apSettings.password);
    if (!wmStatus) {
        log_i("Could not connect");
        return;
    }
    log_i("Connected to WiFi!");
    // log_i("Local IP: %s", WiFi.localIP().toString());
    // log_i("MAC: %s", WiFi.macAddress());


    log_i("Downloading image.");
    int code = zoClient.post("/index.php?timestampCheck=1", buildJsonPayload());
    log_i("Received code: %d", code);
    uint64_t sleepTimeSeconds = DEEP_SLEEP_TIME_S;  // default


    if (code == 200) {
        sleepTimeSeconds = parseSleepTime();
        int bytes = zoClient.readStream();
        log_i("Received %d bytes.", bytes);
        log_i("Redrawing display.");
        display.fullUpdate();
    }

    // shutdown wifi
    log_i("Shutting down WiFi");
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);

    // power off peripherals
    log_i("Peripherals OFF");
    powerOff();

    // deep sleep
    log_i("Sleep for %lluh %llum %llus", sleepTimeSeconds / 3600, (sleepTimeSeconds % 3600) / 60, sleepTimeSeconds % 60);
    esp_sleep_enable_timer_wakeup(sleepTimeSeconds * 1000000);
    esp_deep_sleep_start();
}


void loop()
{
    log_i("test");
    delay(1500);
}

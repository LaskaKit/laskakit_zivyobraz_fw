#include <memory>
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <qrcode.h>
#include <WiFiManager.h>


#include "boards.hpp"

// #define DISPLAY_GDEY075T7
#include "displays.hpp"
#include "espclient.hpp"
#include "sensor.hpp"

// ZIVYOBRAZ CLIENT PARAMS
namespace {
    constexpr const char* ZIVYOBRAZ_HOST = "https://cdn.zivyobraz.eu";
    constexpr const char* ZIVYOBRAZ_FIRMWARE_VERSION = "2.4";
    constexpr const char* ZIVYOBRAZ_FIRMWARE_TYPE = "LaskaKit-0.1";

    constexpr const char* AP_SSID = "ESPINK-Setup";
    constexpr const char* AP_PASS = "zivyobraz";

    constexpr size_t DEEP_SLEEP_TIME_S = 120;
}

// ---------------


using namespace LaskaKit::ZivyObraz;



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


void fillRect(LaskaKit::Epaper::Display& display, int x, int y, int width, int height, uint8_t color)
{
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            display.drawPixel(x + i, y + j, color);
        }
    }
}


void display_qr(LaskaKit::Epaper::Display& display, int pos_x = 0, int pos_y = 0, int scale = 1)
{
    static QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    String qrstr = "WIFI:S:" + String(AP_SSID) + ";T:WPA;P:" + String(AP_PASS) + ";;";
    qrcode_initText(&qrcode, qrcodeData, 3, ECC_LOW, qrstr.c_str());
    
    for (int y = 0; y < qrcode.size; y++) {
        for (int x = 0; x < qrcode.size; x++) {
            if (qrcode_getModule(&qrcode, x, y)) {
                fillRect(display, pos_x + x * scale, pos_y + y * scale, scale, scale, 0b00);
            } else {
                fillRect(display, pos_x + x * scale, pos_y + y * scale, scale, scale, 0b11);
            }
        }
    }
}


void setup()
{
    powerOn();
    // set I2C pins
    Wire.setPins(PIN_I2C_SDA, PIN_I2C_SCL);
    Serial.begin(115200);

    static DISPLAY_T display = DISPLAY_T();

    static WiFiManager wm;
    wm.setConfigPortalTimeout(300);
    wm.setConnectTimeout(30);
    wm.setHostname("ESPINK");

    wm.setAPCallback([](WiFiManager* myWiFiManager) {
        Serial.println("Entered config mode");
        Serial.println(WiFi.softAPIP());
        Serial.println("WiFi Manager");
        Serial.println("Connect to: " + myWiFiManager->getConfigPortalSSID());
        display_qr(display, 150, 80, 10);
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

    StreamingZDEC<COMPRESSION, COLOR_SPACE> decoder(display.width(), display.height());
    COLOR_SPACE rowBuffer[display.width()];

    decoder.setPalette(DISPLAY_PALETTE);
    decoder.setDecodeBuffer(rowBuffer, display.width());
    decoder.setRowCallback(createDrawCallback<COLOR_SPACE, DISPLAY_T>(display));

    static EspClient<StreamingZDEC<COMPRESSION, COLOR_SPACE>> client(ZIVYOBRAZ_HOST, display.width(), display.height(), decoder);
    client.addParam("mac", WiFi.macAddress().c_str());
    client.addParam("x", String(display.width()).c_str());
    client.addParam("y", String(display.height()).c_str());
    client.addParam("c", DISPLAY_COLOR_TYPE);
    client.addParam("fw", ZIVYOBRAZ_FIRMWARE_VERSION);
    client.addParam("fwType", ZIVYOBRAZ_FIRMWARE_TYPE);
    client.addParam("timestamp_check", "1");
    client.addParam("ssid", WiFi.SSID().c_str());
    client.addParam("rssi", std::to_string(WiFi.RSSI()).c_str());
    client.addParam("v", std::to_string(readBattery()).c_str());
    if (sensorReading.flag & SensorReadingFlag::TEMPERATURE) {
        client.addParam("temp", std::to_string(sensorReading.temperature).c_str());
    }
    if (sensorReading.flag & SensorReadingFlag::RELATIVE_HUMIDITY) {
        client.addParam("hum", std::to_string(sensorReading.relativeHumidity).c_str());
    }
    if (sensorReading.flag & SensorReadingFlag::PRESSURE) {
        client.addParam("pres", std::to_string(sensorReading.pressure).c_str());
    }
    if (sensorReading.flag & SensorReadingFlag::CO2) {
        client.addParam("pres", std::to_string(sensorReading.co2).c_str());
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
    int sleepTimeMinutes = 0;
    int sleepTimeSeconds = 0;
    client.getHeader(headerValue, "Sleep");
    sleepTimeMinutes = String(headerValue).toInt();
    client.getHeader(headerValue, "SleepSeonds");
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

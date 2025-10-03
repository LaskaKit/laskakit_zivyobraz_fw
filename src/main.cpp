#include <Arduino.h>

#include <qrcode.h>
#include <laskakit_epaper.hpp>
#include <espclient.hpp>

#include <WiFi.h>
#include <WiFiManager.h>
#include <esp_wifi.h>
#include <memory>


// ZIVYOBRAZ CLIENT PARAMS
namespace {
    constexpr const char* ZIVYOBRAZ_HOST = "https://cdn.zivyobraz.eu";
    constexpr const char* ZIVYOBRAZ_FIRMWARE_VERSION = "2.5";
    constexpr const char* ZIVYOBRAZ_COLOR_TYPE = "4G";

    constexpr const char* AP_SSID = "ESPINK-Setup";
    constexpr const char* AP_PASS = "123456789";

    constexpr size_t DEEP_SLEEP_TIME_S = 120;
}

// ---------------


using namespace LaskaKit::ZivyObraz;

#include <Adafruit_SHT4x.h>
Adafruit_SHT4x sht4 = Adafruit_SHT4x();

struct TempHumReading
{
    float temperature;
    float relativeHumidity;
};

TempHumReading readSHT40()
{
    TempHumReading thr;
    if (sht4.begin()) {
        sht4.setPrecision(SHT4X_LOW_PRECISION);
        sht4.setHeater(SHT4X_NO_HEATER);
        sensors_event_t hum, temp;
        sht4.getEvent(&hum, &temp);
        thr.temperature = temp.temperature;
        thr.relativeHumidity = hum.relative_humidity;
    } else {
        Serial.println("SHT4x NOT FOUND");
    }
    return thr;
}


float readBattery()
{
    pinMode(PIN_VBAT, INPUT);
    uint32_t milliVolts = analogReadMilliVolts(PIN_VBAT);
    float voltage = (milliVolts / 1000.0) * VBAT_DIVIDER_RATIO;
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


uint8_t rgb_to_4_gray_alt(uint8_t r, uint8_t g, uint8_t b) {
    // Convert RGB to grayscale using standard luminance formula
    // This weights the channels based on human eye sensitivity
    float gray = 0.299f * r + 0.587f * g + 0.114f * b;

    // Quantize to 4 levels (2 bits)
    // Map 0-255 range to 0-3 range
    if (gray < 64.1f) {        // 0-63   -> 0 (darkest)
        return 0b00;
    } else if (gray < 128.1f) { // 64-127 -> 1 (dark gray)
        return 0b01;
    } else if (gray < 192.1f) { // 128-191 -> 2 (light gray)
        return 0b10;
    } else {                   // 192-255 -> 3 (lightest)
        return 0b11;
    }
}


void setup()
{
    Serial.begin(115200);
    delay(2000);
    static LaskaKit::Epaper::GDEY075T7 display = LaskaKit::Epaper::GDEY075T7(PIN_CS, PIN_DC, PIN_RST, PIN_BUSY, PIN_PWR);
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

    Serial.println("Connected to WiFi!");
    Serial.println("Local IP: " + WiFi.localIP().toString());
    Serial.println(WiFi.macAddress());


    auto drawCallback = [](Pixel* rowData, uint16_t row){
        for (int col = 0; col < 800; col++) {
            // uint32_t color = rowData[col].red << 16 | rowData[col].green << 8 | rowData[col].blue;
            // Serial.printf("row:%u col:%u R:%u G:%u B:%u -> C:%x\n", row, col, rowData[col].red, rowData[col].green, rowData[col].blue, color);
            uint8_t convertedColor = rgb_to_4_gray_alt(rowData[col].red, rowData[col].green, rowData[col].blue);
            display.drawPixel(col, row, convertedColor);
        }
    };

    TempHumReading thr = readSHT40();

    static EspClient client(ZIVYOBRAZ_HOST, display.width(), display.height(), drawCallback);
    client.addParam("mac", WiFi.macAddress().c_str());
    client.addParam("x", String(display.width()).c_str());
    client.addParam("y", String(display.height()).c_str());
    client.addParam("c", ZIVYOBRAZ_COLOR_TYPE);
    client.addParam("fw", ZIVYOBRAZ_FIRMWARE_VERSION);
    client.addParam("timestamp_check", "1");
    client.addParam("ssid", WiFi.SSID().c_str());
    client.addParam("rssi", std::to_string(WiFi.RSSI()).c_str());
    client.addParam("v", std::to_string(readBattery()).c_str());
    client.addParam("temp", std::to_string(thr.temperature).c_str());
    client.addParam("hum", std::to_string(thr.relativeHumidity).c_str());

    unsigned long start = millis();

    client.get();
    Serial.printf("Download time: %lu ms\n", millis() - start);

    start = millis();
    display.fullUpdate();
    Serial.printf("Update time: %lu ms\n", millis() - start);

    // Process headers
    char headerValue[20];
    client.getHeader(headerValue, "Sleep");
    Serial.print("Deep sleep for: ");
    Serial.print(headerValue);
    Serial.print(" minutes -> ");

    int sleepTimeS = String(headerValue).toInt() * 60;
    Serial.print(String(sleepTimeS).toInt());
    Serial.println(" seconds");
    esp_sleep_enable_timer_wakeup(sleepTimeS * 1000000);
    delay(100);
    esp_deep_sleep_start();
}

void loop()
{
    Serial.println("display test");
    delay(15000);
}

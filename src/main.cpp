#include <Arduino.h>

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
}

// ---------------


using namespace LaskaKit::ZivyObraz;


uint8_t rgb_to_4_gray_alt(uint8_t r, uint8_t g, uint8_t b) {
    // Convert RGB to grayscale using standard luminance formula
    // This weights the channels based on human eye sensitivity
    float gray = 0.299f * r + 0.587f * g + 0.114f * b;

    // Quantize to 4 levels (2 bits)
    // Map 0-255 range to 0-3 range
    if (gray < 64.0f) {        // 0-63   -> 0 (darkest)
        return 0x11;
    } else if (gray < 128.0f) { // 64-127 -> 1 (dark gray)
        return 0x10;
    } else if (gray < 192.0f) { // 128-191 -> 2 (light gray)
        return 0x01;
    } else {                   // 192-255 -> 3 (lightest)
        return 0x00;
    }
}


void setup()
{
    Serial.begin(115200);
    delay(2000);

    std::unique_ptr<LaskaKit::Epaper::Display> display;
    display.reset(new LaskaKit::Epaper::GDEY075T7(PIN_CS, PIN_DC, PIN_RST, PIN_BUSY, PIN_PWR));
    display->fullUpdate();

    WiFiManager wm;
    wm.setConfigPortalTimeout(300);
    wm.setConnectTimeout(30);
    wm.setHostname("ESPINK");

    wm.setAPCallback([](WiFiManager* myWiFiManager) {
        Serial.println("Entered config mode");
        Serial.println(WiFi.softAPIP());
        Serial.println("WiFi Manager");
        Serial.println("Connect to: " + myWiFiManager->getConfigPortalSSID());
    });

  
    if (!wm.autoConnect(AP_SSID, AP_PASS)) {
        Serial.println("Failed to connect, starting configuration portal");
        if (!wm.startConfigPortal(AP_SSID, AP_PASS)) {
            Serial.println("Failed to connect and hit timeout.");
            ESP.restart();
        }
    }

    Serial.println("Connected to WiFi!");
    Serial.println("Local IP: " + WiFi.localIP().toString());
    Serial.println(WiFi.macAddress());

    EspClient client(ZIVYOBRAZ_HOST);
    client.addParam("mac", WiFi.macAddress().c_str());
    client.addParam("x", String(display->width()).c_str());
    client.addParam("y", String(display->height()).c_str());
    client.addParam("c", ZIVYOBRAZ_COLOR_TYPE);
    client.addParam("fw", ZIVYOBRAZ_FIRMWARE_VERSION);

    unsigned long start = millis();
    client.get();

    char header[20];
    client.getHeader(header, "Timestamp");
    Serial.println(header);

    client.process([&display](Pixel* rowData, uint16_t row){
        for (int col = 0; col < 800; col++) {
            // uint32_t color = rowData[col].red << 16 | rowData[col].green << 8 | rowData[col].blue;
            // Serial.printf("row:%u col:%u R:%u G:%u B:%u -> C:%x\n", row, col, rowData[col].red, rowData[col].green, rowData[col].blue, color);
            uint8_t convertedColor = rgb_to_4_gray_alt(rowData[col].red, rowData[col].green, rowData[col].blue);
            display->drawPixel(col, row, convertedColor);
        }
    });
    Serial.printf("Download: %lu\n", millis() - start);

    start = millis();
    display->fullUpdate();
    Serial.printf("Update: %lu\n", millis() - start);
}

void loop()
{
    Serial.println("display test");
    delay(15000);
}

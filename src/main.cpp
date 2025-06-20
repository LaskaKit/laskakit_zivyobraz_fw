#include <Arduino.h>

#include <LaskaKitEpaper.hpp>

#include <ZivyobrazClient.hpp>

#include <Utils.hpp>

#include <WiFi.h>
#include <esp_wifi.h>
#include <memory>

#include "graphics.c"

// s3
// #define CS 10
// #define DC 48
// #define RST 45
// #define BUSY 36
// #define PWR   2

// espink 4.2
// #define CS    5
// #define DC   17
// #define RST  16
// #define BUSY  4
// #define PWR   2


// espink v3.3
#define SDA  11
#define SCL  12
#define CS   10
#define DC   48
#define RST  45
#define BUSY 36
#define PWR  47

// #define ENABLE_GxEPD2_GFX 0


std::unique_ptr<LaskaKit::EpaperDisplay> display;
std::unique_ptr<LaskaKit::EpaperDisplayGFX> display_gfx;
LaskaKit::ZivyobrazClient client;

void setup() {
  Serial.begin(115200);
  delay(2000);

  display.reset(new LaskaKit::GDEQ0426T82(CS, DC, RST, BUSY, PWR));
  // display.reset(new LaskaKit::GDEY042T81(CS, DC, RST, BUSY, PWR));
  // display.reset(new LaskaKit::E2741FS081(SDA, SCL, DC, CS, RST, BUSY, PWR));
  return;

  // display.reset(new LaskaKit::WFT042Z15(CS, DC, RST, BUSY, PWR));
  // display->on();
  // display->setRotation(LaskaKit::DisplayRotation::NO_ROTATION);
  // display->fillScreen(0xf);
  // display->fullUpdate();


  // display_gfx.reset(new LaskaKit::EpaperDisplayGFX(display));
  // display_gfx->drawChar(0, 10, 'L', 0, 0, 10);
  // display_gfx->drawChar(100, 10, 'A', 0, 0, 10);
  // display_gfx->drawChar(200, 10, 'S', 0, 0, 10);
  // display_gfx->drawChar(300, 10, 'K', 0, 0, 10);
  // display_gfx->drawChar(400, 10, 'A', 0, 0, 10);
  // display_gfx->drawChar(500, 10, 'K', 0, 0, 10);
  // display_gfx->drawChar(600, 10, 'I', 0, 0, 10);
  // display_gfx->drawChar(700, 10, 'T', 0, 0, 10);

  // display_gfx->drawBitmap(0, 100, car, 195, 146, 0);
  // display_gfx->fullUpdate();

  delay(2000);
  WiFi.begin("ssid", "pass");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected");
  // esp_wifi_set_ps(WIFI_PS_NONE);

  Serial.println(WiFi.macAddress());

  uint8_t* buf = (uint8_t*)malloc(100000);
  if (buf == nullptr) {
      Serial.println("Malloc failed.");
      return;
  }

  int total_bytes = client.test(buf);

  uint8_t* buf_start = buf + 2;
  
  LaskaKit::PixelPosition pos;
  LaskaKit::draw_z2_image_chunk(*display, buf_start, total_bytes - 2, pos);

  display->fullUpdate();
  free(buf);
}

void loop()
{
  Serial.println("display test");
  delay(5000);
}

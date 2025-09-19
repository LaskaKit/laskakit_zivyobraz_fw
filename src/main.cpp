#include <Arduino.h>

#include <laskakit_epaper.hpp>
#include <espclient.hpp>

#include <WiFi.h>
#include <WiFiManager.h>
#include <esp_wifi.h>
#include <memory>

// espink v3.5
#define SDA 11
#define SCL 12
#define CS 10
#define DC 48
#define RST 45
#define BUSY 38
#define PWR 47

// ZIVYOBRAZ CLIENT PARAMS
#define ZIVYOBRAZ_HOST "https://cdn.zivyobraz.eu"
#define ZIVYOBRAZ_FIRMWARE_VERSION "2.5"
#define ZIVYOBRAZ_COLOR_TYPE "4G"

// ---------------

#include "esp_heap_caps.h"
#include "esp_system.h"
#include "soc/soc_caps.h"

#include "secret.h"

using namespace LaskaKit::ZivyObraz;

void displayPSRAMStats()
{
    Serial.println("\n=== PSRAM Statistics ===");

    // Check if PSRAM is available using heap caps
    size_t psram_size = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);

    if (psram_size > 0)
    {
        Serial.println("PSRAM: Available");

        // Total PSRAM size
        Serial.printf("Total PSRAM Size: %u bytes (%.2f MB)\n",
                      psram_size, psram_size / (1024.0 * 1024.0));

        // Free PSRAM
        size_t free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
        Serial.printf("Free PSRAM: %u bytes (%.2f MB)\n",
                      free_psram, free_psram / (1024.0 * 1024.0));

        // Used PSRAM
        size_t used_psram = psram_size - free_psram;
        Serial.printf("Used PSRAM: %u bytes (%.2f MB)\n",
                      used_psram, used_psram / (1024.0 * 1024.0));

        // Usage percentage
        float usage_percent = (float)used_psram / psram_size * 100.0;
        Serial.printf("PSRAM Usage: %.1f%%\n", usage_percent);

        // Largest free block
        size_t largest_free = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM);
        Serial.printf("Largest Free Block: %u bytes (%.2f KB)\n",
                      largest_free, largest_free / 1024.0);

        // Minimum free PSRAM ever
        size_t min_free = heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM);
        Serial.printf("Minimum Free Ever: %u bytes (%.2f KB)\n",
                      min_free, min_free / 1024.0);

        // Additional heap info
        multi_heap_info_t info;
        heap_caps_get_info(&info, MALLOC_CAP_SPIRAM);
        Serial.printf("Total Blocks: %u\n", info.total_blocks);
        Serial.printf("Total Free Bytes: %u\n", info.total_free_bytes);
        Serial.printf("Total Allocated Bytes: %u\n", info.total_allocated_bytes);
        Serial.printf("Minimum Free Bytes: %u\n", info.minimum_free_bytes);
        Serial.printf("Allocated Blocks: %u\n", info.allocated_blocks);
        Serial.printf("Free Blocks: %u\n", info.free_blocks);
    }
    else
    {
        Serial.println("PSRAM: Not available or not enabled");
        Serial.println("Make sure your board has PSRAM and it's enabled in menuconfig");
    }

    Serial.println("========================\n");
}

void displayMemoryComparison()
{
    Serial.println("\n=== Memory Comparison ===");

    // Internal RAM
    size_t free_internal = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    size_t total_internal = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);

    Serial.printf("Internal RAM - Free: %u bytes (%.1f KB), Total: %u bytes (%.1f KB)\n",
                  free_internal, free_internal / 1024.0,
                  total_internal, total_internal / 1024.0);

    size_t psram_size = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    if (psram_size > 0)
    {
        size_t free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);

        Serial.printf("PSRAM - Free: %u bytes (%.1f MB), Total: %u bytes (%.1f MB)\n",
                      free_psram, free_psram / (1024.0 * 1024.0),
                      psram_size, psram_size / (1024.0 * 1024.0));

        float ratio = (float)psram_size / total_internal;
        Serial.printf("PSRAM is %.1fx larger than internal RAM\n", ratio);
    }

    // DMA capable memory
    size_t dma_free = heap_caps_get_free_size(MALLOC_CAP_DMA);
    size_t dma_total = heap_caps_get_total_size(MALLOC_CAP_DMA);
    Serial.printf("DMA Memory - Free: %u bytes (%.1f KB), Total: %u bytes (%.1f KB)\n",
                  dma_free, dma_free / 1024.0, dma_total, dma_total / 1024.0);

    Serial.println("=========================\n");
}

void testPSRAMAllocation()
{
    Serial.println("\n=== PSRAM Allocation Test ===");

    size_t psram_size = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    if (psram_size == 0)
    {
        Serial.println("PSRAM not available for allocation test");
        return;
    }

    // Test different allocation sizes
    size_t test_sizes[] = {1024, 10 * 1024, 100 * 1024, 1024 * 1024}; // 1KB, 10KB, 100KB, 1MB
    int num_tests = sizeof(test_sizes) / sizeof(test_sizes[0]);

    for (int i = 0; i < num_tests; i++)
    {
        size_t test_size = test_sizes[i];

        Serial.printf("Testing allocation of %u bytes (%.1f KB)... ",
                      test_size, test_size / 1024.0);

        void *psram_ptr = heap_caps_malloc(test_size, MALLOC_CAP_SPIRAM);

        if (psram_ptr)
        {
            // Write test pattern
            memset(psram_ptr, 0x55 + i, test_size);

            // Verify
            uint8_t *byte_ptr = (uint8_t *)psram_ptr;
            bool test_passed = true;
            uint8_t expected = 0x55 + i;

            for (size_t j = 0; j < test_size && test_passed; j++)
            {
                if (byte_ptr[j] != expected)
                {
                    test_passed = false;
                }
            }

            Serial.printf("%s\n", test_passed ? "PASSED" : "FAILED");

            // Free the memory
            heap_caps_free(psram_ptr);
        }
        else
        {
            Serial.println("FAILED (allocation failed)");
        }
    }

    Serial.println("=============================\n");
}

void displayHeapCapabilities()
{
    Serial.println("\n=== Heap Capabilities ===");

    // Different memory types available
    uint32_t caps_list[] = {
        MALLOC_CAP_EXEC,
        MALLOC_CAP_32BIT,
        MALLOC_CAP_8BIT,
        MALLOC_CAP_DMA,
        MALLOC_CAP_PID2,
        MALLOC_CAP_PID3,
        MALLOC_CAP_PID4,
        MALLOC_CAP_PID5,
        MALLOC_CAP_PID6,
        MALLOC_CAP_PID7,
        MALLOC_CAP_SPIRAM,
        MALLOC_CAP_INTERNAL,
        MALLOC_CAP_DEFAULT};

    const char *caps_names[] = {
        "EXEC", "32BIT", "8BIT", "DMA", "PID2", "PID3",
        "PID4", "PID5", "PID6", "PID7", "SPIRAM", "INTERNAL", "DEFAULT"};

    int num_caps = sizeof(caps_list) / sizeof(caps_list[0]);

    for (int i = 0; i < num_caps; i++)
    {
        size_t free_size = heap_caps_get_free_size(caps_list[i]);
        size_t total_size = heap_caps_get_total_size(caps_list[i]);

        if (total_size > 0)
        {
            Serial.printf("%-10s: Free=%6u bytes, Total=%6u bytes\n",
                          caps_names[i], free_size, total_size);
        }
    }
    Serial.println("=========================\n");
}


std::unique_ptr<LaskaKit::Epaper::Display> display;
// std::unique_ptr<LaskaKit::Epaper::DisplayGFX> display_gfx;

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


void mycb(Pixel* rowData, uint16_t row)
{
    for (int col = 0; col < 800; col++) {
        // uint32_t color = rowData[col].red << 16 | rowData[col].green << 8 | rowData[col].blue;
        // Serial.printf("row:%u col:%u R:%u G:%u B:%u -> C:%x\n", row, col, rowData[col].red, rowData[col].green, rowData[col].blue, color);
        uint8_t convertedColor = rgb_to_4_gray_alt(rowData[col].red, rowData[col].green, rowData[col].blue);
        display->drawPixel(col, row, convertedColor);
    }
}


void setup()
{
    Serial.begin(115200);
    delay(2000);

    display.reset(new LaskaKit::Epaper::GDEY075T7(CS, DC, RST, BUSY, PWR));
    display->fullUpdate();

    // display_gfx.reset(new LaskaKit::Epaper::DisplayGFX(display));
    // display_gfx->drawChar(0, 10, 'L', 0, 0, 10);
    // display_gfx->drawChar(100, 10, 'A', 0, 0, 10);
    // display_gfx->drawChar(200, 10, 'S', 0, 0, 10);
    // display_gfx->drawChar(300, 10, 'K', 0, 0, 10);
    // display_gfx->drawChar(400, 10, 'A', 0, 0, 10);
    // display_gfx->drawChar(500, 10, 'K', 0, 0, 10);
    // display_gfx->drawChar(600, 10, 'I', 0, 0, 10);
    // display_gfx->drawChar(700, 10, 'T', 0, 0, 10);


    WiFiManager wm;
    wm.setConfigPortalTimeout(300);
    wm.setConnectTimeout(30);
    wm.setHostname("ESPINK");

    WiFiManagerParameter custom_display("display", "E-Paper display", "default type", 40);
    wm.addParameter(&custom_display);

    wm.setAPCallback([](WiFiManager* myWiFiManager) {
        Serial.println("Entered config mode");
        Serial.println(WiFi.softAPIP());
        Serial.println("WiFi Manager");
        Serial.println("Connect to: " + myWiFiManager->getConfigPortalSSID());
    });

    wm.setSaveConfigCallback([]() {
        Serial.println("Saving config");
        Serial.println("WiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    });

    bool shouldStartPortal = false;

    if (shouldStartPortal) {
        if (!wm.startConfigPortal("ESPINK-Setup", "123456789")) {
            Serial.println("Failed to connect and hit timeout.");
            ESP.restart();
        }
    } else {
        if (!wm.autoConnect("ESPINK-Setup", "123456789")) {
            Serial.println("Failed to connect, starting configuration portal");
            if (!wm.startConfigPortal("ESPINK-Setup", "123456789")) {
                Serial.println("Failed to connect and hit timeout.");
                ESP.restart();
            }
        }
    }

    Serial.println("Connected to WiFi!");
    Serial.println("Local IP: " + WiFi.localIP().toString());

    String custom_display_param = custom_display.getValue();
    Serial.println("Custom parameters:");
    Serial.println("Display: " + custom_display_param);

    Serial.println(WiFi.macAddress());

    EspClient client(ZIVYOBRAZ_HOST);
    client.addParam("mac", espMac);
    client.addParam("x", "800");
    client.addParam("y", "480");
    client.addParam("c", "4G");
    client.addParam("fw", "2.5");

    unsigned long start = millis();
    client.get();

    char header[20];
    client.getHeader(header, "Timestamp");
    Serial.println(header);

    client.process(mycb);
    Serial.printf("Download: %lu\n", millis() - start);

    start = millis();
    display->fullUpdate();
    Serial.printf("Update: %lu\n", millis() - start);
}

void loop()
{
    Serial.println("display test");
    // Serial.println("PSRAM Statistics Monitor (ESP-IDF v6.x)");
    // Serial.println("========================================");

    // // Display chip info
    // esp_chip_info_t chip_info;
    // esp_chip_info(&chip_info);
    // Serial.printf("Chip: %s with %d CPU cores, WiFi%s%s, Silicon revision %d\n",
    //               CONFIG_IDF_TARGET, chip_info.cores,
    //               (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
    //               (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "",
    //               chip_info.revision);

    // // Initial stats
    // displayMemoryComparison();
    // displayHeapCapabilities();
    // testPSRAMAllocation();
    delay(15000);
}

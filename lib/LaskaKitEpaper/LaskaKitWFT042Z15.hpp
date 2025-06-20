#ifndef LASKAKITWFT042Z15_HPP
#define LASKAKITWFT042Z15_HPP

#include <LaskaKitEpaper.hpp>
#include <GxEPD2_3C.h>
#include <GxEPD2_BW.h>

#include <OpenSansSB_50px.h>

namespace LaskaKit {
    class WFT042Z15 : public EpaperDisplay
    {
    private:
        GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display;
        int pin_power;
    public:
        WFT042Z15(int pin_ss, int pin_dc, int pin_rst, int pin_busy, int pin_power)
            : display(GxEPD2_420(pin_ss, pin_dc, pin_rst, pin_busy)),
              pin_power(pin_power)
        {
            this->display.init();
            pinMode(pin_power, OUTPUT);
        }

        uint width()
        {
            return this->display.width();
        }

        uint height()
        {
            return this->display.height();
        }

        void on()
        {
            digitalWrite(this->pin_power, HIGH);
        }

        void off()
        {
            digitalWrite(this->pin_power, LOW);
        }

        void fullUpdate()
        {
            this->display.display(false);
        }

        void setRotation(DisplayRotation rot)
        {
            switch (rot) {
                case DisplayRotation::NO_ROTATION:
                    this->display.setRotation(0);
                    break;
                case DisplayRotation::RIGHT_90:
                case DisplayRotation::LEFT_270:
                    this->display.setRotation(3);
                    break;
                case DisplayRotation::RIGHT_180:
                case DisplayRotation::LEFT_180:
                    this->display.setRotation(2);
                    break;
                case DisplayRotation::RIGHT_270:
                case DisplayRotation::LEFT_90:
                    this->display.setRotation(1);
                    break;
            }
        }

        void fillScreen(int color)
        {
            this->display.fillScreen(color);
        }
        
        void drawPixel(int x, int y, uint16_t color)
        {
            this->display.drawPixel(x, y, color);
        }
        
        void drawText(int x, int y, const String& text, PixelColor px)
        {
            this->display.setTextColor(GxEPD_BLACK);
            this->display.setFont(&OpenSansSB_50px);
            this->display.setCursor(x, y);
            this->display.print(text);
        }
        
        void drawCenteredText(int x, int y, const String& text)
        {
            int16_t x_center, y_center;
            uint16_t width, height;
            this->display.setFont(&OpenSansSB_50px);
            this->display.setTextColor(0);
            this->display.getTextBounds(text.c_str(), 0, 0, &x_center, &y_center, &width, &height);
            this->display.setCursor(x - (width / 2), y + (height / 2));
            this->display.print(text);
        }
        
        void fillRect(int x, int y, int width, int height)
        {
            this->display.fillRect(x, y, width, height, GxEPD_BLACK);
        }
        
        void drawQrCode(int x, int y, int width, const uint8_t* data)
        {}

        ~WFT042Z15()
        {
            this->display.end();
            this->off();
        }
    };
}

#endif  // LASKAKITGDEQ0426T82_HPP

#ifndef LASKAKITEPAPER_HPP
#define LASKAKITEPAPER_HPP

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <memory>


namespace LaskaKit {

    class PixelColor
    {
    protected:
        int color;
    public:
        PixelColor(int color)
            : color(color)
        {}

        int get_color()
        {
            return this->color;
        }
    };


    enum class DisplayRotation
    {
        NO_ROTATION,
        RIGHT_90,
        LEFT_90,
        RIGHT_180,
        LEFT_180,
        RIGHT_270,
        LEFT_270
    };


    class EpaperDisplay
    {
    public:
        virtual uint width() = 0;
        virtual uint height() = 0;
        virtual void on() = 0;
        virtual void off() = 0;
        virtual void fullUpdate() = 0;
        virtual void setRotation(DisplayRotation rot) = 0;
        virtual void fillScreen(int color) = 0;
        virtual void drawPixel(int x, int y, uint16_t color) = 0;
        virtual void drawText(int x, int y, const String& text, PixelColor px) = 0;
        virtual void drawCenteredText(int x, int y, const String& text) = 0;
        virtual void fillRect(int x, int y, int width, int heitht) = 0;
        virtual void drawQrCode(int x, int y, int width, const uint8_t* data) = 0;
        virtual ~EpaperDisplay() {}
    };


    class EpaperDisplayGFX : public Adafruit_GFX
    {
    private:
        std::unique_ptr<EpaperDisplay> display;
    public:
        EpaperDisplayGFX(std::unique_ptr<EpaperDisplay>& display)
            : Adafruit_GFX(display->width(), display->height()),
              display(std::move(display))
        {
        }

        void drawPixel(int16_t x, int16_t y, uint16_t color)
        {
            this->display->drawPixel(x, y, color);
        };

        void fullUpdate()
        {
            this->display->fullUpdate();
        }
    };
}


#include <LaskaKitGDEQ0426T82.hpp>
#include <LaskaKitGDEY042T81.hpp>
#include <LaskaKitWFT042Z15.hpp>
// #include <LaskaKitE2741FS081.hpp>

#endif  // LASKAKITEPAPER_HPP

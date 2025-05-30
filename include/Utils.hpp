#ifndef UTILS_HPP
#define UTILS_HPP

#include <LaskaKitEpaper.hpp>

namespace LaskaKit {

    uint8_t map_color_from_z(uint8_t z_color)
    {
        // color picker (8 shades of gray)  0 3 5 7 9 a c f
        switch (z_color) {
            case 0:  // white
                return 0xf;
            case 1:  // black
                return 0x0;
            case 2:  // light gray
                return 0xc;
            case 3:  // dark gray
                return 0x7;
            case 4:  // light gray 2
                return 0xa;
            case 5:  // light gray 3
                return 0x9;
            case 6:  // dark gray 2
                return 0x5;
            case 7:  // dark gray 3
                return 0x3;
            default:
                return 0xf;  // default white
        }
    }

    enum class ZEncoding {Z1, Z2, Z3, UNKNOWN};


    struct Z1Pixel
    {
        uint8_t px_color;
        uint8_t px_count;
        Z1Pixel(uint16_t data)
        {
            px_color = data >> 8;
            px_count = data;
        }
        Z1Pixel(uint8_t count, uint8_t color)
            : px_count(count), px_color(color)
        {}
        inline uint8_t color() { return px_color; }
        inline uint8_t count() { return px_count; }
    };


    struct Z2Pixel
    {
        uint8_t data;
        Z2Pixel(uint8_t data) : data(data) {}
        inline uint8_t color() { return data >> 6; }
        inline uint8_t count() { return data & (0xFF >> 2); }
    };


    struct Z3Pixel
    {
        uint8_t data;
        Z3Pixel(uint8_t data) : data(data) {}
        inline uint8_t color() { return data >> 5; }
        inline uint8_t count() { return data & (0xFF >> 3); }
    };

    struct PixelPosition
    {
        size_t x;
        size_t y;

        PixelPosition() : x(0), y(0) {}
        PixelPosition(size_t x, size_t y) : x(x), y(y) {}

        bool is_inside(EpaperDisplay& display)
        {
            return this->x < display.width() && this->y < display.height();
        }

        /**
         * Update the position to the next pixel on the display.
         * The position is updated row by row.
         * 
         * @return bool False if position is out of bounds otherwise true.
        */
        bool next(EpaperDisplay& display)
        {
            if (++this->x >= display.width()) {
                this->x = 0;
                if (++this->y >= display.height()) {
                    return false;
                }
            }
            return true;
        }
    };

    size_t draw_z2_image_chunk(EpaperDisplay& display,
                          uint8_t buffer[],
                          size_t buflen,
                          PixelPosition& position)
    {
        if (!position.is_inside(display)) {
            return 0;  // do not draw out of bounds
        }

        size_t drawn_pixels = 0;
        for (size_t i = 0; i < buflen; i++) {
            Z2Pixel px(buffer[i]);
            uint8_t mapped_color = map_color_from_z(px.color());
            for (size_t count = 0; count < px.count(); count++) {
                // Serial.print(position.x); Serial.print(" "); Serial.println(position.y)
                drawn_pixels++;
                display.drawPixel(position.x, position.y, mapped_color);
                if (!position.next(display)) {
                    return drawn_pixels;
                }
            }
        }
        return drawn_pixels;
    }

}


// /**
//  * Map color from z1/2/3 encoded pixel space to display
//  * color space. Custom made mapping.
// */
// uint8_t map_color_from_z(uint8_t z_color);


// /**
//  * Draws content of a buffer encoded in either one
//  * of Z image encodings.
//  * 
//  * @returns number of pixels written
// */
// size_t draw_z_image_chunk(FASTEPD& display,
//                           uint8_t buffer[],
//                           size_t buflen,
//                           PixelPosition& positon,
//                           ZEncoding encoding);


// /**
//  * Draws contents of Z1 encoded buffer onto a display.
//  *
//  * @return int The number of drawn pixels.
// */
// size_t draw_z1_image_chunk(FASTEPD& display,
//                           uint8_t buffer[],
//                           size_t buflen,
//                           PixelPosition& positon);


// /**
//  * Draws contents of Z2 encoded buffer onto a display.
//  * 
//  * @return int The number of drawn pixels.
// */
// size_t draw_z2_image_chunk(FASTEPD& display,
//                           uint8_t buffer[],
//                           size_t buflen,
//                           PixelPosition& positon);


// /**
//  * Draws contents of Z3 encoded buffer onto a display.
//  * 
//  * @return int The number of drawn pixels.
// */
// size_t draw_z3_image_chunk(FASTEPD& display,
//                           uint8_t buffer[],
//                           size_t buflen,
//                           PixelPosition& positon);



#endif  // UTILS_HPP

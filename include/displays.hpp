#pragma once

#include <cstdint>

#include "laskakit_epaper.hpp"
#include "ZDEC.hpp"


template<class T_COLOR, class T_DISPLAY>
uint8_t convertColor(T_COLOR color)
{
    return 0;
}


template<class T_COLOR, class T_DISPLAY>
std::function<void(T_COLOR *decodedRow, uint16_t rowIdx)> createDrawCallback(T_DISPLAY& display)
{
    return [&display](T_COLOR* rowData, uint16_t row){
        for (int col = 0; col < display.width(); col++) {
            uint8_t convertedColor = convertColor<T_COLOR, T_DISPLAY>(rowData[col]);
            display.drawPixel(col, row, convertedColor);
        }
    };
}


// #define DISPLAY_GDEY075T7
// #define DISPLAY_E2741FS081
#ifdef DISPLAY_GDEY075T7
    #include "laskakit_GDEY075T7.hpp"
    #define DISPLAY_T LaskaKit::Epaper::GDEY075T7
    #define COLOR_SPACE LaskaKit::ZivyObraz::ZColor
    #define COMPRESSION LaskaKit::ZivyObraz::ZCompressionType::Z2
    #define DISPLAY_PALETTE makeStandardPalette(ZColorType::G4)
    #define DISPLAY_COLOR_TYPE "4G"

    template<>
    uint8_t convertColor<COLOR_SPACE, DISPLAY_T>(COLOR_SPACE color)
    {
        switch (color) {
            case COLOR_SPACE::White:
                return 0b11;
            case COLOR_SPACE::Black:
                return 0b00;
            case COLOR_SPACE::LightGray:
                return 0b01;
            case COLOR_SPACE::DarkGray:
                return 0b10;
            default:
                return 0b00;
        }
    }

#elif defined DISPLAY_GDEY1248F51
    #include "laskakit_GDEY1248F51.hpp"
    #define DISPLAY_T LaskaKit::Epaper::GDEY1248F51
    #define COLOR_SPACE ZColor
    #define COMPRESSION ZCompressionType::Z2
    #define DISPLAY_PALETTE makeStandardPalette(ZColorType::C4)
    #define DISPLAY_COLOR_TYPE "4C"

#elif defined DISPLAY_E2741FS081
    #include "laskakit_E2741FS081.hpp"
    #define DISPLAY_T LaskaKit::Epaper::E2741FS081
    #define COLOR_SPACE LaskaKit::ZivyObraz::ZColor
    #define COMPRESSION ZCompressionType::Z2
    #define DISPLAY_PALETTE makeStandardPalette(ZColorType::RBW)
    #define DISPLAY_COLOR_TYPE "RBW"

    template<>
    uint8_t convertColor<COLOR_SPACE, DISPLAY_T>(COLOR_SPACE color)
    {
        switch (color) {
            case COLOR_SPACE::White:
                return 0b00;
            case COLOR_SPACE::Black:
                return 0b10;
            case COLOR_SPACE::Red:
                return 0b01;
            default:
                return 0b10;
        }
    }

#else
    #include "laskakit_none.hpp"    
    #define DISPLAY_T LaskaKit::Epaper::None
    #define COLOR_SPACE uint8_t
    #define COMPRESSION ZCompressionType::Z2
    #define DISPLAY_PALETTE LaskaKit::ZivyObraz::ZPalette<uint8_t>({0, 1})
    #define DISPLAY_COLOR_TYPE "BW"
    
#endif



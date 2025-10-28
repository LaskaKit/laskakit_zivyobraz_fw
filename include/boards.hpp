#pragma once

#ifdef ESPINK_V2
    static constexpr int PIN_EPD_SDA = 23; 
    static constexpr int PIN_EPD_SCL = 18; 
    static constexpr int PIN_EPD_CS = 5; 
    static constexpr int PIN_EPD_DC = 17; 
    static constexpr int PIN_EPD_RST = 16; 
    static constexpr int PIN_EPD_BUSY = 4;
    static constexpr int PIN_I2C_SDA = 21; 
    static constexpr int PIN_I2C_SCL = 22; 
    static constexpr int PIN_PWR = 2;
    static constexpr int PIN_VBAT = 34;
    static constexpr double VBAT_DIVIDER_RATIO = 1.769;
#elif defined ESPINK_V3
    static constexpr int PIN_EPD_SDA = 11; 
    static constexpr int PIN_EPD_SCL = 12; 
    static constexpr int PIN_EPD_CS = 10; 
    static constexpr int PIN_EPD_DC = 48; 
    static constexpr int PIN_EPD_RST = 45; 
    static constexpr int PIN_EPD_BUSY = 38; 
    static constexpr int PIN_I2C_SDA = 42; 
    static constexpr int PIN_I2C_SCL = 2; 
    static constexpr int PIN_PWR = 47;
    static constexpr int PIN_VBAT = 9;
    static constexpr double VBAT_DIVIDER_RATIO = 1.769;
#else
    #error "Unknown BOARD type."
#endif

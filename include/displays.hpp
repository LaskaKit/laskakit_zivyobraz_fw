#pragma once

#ifdef DISPLAY_GDEY075T7
    #include "laskakit_GDEY075T7.hpp"
    #define DISPLAY_T LaskaKit::Epaper::GDEY075T7

#elif defined DISPLAY_GDEY075Z08
    #include "laskakit_GDEY075Z08.hpp"
    #define DISPLAY_T LaskaKit::Epaper::GDEY075Z08

#elif defined DISPLAY_GDEY1248F51
    #include "laskakit_GDEY1248F51.hpp"
    #define DISPLAY_T LaskaKit::Epaper::GDEY1248F51

#elif defined DISPLAY_E2741FS081
    #include "laskakit_E2741FS081.hpp"
    #define DISPLAY_T LaskaKit::Epaper::E2741FS081

#elif defined DISPLAY_E2741CS0B2
    #include "laskakit_E2741CS0B2.hpp"
    #define DISPLAY_T LaskaKit::Epaper::E2741FS081

#elif defined DISPLAY_GDEM102F91
    #include "laskakit_GDEM102F91.hpp"
    #define DISPLAY_T LaskaKit::Epaper::GDEM102F91

#elif defined DISPLAY_GDEM075F52
    #include "laskakit_GDEM075F52.hpp"
    #define DISPLAY_T LaskaKit::Epaper::GDEM075F52

#elif defined DISPLAY_GDEM0154F51H
    #include "laskakit_GDEM0154F51H.hpp"
    #define DISPLAY_T LaskaKit::Epaper::GDEM0154F51H

#else
    #include "laskakit_none.hpp"
    #define DISPLAY_T LaskaKit::Epaper::None

#endif

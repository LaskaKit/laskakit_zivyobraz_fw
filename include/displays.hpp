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

#elif defined DISPLAY_GDEM042F52
    #include "laskakit_GDEM042F52.hpp"
    #define DISPLAY_T LaskaKit::Epaper::GDEM042F52

#elif defined DISPLAY_GDEQ0426T82
    #include "laskakit_GDEQ0426T82.hpp"
    #define DISPLAY_T LaskaKit::Epaper::GDEQ0426T82

#elif defined DISPLAY_GDEY0583F41
    #include "laskakit_GDEY0583F41.hpp"
    #define DISPLAY_T LaskaKit::Epaper::GDEY0583F41

#elif defined DISPLAY_GDEY0213B74
    #include "laskakit_GDEY0213B74.hpp"
    #define DISPLAY_T LaskaKit::Epaper::GDEY0213B74

#elif defined DISPLAY_GDEY029T71H
    #include "laskakit_GDEY029T71H.hpp"
    #define DISPLAY_T LaskaKit::Epaper::GDEY029T71H

#elif defined DISPLAY_GDEY116F51
    #include "laskakit_GDEY116F51.hpp"
    #define DISPLAY_T LaskaKit::Epaper::GDEY116F51

#elif defined DISPLAY_GDEY029F51H
    #include "laskakit_GDEY029F51H.hpp"
    #define DISPLAY_T LaskaKit::Epaper::GDEY029F51H

#elif defined DISPLAY_GDEY042T81
    #include "laskakit_GDEY042T81.hpp"
    #define DISPLAY_T LaskaKit::Epaper::GDEY042T81

#elif defined DISPLAY_GDEM102T91
    #include "laskakit_GDEM102T91.hpp"
    #define DISPLAY_T LaskaKit::Epaper::GDEM102T91

#else
    #include "laskakit_none.hpp"
    #define DISPLAY_T LaskaKit::Epaper::None
#endif

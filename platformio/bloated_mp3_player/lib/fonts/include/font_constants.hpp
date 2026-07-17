#pragma once

#include <stdint.h>

namespace BakedFonts {

static constexpr uint16_t STYLE_VARIANT_ABSENT = 0x0000;

enum StyleIndex : uint8_t {
    STYLE_NORMAL = 0,
    STYLE_ITALIC = 1,
    STYLE_BOLD = 2,
    STYLE_UNDERLINE = 3,
    STYLE_STRIKETHROUGH = 4,
    STYLE_COUNT = 5
};

namespace Size {
    static constexpr uint8_t PT_8 = 8;
    static constexpr uint8_t PT_10 = 10;
    static constexpr uint8_t PT_12 = 12;
    static constexpr uint8_t PT_14 = 14;
    static constexpr uint8_t PT_16 = 16;
    static constexpr uint8_t PT_18 = 18;
    static constexpr uint8_t PT_20 = 20;
}

namespace Family {
    static constexpr char BITCOUNTSINGLE[] = "bitcountsingle";
    static constexpr char BITCOUNTSINGLE_CURSIVE[] = "bitcountsingle_cursive";
    static constexpr char BITCOUNTSINGLE_ROMAN[] = "bitcountsingle_roman";
    static constexpr char DATATYPE[] = "datatype";
    static constexpr char DATATYPE_CONDENSED[] = "datatype_condensed";
    static constexpr char DATATYPE_EXPANDED[] = "datatype_expanded";
    static constexpr char DATATYPE_EXTRACONDENSED[] = "datatype_extracondensed";
    static constexpr char DATATYPE_EXTRAEXPANDED[] = "datatype_extraexpanded";
    static constexpr char DATATYPE_SEMICONDENSED[] = "datatype_semicondensed";
    static constexpr char DATATYPE_SEMIEXPANDED[] = "datatype_semiexpanded";
    static constexpr char DATATYPE_ULTRACONDENSED[] = "datatype_ultracondensed";
    static constexpr char DOTO[] = "doto";
    static constexpr char DOTO_ROUNDED[] = "doto_rounded";
    static constexpr char JACQUARD12[] = "jacquard12";
    static constexpr char MONOFETT[] = "monofett";
    static constexpr char NOTOEMOJI[] = "notoemoji";
    static constexpr char TINY5[] = "tiny5";
    static constexpr char WORKBENCH[] = "workbench";
}

}

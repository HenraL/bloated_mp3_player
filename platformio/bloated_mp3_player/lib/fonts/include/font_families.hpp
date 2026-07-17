#pragma once

#include <stdint.h>

namespace BakedFonts { namespace Family {

struct Entry {
    const char* name;
    uint8_t size_count;
};

static constexpr Entry AVAILABLE[] = {
    { BITCOUNTSINGLE, 7 },
    { BITCOUNTSINGLE_CURSIVE, 7 },
    { BITCOUNTSINGLE_ROMAN, 7 },
    { DATATYPE, 7 },
    { DATATYPE_CONDENSED, 7 },
    { DATATYPE_EXPANDED, 7 },
    { DATATYPE_EXTRACONDENSED, 7 },
    { DATATYPE_EXTRAEXPANDED, 7 },
    { DATATYPE_SEMICONDENSED, 7 },
    { DATATYPE_SEMIEXPANDED, 7 },
    { DATATYPE_ULTRACONDENSED, 7 },
    { DOTO, 7 },
    { DOTO_ROUNDED, 7 },
    { JACQUARD12, 7 },
    { MONOFETT, 7 },
    { NOTOEMOJI, 7 },
    { TINY5, 7 },
    { WORKBENCH, 7 },
};

static constexpr uint8_t COUNT = 18;

} }

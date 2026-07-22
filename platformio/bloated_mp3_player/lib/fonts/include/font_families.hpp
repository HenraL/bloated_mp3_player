#pragma once

#include <stdint.h>

namespace BakedFonts {
namespace Family {

struct Entry {
    const char* name;
    uint8_t size_count;
};

static constexpr Entry AVAILABLE[] = {
    { BITCOUNTSINGLE, 10 },
    { BITCOUNTSINGLE_CURSIVE, 10 },
    { BITCOUNTSINGLE_ROMAN, 10 },
    { DATATYPE, 10 },
    { DATATYPE_CONDENSED, 10 },
    { DATATYPE_EXPANDED, 10 },
    { DATATYPE_EXTRACONDENSED, 10 },
    { DATATYPE_EXTRAEXPANDED, 10 },
    { DATATYPE_SEMICONDENSED, 10 },
    { DATATYPE_SEMIEXPANDED, 10 },
    { DATATYPE_ULTRACONDENSED, 10 },
    { DOTO, 10 },
    { DOTO_ROUNDED, 10 },
    { JACQUARD12, 10 },
    { MONOFETT, 10 },
    { NOTOEMOJI, 10 },
    { TINY5, 10 },
    { WORKBENCH, 10 },
};

static constexpr uint8_t COUNT = 18;

}
}

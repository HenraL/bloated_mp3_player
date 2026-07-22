#ifndef BAKEDFONTS_DATATYPE_2PT_H_
#define BAKEDFONTS_DATATYPE_2PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DATATYPE_2PT_WIDTH  = 2;
static const uint8_t DATATYPE_2PT_HEIGHT = 6;
static const uint8_t DATATYPE_2PT_ASCENT = 2;
static const uint32_t DATATYPE_2PT_FIRST  = 32;
static const uint32_t DATATYPE_2PT_LAST   = 8722;
static const uint16_t DATATYPE_2PT_COUNT  = 319;

extern const uint32_t datatype_2pt_codes[];

extern const uint8_t datatype_2pt_bits[] PROGMEM;
extern const uint8_t datatype_2pt_widths[];
extern const FontHandle datatype_2pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_2PT_H_ */

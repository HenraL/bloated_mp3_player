#ifndef BAKEDFONTS_DATATYPE_6PT_H_
#define BAKEDFONTS_DATATYPE_6PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DATATYPE_6PT_WIDTH  = 5;
static const uint8_t DATATYPE_6PT_HEIGHT = 14;
static const uint32_t DATATYPE_6PT_FIRST  = 32;
static const uint32_t DATATYPE_6PT_LAST   = 8722;
static const uint16_t DATATYPE_6PT_COUNT  = 319;

extern const uint32_t datatype_6pt_codes[];

extern const uint8_t datatype_6pt_bits[] PROGMEM;
extern const uint8_t datatype_6pt_widths[];
extern const FontHandle datatype_6pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_6PT_H_ */

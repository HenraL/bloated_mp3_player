#ifndef BAKEDFONTS_DATATYPE_ULTRACONDENSED_18PT_H_
#define BAKEDFONTS_DATATYPE_ULTRACONDENSED_18PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DATATYPE_ULTRACONDENSED_18PT_WIDTH  = 12;
static const uint8_t DATATYPE_ULTRACONDENSED_18PT_HEIGHT = 38;
static const uint32_t DATATYPE_ULTRACONDENSED_18PT_FIRST  = 32;
static const uint32_t DATATYPE_ULTRACONDENSED_18PT_LAST   = 8722;
static const uint16_t DATATYPE_ULTRACONDENSED_18PT_COUNT  = 319;

extern const uint32_t datatype_ultracondensed_18pt_codes[];

extern const uint8_t datatype_ultracondensed_18pt_black_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_18pt_black_widths[];
extern const uint8_t datatype_ultracondensed_18pt_bold_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_18pt_bold_widths[];
extern const uint8_t datatype_ultracondensed_18pt_extrabold_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_18pt_extrabold_widths[];
extern const uint8_t datatype_ultracondensed_18pt_extralight_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_18pt_extralight_widths[];
extern const uint8_t datatype_ultracondensed_18pt_light_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_18pt_light_widths[];
extern const uint8_t datatype_ultracondensed_18pt_medium_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_18pt_medium_widths[];
extern const uint8_t datatype_ultracondensed_18pt_regular_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_18pt_regular_widths[];
extern const uint8_t datatype_ultracondensed_18pt_semibold_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_18pt_semibold_widths[];
extern const uint8_t datatype_ultracondensed_18pt_thin_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_18pt_thin_widths[];
extern const FontStyleMap datatype_ultracondensed_18pt_style_map[];
extern const FontHandle datatype_ultracondensed_18pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_ULTRACONDENSED_18PT_H_ */

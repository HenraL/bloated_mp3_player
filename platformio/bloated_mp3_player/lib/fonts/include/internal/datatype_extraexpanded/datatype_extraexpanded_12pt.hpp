#ifndef BAKEDFONTS_DATATYPE_EXTRAEXPANDED_12PT_H_
#define BAKEDFONTS_DATATYPE_EXTRAEXPANDED_12PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DATATYPE_EXTRAEXPANDED_12PT_WIDTH  = 8;
static const uint8_t DATATYPE_EXTRAEXPANDED_12PT_HEIGHT = 25;
static const uint32_t DATATYPE_EXTRAEXPANDED_12PT_FIRST  = 32;
static const uint32_t DATATYPE_EXTRAEXPANDED_12PT_LAST   = 8722;
static const uint16_t DATATYPE_EXTRAEXPANDED_12PT_COUNT  = 319;

extern const uint32_t datatype_extraexpanded_12pt_codes[];

extern const uint8_t datatype_extraexpanded_12pt_black_bits[] PROGMEM;
extern const uint8_t datatype_extraexpanded_12pt_black_widths[];
extern const uint8_t datatype_extraexpanded_12pt_bold_bits[] PROGMEM;
extern const uint8_t datatype_extraexpanded_12pt_bold_widths[];
extern const uint8_t datatype_extraexpanded_12pt_extrabold_bits[] PROGMEM;
extern const uint8_t datatype_extraexpanded_12pt_extrabold_widths[];
extern const uint8_t datatype_extraexpanded_12pt_extralight_bits[] PROGMEM;
extern const uint8_t datatype_extraexpanded_12pt_extralight_widths[];
extern const uint8_t datatype_extraexpanded_12pt_light_bits[] PROGMEM;
extern const uint8_t datatype_extraexpanded_12pt_light_widths[];
extern const uint8_t datatype_extraexpanded_12pt_medium_bits[] PROGMEM;
extern const uint8_t datatype_extraexpanded_12pt_medium_widths[];
extern const uint8_t datatype_extraexpanded_12pt_regular_bits[] PROGMEM;
extern const uint8_t datatype_extraexpanded_12pt_regular_widths[];
extern const uint8_t datatype_extraexpanded_12pt_semibold_bits[] PROGMEM;
extern const uint8_t datatype_extraexpanded_12pt_semibold_widths[];
extern const uint8_t datatype_extraexpanded_12pt_thin_bits[] PROGMEM;
extern const uint8_t datatype_extraexpanded_12pt_thin_widths[];
extern const FontStyleMap datatype_extraexpanded_12pt_style_map[];
extern const FontHandle datatype_extraexpanded_12pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_EXTRAEXPANDED_12PT_H_ */

#ifndef BAKEDFONTS_DATATYPE_EXPANDED_6PT_H_
#define BAKEDFONTS_DATATYPE_EXPANDED_6PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DATATYPE_EXPANDED_6PT_WIDTH  = 5;
static const uint8_t DATATYPE_EXPANDED_6PT_HEIGHT = 14;
static const uint8_t DATATYPE_EXPANDED_6PT_ASCENT = 6;
static const uint32_t DATATYPE_EXPANDED_6PT_FIRST  = 32;
static const uint32_t DATATYPE_EXPANDED_6PT_LAST   = 8722;
static const uint16_t DATATYPE_EXPANDED_6PT_COUNT  = 319;

extern const uint32_t datatype_expanded_6pt_codes[];

extern const uint8_t datatype_expanded_6pt_black_bits[] PROGMEM;
extern const uint8_t datatype_expanded_6pt_black_widths[];
extern const uint8_t datatype_expanded_6pt_bold_bits[] PROGMEM;
extern const uint8_t datatype_expanded_6pt_bold_widths[];
extern const uint8_t datatype_expanded_6pt_extrabold_bits[] PROGMEM;
extern const uint8_t datatype_expanded_6pt_extrabold_widths[];
extern const uint8_t datatype_expanded_6pt_extralight_bits[] PROGMEM;
extern const uint8_t datatype_expanded_6pt_extralight_widths[];
extern const uint8_t datatype_expanded_6pt_light_bits[] PROGMEM;
extern const uint8_t datatype_expanded_6pt_light_widths[];
extern const uint8_t datatype_expanded_6pt_medium_bits[] PROGMEM;
extern const uint8_t datatype_expanded_6pt_medium_widths[];
extern const uint8_t datatype_expanded_6pt_regular_bits[] PROGMEM;
extern const uint8_t datatype_expanded_6pt_regular_widths[];
extern const uint8_t datatype_expanded_6pt_semibold_bits[] PROGMEM;
extern const uint8_t datatype_expanded_6pt_semibold_widths[];
extern const uint8_t datatype_expanded_6pt_thin_bits[] PROGMEM;
extern const uint8_t datatype_expanded_6pt_thin_widths[];
extern const FontStyleMap datatype_expanded_6pt_style_map[];
extern const FontHandle datatype_expanded_6pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_EXPANDED_6PT_H_ */

#ifndef BAKEDFONTS_DATATYPE_EXPANDED_10PT_H_
#define BAKEDFONTS_DATATYPE_EXPANDED_10PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DATATYPE_EXPANDED_10PT_WIDTH  = 7;
static const uint8_t DATATYPE_EXPANDED_10PT_HEIGHT = 21;
static const uint32_t DATATYPE_EXPANDED_10PT_FIRST  = 32;
static const uint32_t DATATYPE_EXPANDED_10PT_LAST   = 8722;
static const uint16_t DATATYPE_EXPANDED_10PT_COUNT  = 319;

extern const uint32_t datatype_expanded_10pt_codes[];

extern const uint8_t datatype_expanded_10pt_black_bits[] PROGMEM;
extern const uint8_t datatype_expanded_10pt_black_widths[];
extern const uint8_t datatype_expanded_10pt_bold_bits[] PROGMEM;
extern const uint8_t datatype_expanded_10pt_bold_widths[];
extern const uint8_t datatype_expanded_10pt_extrabold_bits[] PROGMEM;
extern const uint8_t datatype_expanded_10pt_extrabold_widths[];
extern const uint8_t datatype_expanded_10pt_extralight_bits[] PROGMEM;
extern const uint8_t datatype_expanded_10pt_extralight_widths[];
extern const uint8_t datatype_expanded_10pt_light_bits[] PROGMEM;
extern const uint8_t datatype_expanded_10pt_light_widths[];
extern const uint8_t datatype_expanded_10pt_medium_bits[] PROGMEM;
extern const uint8_t datatype_expanded_10pt_medium_widths[];
extern const uint8_t datatype_expanded_10pt_regular_bits[] PROGMEM;
extern const uint8_t datatype_expanded_10pt_regular_widths[];
extern const uint8_t datatype_expanded_10pt_semibold_bits[] PROGMEM;
extern const uint8_t datatype_expanded_10pt_semibold_widths[];
extern const uint8_t datatype_expanded_10pt_thin_bits[] PROGMEM;
extern const uint8_t datatype_expanded_10pt_thin_widths[];
extern const FontStyleMap datatype_expanded_10pt_style_map[];
extern const FontHandle datatype_expanded_10pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_EXPANDED_10PT_H_ */

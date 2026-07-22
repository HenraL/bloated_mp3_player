#ifndef BAKEDFONTS_DATATYPE_CONDENSED_14PT_H_
#define BAKEDFONTS_DATATYPE_CONDENSED_14PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DATATYPE_CONDENSED_14PT_WIDTH  = 9;
static const uint8_t DATATYPE_CONDENSED_14PT_HEIGHT = 32;
static const uint8_t DATATYPE_CONDENSED_14PT_ASCENT = 14;
static const uint32_t DATATYPE_CONDENSED_14PT_FIRST  = 32;
static const uint32_t DATATYPE_CONDENSED_14PT_LAST   = 8722;
static const uint16_t DATATYPE_CONDENSED_14PT_COUNT  = 319;

extern const uint32_t datatype_condensed_14pt_codes[];

extern const uint8_t datatype_condensed_14pt_black_bits[] PROGMEM;
extern const uint8_t datatype_condensed_14pt_black_widths[];
extern const uint8_t datatype_condensed_14pt_bold_bits[] PROGMEM;
extern const uint8_t datatype_condensed_14pt_bold_widths[];
extern const uint8_t datatype_condensed_14pt_extrabold_bits[] PROGMEM;
extern const uint8_t datatype_condensed_14pt_extrabold_widths[];
extern const uint8_t datatype_condensed_14pt_extralight_bits[] PROGMEM;
extern const uint8_t datatype_condensed_14pt_extralight_widths[];
extern const uint8_t datatype_condensed_14pt_light_bits[] PROGMEM;
extern const uint8_t datatype_condensed_14pt_light_widths[];
extern const uint8_t datatype_condensed_14pt_medium_bits[] PROGMEM;
extern const uint8_t datatype_condensed_14pt_medium_widths[];
extern const uint8_t datatype_condensed_14pt_regular_bits[] PROGMEM;
extern const uint8_t datatype_condensed_14pt_regular_widths[];
extern const uint8_t datatype_condensed_14pt_semibold_bits[] PROGMEM;
extern const uint8_t datatype_condensed_14pt_semibold_widths[];
extern const uint8_t datatype_condensed_14pt_thin_bits[] PROGMEM;
extern const uint8_t datatype_condensed_14pt_thin_widths[];
extern const FontStyleMap datatype_condensed_14pt_style_map[];
extern const FontHandle datatype_condensed_14pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_CONDENSED_14PT_H_ */

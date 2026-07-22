#ifndef BAKEDFONTS_BITCOUNTSINGLE_ROMAN_6PT_H_
#define BAKEDFONTS_BITCOUNTSINGLE_ROMAN_6PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t BITCOUNTSINGLE_ROMAN_6PT_WIDTH  = 7;
static const uint8_t BITCOUNTSINGLE_ROMAN_6PT_HEIGHT = 7;
static const uint32_t BITCOUNTSINGLE_ROMAN_6PT_FIRST  = 32;
static const uint32_t BITCOUNTSINGLE_ROMAN_6PT_LAST   = 64258;
static const uint16_t BITCOUNTSINGLE_ROMAN_6PT_COUNT  = 396;

extern const uint32_t bitcountsingle_roman_6pt_codes[];

extern const uint8_t bitcountsingle_roman_6pt_black_bits[] PROGMEM;
extern const uint8_t bitcountsingle_roman_6pt_black_widths[];
extern const uint8_t bitcountsingle_roman_6pt_bold_bits[] PROGMEM;
extern const uint8_t bitcountsingle_roman_6pt_bold_widths[];
extern const uint8_t bitcountsingle_roman_6pt_extrabold_bits[] PROGMEM;
extern const uint8_t bitcountsingle_roman_6pt_extrabold_widths[];
extern const uint8_t bitcountsingle_roman_6pt_extralight_bits[] PROGMEM;
extern const uint8_t bitcountsingle_roman_6pt_extralight_widths[];
extern const uint8_t bitcountsingle_roman_6pt_light_bits[] PROGMEM;
extern const uint8_t bitcountsingle_roman_6pt_light_widths[];
extern const uint8_t bitcountsingle_roman_6pt_medium_bits[] PROGMEM;
extern const uint8_t bitcountsingle_roman_6pt_medium_widths[];
extern const uint8_t bitcountsingle_roman_6pt_regular_bits[] PROGMEM;
extern const uint8_t bitcountsingle_roman_6pt_regular_widths[];
extern const uint8_t bitcountsingle_roman_6pt_semibold_bits[] PROGMEM;
extern const uint8_t bitcountsingle_roman_6pt_semibold_widths[];
extern const uint8_t bitcountsingle_roman_6pt_thin_bits[] PROGMEM;
extern const uint8_t bitcountsingle_roman_6pt_thin_widths[];
extern const FontStyleMap bitcountsingle_roman_6pt_style_map[];
extern const FontHandle bitcountsingle_roman_6pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_BITCOUNTSINGLE_ROMAN_6PT_H_ */

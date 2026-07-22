#ifndef BAKEDFONTS_DOTO_ROUNDED_20PT_H_
#define BAKEDFONTS_DOTO_ROUNDED_20PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DOTO_ROUNDED_20PT_WIDTH  = 13;
static const uint8_t DOTO_ROUNDED_20PT_HEIGHT = 38;
static const uint32_t DOTO_ROUNDED_20PT_FIRST  = 32;
static const uint32_t DOTO_ROUNDED_20PT_LAST   = 8722;
static const uint16_t DOTO_ROUNDED_20PT_COUNT  = 319;

extern const uint32_t doto_rounded_20pt_codes[];

extern const uint8_t doto_rounded_20pt_black_bits[] PROGMEM;
extern const uint8_t doto_rounded_20pt_black_widths[];
extern const uint8_t doto_rounded_20pt_bold_bits[] PROGMEM;
extern const uint8_t doto_rounded_20pt_bold_widths[];
extern const uint8_t doto_rounded_20pt_extrabold_bits[] PROGMEM;
extern const uint8_t doto_rounded_20pt_extrabold_widths[];
extern const uint8_t doto_rounded_20pt_extralight_bits[] PROGMEM;
extern const uint8_t doto_rounded_20pt_extralight_widths[];
extern const uint8_t doto_rounded_20pt_light_bits[] PROGMEM;
extern const uint8_t doto_rounded_20pt_light_widths[];
extern const uint8_t doto_rounded_20pt_medium_bits[] PROGMEM;
extern const uint8_t doto_rounded_20pt_medium_widths[];
extern const uint8_t doto_rounded_20pt_regular_bits[] PROGMEM;
extern const uint8_t doto_rounded_20pt_regular_widths[];
extern const uint8_t doto_rounded_20pt_semibold_bits[] PROGMEM;
extern const uint8_t doto_rounded_20pt_semibold_widths[];
extern const uint8_t doto_rounded_20pt_thin_bits[] PROGMEM;
extern const uint8_t doto_rounded_20pt_thin_widths[];
extern const FontStyleMap doto_rounded_20pt_style_map[];
extern const FontHandle doto_rounded_20pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_ROUNDED_20PT_H_ */

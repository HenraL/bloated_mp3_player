#ifndef BAKEDFONTS_DOTO_ROUNDED_4PT_H_
#define BAKEDFONTS_DOTO_ROUNDED_4PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DOTO_ROUNDED_4PT_WIDTH  = 3;
static const uint8_t DOTO_ROUNDED_4PT_HEIGHT = 11;
static const uint8_t DOTO_ROUNDED_4PT_ASCENT = 4;
static const uint32_t DOTO_ROUNDED_4PT_FIRST  = 32;
static const uint32_t DOTO_ROUNDED_4PT_LAST   = 8722;
static const uint16_t DOTO_ROUNDED_4PT_COUNT  = 319;

extern const uint32_t doto_rounded_4pt_codes[];

extern const uint8_t doto_rounded_4pt_black_bits[] PROGMEM;
extern const uint8_t doto_rounded_4pt_black_widths[];
extern const uint8_t doto_rounded_4pt_bold_bits[] PROGMEM;
extern const uint8_t doto_rounded_4pt_bold_widths[];
extern const uint8_t doto_rounded_4pt_extrabold_bits[] PROGMEM;
extern const uint8_t doto_rounded_4pt_extrabold_widths[];
extern const uint8_t doto_rounded_4pt_extralight_bits[] PROGMEM;
extern const uint8_t doto_rounded_4pt_extralight_widths[];
extern const uint8_t doto_rounded_4pt_light_bits[] PROGMEM;
extern const uint8_t doto_rounded_4pt_light_widths[];
extern const uint8_t doto_rounded_4pt_medium_bits[] PROGMEM;
extern const uint8_t doto_rounded_4pt_medium_widths[];
extern const uint8_t doto_rounded_4pt_regular_bits[] PROGMEM;
extern const uint8_t doto_rounded_4pt_regular_widths[];
extern const uint8_t doto_rounded_4pt_semibold_bits[] PROGMEM;
extern const uint8_t doto_rounded_4pt_semibold_widths[];
extern const uint8_t doto_rounded_4pt_thin_bits[] PROGMEM;
extern const uint8_t doto_rounded_4pt_thin_widths[];
extern const FontStyleMap doto_rounded_4pt_style_map[];
extern const FontHandle doto_rounded_4pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_ROUNDED_4PT_H_ */

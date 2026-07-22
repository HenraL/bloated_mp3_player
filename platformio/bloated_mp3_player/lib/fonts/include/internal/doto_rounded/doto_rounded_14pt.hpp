#ifndef BAKEDFONTS_DOTO_ROUNDED_14PT_H_
#define BAKEDFONTS_DOTO_ROUNDED_14PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t DOTO_ROUNDED_14PT_WIDTH  = 9;
static const uint8_t DOTO_ROUNDED_14PT_HEIGHT = 27;
static const uint32_t DOTO_ROUNDED_14PT_FIRST  = 32;
static const uint32_t DOTO_ROUNDED_14PT_LAST   = 8722;
static const uint16_t DOTO_ROUNDED_14PT_COUNT  = 319;

extern const uint32_t doto_rounded_14pt_codes[];

extern const uint8_t doto_rounded_14pt_black_bits[] PROGMEM;
extern const uint8_t doto_rounded_14pt_black_widths[];
extern const uint8_t doto_rounded_14pt_bold_bits[] PROGMEM;
extern const uint8_t doto_rounded_14pt_bold_widths[];
extern const uint8_t doto_rounded_14pt_extrabold_bits[] PROGMEM;
extern const uint8_t doto_rounded_14pt_extrabold_widths[];
extern const uint8_t doto_rounded_14pt_extralight_bits[] PROGMEM;
extern const uint8_t doto_rounded_14pt_extralight_widths[];
extern const uint8_t doto_rounded_14pt_light_bits[] PROGMEM;
extern const uint8_t doto_rounded_14pt_light_widths[];
extern const uint8_t doto_rounded_14pt_medium_bits[] PROGMEM;
extern const uint8_t doto_rounded_14pt_medium_widths[];
extern const uint8_t doto_rounded_14pt_regular_bits[] PROGMEM;
extern const uint8_t doto_rounded_14pt_regular_widths[];
extern const uint8_t doto_rounded_14pt_semibold_bits[] PROGMEM;
extern const uint8_t doto_rounded_14pt_semibold_widths[];
extern const uint8_t doto_rounded_14pt_thin_bits[] PROGMEM;
extern const uint8_t doto_rounded_14pt_thin_widths[];
extern const FontStyleMap doto_rounded_14pt_style_map[];
extern const FontHandle doto_rounded_14pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_ROUNDED_14PT_H_ */

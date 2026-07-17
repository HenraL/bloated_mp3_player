#ifndef BAKEDFONTS_DOTO_ROUNDED_10PT_H_
#define BAKEDFONTS_DOTO_ROUNDED_10PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DOTO_ROUNDED_10PT_WIDTH  6
#define DOTO_ROUNDED_10PT_HEIGHT 10
#define DOTO_ROUNDED_10PT_FIRST  32
#define DOTO_ROUNDED_10PT_LAST   8722
#define DOTO_ROUNDED_10PT_COUNT  319

extern const uint16_t doto_rounded_10pt_codes[];

extern const uint8_t doto_rounded_10pt_black_bits[] PROGMEM;
extern const uint8_t doto_rounded_10pt_black_widths[];
extern const uint8_t doto_rounded_10pt_bold_bits[] PROGMEM;
extern const uint8_t doto_rounded_10pt_bold_widths[];
extern const uint8_t doto_rounded_10pt_extrabold_bits[] PROGMEM;
extern const uint8_t doto_rounded_10pt_extrabold_widths[];
extern const uint8_t doto_rounded_10pt_extralight_bits[] PROGMEM;
extern const uint8_t doto_rounded_10pt_extralight_widths[];
extern const uint8_t doto_rounded_10pt_light_bits[] PROGMEM;
extern const uint8_t doto_rounded_10pt_light_widths[];
extern const uint8_t doto_rounded_10pt_medium_bits[] PROGMEM;
extern const uint8_t doto_rounded_10pt_medium_widths[];
extern const uint8_t doto_rounded_10pt_regular_bits[] PROGMEM;
extern const uint8_t doto_rounded_10pt_regular_widths[];
extern const uint8_t doto_rounded_10pt_semibold_bits[] PROGMEM;
extern const uint8_t doto_rounded_10pt_semibold_widths[];
extern const uint8_t doto_rounded_10pt_thin_bits[] PROGMEM;
extern const uint8_t doto_rounded_10pt_thin_widths[];
extern const FontStyleMap doto_rounded_10pt_style_map[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_ROUNDED_10PT_H_ */

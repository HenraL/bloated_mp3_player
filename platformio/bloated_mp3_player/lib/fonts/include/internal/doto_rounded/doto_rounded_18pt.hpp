#ifndef BAKEDFONTS_DOTO_ROUNDED_18PT_H_
#define BAKEDFONTS_DOTO_ROUNDED_18PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DOTO_ROUNDED_18PT_WIDTH  11
#define DOTO_ROUNDED_18PT_HEIGHT 19
#define DOTO_ROUNDED_18PT_FIRST  32
#define DOTO_ROUNDED_18PT_LAST   8722
#define DOTO_ROUNDED_18PT_COUNT  319

extern const uint16_t doto_rounded_18pt_codes[];

extern const uint8_t doto_rounded_18pt_black_bits[] PROGMEM;
extern const uint8_t doto_rounded_18pt_black_widths[];
extern const uint8_t doto_rounded_18pt_bold_bits[] PROGMEM;
extern const uint8_t doto_rounded_18pt_bold_widths[];
extern const uint8_t doto_rounded_18pt_extrabold_bits[] PROGMEM;
extern const uint8_t doto_rounded_18pt_extrabold_widths[];
extern const uint8_t doto_rounded_18pt_extralight_bits[] PROGMEM;
extern const uint8_t doto_rounded_18pt_extralight_widths[];
extern const uint8_t doto_rounded_18pt_light_bits[] PROGMEM;
extern const uint8_t doto_rounded_18pt_light_widths[];
extern const uint8_t doto_rounded_18pt_medium_bits[] PROGMEM;
extern const uint8_t doto_rounded_18pt_medium_widths[];
extern const uint8_t doto_rounded_18pt_regular_bits[] PROGMEM;
extern const uint8_t doto_rounded_18pt_regular_widths[];
extern const uint8_t doto_rounded_18pt_semibold_bits[] PROGMEM;
extern const uint8_t doto_rounded_18pt_semibold_widths[];
extern const uint8_t doto_rounded_18pt_thin_bits[] PROGMEM;
extern const uint8_t doto_rounded_18pt_thin_widths[];
extern const FontStyleMap doto_rounded_18pt_style_map[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_ROUNDED_18PT_H_ */

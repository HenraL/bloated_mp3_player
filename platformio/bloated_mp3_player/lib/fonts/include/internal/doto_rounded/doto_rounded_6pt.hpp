#ifndef BAKEDFONTS_DOTO_ROUNDED_6PT_H_
#define BAKEDFONTS_DOTO_ROUNDED_6PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DOTO_ROUNDED_6PT_WIDTH  4
#define DOTO_ROUNDED_6PT_HEIGHT 7
#define DOTO_ROUNDED_6PT_FIRST  32
#define DOTO_ROUNDED_6PT_LAST   8722
#define DOTO_ROUNDED_6PT_COUNT  319

extern const uint16_t doto_rounded_6pt_codes[];

extern const uint8_t doto_rounded_6pt_black_bits[] PROGMEM;
extern const uint8_t doto_rounded_6pt_black_widths[];
extern const uint8_t doto_rounded_6pt_bold_bits[] PROGMEM;
extern const uint8_t doto_rounded_6pt_bold_widths[];
extern const uint8_t doto_rounded_6pt_extrabold_bits[] PROGMEM;
extern const uint8_t doto_rounded_6pt_extrabold_widths[];
extern const uint8_t doto_rounded_6pt_extralight_bits[] PROGMEM;
extern const uint8_t doto_rounded_6pt_extralight_widths[];
extern const uint8_t doto_rounded_6pt_light_bits[] PROGMEM;
extern const uint8_t doto_rounded_6pt_light_widths[];
extern const uint8_t doto_rounded_6pt_medium_bits[] PROGMEM;
extern const uint8_t doto_rounded_6pt_medium_widths[];
extern const uint8_t doto_rounded_6pt_regular_bits[] PROGMEM;
extern const uint8_t doto_rounded_6pt_regular_widths[];
extern const uint8_t doto_rounded_6pt_semibold_bits[] PROGMEM;
extern const uint8_t doto_rounded_6pt_semibold_widths[];
extern const uint8_t doto_rounded_6pt_thin_bits[] PROGMEM;
extern const uint8_t doto_rounded_6pt_thin_widths[];
extern const FontStyleMap doto_rounded_6pt_style_map[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_ROUNDED_6PT_H_ */

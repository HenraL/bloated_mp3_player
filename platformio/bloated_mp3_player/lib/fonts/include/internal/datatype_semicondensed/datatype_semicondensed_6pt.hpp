#ifndef BAKEDFONTS_DATATYPE_SEMICONDENSED_6PT_H_
#define BAKEDFONTS_DATATYPE_SEMICONDENSED_6PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DATATYPE_SEMICONDENSED_6PT_WIDTH  4
#define DATATYPE_SEMICONDENSED_6PT_HEIGHT 7
#define DATATYPE_SEMICONDENSED_6PT_FIRST  32
#define DATATYPE_SEMICONDENSED_6PT_LAST   8722
#define DATATYPE_SEMICONDENSED_6PT_COUNT  319

extern const uint16_t datatype_semicondensed_6pt_codes[];

extern const uint8_t datatype_semicondensed_6pt_black_bits[] PROGMEM;
extern const uint8_t datatype_semicondensed_6pt_black_widths[];
extern const uint8_t datatype_semicondensed_6pt_bold_bits[] PROGMEM;
extern const uint8_t datatype_semicondensed_6pt_bold_widths[];
extern const uint8_t datatype_semicondensed_6pt_extrabold_bits[] PROGMEM;
extern const uint8_t datatype_semicondensed_6pt_extrabold_widths[];
extern const uint8_t datatype_semicondensed_6pt_extralight_bits[] PROGMEM;
extern const uint8_t datatype_semicondensed_6pt_extralight_widths[];
extern const uint8_t datatype_semicondensed_6pt_light_bits[] PROGMEM;
extern const uint8_t datatype_semicondensed_6pt_light_widths[];
extern const uint8_t datatype_semicondensed_6pt_medium_bits[] PROGMEM;
extern const uint8_t datatype_semicondensed_6pt_medium_widths[];
extern const uint8_t datatype_semicondensed_6pt_regular_bits[] PROGMEM;
extern const uint8_t datatype_semicondensed_6pt_regular_widths[];
extern const uint8_t datatype_semicondensed_6pt_semibold_bits[] PROGMEM;
extern const uint8_t datatype_semicondensed_6pt_semibold_widths[];
extern const uint8_t datatype_semicondensed_6pt_thin_bits[] PROGMEM;
extern const uint8_t datatype_semicondensed_6pt_thin_widths[];
extern const FontStyleMap datatype_semicondensed_6pt_style_map[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_SEMICONDENSED_6PT_H_ */

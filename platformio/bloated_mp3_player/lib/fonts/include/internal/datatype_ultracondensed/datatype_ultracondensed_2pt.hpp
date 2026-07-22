#ifndef BAKEDFONTS_DATATYPE_ULTRACONDENSED_2PT_H_
#define BAKEDFONTS_DATATYPE_ULTRACONDENSED_2PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DATATYPE_ULTRACONDENSED_2PT_WIDTH  2
#define DATATYPE_ULTRACONDENSED_2PT_HEIGHT 3
#define DATATYPE_ULTRACONDENSED_2PT_FIRST  32
#define DATATYPE_ULTRACONDENSED_2PT_LAST   8722
#define DATATYPE_ULTRACONDENSED_2PT_COUNT  319

extern const uint16_t datatype_ultracondensed_2pt_codes[];

extern const uint8_t datatype_ultracondensed_2pt_black_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_2pt_black_widths[];
extern const uint8_t datatype_ultracondensed_2pt_bold_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_2pt_bold_widths[];
extern const uint8_t datatype_ultracondensed_2pt_extrabold_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_2pt_extrabold_widths[];
extern const uint8_t datatype_ultracondensed_2pt_extralight_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_2pt_extralight_widths[];
extern const uint8_t datatype_ultracondensed_2pt_light_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_2pt_light_widths[];
extern const uint8_t datatype_ultracondensed_2pt_medium_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_2pt_medium_widths[];
extern const uint8_t datatype_ultracondensed_2pt_regular_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_2pt_regular_widths[];
extern const uint8_t datatype_ultracondensed_2pt_semibold_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_2pt_semibold_widths[];
extern const uint8_t datatype_ultracondensed_2pt_thin_bits[] PROGMEM;
extern const uint8_t datatype_ultracondensed_2pt_thin_widths[];
extern const FontStyleMap datatype_ultracondensed_2pt_style_map[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DATATYPE_ULTRACONDENSED_2PT_H_ */

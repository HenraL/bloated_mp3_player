#ifndef BAKEDFONTS_DOTO_8PT_H_
#define BAKEDFONTS_DOTO_8PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DOTO_8PT_WIDTH  5
#define DOTO_8PT_HEIGHT 9
#define DOTO_8PT_FIRST  32
#define DOTO_8PT_LAST   8722
#define DOTO_8PT_COUNT  319

extern const uint16_t doto_8pt_codes[];

extern const uint8_t doto_8pt_bits[] PROGMEM;
extern const uint8_t doto_8pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_8PT_H_ */

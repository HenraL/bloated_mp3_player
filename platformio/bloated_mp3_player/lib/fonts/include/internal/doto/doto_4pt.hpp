#ifndef BAKEDFONTS_DOTO_4PT_H_
#define BAKEDFONTS_DOTO_4PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DOTO_4PT_WIDTH  3
#define DOTO_4PT_HEIGHT 5
#define DOTO_4PT_FIRST  32
#define DOTO_4PT_LAST   8722
#define DOTO_4PT_COUNT  319

extern const uint16_t doto_4pt_codes[];

extern const uint8_t doto_4pt_bits[] PROGMEM;
extern const uint8_t doto_4pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_4PT_H_ */

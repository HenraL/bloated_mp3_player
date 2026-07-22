#ifndef BAKEDFONTS_DOTO_2PT_H_
#define BAKEDFONTS_DOTO_2PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define DOTO_2PT_WIDTH  2
#define DOTO_2PT_HEIGHT 3
#define DOTO_2PT_FIRST  32
#define DOTO_2PT_LAST   8722
#define DOTO_2PT_COUNT  319

extern const uint16_t doto_2pt_codes[];

extern const uint8_t doto_2pt_bits[] PROGMEM;
extern const uint8_t doto_2pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_DOTO_2PT_H_ */

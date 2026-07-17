#ifndef BAKEDFONTS_BITCOUNTSINGLE_12PT_H_
#define BAKEDFONTS_BITCOUNTSINGLE_12PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define BITCOUNTSINGLE_12PT_WIDTH  14
#define BITCOUNTSINGLE_12PT_HEIGHT 12
#define BITCOUNTSINGLE_12PT_FIRST  32
#define BITCOUNTSINGLE_12PT_LAST   64258
#define BITCOUNTSINGLE_12PT_COUNT  396

extern const uint16_t bitcountsingle_12pt_codes[];

extern const uint8_t bitcountsingle_12pt_bits[] PROGMEM;
extern const uint8_t bitcountsingle_12pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_BITCOUNTSINGLE_12PT_H_ */

#ifndef BAKEDFONTS_BITCOUNTSINGLE_20PT_H_
#define BAKEDFONTS_BITCOUNTSINGLE_20PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define BITCOUNTSINGLE_20PT_WIDTH  24
#define BITCOUNTSINGLE_20PT_HEIGHT 18
#define BITCOUNTSINGLE_20PT_FIRST  32
#define BITCOUNTSINGLE_20PT_LAST   64258
#define BITCOUNTSINGLE_20PT_COUNT  396

extern const uint16_t bitcountsingle_20pt_codes[];

extern const uint8_t bitcountsingle_20pt_bits[] PROGMEM;
extern const uint8_t bitcountsingle_20pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_BITCOUNTSINGLE_20PT_H_ */

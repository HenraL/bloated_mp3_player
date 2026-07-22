#ifndef BAKEDFONTS_BITCOUNTSINGLE_6PT_H_
#define BAKEDFONTS_BITCOUNTSINGLE_6PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define BITCOUNTSINGLE_6PT_WIDTH  7
#define BITCOUNTSINGLE_6PT_HEIGHT 6
#define BITCOUNTSINGLE_6PT_FIRST  32
#define BITCOUNTSINGLE_6PT_LAST   64258
#define BITCOUNTSINGLE_6PT_COUNT  396

extern const uint16_t bitcountsingle_6pt_codes[];

extern const uint8_t bitcountsingle_6pt_bits[] PROGMEM;
extern const uint8_t bitcountsingle_6pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_BITCOUNTSINGLE_6PT_H_ */

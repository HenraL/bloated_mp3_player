#ifndef BAKEDFONTS_BITCOUNTSINGLE_16PT_H_
#define BAKEDFONTS_BITCOUNTSINGLE_16PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define BITCOUNTSINGLE_16PT_WIDTH  19
#define BITCOUNTSINGLE_16PT_HEIGHT 15
#define BITCOUNTSINGLE_16PT_FIRST  32
#define BITCOUNTSINGLE_16PT_LAST   64258
#define BITCOUNTSINGLE_16PT_COUNT  396

extern const uint16_t bitcountsingle_16pt_codes[];

extern const uint8_t bitcountsingle_16pt_bits[] PROGMEM;
extern const uint8_t bitcountsingle_16pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_BITCOUNTSINGLE_16PT_H_ */

#ifndef BAKEDFONTS_BITCOUNTSINGLE_2PT_H_
#define BAKEDFONTS_BITCOUNTSINGLE_2PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define BITCOUNTSINGLE_2PT_WIDTH  3
#define BITCOUNTSINGLE_2PT_HEIGHT 3
#define BITCOUNTSINGLE_2PT_FIRST  32
#define BITCOUNTSINGLE_2PT_LAST   64258
#define BITCOUNTSINGLE_2PT_COUNT  396

extern const uint16_t bitcountsingle_2pt_codes[];

extern const uint8_t bitcountsingle_2pt_bits[] PROGMEM;
extern const uint8_t bitcountsingle_2pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_BITCOUNTSINGLE_2PT_H_ */

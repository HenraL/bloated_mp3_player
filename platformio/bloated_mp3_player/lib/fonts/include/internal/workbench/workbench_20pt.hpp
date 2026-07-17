#ifndef BAKEDFONTS_WORKBENCH_20PT_H_
#define BAKEDFONTS_WORKBENCH_20PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define WORKBENCH_20PT_WIDTH  12
#define WORKBENCH_20PT_HEIGHT 20
#define WORKBENCH_20PT_FIRST  32
#define WORKBENCH_20PT_LAST   8725
#define WORKBENCH_20PT_COUNT  215

extern const uint16_t workbench_20pt_codes[];

extern const uint8_t workbench_20pt_bits[] PROGMEM;
extern const uint8_t workbench_20pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_WORKBENCH_20PT_H_ */

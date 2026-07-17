#ifndef BAKEDFONTS_WORKBENCH_12PT_H_
#define BAKEDFONTS_WORKBENCH_12PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define WORKBENCH_12PT_WIDTH  8
#define WORKBENCH_12PT_HEIGHT 12
#define WORKBENCH_12PT_FIRST  32
#define WORKBENCH_12PT_LAST   8725
#define WORKBENCH_12PT_COUNT  215

extern const uint16_t workbench_12pt_codes[];

extern const uint8_t workbench_12pt_bits[] PROGMEM;
extern const uint8_t workbench_12pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_WORKBENCH_12PT_H_ */

#ifndef BAKEDFONTS_WORKBENCH_16PT_H_
#define BAKEDFONTS_WORKBENCH_16PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define WORKBENCH_16PT_WIDTH  10
#define WORKBENCH_16PT_HEIGHT 16
#define WORKBENCH_16PT_FIRST  32
#define WORKBENCH_16PT_LAST   8725
#define WORKBENCH_16PT_COUNT  215

extern const uint16_t workbench_16pt_codes[];

extern const uint8_t workbench_16pt_bits[] PROGMEM;
extern const uint8_t workbench_16pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_WORKBENCH_16PT_H_ */

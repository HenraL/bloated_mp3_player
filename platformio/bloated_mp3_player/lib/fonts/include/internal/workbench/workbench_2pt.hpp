#ifndef BAKEDFONTS_WORKBENCH_2PT_H_
#define BAKEDFONTS_WORKBENCH_2PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

#define WORKBENCH_2PT_WIDTH  3
#define WORKBENCH_2PT_HEIGHT 3
#define WORKBENCH_2PT_FIRST  32
#define WORKBENCH_2PT_LAST   8725
#define WORKBENCH_2PT_COUNT  215

extern const uint16_t workbench_2pt_codes[];

extern const uint8_t workbench_2pt_bits[] PROGMEM;
extern const uint8_t workbench_2pt_widths[];

}  // namespace BakedFonts

#endif /* BAKEDFONTS_WORKBENCH_2PT_H_ */

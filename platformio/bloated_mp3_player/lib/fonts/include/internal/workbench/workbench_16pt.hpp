#ifndef BAKEDFONTS_WORKBENCH_16PT_H_
#define BAKEDFONTS_WORKBENCH_16PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t WORKBENCH_16PT_WIDTH  = 10;
static const uint8_t WORKBENCH_16PT_HEIGHT = 30;
static const uint32_t WORKBENCH_16PT_FIRST  = 32;
static const uint32_t WORKBENCH_16PT_LAST   = 8725;
static const uint16_t WORKBENCH_16PT_COUNT  = 215;

extern const uint32_t workbench_16pt_codes[];

extern const uint8_t workbench_16pt_bits[] PROGMEM;
extern const uint8_t workbench_16pt_widths[];
extern const FontHandle workbench_16pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_WORKBENCH_16PT_H_ */

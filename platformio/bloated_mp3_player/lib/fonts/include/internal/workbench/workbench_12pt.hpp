#ifndef BAKEDFONTS_WORKBENCH_12PT_H_
#define BAKEDFONTS_WORKBENCH_12PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t WORKBENCH_12PT_WIDTH  = 8;
static const uint8_t WORKBENCH_12PT_HEIGHT = 22;
static const uint32_t WORKBENCH_12PT_FIRST  = 32;
static const uint32_t WORKBENCH_12PT_LAST   = 8725;
static const uint16_t WORKBENCH_12PT_COUNT  = 215;

extern const uint32_t workbench_12pt_codes[];

extern const uint8_t workbench_12pt_bits[] PROGMEM;
extern const uint8_t workbench_12pt_widths[];
extern const FontHandle workbench_12pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_WORKBENCH_12PT_H_ */

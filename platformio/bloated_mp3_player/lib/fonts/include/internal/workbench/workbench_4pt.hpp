#ifndef BAKEDFONTS_WORKBENCH_4PT_H_
#define BAKEDFONTS_WORKBENCH_4PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t WORKBENCH_4PT_WIDTH  = 4;
static const uint8_t WORKBENCH_4PT_HEIGHT = 4;
static const uint32_t WORKBENCH_4PT_FIRST  = 32;
static const uint32_t WORKBENCH_4PT_LAST   = 8725;
static const uint16_t WORKBENCH_4PT_COUNT  = 215;

extern const uint32_t workbench_4pt_codes[];

extern const uint8_t workbench_4pt_bits[] PROGMEM;
extern const uint8_t workbench_4pt_widths[];
extern const FontHandle workbench_4pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_WORKBENCH_4PT_H_ */

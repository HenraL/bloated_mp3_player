#ifndef BAKEDFONTS_WORKBENCH_14PT_H_
#define BAKEDFONTS_WORKBENCH_14PT_H_

#include <stdint.h>
#include <Arduino.h>
#include "font_structs.hpp"

namespace BakedFonts {

static const uint8_t WORKBENCH_14PT_WIDTH  = 10;
static const uint8_t WORKBENCH_14PT_HEIGHT = 28;
static const uint32_t WORKBENCH_14PT_FIRST  = 32;
static const uint32_t WORKBENCH_14PT_LAST   = 8725;
static const uint16_t WORKBENCH_14PT_COUNT  = 215;

extern const uint32_t workbench_14pt_codes[];

extern const uint8_t workbench_14pt_bits[] PROGMEM;
extern const uint8_t workbench_14pt_widths[];
extern const FontHandle workbench_14pt_handle;

}  // namespace BakedFonts

#endif /* BAKEDFONTS_WORKBENCH_14PT_H_ */

#pragma once

#include "plugin.hpp"

// display at default zoom level
static const int DEFAULT_CELLS_DISPLAYED_X = 40;
static const int DEFAULT_CELLS_DISPLAYED_Y = 40;
// depends on how many octaves you want to represent.
// 9 is good to me, so 9 * 12
// square grid to facilitate evolution
static const int NUMCELLS_X = 108;
static const int NUMCELLS_Y = 108;
// at position 57 is A4 = 440.0 Hz
static const int REFERENCE_POS = 48;
static const float REFERENCE_VOCT = 0.0f;
static const float HIGHEST_FREQUENCY = 7902.13f;

static const int ALPHA_OPAQUE = 255;
static const int ALPHA_TPT = 110;
// palette
static const NVGcolor OPAQUE_C1_DARK = nvgRGBA(0x2d, 0x38, 0x55, ALPHA_OPAQUE);
static const NVGcolor OPAQUE_C1_MEDIUM = nvgRGBA(104, 137, 204, ALPHA_OPAQUE);
static const NVGcolor OPAQUE_C1_LIGHT = nvgRGBA(0xe3, 0xe5, 0xf1, ALPHA_OPAQUE);
static const NVGcolor OPAQUE_C2 = nvgRGBA(101, 99, 26, ALPHA_OPAQUE);
static const NVGcolor OPAQUE_C3 = nvgRGBA(246, 198, 184, ALPHA_OPAQUE);
static const NVGcolor OPAQUE_C2_DARKER = nvgRGBA(51, 50, 13, ALPHA_OPAQUE);
static const NVGcolor OPAQUE_C3_DARKER = nvgRGBA(123, 99, 92, ALPHA_OPAQUE);
// transparents
static const NVGcolor TPT_C1_DARK = nvgRGBA(0x2d, 0x38, 0x55, ALPHA_TPT);
static const NVGcolor TPT_C1_MEDIUM = nvgRGBA(104, 137, 204, ALPHA_TPT);
static const NVGcolor TPT_C1_LIGHT = nvgRGBA(0xe3, 0xe5, 0xf1, ALPHA_TPT);
static const NVGcolor TPT_C2 = nvgRGBA(101, 99, 26, ALPHA_TPT);
static const NVGcolor TPT_C3 = nvgRGBA(246, 198, 184, ALPHA_TPT);
static const NVGcolor TPT_C2_DARKER = nvgRGBA(51, 50, 13, ALPHA_TPT);
static const NVGcolor TPT_C3_DARKER = nvgRGBA(123, 99, 92, ALPHA_TPT);
//
static const NVGcolor COLOR_RED_BUTTON_T = nvgRGBA(186, 0, 0, ALPHA_TPT);
static const NVGcolor COLOR_YELLOW_BUTTON_T = nvgRGBA(186, 186, 0, ALPHA_TPT);
static const NVGcolor COLOR_GREEN_BUTTON_T = nvgRGBA(0, 186, 0, ALPHA_TPT);
static const NVGcolor COLOR_GREEN_BUTTON_O = nvgRGBA(0, 186, 0, ALPHA_OPAQUE);
static const NVGcolor COLOR_NOK_BUTTON_O = nvgRGBA(186, 0, 0, ALPHA_OPAQUE);
static const NVGcolor COLOR_OK_BUTTON_O = nvgRGBA(0, 186, 0, ALPHA_OPAQUE);

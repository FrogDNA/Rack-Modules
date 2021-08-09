#pragma once

#include "plugin.hpp"

// depends on how many octaves you want to represent.
// 9 is good to me, so 9 * 12
// square grid to facilitate evolution
static const int NUMCELLS_X = 108;
static const int NUMCELLS_Y = 108;
// at position 57 is A4 = 440.0 Hz
static const int REFERENCE_POS = 57;
static const float REFERENCE_FREQ = 440.0f;
static const float HIGHEST_FREQUENCY = 7902.13f;

static const NVGcolor COLOR_DARK_GRAY = nvgRGBA(0x11, 0x11, 0x11, 0xff);
static const NVGcolor COLOR_MEDIUM_DARK_GRAY = nvgRGBA(0x6e, 0x6e, 0x6e, 0xff);
static const NVGcolor COLOR_MEDIUM_LIGHT_GRAY = nvgRGBA(0xdd, 0xdd, 0xdd, 0xff);
static const NVGcolor COLOR_LIGHT_GRAY = nvgRGBA(0xf0, 0xf0, 0xf0, 0xff);

// transparent colors
static const NVGcolor COLOR_YELLOW_BUTTON_T = nvgRGBA(0xba, 0xba, 0x00, 0x6e);
static const NVGcolor COLOR_RED_BUTTON_T = nvgRGBA(0xba, 0x00, 0x00, 0x6e);
static const NVGcolor COLOR_GREEN_BUTTON_T = nvgRGBA(0x00, 0xba, 0x00, 0x6e);

static const NVGcolor COLOR_RED_BUTTON_O = nvgRGBA(0xba, 0x00, 0x00, 0xff);
static const NVGcolor COLOR_GREEN_BUTTON_O = nvgRGBA(0x00, 0xba, 0x00, 0xff);

// palette
// 68, 41, 80
static const NVGcolor PALETTE_DARK_PURPLE_O = nvgRGBA(0x44, 0x29, 0x59, 0xff);
// 118, 78, 128
static const NVGcolor PALETTE_MEDIUM_PURPLE_O = nvgRGBA(0x76, 0x4e, 0x80, 0xff);
// rgb(177, 96, 142)
static const NVGcolor PALETTE_LIGHT_PURPLE_O = nvgRGBA(0xb1, 0x60, 0x8e, 0xff);
// rgb(182, 204, 177)
static const NVGcolor PALETTE_LIGHT_GREEN_O = nvgRGBA(0xb6, 0xcc, 0xb1, 0xff);
// rgb(149, 175, 92)
static const NVGcolor PALETTE_DARK_GREEN_O = nvgRGBA(0x95, 0xaf, 0x5c, 0xff);
// transparents
static const NVGcolor PALETTE_DARK_PURPLE_T = nvgRGBA(0x44, 0x29, 0x59, 0x6e);
static const NVGcolor ALETTE_MEDIUM_PURPLE_T = nvgRGBA(0x76, 0x4e, 0x80, 0x6e);
static const NVGcolor PALETTE_LIGHT_PURPLE_T = nvgRGBA(0xb1, 0x60, 0x8e, 0x6e);
static const NVGcolor PALETTE_LIGHT_GREEN_T = nvgRGBA(0xb6, 0xcc, 0xb1, 0x6e);
static const NVGcolor PALETTE_DARK_GREEN_T = nvgRGBA(0x95, 0xaf, 0x5c, 0x6e);

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

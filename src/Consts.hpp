#ifndef CONSTS_H
#define CONSTS_H

// depends on how many octaves you want to represent.
// 9 is good to me, so 9 * 12
// square grid to facilitate evolution
// static const int NUMCELLS_X = 108;
// static const int NUMCELLS_Y = 108;
static const int NUMCELLS_X = 40;
static const int NUMCELLS_Y = 40;
// at position 57 is A4 = 440.0 Hz
// static const int REFERENCE_POS = 57;
static const int REFERENCE_POS = 20;
static const float REFERENCE_FREQ = 440.0f;
static const float HIGHEST_FREQUENCY = 7902.13f;

#endif

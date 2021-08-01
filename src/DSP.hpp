#ifndef DSP_H
#define DSP_H

#include "GameOfLifeGrid.hpp"
#include "robin_hood.h"

const float ENVELOPE_DURATION = 0.1f;
const int SIN_LUT_SIZE = 1000000;
const float WIDENESS_REF = 5.0f;

struct SoundParams {
  float round;
  float wide;
  SoundParams(float wideness, float roundness);
};

class AudibleCell {

  float x = 0.f;
  float harmonicNumber = 1.f;
  float baseFrequency = 440.0f;
  float amplitude = 1.0f;
  float phase = 0.f;
  float freqMultiplicator = 1.0f;
  AudibleCell(float x, float baseFrequency, float harmonicNumber,
              float amplitude);
  float nextValue(float sampleTime);
  float currentAmplitude();
  friend class DSP;
};

class DSP {
  std::vector<bool> audibleCols;
  std::vector<bool> audibleRows;
  std::vector<float> baseFreqLut;
  std::vector<Cell *> alive;
  std::vector<AudibleCell *> audibles;
  float wideness = -1.f;
  float roundness = -1.f;
  float vOct = 0.f;
  float lowestFreq =
      7902.13f; // B8 - for optimization purposes, but is it the right place ?
  float computeAmplitude(SoundParams *sp, float numHarmonic);

public:
  static std::vector<float> sinLut;
  static std::vector<float> initSinLut();
  DSP();
  void paramValues(GridState gs, float wideness, float roundness, float vOct);
  float nextValue(float sampleTime);
  float getLowestFreq();
  void muteUnmuteCol(int x, bool muted);
  void muteUnmuteRow(int y, bool muted);
  bool isCellAudible(Cell *c);
  bool isColAudible(int x);
  bool isRowAudible(int y);
};

#endif

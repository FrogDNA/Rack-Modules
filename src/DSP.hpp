#ifndef DSP_H
#define DSP_H

#include "GameOfLifeGrid.hpp"
#include "robin_hood.h"

const float ENVELOPE_DURATION = 0.1f;
const float BASE_FREQ = 440.f;
const int SIN_LUT_SIZE = 1000000;

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
  std::vector<float> baseFreqLut;
  std::vector<Cell *> alive;
  std::vector<AudibleCell *> audibles;
  float wideness = -1.f;
  float roundness = -1.f;
  float vOct = 0.f;
  float lowestFreq =
      7902.13f; // B8 - for optimization purposes, but is it the right place ?
  float computeAmplitude(float wideness, float roundness, float numHarmonic);

public:
  static std::vector<float> sinLut;
  static std::vector<float> initSinLut();
  DSP();
  void paramValues(GridState gs, float wideness, float roundness, float vOct);
  float nextValue(float sampleTime);
  float getLowestFreq();
};

#endif

#ifndef DSP_H
#define DSP_H

#include "GameOfLifeGrid.hpp"
#include "robin_hood.h"

const float ENVELOPE_DURATION = 0.1f;
const float BASE_FREQ = 440.f;
const int SIN_LUT_SIZE = 1000000;

class AudibleCell {

  float baseFrequency = 440.0f;
  float amplitude = 1.0f;
  float envelope = 1.0f;
  float phase = 0.f;

  void nextEnvelopeValue(float sampleTime);

public:
  bool raising = true;
  bool decaying = false;
  float envelopePhase = 0.f;
  float x = 0.f;
  float harmonicNumber = 1.f;
  AudibleCell(float x, float baseFrequency, float harmonicNumber,
              float amplitude);
  float nextValue(float sampleTime, float vOct);
  float currentAmplitude();
};

class DSP {
  std::vector<float> baseFreqLut;
  std::vector<Cell *> alive;
  std::vector<AudibleCell *> audibles;
  std::vector<AudibleCell *> oldAudibles;
  float wideness = -1.f;
  float center = -1.f;
  float vOct = 0.f;

public:
  static std::vector<float> sinLut;
  static std::vector<float> initSinLut();
  DSP();
  void paramValues(GridState gs, float wideness, float center, float vOct);
  float nextValue(float sampleTime);
};

#endif

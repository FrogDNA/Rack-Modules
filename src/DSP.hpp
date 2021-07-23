#ifndef DSP_H
#define DSP_H

#include "GameOfLifeGrid.hpp"
#include "robin_hood.h"

const float BASE_FREQ = 440.f;
const int LUT_SIZE = 1000000;

class DSP {
  std::vector<float> lut;
  float time = 0.f; // todo use a phase accumulation
  std::vector<Cell *> alive;
  float wideness = -1.f;
  float center = -1.f;
  float vOct = 0.f;
  robin_hood::unordered_map<int, int> harmonics;
  std::vector<std::vector<float>> frequencies;
  std::vector<std::vector<float>> phases;
  std::vector<std::vector<float>> amplitudes;
  std::vector<std::vector<float>> futureFrequencies;
  std::vector<std::vector<float>> futurePhases;
  std::vector<std::vector<float>> futureAmplitudes;

public:
  DSP();
  void paramValues(std::vector<Cell *> alive, float wideness, float center,
                   float vOct);
  float nextValue(float sampleTime);
};

#endif

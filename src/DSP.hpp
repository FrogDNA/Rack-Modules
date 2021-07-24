#ifndef DSP_H
#define DSP_H

#include "GameOfLifeGrid.hpp"
#include "robin_hood.h"

const float ENVELOPE_DURATION = 0.2f;
const float BASE_FREQ = 440.f;
const int LUT_SIZE = 1000000;

class AMGenerator {
  float phase = 0.f;
  bool running = false;

public:
  AMGenerator();
  void start();
  void stopAndReset();
  bool isRunning();
  float nextValue(float sampleTime);
};

class DSP {
  AMGenerator *am = NULL;
  std::vector<float> lut;
  float time = 0.f; // todo use a phase accumulation
  std::vector<Cell *> alive;
  float wideness = -1.f;
  float center = -1.f;
  float vOct = 0.f;
  std::vector<int> harmonics;
  std::vector<int> futureHarmonics;
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

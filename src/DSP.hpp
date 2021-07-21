#ifndef DSP_H
#define DSP_H

#include "GameOfLifeGrid.hpp"
#include <set>
#include <unordered_map>

const float BASE_FREQ = 440.f;

class DSP {
  float time = 0.f; // todo use a phase accumulation
  std::set<Cell *> alive;
  float wideness = -1.f;
  float center = -1.f;
  float vOct = 0.f;
  std::unordered_map<int, int> harmonics;
  std::unordered_map<int, std::unordered_map<int, float>> harmonicAmplitudes;

public:
  DSP();
  void paramValues(std::set<Cell *> alive, float wideness, float center,
                   float vOct);
  float nextValue(float sampleTime);
};

#endif

#pragma once

#include "GameOfLifeGrid.hpp"

struct Outputs {
  std::vector<float> xOutputs;
  std::vector<float> xPresents;
  std::vector<float> yOutputs;
  std::vector<float> yPresents;
  Outputs();
};

class DSP {
  int offsetX = 0;
  int offsetY = 0;
  std::set<int> usableX;
  std::set<int> usableY;
  std::vector<bool> audibleCols;
  std::vector<bool> audibleRows;
  std::vector<float> xFrequencies;
  std::vector<float> yFrequencies;
  std::vector<std::pair<int, int> *> knownState;
  bool audibilityChanged = false;
  bool outputChanged = false;
  int vSize = 0;

public:
  DSP();
  void resetOutputChanged();
  bool isOutputChanged();
  Outputs getOutputs();
  void paramValues(std::vector<std::pair<int, int> *> state, float rowVoct,
                   float colVoct);
  void muteUnmuteCol(int x, bool muted);
  void muteUnmuteRow(int y, bool muted);
  bool isCellAudible(std::pair<int, int> *c);
  bool isColAudible(int x);
  bool isRowAudible(int y);
};

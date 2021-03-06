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
  std::set<int> usableX;
  std::set<int> usableY;
  std::vector<bool> audibleCols;
  std::vector<bool> audibleRows;
  std::vector<float> xFrequencies;
  std::vector<float> yFrequencies;
  bool outputChanged = false;
  int vSize = 0;

public:
  DSP();
  void resetOutputChanged();
  bool isOutputChanged();
  Outputs getOutputs();
  void update(std::vector<std::pair<int, int> *> state);
  void muteUnmuteCol(int x, bool muted);
  void muteUnmuteRow(int y, bool muted);
  bool isCellAudible(std::pair<int, int> *c);
  bool isColAudible(int x);
  bool isRowAudible(int y);
};

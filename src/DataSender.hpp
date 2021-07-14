#ifndef DATASENDER_H
#define DATASENDER_H

#include "GameOfLifeGrid.hpp"
#include <set>

class DataSender {
  std::set<Cell *> aliveCells;
  std::set<Cell *>::iterator itr;
  bool transferInProgress;
  float clock;
  float data;

public:
  DataSender();
  void init(std::set<Cell *> aliveCells);
  void next();
  float getClock();
  float getData();
  bool isTransferInProgress();
};

#endif

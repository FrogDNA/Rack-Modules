#ifndef DATASENDER_H
#define DATASENDER_H

#include "GameOfLifeGrid.hpp"
#include <set>

const int SEND_CLOCK_CYLES = 10;

class DataSender {
  std::set<Cell *> aliveCells;
  std::set<Cell *>::iterator itr;
  bool transferInProgress = false;
  bool sendX = true;
  float clock = 0.f;
  float data = 0.f;
  int state = 0;
  int cycles = 0;

public:
  DataSender();
  void init(std::set<Cell *> aliveCells);
  void next();
  float getClock();
  float getData();
  bool isTransferInProgress();
};

#endif

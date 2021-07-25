#ifndef DATASENDER_H
#define DATASENDER_H

#include "GameOfLifeGrid.hpp"
#include <set>

const int SEND_SPEED = 1000;

class DataSender {
  std::vector<Cell *> aliveCells;
  std::vector<Cell *>::iterator itr;
  bool transferInProgress = false;
  bool sendX = true;
  float clock = 0.f;
  float data = 0.f;
  int state = 0;
  int cycles = 0;

public:
  DataSender();
  void init(std::vector<Cell *> aliveCells);
  void next();
  float getClock();
  float getData();
  bool isTransferInProgress();
};

#endif

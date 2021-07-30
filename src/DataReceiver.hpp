#ifndef DATARECEIVER_H
#define DATARECEIVER_H

#include "Coordinate.hpp"
#include "GameOfLifeGrid.hpp"
#include <set>

class DataReceiver {
  int x = 0;
  int y = 0;
  float lastBusyIn = 0.f;
  bool isXNext = true;
  bool risingEdgeReceived = false;
  GameOfLifeGrid *gridInPreparation;
  bool gridReady = false;

public:
  DataReceiver();
  void check(float busyIn, float clockIn, float dataIn);
  bool isNewGridReady();
  GameOfLifeGrid *getGrid();
};

#endif

#ifndef DATARECEIVER_H
#define DATARECEIVER_H

#include "Coordinate.hpp"
#include "GameOfLifeGrid.hpp"
#include <set>

class DataReceiver {
  int x = 0;
  int y = 0;
  bool isXNext = true;
  bool risingEdgeReceived = false;

public:
  DataReceiver();
  void checkAndUpdateGrid(float busyIn, float clockIn, float dataIn,
                          GameOfLifeGrid *golGrid);
};

#endif

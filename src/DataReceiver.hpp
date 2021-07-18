#ifndef DATARECEIVER_H
#define DATARECEIVER_H

#include "Coordinate.hpp"
#include "GameOfLifeGrid.hpp"
#include <set>

class DataReceiver {
  Coordinate *data = NULL;

public:
  DataReceiver();
  bool hasNewData();
  Coordinate getData();
};

#endif

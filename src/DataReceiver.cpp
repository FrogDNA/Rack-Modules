#include "DataReceiver.hpp"
#include "GameOfLifeGrid.hpp"

DataReceiver::DataReceiver() {}

void DataReceiver::checkAndUpdateGrid(float busyIn, float clockIn, float dataIn,
                                      GameOfLifeGrid *golGrid) {
  if (busyIn > 3.5f) {
    if (clockIn > 3.5f && !risingEdgeReceived) {
      risingEdgeReceived = true;
      if (isXNext) {
        x = (int)((dataIn / 10.f) * NUMCELLSX);
        isXNext = false;
      } else {
        y = (int)((dataIn / 10.f) * NUMCELLSY);
        golGrid->setCellState(x, y, true);
        isXNext = true;
      }
    } else if (clockIn < 1.5f && risingEdgeReceived) {
      risingEdgeReceived = false;
    }
  } else {
    isXNext = true;
    risingEdgeReceived = false;
  }
}

#include "DataReceiver.hpp"
#include "GameOfLifeGrid.hpp"

DataReceiver::DataReceiver() {}

void DataReceiver::check(float busyIn, float clockIn, float dataIn) {
  if (busyIn > 3.5f) {
    if (lastBusyIn < 1.5f) {
      gridReady = false;
      gridInPreparation = new GameOfLifeGrid();
    }
    if (clockIn > 3.5f && !risingEdgeReceived) {
      risingEdgeReceived = true;
      if (isXNext) {
        x = (int)((dataIn / 10.f) * NUMCELLSX);
        isXNext = false;
      } else {
        y = (int)((dataIn / 10.f) * NUMCELLSY);
        gridInPreparation->setCellState(x, y, true);
        isXNext = true;
      }
    } else if (clockIn < 1.5f && risingEdgeReceived) {
      risingEdgeReceived = false;
    }
  } else {
    isXNext = true;
    risingEdgeReceived = false;
    if (lastBusyIn > 3.5f) {
      gridReady = true;
    }
  }
  lastBusyIn = busyIn;
}

bool DataReceiver::isNewGridReady() { return gridReady; }

GameOfLifeGrid *DataReceiver::getGrid() {
  gridReady = false;
  return gridInPreparation;
}

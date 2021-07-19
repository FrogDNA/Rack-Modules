#include "DataSender.hpp"
#include "GameOfLifeGrid.hpp"

DataSender::DataSender() {
  transferInProgress = false;
  sendX = true;
}

void DataSender::init(std::set<Cell *> aliveCells) {
  // do not init if a transfer is in progress
  if (!transferInProgress) {
    this->aliveCells = aliveCells;
    itr = this->aliveCells.begin();
    state = 0;
    clock = 0.f;
    data = 0.f;
    cycles = 0;
    if (itr != aliveCells.end()) {
      transferInProgress = true;
    } else {
      transferInProgress = false;
    }
  }
}

void DataSender::next() {
  if (transferInProgress) {
    switch (state) {
    case 0: {
      clock = 0.f;
      int x = (*itr)->getX();
      float xData = 10.0f * (float)(x) / (float)NUMCELLSX;
      data = xData;
    } break;
    case 1:
      clock = 10.f;
      break;
    case 2: {
      clock = 0.f;
      int y = (*itr)->getY();
      float yData = 10.0f * (float)(y) / (float)NUMCELLSY;
      data = yData;
    } break;
    case 3:
      clock = 10.f;
      break;
    default:
      break;
    }
    ++cycles;
    cycles = cycles % SEND_SPEED;
    if (cycles == 0) {
      ++state;
      state = state % 4;
      if (state == 0) {
        itr = std::next(itr);
        if (itr == aliveCells.end()) {
          transferInProgress = false;
        }
      }
    }
  } else {
    clock = 0.f;
    data = 0.f;
  }
}
float DataSender::getClock() { return clock; }
float DataSender::getData() { return data; }
bool DataSender::isTransferInProgress() { return transferInProgress; }

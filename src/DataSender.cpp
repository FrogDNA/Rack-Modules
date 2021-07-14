#include "DataSender.hpp"
#include "GameOfLifeGrid.hpp"
DataSender::DataSender() { transferInProgress = false; }

void DataSender::init(std::set<Cell *> aliveCells) {
  this->aliveCells = aliveCells;
  itr = aliveCells.begin();
  /*if (itr != aliveCells.end()) {
    transferInProgress = true;
    Cell *c = (*itr);
  } else {*/
  transferInProgress = false;
  data = 0.f;
  //}
  this->clock = 0.f;
}

void DataSender::next() {}
float DataSender::getClock() { return clock; }
float DataSender::getData() { return data; }
bool DataSender::isTransferInProgress() { return transferInProgress; }

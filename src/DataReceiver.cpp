#include "DataReceiver.hpp"
#include "GameOfLifeGrid.hpp"

DataReceiver::DataReceiver() {}

bool DataReceiver::hasNewData() { return false; }
Coordinate DataReceiver::getData() { return Coordinate(0, 0); }

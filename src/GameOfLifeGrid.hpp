#ifndef GOLGRID_H
#define GOLGRID_H

#include "Coordinate.hpp"
#include "plugin.hpp"
#include <set>
#include <unordered_map>

static const int NUMCELLSX = 30;
static const int NUMCELLSY = 30;

class Cell : public Coordinate {
  bool alive;

public:
  Cell(int x, int y, bool alive);
  bool isAlive();
  void setAlive(bool isAlive);
};

class GameOfLifeGrid {
  std::unordered_map<int, std::unordered_map<int, Cell *>> allCells;
  std::set<Cell *> watchList;
  std::set<Cell *> currentlyAlive;
  int steps = 0;

  int countAlive(std::set<Cell *> ca, Cell *c);

public:
  GameOfLifeGrid();
  void defaultInit();
  void update();
  std::set<Cell *> getCurrentlyAlive();
  void setCellState(int x, int y, bool state);
  Cell *getCell(int x, int y);
};

#endif

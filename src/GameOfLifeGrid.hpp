#ifndef GOLGRID_H
#define GOLGRID_H

#include "Coordinate.hpp"
#include "plugin.hpp"
#include "robin_hood.h"
#include <set>
#include <unordered_map>

static const int NUMCELLSX = 30;
static const int NUMCELLSY = 30;

class Cell : public Coordinate {
  bool alive;

public:
  bool rowMuted;
  bool colMuted;
  Cell(int x, int y, bool alive);
  bool isAlive();
  void setAlive(bool isAlive);
  bool isAudible();
};

class GameOfLifeGrid {
  robin_hood::unordered_map<int, robin_hood::unordered_map<int, Cell *>>
      allCells;
  std::set<Cell *> watchList;
  std::set<Cell *> currentlyAlive;
  std::set<Cell *> oldAlive;

  int countAlive(std::set<Cell *> ca, Cell *c);

public:
  GameOfLifeGrid();
  void defaultInit();
  void update();
  void emptyGrid();
  std::set<Cell *> getCurrentlyAlive();
  std::vector<Cell *> getCurrentlyAliveV();
  void setCellState(int x, int y, bool state);
  Cell *getCell(int x, int y);
  bool isEmpty();
  bool isStillEvolving(); // return false if old is same as current
};

#endif

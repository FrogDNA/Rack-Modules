#ifndef GOLGRID_H
#define GOLGRID_H

#include "Consts.hpp"
#include "Coordinate.hpp"
#include "plugin.hpp"
#include <set>

class Cell : public Coordinate {
  bool alive;

public:
  Cell(int x, int y, bool alive);
  bool isAlive();
  void setAlive(bool isAlive);
};

class GameOfLifeGrid {
  std::vector<std::vector<Cell *>> allCells;
  std::set<Cell *> oldAlive;
  std::set<Cell *> watchList;
  std::set<Cell *> currentlyAlive;

  int countAlive(std::set<Cell *> ca, Cell *c, bool loop);

public:
  GameOfLifeGrid();
  void init();
  void init(std::vector<Cell *> alive);
  void update(bool loop);
  void emptyGrid();
  std::vector<Cell *> getCurrentlyAlive();
  void setCellState(int x, int y, bool state);
  Cell *getCell(int x, int y);
  bool isEmpty();
  bool isStillEvolving(); // return false if old is same as current
};

#endif

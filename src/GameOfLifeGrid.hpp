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
  bool loop = false;
  std::vector<std::vector<Cell *>> allCells;
  std::vector<std::vector<int>> neighbours;
  std::set<Cell *> oldAlive;
  std::set<Cell *> watchList;
  std::set<Cell *> currentlyAlive;
  void updateNeighboursAndWatchlist(int x, int y, int val);

public:
  GameOfLifeGrid();
  void init();
  void init(std::vector<Cell *> alive);
  void update();
  void emptyGrid();
  std::vector<Cell *> getCurrentlyAlive();
  void setCellState(int x, int y, bool state);
  Cell *getCell(int x, int y);
  void setLoop(bool loop);
  bool isEmpty();
  bool isStillEvolving(); // return false if old is same as current
};

#endif

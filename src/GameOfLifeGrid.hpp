#ifndef GOLGRID_H
#define GOLGRID_H

#include "Consts.hpp"
#include "plugin.hpp"
#include <unordered_set>

class GameOfLifeGrid {
  bool loop = false;
  int numCellsX = NUMCELLS_X;
  int numCellsY = NUMCELLS_Y;
  std::vector<std::vector<std::pair<int, int> *>> allCells;
  std::vector<std::vector<int>> aliveMap;
  std::vector<std::vector<int>> neighbours;
  std::unordered_set<std::pair<int, int> *> oldAlive;
  std::unordered_set<std::pair<int, int> *> watchList;
  std::unordered_set<std::pair<int, int> *> currentlyAlive;
  void updateNeighboursAndWatchlist(int x, int y, int val);

public:
  GameOfLifeGrid();
  void init();
  void init(std::vector<std::pair<int, int> *> alive);
  void update();
  void emptyGrid();
  std::vector<std::pair<int, int> *> getCurrentlyAlive();
  void setCellState(int x, int y, bool state);
  void setCellState(std::pair<int, int> *cell, bool state);
  std::pair<int, int> *getCell(int x, int y);
  void setLoop(bool loop);
  bool getLoop();
  bool isAlive(int x, int y);
  bool isAlive(std::pair<int, int> *);
  bool isInGrid(int x, int y);
  bool isEmpty();
  bool isStillEvolving(); // return false if old is same as current
};

#endif

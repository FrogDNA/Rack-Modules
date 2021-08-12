#ifndef GOLGRID_H
#define GOLGRID_H

#include "Consts.hpp"
#include "plugin.hpp"
#include <unordered_set>

class GameOfLifeGrid {
  static std::vector<std::vector<std::pair<int, int> *>> allCells;
  static std::vector<std::vector<std::pair<int, int> *>> initAllCells();
  bool hasCAChanged = true;
  bool loop = false;
  int numCellsX = NUMCELLS_X;
  int numCellsY = NUMCELLS_Y;
  std::vector<std::vector<int>> aliveMap;
  std::vector<std::vector<int>> neighbours;
  std::unordered_set<std::pair<int, int> *> oldAlive;
  std::unordered_set<std::pair<int, int> *> watchList;
  std::unordered_set<std::pair<int, int> *> currentlyAlive;
  void updateNeighboursAndWatchlist(int x, int y, int val);
  void emptyGrid();

public:
  static std::vector<std::pair<int, int> *> createRandomGrid();
  static std::vector<std::pair<int, int> *> getDefaultInit();
  static std::pair<int, int> *getCell(int x, int y);
  static bool isInGrid(int x, int y);
  GameOfLifeGrid();
  void init(std::vector<std::pair<int, int> *> alive);
  void update();
  std::vector<std::pair<int, int> *> getCurrentlyAlive();
  // quick and dirty trick, to inform if currentlyAlive has changed since last
  // asked
  std::vector<std::pair<int, int> *> getCurrentlyAliveRegister();
  bool isHasCAChanged();
  void setCellState(int x, int y, bool state);
  void setCellState(std::pair<int, int> *cell, bool state);
  void setLoop(bool loop);
  bool getLoop();
  bool isAlive(int x, int y);
  bool isAlive(std::pair<int, int> *);
  bool isEmpty();
  bool isStillEvolving(); // return false if old is same as current
};

#endif

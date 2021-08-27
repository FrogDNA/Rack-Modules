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
  int startX = 0;
  int startY = 0;
  std::vector<std::vector<int>> aliveMap;
  std::vector<std::vector<int>> neighbours;
  int oldAliveLength = 1;
  int oldCounter = 0;
  std::vector<std::unordered_set<std::pair<int, int> *>> oldAlive;
  std::unordered_set<std::pair<int, int> *> watchList;
  std::unordered_set<std::pair<int, int> *> currentlyAlive;
  void updateNeighboursAndWatchlist(int x, int y, int val);
  void emptyGrid();
  void killOutsideLimits();

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
  void setStillEvolvingLength(int value);
  bool isStillEvolving(); // return false if old is same as current
  void setColLimit(int start, int size);
  void setRowLimit(int start, int size);
};

#endif

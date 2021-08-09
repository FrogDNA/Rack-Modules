#include "GameOfLifeGrid.hpp"
#include "Consts.hpp"

GameOfLifeGrid::GameOfLifeGrid() {
  allCells.reserve(NUMCELLS_X);
  neighbours.reserve(NUMCELLS_X);
  aliveMap.reserve(NUMCELLS_X);
  for (int i = 0; i < NUMCELLS_X; i++) {
    std::vector<std::pair<int, int> *> v;
    v.reserve(NUMCELLS_Y);
    std::vector<int> n;
    n.reserve(NUMCELLS_Y);
    std::vector<int> a;
    a.reserve(NUMCELLS_Y);
    for (int j = 0; j < NUMCELLS_Y; j++) {
      v.push_back(new std::pair<int, int>(i, j));
      n.push_back(0);
      a.push_back(0);
    }
    allCells.push_back(v);
    neighbours.push_back(n);
    aliveMap.push_back(a);
  }
}

void GameOfLifeGrid::init() {
  emptyGrid();
  // optional, interesting to test all manner of things
  //(Gosper Glider Gun)
  /*setCellState(REFERENCE_POS - 19, REFERENCE_POS, true);
  setCellState(REFERENCE_POS - 19, REFERENCE_POS + 1, true);
  setCellState(REFERENCE_POS - 18, REFERENCE_POS, true);
  setCellState(REFERENCE_POS - 18, REFERENCE_POS + 1, true);
  setCellState(REFERENCE_POS - 9, REFERENCE_POS, true);
  setCellState(REFERENCE_POS - 9, REFERENCE_POS + 1, true);
  setCellState(REFERENCE_POS - 9, REFERENCE_POS + 2, true);
  setCellState(REFERENCE_POS - 8, REFERENCE_POS - 1, true);
  setCellState(REFERENCE_POS - 8, REFERENCE_POS + 3, true);
  setCellState(REFERENCE_POS - 7, REFERENCE_POS - 2, true);
  setCellState(REFERENCE_POS - 7, REFERENCE_POS + 4, true);
  setCellState(REFERENCE_POS - 6, REFERENCE_POS - 2, true);
  setCellState(REFERENCE_POS - 6, REFERENCE_POS + 4, true);
  setCellState(REFERENCE_POS - 5, REFERENCE_POS + 1, true);
  setCellState(REFERENCE_POS - 4, REFERENCE_POS - 1, true);
  setCellState(REFERENCE_POS - 4, REFERENCE_POS + 3, true);
  setCellState(REFERENCE_POS - 3, REFERENCE_POS, true);
  setCellState(REFERENCE_POS - 3, REFERENCE_POS + 1, true);
  setCellState(REFERENCE_POS - 3, REFERENCE_POS + 2, true);
  setCellState(REFERENCE_POS - 2, REFERENCE_POS + 1, true);
  setCellState(REFERENCE_POS + 1, REFERENCE_POS, true);
  setCellState(REFERENCE_POS + 1, REFERENCE_POS - 1, true);
  setCellState(REFERENCE_POS + 1, REFERENCE_POS - 2, true);
  setCellState(REFERENCE_POS + 2, REFERENCE_POS, true);
  setCellState(REFERENCE_POS + 2, REFERENCE_POS - 1, true);
  setCellState(REFERENCE_POS + 2, REFERENCE_POS - 2, true);
  setCellState(REFERENCE_POS + 3, REFERENCE_POS - 3, true);
  setCellState(REFERENCE_POS + 3, REFERENCE_POS + 1, true);
  setCellState(REFERENCE_POS + 5, REFERENCE_POS - 3, true);
  setCellState(REFERENCE_POS + 5, REFERENCE_POS + 1, true);
  setCellState(REFERENCE_POS + 5, REFERENCE_POS - 4, true);
  setCellState(REFERENCE_POS + 5, REFERENCE_POS + 2, true);
  setCellState(REFERENCE_POS + 15, REFERENCE_POS - 1, true);
  setCellState(REFERENCE_POS + 15, REFERENCE_POS - 2, true);
  setCellState(REFERENCE_POS + 16, REFERENCE_POS - 1, true);
  setCellState(REFERENCE_POS + 16, REFERENCE_POS - 2, true);*/
}

void GameOfLifeGrid::init(std::vector<std::pair<int, int> *> alive) {
  emptyGrid();
  for (std::vector<std::pair<int, int> *>::iterator it = alive.begin();
       it != alive.end(); ++it) {
    std::pair<int, int> *c = *it;
    setCellState(c->first, c->second, true);
  }
}

void GameOfLifeGrid::setCellState(int x, int y, bool state) {
  if (state && !isAlive(x, y)) {
    // becomes alive
    currentlyAlive.insert(allCells[x][y]);
    // into watchlist and add neighbours
    updateNeighboursAndWatchlist(x, y, 1);
  }
  if (!state && isAlive(x, y)) {
    // becomes dead
    currentlyAlive.erase(allCells[x][y]);
    updateNeighboursAndWatchlist(x, y, -1);
  }
  // set cell state
  aliveMap[x][y] = state ? 1 : 0;
  // in all cases insert neighbouring cells into watchlist
}

void GameOfLifeGrid::setCellState(std::pair<int, int> *cell, bool state) {
  setCellState(cell->first, cell->second, state);
}

void GameOfLifeGrid::updateNeighboursAndWatchlist(int x, int y, int val) {
  if (loop) {
    for (int i = x - 1; i < x + 2; i++) {
      int ci = (i + numCellsX) % numCellsX;
      for (int j = y - 1; j < y + 2; j++) {
        int cj = (j + numCellsY) % numCellsY;
        watchList.insert(allCells[ci][cj]);
        if (ci != x || cj != y) {
          neighbours[ci][cj] += val;
        }
      }
    }
  } else {
    for (int i = std::max(x - 1, 0); i < std::min(x + 2, NUMCELLS_X); i++) {
      for (int j = std::max(y - 1, 0); j < std::min(y + 2, NUMCELLS_Y); j++) {
        watchList.insert(allCells[i][j]);
        if (i != x || j != y) {
          neighbours[i][j] += val;
        }
      }
    }
  }
}

void GameOfLifeGrid::update() {
  std::set<std::pair<int, int> *> wl = watchList;
  std::set<std::pair<int, int> *> ca = currentlyAlive;
  std::vector<std::vector<int>> neigh = neighbours;
  oldAlive = currentlyAlive;
  watchList.clear();
  for (std::set<std::pair<int, int> *>::iterator it = wl.begin();
       it != wl.end(); ++it) {
    std::pair<int, int> *c = *it;
    int x = c->first;
    int y = c->second;
    int count = neigh[x][y];
    if (x >= numCellsX || y >= numCellsY) {
      setCellState(c, false);
    }
    if (count == 3) {
      setCellState(c, true);
    } else if (count == 2) {
      setCellState(c, isAlive(x, y));
    } else {
      setCellState(c, false);
    }
  }
}

void GameOfLifeGrid::emptyGrid() {
  std::set<std::pair<int, int> *> ca = currentlyAlive;
  for (std::set<std::pair<int, int> *>::iterator it = ca.begin();
       it != ca.end(); ++it) {
    std::pair<int, int> *c = *it;
    setCellState(c->first, c->second, false);
  }
}

void GameOfLifeGrid::setLoop(bool loop) { this->loop = loop; }

std::pair<int, int> *GameOfLifeGrid::getCell(int x, int y) {
  if (isInGrid(x, y)) {
    return allCells[x][y];
  } else {
    return NULL;
  }
}

std::vector<std::pair<int, int> *> GameOfLifeGrid::getCurrentlyAlive() {
  std::vector<std::pair<int, int> *> ca(currentlyAlive.begin(),
                                        currentlyAlive.end());
  return ca;
}

bool GameOfLifeGrid::isEmpty() { return currentlyAlive.empty(); }

bool GameOfLifeGrid::isStillEvolving() { return !(currentlyAlive == oldAlive); }

bool GameOfLifeGrid::isAlive(int x, int y) { return aliveMap[x][y] == 1; }
bool GameOfLifeGrid::isAlive(std::pair<int, int> *cell) {
  return aliveMap[cell->first][cell->second] == 1;
}
bool GameOfLifeGrid::isInGrid(int x, int y) {
  if (x >= 0 && y >= 0 && x < NUMCELLS_X && y < NUMCELLS_Y) {
    return true;
  }
  return false;
}

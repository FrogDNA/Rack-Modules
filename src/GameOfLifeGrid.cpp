#include "GameOfLifeGrid.hpp"
#include "Consts.hpp"

Cell::Cell(int x, int y, bool alive) : Coordinate(x, y) { this->alive = alive; }

bool Cell::isAlive() { return alive; }

void Cell::setAlive(bool isAlive) { alive = isAlive; }

GameOfLifeGrid::GameOfLifeGrid() {
  allCells.reserve(NUMCELLS_X);
  neighbours.reserve(NUMCELLS_X);
  for (int i = 0; i < NUMCELLS_X; i++) {
    std::vector<Cell *> v;
    v.reserve(NUMCELLS_Y);
    std::vector<int> n;
    n.reserve(NUMCELLS_Y);
    for (int j = 0; j < NUMCELLS_Y; j++) {
      v.push_back(new Cell(i, j, false));
      n.push_back(0);
    }
    allCells.push_back(v);
    neighbours.push_back(n);
  }
}

void GameOfLifeGrid::init() {
  emptyGrid();
  // optional, interesting to test all manner of things
  //(Gosper Glider Gun)
  setCellState(REFERENCE_POS - 19, REFERENCE_POS, true);
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
  setCellState(REFERENCE_POS + 16, REFERENCE_POS - 2, true);
}

void GameOfLifeGrid::init(std::vector<Cell *> alive) {
  emptyGrid();
  for (std::vector<Cell *>::iterator it = alive.begin(); it != alive.end();
       ++it) {
    Cell *c = *it;
    setCellState(c->getX(), c->getY(), true);
  }
}

void GameOfLifeGrid::setCellState(int x, int y, bool state) {
  Cell *c = allCells[x][y];
  if (state && !c->isAlive()) {
    // becomes alive
    currentlyAlive.insert(allCells[x][y]);
    // into watchlist and add neighbours
    updateNeighboursAndWatchlist(x, y, 1);
  }
  if (!state && c->isAlive()) {
    // becomes dead
    currentlyAlive.erase(allCells[x][y]);
    updateNeighboursAndWatchlist(x, y, -1);
  }
  // set cell state
  allCells[x][y]->setAlive(state);
  // in all cases insert neighbouring cells into watchlist
}

void GameOfLifeGrid::updateNeighboursAndWatchlist(int x, int y, int val) {
  if (loop) {
    for (int i = x - 1; i < x + 2; i++) {
      int ci = (i + NUMCELLS_X) % NUMCELLS_X;
      for (int j = y - 1; j < y + 2; j++) {
        int cj = (j + NUMCELLS_Y) % NUMCELLS_Y;
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
  std::set<Cell *> wl = watchList;
  std::set<Cell *> ca = currentlyAlive;
  std::vector<std::vector<int>> neigh = neighbours;
  oldAlive = currentlyAlive;
  watchList.clear();
  for (std::set<Cell *>::iterator it = wl.begin(); it != wl.end(); ++it) {
    Cell *c = *it;
    int count = neigh[c->getX()][c->getY()];
    if (count == 3) {
      setCellState(c->getX(), c->getY(), true);
    } else if (count == 2) {
      setCellState(c->getX(), c->getY(), c->isAlive());
    } else {
      setCellState(c->getX(), c->getY(), false);
    }
  }
}

void GameOfLifeGrid::emptyGrid() {
  std::set<Cell *> ca = currentlyAlive;
  for (std::set<Cell *>::iterator it = ca.begin(); it != ca.end(); ++it) {
    Cell *c = *it;
    setCellState(c->getX(), c->getY(), false);
  }
}

void GameOfLifeGrid::setLoop(bool loop) { this->loop = loop; }

Cell *GameOfLifeGrid::getCell(int x, int y) {
  if (x >= 0 && x < NUMCELLS_X && y >= 0 && y < NUMCELLS_Y) {
    return allCells[x][y];
  } else {
    return NULL;
  }
}

std::vector<Cell *> GameOfLifeGrid::getCurrentlyAlive() {
  std::vector<Cell *> ca(currentlyAlive.begin(), currentlyAlive.end());
  return ca;
}

bool GameOfLifeGrid::isEmpty() { return currentlyAlive.empty(); }

bool GameOfLifeGrid::isStillEvolving() { return !(currentlyAlive == oldAlive); }

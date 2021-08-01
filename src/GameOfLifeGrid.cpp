#include "GameOfLifeGrid.hpp"
#include "Consts.hpp"
#include <algorithm>
#include <cstdio>

Cell::Cell(int x, int y, bool alive) : Coordinate(x, y) { this->alive = alive; }

bool Cell::isAlive() { return alive; }

void Cell::setAlive(bool isAlive) { alive = isAlive; }

GameOfLifeGrid::GameOfLifeGrid() {
  for (int i = 0; i < NUMCELLS_X; i++) {
    for (int j = 0; j < NUMCELLS_Y; j++) {
      allCells[i][j] = new Cell(i, j, false);
    }
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
  if (state) {
    // becomes alive
    currentlyAlive.insert(allCells[x][y]);
    added.insert(allCells[x][y]);
    // we could check if it was previously in removed, but the logic will be
    // different to save time
  } else {
    // becomes dead
    currentlyAlive.erase(allCells[x][y]);
    removed.insert(allCells[x][y]);
  }
  // set cell state
  allCells[x][y]->setAlive(state);
  // in all cases insert neighbouring cells into watchlist
  for (int i = x - 1; i < x + 2; i++) {
    int ci = (i + NUMCELLS_X) % NUMCELLS_X;
    for (int j = y - 1; j < y + 2; j++) {
      int cj = (j + NUMCELLS_Y) % NUMCELLS_Y;
      watchList.insert(allCells[ci][cj]);
    }
  }
}

void GameOfLifeGrid::update(bool loop) {
  std::set<Cell *> wl = watchList;
  std::set<Cell *> ca = currentlyAlive;
  oldAlive = currentlyAlive;
  watchList.clear();
  for (std::set<Cell *>::iterator it = wl.begin(); it != wl.end(); ++it) {
    Cell *c = *it;
    int count = countAlive(ca, c, loop);
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

int GameOfLifeGrid::countAlive(std::set<Cell *> ca, Cell *c, bool loop) {
  int count = 0;
  int x = c->getX();
  int y = c->getY();
  if (loop) {
    for (int i = x - 1; i < x + 2; i++) {
      int ci = (i + NUMCELLS_X) % NUMCELLS_X;
      for (int j = y - 1; j < y + 2; j++) {
        int cj = (j + NUMCELLS_Y) % NUMCELLS_Y;
        if (i != x || j != y) {
          bool is_in = ca.find(allCells[ci][cj]) != ca.end();
          if (is_in) {
            ++count;
          }
        }
      }
    }
  } else {
    for (int i = std::max(x - 1, 0); i < std::min(x + 2, NUMCELLS_X); i++) {
      for (int j = std::max(y - 1, 0); j < std::min(y + 2, NUMCELLS_Y); j++) {
        if (i != x || j != y) {
          bool is_in = ca.find(allCells[i][j]) != ca.end();
          if (is_in) {
            ++count;
          }
        }
      }
    }
  }
  return count;
}

Cell *GameOfLifeGrid::getCell(int x, int y) { return allCells[x][y]; }

GridState GameOfLifeGrid::getCurrentlyAlive() {
  std::vector<Cell *> v(currentlyAlive.begin(), currentlyAlive.end());
  std::vector<Cell *> a(added.begin(), added.end());
  std::vector<Cell *> r(removed.begin(), removed.end());
  return GridState(v, a, r);
}

void GameOfLifeGrid::resetModified() {
  added.clear();
  removed.clear();
}

bool GameOfLifeGrid::isEmpty() { return currentlyAlive.empty(); }

bool GameOfLifeGrid::isStillEvolving() { return !(currentlyAlive == oldAlive); }

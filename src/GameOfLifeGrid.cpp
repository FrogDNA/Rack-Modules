#include "GameOfLifeGrid.hpp"
#include <algorithm>
#include <cstdio>

Cell::Cell(int x, int y, bool alive) : Coordinate(x, y) {
  this->alive = alive;
  rowMuted = false;
  colMuted = false;
}

bool Cell::isAlive() { return alive; }

void Cell::setAlive(bool isAlive) { alive = isAlive; }

bool Cell::isAudible() { return !(rowMuted || colMuted); }

GameOfLifeGrid::GameOfLifeGrid() {
  for (int i = 0; i < NUMCELLSX; i++) {
    for (int j = 0; j < NUMCELLSY; j++) {
      allCells[i][j] = new Cell(i, j, false);
    }
  }
}

void GameOfLifeGrid::defaultInit() {
  for (int i = 0; i < NUMCELLSX; i++) {
    for (int j = 0; j < NUMCELLSY; j++) {
      allCells[i][j]->setAlive(false);
    }
  }
  // optional
  setCellState(NUMCELLSX / 2, NUMCELLSY / 2, true);
  setCellState(NUMCELLSX / 2 - 1, NUMCELLSY / 2, true);
  setCellState(NUMCELLSX / 2 + 1, NUMCELLSY / 2, true);
  setCellState(NUMCELLSX / 2 - 1, NUMCELLSY / 2 - 1, true);
  setCellState(NUMCELLSX / 2, NUMCELLSY / 2 + 1, true);
  setCellState(NUMCELLSX / 2 + 1, NUMCELLSY / 2 - 1, true);
  setCellState(NUMCELLSX / 2 + 2, NUMCELLSY / 2 - 1, true);
}

void GameOfLifeGrid::setCellState(int x, int y, bool state) {
  if (state) {
    // becomes alive
    currentlyAlive.insert(allCells[x][y]);
  } else {
    // becomes dead
    currentlyAlive.erase(allCells[x][y]);
  }
  // set cell state
  allCells[x][y]->setAlive(state);
  // in all cases insert neighbouring cells into watchlist
  for (int i = x - 1; i < x + 2; i++) {
    int ci = (i + NUMCELLSX) % NUMCELLSX;
    for (int j = y - 1; j < y + 2; j++) {
      int cj = (j + NUMCELLSY) % NUMCELLSY;
      watchList.insert(allCells[ci][cj]);
    }
  }
}

void GameOfLifeGrid::update() {
  std::set<Cell *> wl = watchList;
  std::set<Cell *> ca = currentlyAlive;
  oldAlive = currentlyAlive;
  watchList.clear();
  for (std::set<Cell *>::iterator it = wl.begin(); it != wl.end(); ++it) {
    Cell *c = *it;
    int count = countAlive(ca, c);
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

int GameOfLifeGrid::countAlive(std::set<Cell *> ca, Cell *c) {
  int count = 0;
  int x = c->getX();
  int y = c->getY();
  for (int i = x - 1; i < x + 2; i++) {
    int ci = (i + NUMCELLSX) % NUMCELLSX;
    for (int j = y - 1; j < y + 2; j++) {
      int cj = (j + NUMCELLSY) % NUMCELLSY;
      if (i != x || j != y) {
        bool is_in = ca.find(allCells[ci][cj]) != ca.end();
        if (is_in) {
          ++count;
        }
      }
    }
  }
  return count;
}

Cell *GameOfLifeGrid::getCell(int x, int y) { return allCells[x][y]; }

std::set<Cell *> GameOfLifeGrid::getCurrentlyAlive() { return currentlyAlive; }

std::vector<Cell *> GameOfLifeGrid::getCurrentlyAliveV() {
  std::vector<Cell *> v(currentlyAlive.begin(), currentlyAlive.end());
  return v;
}

bool GameOfLifeGrid::isEmpty() { return currentlyAlive.empty(); }

bool GameOfLifeGrid::isStillEvolving() { return !(currentlyAlive == oldAlive); }

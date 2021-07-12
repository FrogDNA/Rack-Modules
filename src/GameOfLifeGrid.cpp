#include "GameOfLifeGrid.hpp"
#include <algorithm>
#include <cstdio>

Cell::Cell(int x, int y, bool alive) : Coordinate(x, y) { this->alive = alive; }

bool Cell::isAlive() { return alive; }

void Cell::setAlive(bool isAlive) { alive = isAlive; }

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
  setCellState(NUMCELLSX / 2, NUMCELLSY / 2 + 2, true);
  setCellState(NUMCELLSX / 2 + 1, NUMCELLSY / 2 + 1, true);
}

void GameOfLifeGrid::setCellState(int x, int y, bool state) {
  if (state != allCells[x][y]->isAlive()) {
    // do something because cell changes state
    if (state) {
      // currently dead
      currentlyAlive.insert(allCells[x][y]);
    } else {
      // currently living
      currentlyAlive.erase(allCells[x][y]);
    }
    // set cell state
    allCells[x][y]->setAlive(state);
    // in all cases insert neighbouring cells into watchlist
    for (int i = x - 1; i < x + 2; i++) {
      int ci = (i + NUMCELLSX) % NUMCELLSX;
      for (int j = y - 1; j < y + 2; j++) {
        int cj = (j + NUMCELLSY) % NUMCELLSY;
        if (i != j) {
          watchList.insert(allCells[ci][cj]);
        }
      }
    }
  }
}

void GameOfLifeGrid::update() {
  ++steps;
  for (std::set<Cell *>::iterator it = watchList.begin(); it != watchList.end();
       ++it) {
    Cell *c = *it;
    int count = countAlive(c);
    if (count == 3) {
      setCellState(c->getX(), c->getY(), true);
    } else if (count != 2) {
      setCellState(c->getX(), c->getY(), false);
    } // case 2 do nothing
  }
}

int GameOfLifeGrid::countAlive(Cell *c) {
  int count = 0;
  int x = c->getX();
  int y = c->getY();
  for (int i = x - 1; i < x + 2; i++) {
    int ci = (i + NUMCELLSX) % NUMCELLSX;
    for (int j = y - 1; j < y + 2; j++) {
      int cj = (j + NUMCELLSY) % NUMCELLSY;
      if (i != j && allCells[ci][cj]->isAlive()) {
        ++count;
      }
    }
  }
  return count;
}

Cell *GameOfLifeGrid::getCell(int x, int y) { return allCells[x][y]; }

std::set<Cell *> GameOfLifeGrid::getCurrentlyAlive() { return currentlyAlive; }

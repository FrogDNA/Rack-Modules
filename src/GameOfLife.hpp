#include "GameOfLifeGrid.hpp"
#include "plugin.hpp"

#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H

// todo inherit from cell also
struct DrawableCell : Widget {
  Cell *cell;
  DrawableCell(Cell *cell);
  void draw(const DrawArgs &args) override;
  void onButton(const event::Button &e) override;
};

#endif

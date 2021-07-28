#ifndef GOLDISPLAY_H
#define GOLDISPLAY_H

#include "GameOfLifeGrid.hpp"
#include "Mitosis.hpp"
#include "plugin.hpp"

struct DrawableCell : Widget {
  Cell *cell;
  DrawableCell(Cell *cell);
  void draw(const DrawArgs &args) override;
  void onButton(const event::Button &e) override;
};

struct LineHeader : Widget {
  int coordinate;
  bool isRow;
  bool isActivated;
  LineHeader(int coordinate, bool isLine, bool activate);
  void draw(const DrawArgs &args) override;
  void onButton(const event::Button &e) override;
};

struct GolDisplay : OpaqueWidget {
  Mitosis *module = NULL;
  float offsetX = 0.f;
  float offsetY = 0.f;
  float sizeX = 0.f;
  float sizeY = 0.f;
  float cellSizeX = 0.f;
  float cellSizeY = 0.f;
  float cellSpaceX = 0.f;
  float cellSpaceY = 0.f;
  float numCellsX = 0.f;
  float numCellsY = 0.f;
  bool firstDraw = true;

  GolDisplay();
  void draw(const DrawArgs &args) override;
};

#endif

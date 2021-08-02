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
  dsp::RingBuffer<int, 64> *rb;
  LineHeader(int coordinate, bool isLine);
  void draw(const DrawArgs &args) override;
  void onButton(const event::Button &e) override;
};

struct GolDisplay : OpaqueWidget {
  Mitosis *module = NULL;
  float sizeX = 0.f;
  float sizeY = 0.f;
  float cellSizeX = 0.f;
  float cellSizeY = 0.f;
  float cellSpaceX = 0.f;
  float cellSpaceY = 0.f;
  float numCellsX = 0.f;
  float numCellsY = 0.f;
  bool firstDraw = true;
  // first x displayed
  int display_x0 = CENTER_DISPLAY_X - NUMCELLS_DISPLAY_X / 2;
  // first y displayed
  int display_y0 = CENTER_DISPLAY_Y - NUMCELLS_DISPLAY_Y / 2;

  GolDisplay();
  void draw(const DrawArgs &args) override;
};

#endif

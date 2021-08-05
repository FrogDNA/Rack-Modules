#ifndef GOLDISPLAY_H
#define GOLDISPLAY_H

#include "GameOfLifeGrid.hpp"
#include "Mitosis.hpp"
#include "plugin.hpp"

// display at default zoom level
static const int NUMCELLS_DISPLAY_X = 40;
static const int NUMCELLS_DISPLAY_Y = 40;
// may be the same as REFERENCE_POS
static const int CENTER_DISPLAY_X = 20;
static const int CENTER_DISPLAY_Y = 20;

// proportion of the cell size between cell
static const float CELL_PADDING = 0.2f;
// zoom and scroll line params in mm
// recommendation is a strict min of 7
// with a padding of 2
static const float ICON_SIZE = 10.0f;
static const float ICON_PADDING = 2.0f;

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

struct GridDisplay : OpaqueWidget {
  Mitosis *module = NULL;
  float sizeX = 0.f;
  float sizeY = 0.f;
  float cellSizeX = 0.f;
  float cellSizeY = 0.f;
  float cellSpaceX = 0.f;
  float cellSpaceY = 0.f;
  int spotsX = NUMCELLS_DISPLAY_X;
  int spotsY = NUMCELLS_DISPLAY_Y;
  bool firstDraw = true;
  // first x displayed
  int display_x0 = CENTER_DISPLAY_X - NUMCELLS_DISPLAY_X / 2;
  // first y displayed
  int display_y0 = CENTER_DISPLAY_Y - NUMCELLS_DISPLAY_Y / 2;

  GridDisplay();
  void draw(const DrawArgs &args) override;
  void changeZoomLevel(int zoomChange);
};

struct GridScrollBar : Widget {
  bool vertical = false;
  void draw(const DrawArgs &args) override;
};

struct ZoomButton : Widget {
  bool zoomPlus = false;
  void draw(const DrawArgs &args) override;
  void onButton(const event::Button &e) override;
};

struct CenterButton : Widget {
  void draw(const DrawArgs &args) override;
};

struct GoLDisplay : OpaqueWidget {
  GridDisplay *gridDisplay = NULL;
  Mitosis *module = NULL;
  bool firstDraw = true;
  GoLDisplay();
  void draw(const DrawArgs &args) override;
  void zoom(bool zoomIn);
};

#endif

#ifndef GOLDISPLAY_H
#define GOLDISPLAY_H

#include "GameOfLifeGrid.hpp"
#include "Mitosis.hpp"
#include "plugin.hpp"

// display at default zoom level
static const int DEFAULT_CELLS_DISPLAYED_X = 40;
static const int DEFAULT_CELLS_DISPLAYED_Y = 40;

// proportion of the cell size between cell
static const float CELL_PADDING = 0.2f;
// zoom and scroll line params in mm
// recommendation is a strict min of 7
// with a padding of 2
static const float ICON_SIZE = 10.0f;
static const float ICON_PADDING = 2.0f;

// number of frames between two zooms
static const int FRAMES_BETWEEN_ZOOM = 20;
static const int ZOOMS_BEFORE_SPEED_INCREASE = 1;

// minimum per dimension
// real min cells on screen is 10 * 10
static const int MIN_CELLS_ON_SCREEN = 10;

struct CellSpot : Widget {
  Cell *cell = NULL;
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
  int spotsX = DEFAULT_CELLS_DISPLAYED_X;
  int spotsY = DEFAULT_CELLS_DISPLAYED_Y;
  bool firstDraw = true;
  // first x displayed
  int display_x0 = (NUMCELLS_X - DEFAULT_CELLS_DISPLAYED_X) / 2;
  // first y displayed
  int display_y0 = (NUMCELLS_Y - DEFAULT_CELLS_DISPLAYED_Y) / 2;

  GridDisplay();
  void draw(const DrawArgs &args) override;
  void changeZoomLevel(int zoomChange);
};

struct GridScrollBar : Widget {
  bool vertical = false;
  void draw(const DrawArgs &args) override;
};

struct ZoomButton : Widget {
  int zoomSpeed = 1;
  int zoomFramesCount = 0;
  int zoomAccelerationFramesCount = 0;
  bool zoomPlus = false;
  void doZoom();
  void draw(const DrawArgs &args) override;
  void onButton(const event::Button &e) override;
  void onDragHover(const event::DragHover &e) override;
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
  void zoom(int zoomQuantity);
};

#endif

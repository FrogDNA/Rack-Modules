#pragma once

#include "GameOfLifeGrid.hpp"
#include "InterfaceButton.hpp"
#include "Mitosis.hpp"
#include "plugin.hpp"

// display at default zoom level
static const int DEFAULT_CELLS_DISPLAYED_X = 40;
static const int DEFAULT_CELLS_DISPLAYED_Y = 40;
static const int DEFAULT_CENTER = 54;
// proportion of the cell size between cell
static const float CELL_PADDING = 0.2f;
// zoom and scroll line params in mm
// recommendation is a strict min of 7
// with a padding of 2
static const float ICON_SIZE = 7.0f;
static const float ICON_PADDING = 2.0f;

// number of frames between two zooms
static const int FRAMES_BETWEEN_ZOOM = 20;
static const int ZOOMS_BEFORE_SPEED_INCREASE = 2;

// number of frames between two scrolls
static const int FRAMES_BETWEEN_SCROLL = 20;
static const int SCROLLS_BEFORE_SPEED_INCREASE = 2;

// minimum per dimension
// real min cells on screen is 10 * 10
static const int MIN_CELLS_ON_SCREEN = 10;

struct CellSpot : Widget {
  std::pair<int, int> *cell = NULL;
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
  void scroll(int scrollQuantity, bool vertical);
  void resetView();
};

struct GridScrollButton : InterfaceButton {
  int orientation = 1; // 1 or -1
  int scrollSpeed = 1;
  int scrollFramesCount = 0;
  int scrollAccelerationFramesCount = 0;
  void doScroll();
  void buttonReleased() override;
  void whileHovering() override;
};

struct GridScrollBar : Widget {
  float barSize = mm2px(0.5f);
  float r = (mm2px(ICON_SIZE) - 2 * barSize) / 2;
  float pos = 0.5f;
  bool vertical = false;
  GridDisplay *gd = NULL;
  GridScrollBar();
  void draw(const DrawArgs &args) override;
  void onDragHover(const event::DragHover &e) override;
};

struct GridScrollPane : OpaqueWidget {
  bool firstDraw = true;
  GridScrollButton *buttonPlus = NULL;
  GridScrollButton *buttonMinus = NULL;
  bool vertical = false;
  void draw(const DrawArgs &args) override;
};

struct ZoomButton : InterfaceButton {
  int zoomSpeed = 1;
  int zoomFramesCount = 0;
  int zoomAccelerationFramesCount = 0;
  int baseZoom = 1;
  void buttonReleased() override;
  void whileHovering() override;
};

struct CenterButton : InterfaceButton {
  void buttonReleased() override;
  void whileHovering() override;
};

struct GoLDisplay : OpaqueWidget {
  GridDisplay *gridDisplay = NULL;
  Mitosis *module = NULL;
  bool firstDraw = true;
  GoLDisplay();
  void draw(const DrawArgs &args) override;
};

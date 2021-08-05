#include "GoLDisplay.hpp"
#include "Consts.hpp"
#include "Mitosis.hpp"

GridDisplay::GridDisplay() {
  // todo replace with static something
  firstDraw = true;
}

void GridDisplay::draw(const DrawArgs &args) {
  if (module && firstDraw) {
    float numSquareX = (float)(spotsX + 1); // take into account lineheaders
    float numSquareY = (float)(spotsY + 1); // take into account lineheaders
    float numCellsX = (float)std::min(spotsX, NUMCELLS_X);
    float numCellsY = (float)std::min(spotsY, NUMCELLS_Y);
    sizeX = box.size.x;
    sizeY = box.size.y;

    cellSizeX = sizeX / (numSquareX + CELL_PADDING * (numSquareX - 1));
    cellSizeY = sizeY / (numSquareY + CELL_PADDING * (numSquareY - 1));
    cellSpaceX = cellSizeX * CELL_PADDING;
    cellSpaceY = cellSizeY * CELL_PADDING;
    firstDraw = false;
    // define column headers
    for (int i = 0; i < numCellsX; i++) {
      LineHeader *lh = new LineHeader(i + display_x0, false);
      lh->box.pos = Vec((i + 1) * (cellSizeX + cellSpaceX), 0);
      lh->box.size = Vec(cellSizeX, cellSizeY);
      lh->rb = &(module->muteUnmuteColsBuffer);
      addChild(lh);
    }
    // define row headers
    for (int i = 0; i < numCellsY; i++) {
      LineHeader *lh = new LineHeader(i + display_y0, true);
      lh->box.pos = Vec(0, (i + 1) * (cellSizeY + cellSpaceY));
      lh->box.size = Vec(cellSizeX, cellSizeY);
      lh->rb = &(module->muteUnmuteRowsBuffer);
      addChild(lh);
    }
    // define visible cells. todo change to a simpler way
    for (int i = 0; i < NUMCELLS_DISPLAY_X; i++) {
      for (int j = 0; j < NUMCELLS_DISPLAY_Y; j++) {
        Cell *c = module->golGrid->getCell(i + display_x0, j + display_y0);
        CellSpot *spot = new CellSpot();
        spot->box.pos = Vec((i + 1) * (cellSizeX + cellSpaceX),
                            (j + 1) * (cellSizeY + cellSpaceY));
        spot->box.size = Vec(cellSizeX, cellSizeY);
        spot->cell = c;
        addChild(spot);
      }
    }
    // define zoom bars and buttons.TODO
  }
  OpaqueWidget::draw(args);
}

void GridDisplay::changeZoomLevel(int zoomChange) {
  spotsX = std::max(MIN_CELLS_ON_SCREEN,
                    std::min(spotsX + zoomChange, MAX_CELLS_ON_SCREEN));
  spotsY = std::max(MIN_CELLS_ON_SCREEN,
                    std::min(spotsY + zoomChange, MAX_CELLS_ON_SCREEN));
  display_x0 = CENTER_DISPLAY_X - spotsX / 2;
  display_y0 = CENTER_DISPLAY_Y - spotsY / 2;
  clearChildren();
  firstDraw = true;
}

/// LINEHEADER ///

LineHeader::LineHeader(int coordinate, bool isLine) {
  this->coordinate = coordinate;
  this->isRow = isLine;
}

void LineHeader::draw(const DrawArgs &args) {
  DSP *dsp = getAncestorOfType<GridDisplay>()->module->dsp;
  bool muted = (isRow && !dsp->isRowAudible(coordinate)) ||
               (!isRow && !dsp->isColAudible(coordinate));
  if (muted) {
    nvgFillColor(args.vg, nvgRGBA(0x2e, 0x2e, 0x2e, 0xff));
  } else {
    nvgFillColor(args.vg, nvgRGBA(0x6e, 0x6e, 0x6e, 0xff));
  }
  nvgBeginPath(args.vg);
  nvgRect(args.vg, 0, 0, this->box.size.x, this->box.size.y);
  nvgFill(args.vg);
}

void LineHeader::onButton(const event::Button &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
    if (!rb->full()) {
      rb->push(this->coordinate);
    }
    e.consume(this);
  }
  Widget::onButton(e);
}

void CellSpot::draw(const DrawArgs &args) {
  if (cell) {
    DSP *dsp = getAncestorOfType<GridDisplay>()->module->dsp;
    if (cell->isAlive() && dsp->isCellAudible(cell)) {
      nvgFillColor(args.vg, nvgRGBA(0x11, 0x11, 0x11, 0xff));
    } else if (cell->isAlive()) {
      nvgFillColor(args.vg, nvgRGBA(0x6e, 0x6e, 0x6e, 0xff));
    } else if (dsp->isCellAudible(cell)) {
      nvgFillColor(args.vg, nvgRGBA(0xdd, 0xdd, 0xdd, 0xff));
    } else {
      nvgFillColor(args.vg, nvgRGBA(0xc2, 0xc2, 0xc2, 0xff));
    }
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0, 0, this->box.size.x, this->box.size.y);
    nvgFill(args.vg);
  }
}

void CellSpot::onButton(const event::Button &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
    GridDisplay *gd = getAncestorOfType<GridDisplay>();
    // thread safe
    if (!gd->module->clickedCells.full()) {
      gd->module->clickedCells.push(new Coordinate(cell->getX(), cell->getY()));
    }
    e.consume(this);
  }
  Widget::onButton(e);
}

/// SCROLLBAR ///

void GridScrollBar::draw(const DrawArgs &args) {
  if (vertical) {
    nvgFillColor(args.vg, nvgRGBA(0x00, 0xff, 0x00, 0xff));
  } else {
    nvgFillColor(args.vg, nvgRGBA(0x00, 0x00, 0xff, 0xff));
  }
  nvgBeginPath(args.vg);
  nvgRect(args.vg, 0, 0, this->box.size.x, this->box.size.y);
  nvgFill(args.vg);
}

/// CENTER BUTTON ///

void CenterButton::draw(const DrawArgs &args) {
  nvgFillColor(args.vg, nvgRGBA(0xff, 0x00, 0x00, 0xff));
  nvgBeginPath(args.vg);
  nvgRect(args.vg, 0, 0, this->box.size.x, this->box.size.y);
  nvgFill(args.vg);
}

/// ZOOM BUTTON ///

void ZoomButton::draw(const DrawArgs &args) {
  if (zoomPlus) {
    nvgFillColor(args.vg, nvgRGBA(0x00, 0xff, 0x00, 0xff));
  } else {
    nvgFillColor(args.vg, nvgRGBA(0x00, 0x00, 0xff, 0xff));
  }
  nvgBeginPath(args.vg);
  nvgRect(args.vg, 0, 0, this->box.size.x, this->box.size.y);
  nvgFill(args.vg);
}

void ZoomButton::onButton(const event::Button &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
    if (e.action == GLFW_PRESS) {
      doZoom();
    } else if (e.action == GLFW_RELEASE) {
      zoomSpeed = 1;
      zoomFramesCount = 0;
      zoomAccelerationFramesCount = 0;
    }
    e.consume(this);
    Widget::onButton(e);
  }
}

void ZoomButton::doZoom() {
  GoLDisplay *gd = getAncestorOfType<GoLDisplay>();
  int mult = zoomPlus ? 1 : -1;
  gd->zoom(zoomSpeed * mult);
}

void ZoomButton::onDragHover(const event::DragHover &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
    if (zoomFramesCount == FRAMES_BETWEEN_ZOOM) {
      zoomFramesCount = 0;
      doZoom();
      e.consume(this);
      zoomAccelerationFramesCount++;
      if (zoomAccelerationFramesCount == ZOOMS_BEFORE_SPEED_INCREASE) {
        zoomSpeed = zoomSpeed * 2;
        zoomAccelerationFramesCount = 0;
      }
    }
    zoomFramesCount++;
  }
  Widget::onDragHover(e);
}

GoLDisplay::GoLDisplay() { firstDraw = true; }

void GoLDisplay::zoom(int zoomQuantity) {
  gridDisplay->changeZoomLevel(zoomQuantity);
}

void GoLDisplay::draw(const DrawArgs &args) {
  if (firstDraw && module) {
    float iconSizePx = mm2px(ICON_SIZE);
    float iconPaddingPx = mm2px(ICON_PADDING);
    float gridSizeX = this->box.size.x - iconSizePx - iconPaddingPx;
    float gridSizeY = this->box.size.y - iconSizePx - iconPaddingPx;
    // grid display
    gridDisplay = new GridDisplay();
    gridDisplay->module = module;
    gridDisplay->box.pos = Vec(0.f, 0.f);
    gridDisplay->box.size = Vec(gridSizeX, gridSizeY);
    addChild(gridDisplay);
    // vertical scrollBar
    GridScrollBar *vScrollBar = new GridScrollBar();
    vScrollBar->vertical = true;
    vScrollBar->box.size =
        Vec(iconSizePx, gridSizeY - (iconSizePx + iconPaddingPx));
    vScrollBar->box.pos = Vec(gridSizeX + iconPaddingPx, 0.f);
    addChild(vScrollBar);
    // horizontal scrollBar
    GridScrollBar *hScrollBar = new GridScrollBar();
    hScrollBar->vertical = false;
    hScrollBar->box.size =
        Vec(gridSizeX - (iconSizePx + iconPaddingPx), iconSizePx);
    hScrollBar->box.pos = Vec(0.f, gridSizeY + iconPaddingPx);
    addChild(hScrollBar);
    // zoom button +
    ZoomButton *zoomPlus = new ZoomButton();
    zoomPlus->zoomPlus = true;
    zoomPlus->box.size = Vec(iconSizePx, iconSizePx);
    zoomPlus->box.pos = Vec(gridSizeX + iconPaddingPx, gridSizeY - iconSizePx);
    addChild(zoomPlus);
    // zoom button -
    ZoomButton *zoomMinus = new ZoomButton();
    zoomMinus->zoomPlus = false;
    zoomMinus->box.size = Vec(iconSizePx, iconSizePx);
    zoomMinus->box.pos = Vec(gridSizeX - iconSizePx, gridSizeY + iconPaddingPx);
    addChild(zoomMinus);
    // center grid button (or center zoom ? At least extra button)
    CenterButton *centerButton = new CenterButton();
    centerButton->box.size = Vec(iconSizePx, iconSizePx);
    centerButton->box.pos =
        Vec(gridSizeX + iconPaddingPx, gridSizeY + iconPaddingPx);
    addChild(centerButton);
    firstDraw = false;
  }
  OpaqueWidget::draw(args);
}

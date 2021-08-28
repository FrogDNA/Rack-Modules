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
    sizeX = box.size.x;
    sizeY = box.size.y;
    cellSizeX = sizeX / (numSquareX + CELL_PADDING * (numSquareX - 1));
    cellSizeY = sizeY / (numSquareY + CELL_PADDING * (numSquareY - 1));
    cellSpaceX = cellSizeX * CELL_PADDING;
    cellSpaceY = cellSizeY * CELL_PADDING;
    firstDraw = false;
    // define column headers
    for (int i = 0; i < spotsX; i++) {
      LineHeader *lh = new LineHeader(i + display_x0, false);
      lh->box.pos = Vec((i + 1) * (cellSizeX + cellSpaceX), 0);
      lh->box.size = Vec(cellSizeX, cellSizeY);
      lh->rb = &(module->muteUnmuteColsBuffer);
      addChild(lh);
    }
    // define row headers
    for (int i = 0; i < spotsY; i++) {
      LineHeader *lh = new LineHeader(i + display_y0, true);
      lh->box.pos = Vec(0, (i + 1) * (cellSizeY + cellSpaceY));
      lh->box.size = Vec(cellSizeX, cellSizeY);
      lh->rb = &(module->muteUnmuteRowsBuffer);
      addChild(lh);
    }
    // define visible cells.
    for (int i = 0; i < spotsX; i++) {
      for (int j = 0; j < spotsY; j++) {
        std::pair<int, int> *c =
            module->golGrid->getCell(i + display_x0, j + display_y0);
        CellSpot *spot = new CellSpot();
        spot->box.pos = Vec((i + 1) * (cellSizeX + cellSpaceX),
                            (j + 1) * (cellSizeY + cellSpaceY));
        spot->box.size = Vec(cellSizeX, cellSizeY);
        spot->cell = c;
        addChild(spot);
      }
    }
  }
  OpaqueWidget::draw(args);
}

void GridDisplay::changeZoomLevel(int zoomChange) {
  int oldSx = spotsX;
  int oldSy = spotsY;
  spotsX =
      std::max(MIN_CELLS_ON_SCREEN, std::min(spotsX + zoomChange, NUMCELLS_X));
  spotsY =
      std::max(MIN_CELLS_ON_SCREEN, std::min(spotsY + zoomChange, NUMCELLS_Y));
  scroll((oldSx - spotsX) / 2, false);
  scroll((oldSy - spotsY) / 2, true);
  viewUpdated();
  clearChildren();
  firstDraw = true;
}

void GridDisplay::scroll(int scrollQuantity, bool vertical) {
  if (vertical) {
    display_y0 =
        std::max(0, std::min(display_y0 + scrollQuantity, NUMCELLS_Y - spotsY));
  } else {
    display_x0 =
        std::max(0, std::min(display_x0 + scrollQuantity, NUMCELLS_X - spotsX));
  }
  viewUpdated();
  clearChildren();
  firstDraw = true;
}

void GridDisplay::resetView() {
  setView((NUMCELLS_X - DEFAULT_CELLS_DISPLAYED_X) / 2,
          (NUMCELLS_Y - DEFAULT_CELLS_DISPLAYED_Y) / 2,
          DEFAULT_CELLS_DISPLAYED_X, DEFAULT_CELLS_DISPLAYED_Y);
}

void GridDisplay::setView(int startX, int startY, int sizeX, int sizeY) {
  spotsX = std::max(0, std::min(sizeX, NUMCELLS_X));
  spotsY = std::max(0, std::min(sizeY, NUMCELLS_Y));
  display_x0 = std::max(0, std::min(startX, NUMCELLS_X - spotsX));
  display_y0 = std::max(0, std::min(startY, NUMCELLS_Y - spotsY));
  viewUpdated();
  clearChildren();
  firstDraw = true;
}

void GridDisplay::viewUpdated() {
  module->colStartBuffer.push(display_x0);
  module->rowStartBuffer.push(display_y0);
  module->colSizeBuffer.push(spotsX);
  module->rowSizeBuffer.push(spotsY);
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
  if (isRow && muted) {
    nvgFillColor(args.vg, OPAQUE_C3_DARKER);
  } else if (isRow && !muted) {
    nvgFillColor(args.vg, OPAQUE_C3);
  } else if (!isRow && muted) {
    nvgFillColor(args.vg, OPAQUE_C2_DARKER);
  } else {
    //(!isRow && !muted)
    nvgFillColor(args.vg, OPAQUE_C2);
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
    Mitosis *module = getAncestorOfType<GridDisplay>()->module;
    DSP *dsp = module->dsp;
    GameOfLifeGrid *grid = module->golGrid;
    if (grid->isAlive(cell)) {
      nvgFillColor(args.vg, OPAQUE_C1_DARK);
    } else {
      nvgFillColor(args.vg, OPAQUE_C1_LIGHT);
    }
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0, 0, this->box.size.x, this->box.size.y);
    nvgFill(args.vg);
    if (!dsp->isColAudible(cell->first)) {
      nvgFillColor(args.vg, TPT_C2);
      nvgBeginPath(args.vg);
      nvgRect(args.vg, 0, 0, this->box.size.x, this->box.size.y);
      nvgFill(args.vg);
    }
    if (!dsp->isRowAudible(cell->second)) {
      nvgFillColor(args.vg, TPT_C3);
      nvgBeginPath(args.vg);
      nvgRect(args.vg, 0, 0, this->box.size.x, this->box.size.y);
      nvgFill(args.vg);
    }
  }
}

void CellSpot::onButton(const event::Button &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
    GridDisplay *gd = getAncestorOfType<GridDisplay>();
    // thread safe
    if (!gd->module->clickedCells.full()) {
      gd->module->clickedCells.push(cell);
    }
    e.consume(this);
  }
  Widget::onButton(e);
}

/// GRID SCROLL BUTTON ///

void GridScrollButton::buttonReleased() {
  scrollSpeed = 1;
  scrollFramesCount = 0;
  scrollAccelerationFramesCount = 0;
}

void GridScrollButton::whileHovering() {
  if (scrollFramesCount % FRAMES_BETWEEN_SCROLL == 0) {
    scrollFramesCount = 0;
    doScroll();
    scrollAccelerationFramesCount++;
    if (scrollAccelerationFramesCount == SCROLLS_BEFORE_SPEED_INCREASE) {
      scrollSpeed = scrollSpeed * 2;
      scrollAccelerationFramesCount = 0;
    }
  }
  scrollFramesCount++;
}

void GridScrollButton::doScroll() {
  GridScrollPane *sb = getAncestorOfType<GridScrollPane>();
  GoLDisplay *gd = sb->getAncestorOfType<GoLDisplay>();
  gd->gridDisplay->scroll(scrollSpeed * orientation, sb->vertical);
}

/// SCROLLBAR ///

GridScrollBar::GridScrollBar() { r = (mm2px(ICON_SIZE) - 4 * barSize) / 2; }

void GridScrollBar::draw(const DrawArgs &args) {
  if (getAncestorOfType<GridScrollPane>()) {
    gd = getAncestorOfType<GridScrollPane>()
             ->getAncestorOfType<GoLDisplay>()
             ->gridDisplay;
  }
  // rectangles to materialize scroll area
  nvgFillColor(args.vg, OPAQUE_C1_LIGHT);
  nvgBeginPath(args.vg);
  nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
  nvgFill(args.vg);
  nvgFillColor(args.vg, OPAQUE_C1_DARK);
  nvgBeginPath(args.vg);
  if (vertical) {
    nvgRect(args.vg, 0, 0, barSize, box.size.y);
    nvgRect(args.vg, box.size.x - barSize, 0, barSize, this->box.size.y);
    nvgRect(args.vg, 0, 0, box.size.x, barSize);
    nvgRect(args.vg, 0, box.size.y - barSize, box.size.x, barSize);
  } else {
    nvgRect(args.vg, 0, 0, box.size.x, barSize);
    nvgRect(args.vg, 0, box.size.y - barSize, this->box.size.x, barSize);
    nvgRect(args.vg, 0, 0, barSize, box.size.y);
    nvgRect(args.vg, box.size.x - barSize, 0, barSize, box.size.y);
  }
  nvgFill(args.vg);
  nvgBeginPath(args.vg);
  // circle to materialize position
  if (gd) {
    float percent =
        vertical ? (float)(gd->display_y0) / (float)(NUMCELLS_Y - gd->spotsY)
                 : (float)(gd->display_x0) / (float)(NUMCELLS_X - gd->spotsX);
    if (vertical) {
      if (gd->spotsY != NUMCELLS_Y) {
        float position = (1 - percent) * (r + barSize) +
                         percent * (box.size.y - r - barSize);
        nvgFillColor(args.vg, OPAQUE_C1_DARK);
        nvgCircle(args.vg, box.size.x / 2.0f, position, r);
        nvgFill(args.vg);
      }
    } else {
      if (gd->spotsX != NUMCELLS_X) {
        float position = (1 - percent) * (r + barSize) +
                         percent * (box.size.x - r - barSize);
        nvgFillColor(args.vg, OPAQUE_C1_DARK);
        nvgCircle(args.vg, position, box.size.y / 2.0f, r);
        nvgFill(args.vg);
      }
    }
  }
}

void GridScrollBar::onButton(const event::Button &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
    if (e.action == GLFW_PRESS) {
      dragging = true;
    } else if (e.action == GLFW_RELEASE) {
      dragging = false;
    }
    e.consume(this);
    Widget::onButton(e);
  }
}

void GridScrollBar::onDragLeave(const event::DragLeave &e) {
  dragging = false;
  e.consume(this);
}

void GridScrollBar::onDragHover(const event::DragHover &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT && dragging) {
    float infLimit = r + barSize;
    float supLimit = vertical ? box.size.y : box.size.x;
    supLimit -= (r + barSize);
    float position = vertical ? e.pos.y : e.pos.x;
    if (position <= infLimit) {
      if (vertical) {
        gd->display_y0 = 0;
      } else {
        gd->display_x0 = 0;
      }
    } else if (position >= supLimit) {
      if (vertical) {
        gd->display_y0 = NUMCELLS_Y - gd->spotsY;
      } else {
        gd->display_x0 = NUMCELLS_X - gd->spotsX;
      }
    } else {
      float percent = (position - infLimit) / (supLimit - infLimit);
      float max = vertical ? NUMCELLS_Y - gd->spotsY : NUMCELLS_X - gd->spotsX;
      int val = floor(percent * max);
      if (vertical) {
        gd->display_y0 = val;
      } else {
        gd->display_x0 = val;
      }
    }
    e.consume(this);
    Widget::onDragHover(e);
    gd->clearChildren();
    gd->viewUpdated();
    gd->firstDraw = true;
  }
}

/// SCROLLPANE ///
void GridScrollPane::draw(const DrawArgs &args) {
  if (firstDraw) {
    float iconSizePx = mm2px(ICON_SIZE);
    // scrollBar
    if (vertical) {
      GridScrollBar *vBar = new GridScrollBar();
      vBar->box.size = Vec(iconSizePx, box.size.y - 2 * iconSizePx);
      vBar->box.pos = Vec(0, iconSizePx);
      vBar->vertical = true;
      addChild(vBar);
    } else {
      GridScrollBar *hBar = new GridScrollBar();
      hBar->box.size = Vec(box.size.x - 2 * iconSizePx, iconSizePx);
      hBar->box.pos = Vec(iconSizePx, 0);
      hBar->vertical = false;
      addChild(hBar);
    }
    // buttonPlus
    buttonPlus = new GridScrollButton();
    buttonPlus->orientation = 1;
    buttonPlus->box.size = Vec(iconSizePx, iconSizePx);
    if (vertical) {
      buttonPlus->box.pos = Vec(0, box.size.y - iconSizePx);
    } else {
      buttonPlus->box.pos = Vec(box.size.x - iconSizePx, 0);
    }
    buttonMinus = new GridScrollButton();
    buttonMinus->orientation = -1;
    buttonMinus->box.size = Vec(iconSizePx, iconSizePx);
    buttonMinus->box.pos = Vec(0, 0);
    addChild(buttonPlus);
    addChild(buttonMinus);
    firstDraw = false;
  }
  if (!scrollPossible) {
    nvgFillColor(args.vg, TPT_C1_LIGHT);
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0, 0, this->box.size.x, this->box.size.y);
    nvgFill(args.vg);
  } else {
    OpaqueWidget::draw(args);
  }
}

/// CENTER BUTTON ///

void CenterButton::buttonReleased() {
  GoLDisplay *gd = getAncestorOfType<GoLDisplay>();
  gd->gridDisplay->resetView();
}

void CenterButton::whileHovering() {}

/// ZOOM BUTTON ///

void ZoomButton::buttonReleased() {
  zoomSpeed = 1;
  zoomFramesCount = 0;
  zoomAccelerationFramesCount = 0;
}

void ZoomButton::whileHovering() {
  if (zoomFramesCount % FRAMES_BETWEEN_ZOOM == 0) {
    zoomFramesCount = 0;
    GoLDisplay *gd = getAncestorOfType<GoLDisplay>();
    gd->changeZoomLevel(zoomSpeed * baseZoom);
    zoomAccelerationFramesCount++;
    if (zoomAccelerationFramesCount == ZOOMS_BEFORE_SPEED_INCREASE) {
      zoomSpeed = zoomSpeed * 2;
      zoomAccelerationFramesCount = 0;
    }
  }
  zoomFramesCount++;
}

/// ZOOM BAR ///

GridZoomBar::GridZoomBar() { r = (mm2px(ICON_SIZE) - 4 * barSize) / 2; }

void GridZoomBar::draw(const DrawArgs &args) {
  if (getAncestorOfType<GoLDisplay>()) {
    gd = getAncestorOfType<GoLDisplay>()->gridDisplay;
  }
  // rectangles to materialize scroll area
  nvgFillColor(args.vg, OPAQUE_C1_LIGHT);
  nvgBeginPath(args.vg);
  nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
  nvgFill(args.vg);
  nvgFillColor(args.vg, OPAQUE_C1_DARK);
  nvgBeginPath(args.vg);

  nvgRect(args.vg, 0, 0, barSize, box.size.y);
  nvgRect(args.vg, box.size.x - barSize, 0, barSize, this->box.size.y);
  nvgRect(args.vg, 0, 0, box.size.x, barSize);
  nvgRect(args.vg, 0, box.size.y - barSize, box.size.x, barSize);

  nvgFill(args.vg);
  nvgBeginPath(args.vg);
  // circle to materialize position
  if (gd) {
    float percent = (float)(gd->spotsX - MIN_CELLS_ON_SCREEN) /
                    (float)(NUMCELLS_X - MIN_CELLS_ON_SCREEN);
    float position =
        (1 - percent) * (r + barSize) + percent * (box.size.y - r - barSize);
    nvgFillColor(args.vg, OPAQUE_C1_DARK);
    nvgCircle(args.vg, box.size.x / 2.0f, position, r);
    nvgFill(args.vg);
  }
}

void GridZoomBar::onButton(const event::Button &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
    if (e.action == GLFW_PRESS) {
      dragging = true;
    } else if (e.action == GLFW_RELEASE) {
      dragging = false;
    }
    e.consume(this);
    Widget::onButton(e);
  }
}

void GridZoomBar::onDragLeave(const event::DragLeave &e) {
  dragging = false;
  e.consume(this);
}

void GridZoomBar::onDragHover(const event::DragHover &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT && dragging) {
    float infLimit = r + barSize;
    float supLimit = box.size.y;
    supLimit -= (r + barSize);
    float position = e.pos.y;
    float percent = 0.f;
    if (position <= infLimit) {
      percent = 0.f;
    } else if (position >= supLimit) {
      percent = 1.f;
    } else {
      percent = (position - infLimit) / (supLimit - infLimit);
    }
    int xsize = floor(MIN_CELLS_ON_SCREEN +
                      percent * (NUMCELLS_X - MIN_CELLS_ON_SCREEN));
    int ysize = floor(MIN_CELLS_ON_SCREEN +
                      percent * (NUMCELLS_X - MIN_CELLS_ON_SCREEN));
    int oldx = gd->spotsX;
    int oldy = gd->spotsY;
    int new_x0 = gd->display_x0 + (oldx - xsize) / 2;
    int new_y0 = gd->display_y0 + (oldy - ysize) / 2;
    gd->setView(new_x0, new_y0, xsize, ysize);
    e.consume(this);
    Widget::onDragHover(e);
  }
}
/// GOLDISPLAY ///

GoLDisplay::GoLDisplay() { firstDraw = true; }

void GoLDisplay::changeZoomLevel(int zoomChange) {
  gridDisplay->changeZoomLevel(zoomChange);
  if (gridDisplay->spotsX == NUMCELLS_X) {
    hScrollPane->scrollPossible = false;
  } else {
    hScrollPane->scrollPossible = true;
  }
  if (gridDisplay->spotsY == NUMCELLS_Y) {
    vScrollPane->scrollPossible = false;
  } else {
    vScrollPane->scrollPossible = true;
  }
}

void GoLDisplay::draw(const DrawArgs &args) {
  if (firstDraw && module) {
    float iconSizePx = mm2px(ICON_SIZE);
    float iconPaddingPx = mm2px(ICON_PADDING);
    float gridSizeX = this->box.size.x - 2 * (iconSizePx + iconPaddingPx);
    float gridSizeY = this->box.size.y - iconSizePx - iconPaddingPx;
    // grid display
    gridDisplay = new GridDisplay();
    gridDisplay->module = module;
    gridDisplay->box.pos = Vec(iconSizePx + iconPaddingPx, 0.f);
    gridDisplay->box.size = Vec(gridSizeX, gridSizeY);
    addChild(gridDisplay);
    // vertical scrollPane
    vScrollPane = new GridScrollPane();
    vScrollPane->vertical = true;
    vScrollPane->box.size = Vec(iconSizePx, gridSizeY);
    vScrollPane->box.pos = Vec(gridSizeX + 2 * iconPaddingPx + iconSizePx, 0.f);
    addChild(vScrollPane);
    // horizontal scrollPane
    hScrollPane = new GridScrollPane();
    hScrollPane->vertical = false;
    hScrollPane->box.size = Vec(gridSizeX, iconSizePx);
    hScrollPane->box.pos =
        Vec(iconSizePx + iconPaddingPx, gridSizeY + iconPaddingPx);
    addChild(hScrollPane);
    // reset grid button
    CenterButton *centerButton = new CenterButton();
    centerButton->box.size = Vec(iconSizePx, iconSizePx);
    centerButton->box.pos = Vec(0.f, gridSizeY + iconPaddingPx);
    addChild(centerButton);
    // zoom button +
    ZoomButton *zoomPlus = new ZoomButton();
    zoomPlus->baseZoom = -1;
    zoomPlus->box.size = Vec(iconSizePx, iconSizePx);
    zoomPlus->box.pos = Vec(0.f, 0.f);
    addChild(zoomPlus);
    // zoom button -
    ZoomButton *zoomMinus = new ZoomButton();
    zoomMinus->baseZoom = 1;
    zoomMinus->box.size = Vec(iconSizePx, iconSizePx);
    zoomMinus->box.pos = Vec(0.f, gridSizeY - iconSizePx);
    addChild(zoomMinus);
    // zoom bar
    GridZoomBar *zBar = new GridZoomBar();
    zBar->box.size =
        Vec(iconSizePx, gridSizeY - 2 * (iconSizePx + iconPaddingPx));
    zBar->box.pos = Vec(0.f, iconSizePx + iconPaddingPx);
    addChild(zBar);
    // dont do this cursed part ever again
    firstDraw = false;
  }
  OpaqueWidget::draw(args);
}

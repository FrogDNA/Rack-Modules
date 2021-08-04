#include "GoLDisplay.hpp"
#include "Consts.hpp"
#include "Mitosis.hpp"

GridDisplay::GridDisplay() {
  // todo replace with static something
  numCellsX = (float)(NUMCELLS_DISPLAY_X +
                      2); // take into account lineheaders and the right column
  numCellsY = (float)(NUMCELLS_DISPLAY_Y +
                      2); // take into account lineheaders and the bottom line
  float sizeXmm = 110.0;
  float sizeYmm = 110.0;
  sizeX = mm2px(Vec(110.0, 110.0)).x;
  sizeY = mm2px(Vec(110.0, 110.0)).y;
  cellSizeX =
      mm2px(Vec(sizeXmm / (numCellsX * 1.1), sizeYmm / (numCellsY * 1.1))).x;
  cellSizeY =
      mm2px(Vec(sizeXmm / (numCellsX * 1.1), sizeYmm / (numCellsY * 1.1))).y;
  cellSpaceX = cellSizeX / 10;
  cellSpaceY = cellSizeY / 10;
  firstDraw = true;
}

void GridDisplay::draw(const DrawArgs &args) {
  if (module && firstDraw) {
    firstDraw = false;
    // define column headers
    for (int i = 0; i < NUMCELLS_DISPLAY_X; i++) {
      LineHeader *lh = new LineHeader(i + display_x0, false);
      lh->box.pos = Vec((i + 1) * (cellSizeX + cellSpaceX), 0);
      lh->box.size = Vec(cellSizeX, cellSizeY);
      lh->rb = &(module->muteUnmuteColsBuffer);
      addChild(lh);
    }
    // define row headers
    for (int i = 0; i < NUMCELLS_DISPLAY_Y; i++) {
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
        DrawableCell *golCell = new DrawableCell(c);
        golCell->box.pos = Vec((i + 1) * (cellSizeX + cellSpaceX),
                               (j + 1) * (cellSizeY + cellSpaceY));
        golCell->box.size = Vec(cellSizeX, cellSizeY);
        addChild(golCell);
      }
    }
    // define zoom bars and buttons.TODO
  }
  OpaqueWidget::draw(args);
}

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

DrawableCell::DrawableCell(Cell *cell) { this->cell = cell; }

void DrawableCell::draw(const DrawArgs &args) {
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

void DrawableCell::onButton(const event::Button &e) {
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

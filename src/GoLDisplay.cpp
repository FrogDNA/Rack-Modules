#include "GoLDisplay.hpp"
#include "Consts.hpp"
#include "Mitosis.hpp"

GolDisplay::GolDisplay() {
  // todo replace with static something
  numCellsX = (float)(NUMCELLSX + 1); // take into account lineheaders
  numCellsY = (float)(NUMCELLSY + 1); // take into account lineheaders
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

void GolDisplay::draw(const DrawArgs &args) {
  if (module && firstDraw) {
    firstDraw = false;
    for (int i = 0; i < NUMCELLSY; i++) {
      LineHeader *lh = new LineHeader(i, true);
      lh->box.pos = Vec(0, (i + 1) * (cellSizeY + cellSpaceY));
      lh->box.size = Vec(cellSizeX, cellSizeY);
      lh->rb = &(module->muteUnmuteRowsBuffer);
      addChild(lh);
    }
    for (int i = 0; i < NUMCELLSX; i++) {
      LineHeader *lh = new LineHeader(i, false);
      lh->box.pos = Vec((i + 1) * (cellSizeX + cellSpaceX), 0);
      lh->box.size = Vec(cellSizeX, cellSizeY);
      lh->rb = &(module->muteUnmuteColsBuffer);
      addChild(lh);
    }
    for (int i = 0; i < NUMCELLSX; i++) {
      for (int j = 0; j < NUMCELLSY; j++) {
        Cell *c = module->golGrid->getCell(i, j);
        DrawableCell *golCell = new DrawableCell(c);
        golCell->box.pos = Vec((i + 1) * (cellSizeX + cellSpaceX),
                               (j + 1) * (cellSizeY + cellSpaceY));
        golCell->box.size = Vec(cellSizeX, cellSizeY);
        addChild(golCell);
      }
    }
  }
  OpaqueWidget::draw(args);
}

LineHeader::LineHeader(int coordinate, bool isLine) {
  this->coordinate = coordinate;
  this->isRow = isLine;
}

void LineHeader::draw(const DrawArgs &args) {
  DSP *dsp = getAncestorOfType<GolDisplay>()->module->dsp;
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
  DSP *dsp = getAncestorOfType<GolDisplay>()->module->dsp;
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
    GolDisplay *gd = getAncestorOfType<GolDisplay>();
    // thread safe
    if (!gd->module->clickedCells.full()) {
      gd->module->clickedCells.push(new Coordinate(cell->getX(), cell->getY()));
    }
    e.consume(this);
  }
  Widget::onButton(e);
}

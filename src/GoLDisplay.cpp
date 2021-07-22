#include "GoLDisplay.hpp"
#include "GameOfLifeGrid.hpp"

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
    for (int i = 0; i < NUMCELLSX; i++) {
      LineHeader *lh = new LineHeader(i, true);
      lh->box.pos = Vec((i + 1) * (cellSizeX + cellSpaceX), 0);
      lh->box.size = Vec(cellSizeX, cellSizeY);
      addChild(lh);
      lh = new LineHeader(i, false);
      lh->box.pos = Vec(0, (i + 1) * (cellSizeX + cellSpaceX));
      lh->box.size = Vec(cellSizeX, cellSizeY);
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
  this->isActivated = false;
}

void LineHeader::draw(const DrawArgs &args) {
  if (this->isActivated) {
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
    this->isActivated = !this->isActivated;
    GolDisplay *gd = getAncestorOfType<GolDisplay>();
    for (int i = 0; i < NUMCELLSX; i++) {
      Cell *c;
      if (this->isRow) {
        c = gd->module->golGrid->getCell(this->coordinate, i);
      } else {
        c = gd->module->golGrid->getCell(i, this->coordinate);
      }
      if (this->isRow) {
        c->rowMuted = !c->rowMuted;
      } else {
        c->colMuted = !c->colMuted;
      }
    }

    e.consume(this);
  }
  Widget::onButton(e);
}

DrawableCell::DrawableCell(Cell *cell) { this->cell = cell; }

void DrawableCell::draw(const DrawArgs &args) {
  if (cell->isAlive() && cell->isAudible()) {
    nvgFillColor(args.vg, nvgRGBA(0x11, 0x11, 0x11, 0xff));
  } else if (cell->isAlive()) {
    nvgFillColor(args.vg, nvgRGBA(0x6e, 0x6e, 0x6e, 0xff));
  } else if (cell->isAudible()) {
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
    // dont just tell the cell its alive, tell the grid
    gd->module->golGrid->setCellState(cell->getX(), cell->getY(),
                                      !cell->isAlive());
    e.consume(this);
  }
  Widget::onButton(e);
}

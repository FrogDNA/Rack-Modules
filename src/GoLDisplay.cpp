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
      bool activate = module->golGrid->getCell(i, 0)->rowMuted;
      LineHeader *lh = new LineHeader(i, true, activate);
      lh->box.pos = Vec((i + 1) * (cellSizeX + cellSpaceX), 0);
      lh->box.size = Vec(cellSizeX, cellSizeY);
      addChild(lh);
    }
    for (int i = 0; i < NUMCELLSY; i++) {
      bool activate = module->golGrid->getCell(0, i)->colMuted;
      LineHeader *lh = new LineHeader(i, false, activate);
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

LineHeader::LineHeader(int coordinate, bool isLine, bool activate) {
  this->coordinate = coordinate;
  this->isRow = isLine;
  this->isActivated = activate;
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
    // not thread safe. Warning if something else could mute and unmute
    if (this->isRow) {
      for (int i = 0; i < NUMCELLSY; i++) {
        Cell *c = gd->module->golGrid->getCell(this->coordinate, i);
        c->rowMuted = !c->rowMuted;
      }
    } else {
      for (int i = 0; i < NUMCELLSX; i++) {
        Cell *c = gd->module->golGrid->getCell(i, this->coordinate);
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
    // thread safe
    if (!gd->module->clickedCells.full()) {
      gd->module->clickedCells.push(new Coordinate(cell->getX(), cell->getY()));
    }
    e.consume(this);
  }
  Widget::onButton(e);
}

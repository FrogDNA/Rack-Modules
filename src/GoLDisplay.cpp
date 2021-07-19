#include "GolDisplay.hpp"
#include "GameOfLifeGrid.hpp"

GolDisplay::GolDisplay() {
  // todo replace with static something
  numCellsX = (float)NUMCELLSX;
  numCellsY = (float)NUMCELLSY;
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
      for (int j = 0; j < NUMCELLSY; j++) {
        Cell *c = module->golGrid->getCell(i, j);
        DrawableCell *golCell = new DrawableCell(c);
        golCell->box.pos =
            Vec(i * (cellSizeX + cellSpaceX), j * (cellSizeY + cellSpaceY));
        golCell->box.size = Vec(cellSizeX, cellSizeY);
        addChild(golCell);
      }
    }
  }
  OpaqueWidget::draw(args);
}

DrawableCell::DrawableCell(Cell *cell) { this->cell = cell; }

void DrawableCell::draw(const DrawArgs &args) {
  if (cell->isAlive()) {
    nvgFillColor(args.vg, nvgRGBA(0x11, 0x11, 0x11, 0xff));
  } else {
    nvgFillColor(args.vg, nvgRGBA(0xdd, 0xdd, 0xdd, 0xff));
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

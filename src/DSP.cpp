#include "DSP.hpp"

Outputs::Outputs() {
  xOutputs.reserve(10);
  yOutputs.reserve(10);
  xPresents.reserve(10);
  yPresents.reserve(10);
  for (int i = 0; i < 10; i++) {
    xOutputs[i] = 0.0f;
    yOutputs[i] = 0.0f;
    xPresents[i] = 0.0f;
    yPresents[i] = 0.0f;
  }
}

DSP::DSP() {
  audibleCols.reserve(NUMCELLS_X);
  audibleRows.reserve(NUMCELLS_Y);
  xFrequencies.reserve(NUMCELLS_X);
  yFrequencies.reserve(NUMCELLS_Y);

  for (int i = 0; i < NUMCELLS_X; i++) {
    audibleCols[i] = true;
    xFrequencies[i] = (i - REFERENCE_POS) / 12.0f;
  }
  for (int i = 0; i < NUMCELLS_Y; i++) {
    audibleRows[i] = true;
    yFrequencies[i] = (i - REFERENCE_POS) / 12.0f;
  }
}
void DSP::update(float rowVoct, float colVoct) {
  int rowOffset = floor(rowVoct * 12.f);
  int colOffset = floor(colVoct * 12.f);
  if (colOffset != offsetX) {
    offsetX = colOffset;
    outputChanged = true;
  }
  if (rowOffset != offsetY) {
    offsetY = rowOffset;
    outputChanged = true;
  }
}

void DSP::update(std::vector<std::pair<int, int> *> state, float rowVoct,
                 float colVoct) {
  update(rowVoct, colVoct);
  usableX.clear();
  usableY.clear();
  for (std::vector<std::pair<int, int> *>::iterator it = state.begin();
       it != state.end(); ++it) {
    std::pair<int, int> *c = *it;
    int x = c->first;
    int y = c->second;
    if (isCellAudible(c)) {
      usableX.insert(x);
      usableY.insert(y);
    }
  }
  outputChanged = true;
}

Outputs DSP::getOutputs() {
  Outputs out;
  std::set<int>::iterator it;
  std::set<int>::iterator startX = usableX.lower_bound(offsetX);
  if (usableX.begin() != usableX.end()) {
    if (startX == usableX.end()) {
      startX == usableX.begin();
    }
    it = startX;
    int xCounter = 0;
    do {
      out.xOutputs[xCounter] = xFrequencies[*it];
      out.xPresents[xCounter] = 10.0f;
      xCounter++;
      if (xCounter == 10) {
        break;
      }
      it++;
      if (it == usableX.end()) {
        it = usableX.begin();
      }
    } while (it != startX);
  }
  if (usableY.begin() != usableY.end()) {
    std::set<int>::iterator startY = usableY.lower_bound(offsetY);
    if (startY == usableY.end()) {
      startY == usableY.begin();
    }
    it = startY;
    int yCounter = 0;
    do {
      out.yOutputs[yCounter] = yFrequencies[*it];
      out.yPresents[yCounter] = 10.0f;
      yCounter++;
      if (yCounter == 10) {
        break;
      }
      it++;
      if (it == usableY.end()) {
        it = usableY.begin();
      }
    } while (it != startY);
  }
  return out;
}

void DSP::resetOutputChanged() { outputChanged = false; }

bool DSP::isOutputChanged() { return outputChanged; }

void DSP::muteUnmuteCol(int x, bool muted) {
  if (x < 0 || x > NUMCELLS_X) {
    return;
  }
  audibleCols[x] = muted;
}

void DSP::muteUnmuteRow(int y, bool muted) {
  if (y < 0 || y > NUMCELLS_Y) {
    return;
  }
  audibleRows[y] = muted;
}

bool DSP::isColAudible(int x) {
  if (x < 0 || x > NUMCELLS_X) {
    return false;
  }
  return audibleCols[x];
}

bool DSP::isRowAudible(int y) {
  if (y < 0 || y > NUMCELLS_Y) {
    return false;
  }
  return audibleRows[y];
}

bool DSP::isCellAudible(std::pair<int, int> *c) {
  return (audibleRows[c->second] && audibleCols[c->first]);
}

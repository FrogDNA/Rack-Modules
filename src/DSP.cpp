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

void DSP::update(std::vector<std::pair<int, int> *> state) {
  usableX.clear();
  usableY.clear();
  int counter = 0;
  for (std::vector<std::pair<int, int> *>::iterator it = state.begin();
       it != state.end(); ++it) {
    std::pair<int, int> *c = *it;
    int x = c->first;
    int y = c->second;
    if (isCellAudible(c)) {
      usableX.insert(x);
      usableY.insert(y);
      counter++;
      if (counter == 10) {
        break;
      }
    }
  }
  outputChanged = true;
}

Outputs DSP::getOutputs() {
  Outputs out;
  int counter = 0;
  for (std::set<int>::iterator it = usableX.begin(); it != usableX.end();
       ++it) {
    out.xOutputs[counter] = xFrequencies[*it];
    out.xPresents[counter] = 10.0f;
    counter++;
  }
  counter = 0;
  for (std::set<int>::iterator it = usableY.begin(); it != usableY.end();
       ++it) {
    out.yOutputs[counter] = yFrequencies[*it];
    out.yPresents[counter] = 10.0f;
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

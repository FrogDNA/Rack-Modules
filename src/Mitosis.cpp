#include "Mitosis.hpp"
#include "DSP.hpp"
#include "GameOfLifeGrid.hpp"
#include "GoLDisplay.hpp"
#include "ScaleButton.hpp"
#include "SliderParam.hpp"
#include "plugin.hpp"
#include <unordered_map>
#include <vector>

Mitosis::Mitosis() {
  config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
  configParam(DEAD_PARAM, 1.f, 10.f, 4.f,
              "Number of cycles taken into account for DEAD output");
  golGrid = new GameOfLifeGrid();
  for (int i = 0; i < 3; i++) {
    saves.push_back(std::vector<std::pair<int, int> *>());
  }
  dsp = new DSP();
  out = dsp->getOutputs();
  // for debugging
  // setbuf(stdout, NULL);
}

void Mitosis::process(const ProcessArgs &args) {
  float deadValue = params[DEAD_PARAM].getValue();
  float clock = inputs[CLOCK_INPUT].getVoltage();
  float rowLimit = inputs[ROW_LIMIT_INPUT].getVoltage();
  float colLimit = inputs[COL_LIMIT_INPUT].getVoltage();
  float load1 = inputs[LOAD_1_INPUT].getVoltage();
  float load2 = inputs[LOAD_2_INPUT].getVoltage();
  float save1 = inputs[SAVE_1_INPUT].getVoltage();
  float save2 = inputs[SAVE_2_INPUT].getVoltage();
  float reset = inputs[RESET_INPUT].getVoltage();
  float loadSeed = inputs[SEED_INPUT].getVoltage();
  // init
  // save and load can only be triggered if all of them are low beforehand.
  if (save1 < 1.5f && save2 < 1.5f) {
    saveHigh = false;
  }
  if (load1 < 1.5f && load2 < 1.5f && loadSeed < 1.5f && reset < 1.5f) {
    loadHigh = false;
  }
  // save
  if (!saveHigh && (save1 > 3.5f || save2 > 3.5f)) {
    saveHigh = true;
    if (save1 > 3.5f) {
      lastLoaded = 1;
    } else if (save2 > 3.5f) {
      lastLoaded = 2;
    }
    lastLoadedData = golGrid->getCurrentlyAlive();
    saves[lastLoaded - 1] = lastLoadedData;
  }
  if (!loadHigh &&
      (load1 > 3.5f || load2 > 3.5f || loadSeed > 3.5f || reset > 3.5f)) {
    loadHigh = true;
    std::vector<std::pair<int, int> *> cells;
    if (load1 > 3.5f) {
      lastLoaded = 1;
      cells = saves[0];
    } else if (load2 > 3.5f) {
      lastLoaded = 2;
      cells = saves[1];
    } else if (loadSeed > 3.5f) {
      lastLoaded = 3;
      cells = GameOfLifeGrid::createRandomGrid();
    } else if (reset > 3.5f) {
      // reset
      cells = lastLoadedData;
    }
    lastLoadedData = cells;
    golGrid->init(cells);
  }
  // dead param
  int deadValueInt = 0;
  for (int i = 1; i < 11; i++) {
    if (isNear(deadValue, 1.0f * i)) {
      deadValueInt = i;
      break;
    }
  }
  golGrid->setStillEvolvingLength(deadValueInt);
  // col and row limits
  if (rowLimit > 3.5f && !rowLimitUp) {
    rowLimitUp = true;
    golGrid->setRowLimit(rowStart, rowSize);
  } else if (rowLimit < 1.5f && rowLimitUp) {
    rowLimitUp = false;
    golGrid->setRowLimit(0, NUMCELLS_Y);
  }
  if (colLimit > 3.5f && !colLimitUp) {
    colLimitUp = true;
    golGrid->setColLimit(colStart, colSize);
  } else if (colLimit < 1.5f && colLimitUp) {
    colLimitUp = false;
    golGrid->setColLimit(0, NUMCELLS_X);
  }
  // clock
  bool risingEdge = false;
  if (clock > 3.5f) {
    risingEdge = (clockUp == false ? true : false);
    clockUp = true;
  } else if (clock < 1.5f) {
    clockUp = false;
    risingEdge = false;
  }
  if (risingEdge) {
    golGrid->update();
  }
  // check if GUI has some messages
  processBuffers();
  // DSP update and outputs
  bool hasCAChanged = golGrid->isHasCAChanged();
  if (hasCAChanged || audibilityChanged) {
    std::vector<std::pair<int, int> *> state =
        golGrid->getCurrentlyAliveRegister();
    dsp->update(state);
    audibilityChanged = false;
  }
  if (dsp->isOutputChanged()) {
    out = dsp->getOutputs();
    dsp->resetOutputChanged();
  }
  // misc outputs
  outputs[DEAD_OUTPUT].setVoltage(golGrid->isStillEvolving() ? 0.f : 10.f);
  // load/save lights
  lights[LS_1_LIGHT].setBrightness(lastLoaded == 1 ? 1.f : 0.f);
  lights[LS_2_LIGHT].setBrightness(lastLoaded == 2 ? 1.f : 0.f);
  lights[SEED_LIGHT].setBrightness(lastLoaded == 3 ? 1.f : 0.f);
  lights[DEAD_1_LIGHT].setBrightness(deadValueInt == 1 ? 1.f : 0.f);
  lights[DEAD_2_LIGHT].setBrightness(deadValueInt == 2 ? 1.f : 0.f);
  lights[DEAD_3_LIGHT].setBrightness(deadValueInt == 3 ? 1.f : 0.f);
  lights[DEAD_4_LIGHT].setBrightness(deadValueInt == 4 ? 1.f : 0.f);
  lights[DEAD_5_LIGHT].setBrightness(deadValueInt == 5 ? 1.f : 0.f);
  lights[DEAD_6_LIGHT].setBrightness(deadValueInt == 6 ? 1.f : 0.f);
  lights[DEAD_7_LIGHT].setBrightness(deadValueInt == 7 ? 1.f : 0.f);
  lights[DEAD_8_LIGHT].setBrightness(deadValueInt == 8 ? 1.f : 0.f);
  lights[DEAD_9_LIGHT].setBrightness(deadValueInt == 9 ? 1.f : 0.f);
  lights[DEAD_10_LIGHT].setBrightness(deadValueInt == 10 ? 1.f : 0.f);

  // outputs rows
  outputs[ROW_1_OUTPUT].setVoltage(out.yOutputs[0]);
  outputs[ROW_2_OUTPUT].setVoltage(out.yOutputs[1]);
  outputs[ROW_3_OUTPUT].setVoltage(out.yOutputs[2]);
  outputs[ROW_4_OUTPUT].setVoltage(out.yOutputs[3]);
  outputs[ROW_5_OUTPUT].setVoltage(out.yOutputs[4]);
  outputs[ROW_6_OUTPUT].setVoltage(out.yOutputs[5]);
  outputs[ROW_7_OUTPUT].setVoltage(out.yOutputs[6]);
  outputs[ROW_8_OUTPUT].setVoltage(out.yOutputs[7]);
  outputs[ROW_9_OUTPUT].setVoltage(out.yOutputs[8]);
  outputs[ROW_10_OUTPUT].setVoltage(out.yOutputs[9]);
  // outputs presents row
  outputs[ROW_1_P_OUTPUT].setVoltage(out.yPresents[0]);
  outputs[ROW_2_P_OUTPUT].setVoltage(out.yPresents[1]);
  outputs[ROW_3_P_OUTPUT].setVoltage(out.yPresents[2]);
  outputs[ROW_4_P_OUTPUT].setVoltage(out.yPresents[3]);
  outputs[ROW_5_P_OUTPUT].setVoltage(out.yPresents[4]);
  outputs[ROW_6_P_OUTPUT].setVoltage(out.yPresents[5]);
  outputs[ROW_7_P_OUTPUT].setVoltage(out.yPresents[6]);
  outputs[ROW_8_P_OUTPUT].setVoltage(out.yPresents[7]);
  outputs[ROW_9_P_OUTPUT].setVoltage(out.yPresents[8]);
  outputs[ROW_10_P_OUTPUT].setVoltage(out.yPresents[9]);
  // outputs presents lights row
  lights[ROW_1_LIGHT].setBrightness(out.yPresents[0] == 0.f ? 0.f : 1.f);
  lights[ROW_2_LIGHT].setBrightness(out.yPresents[1] == 0.f ? 0.f : 1.f);
  lights[ROW_3_LIGHT].setBrightness(out.yPresents[2] == 0.f ? 0.f : 1.f);
  lights[ROW_4_LIGHT].setBrightness(out.yPresents[3] == 0.f ? 0.f : 1.f);
  lights[ROW_5_LIGHT].setBrightness(out.yPresents[4] == 0.f ? 0.f : 1.f);
  lights[ROW_6_LIGHT].setBrightness(out.yPresents[5] == 0.f ? 0.f : 1.f);
  lights[ROW_7_LIGHT].setBrightness(out.yPresents[6] == 0.f ? 0.f : 1.f);
  lights[ROW_8_LIGHT].setBrightness(out.yPresents[7] == 0.f ? 0.f : 1.f);
  lights[ROW_9_LIGHT].setBrightness(out.yPresents[8] == 0.f ? 0.f : 1.f);
  lights[ROW_10_LIGHT].setBrightness(out.yPresents[9] == 0.f ? 0.f : 1.f);
  // outputs cols
  outputs[COL_1_OUTPUT].setVoltage(out.xOutputs[0]);
  outputs[COL_2_OUTPUT].setVoltage(out.xOutputs[1]);
  outputs[COL_3_OUTPUT].setVoltage(out.xOutputs[2]);
  outputs[COL_4_OUTPUT].setVoltage(out.xOutputs[3]);
  outputs[COL_5_OUTPUT].setVoltage(out.xOutputs[4]);
  outputs[COL_6_OUTPUT].setVoltage(out.xOutputs[5]);
  outputs[COL_7_OUTPUT].setVoltage(out.xOutputs[6]);
  outputs[COL_8_OUTPUT].setVoltage(out.xOutputs[7]);
  outputs[COL_9_OUTPUT].setVoltage(out.xOutputs[8]);
  outputs[COL_10_OUTPUT].setVoltage(out.xOutputs[9]);
  // outputs presents col
  outputs[COL_1_P_OUTPUT].setVoltage(out.xPresents[0]);
  outputs[COL_2_P_OUTPUT].setVoltage(out.xPresents[1]);
  outputs[COL_3_P_OUTPUT].setVoltage(out.xPresents[2]);
  outputs[COL_4_P_OUTPUT].setVoltage(out.xPresents[3]);
  outputs[COL_5_P_OUTPUT].setVoltage(out.xPresents[4]);
  outputs[COL_6_P_OUTPUT].setVoltage(out.xPresents[5]);
  outputs[COL_7_P_OUTPUT].setVoltage(out.xPresents[6]);
  outputs[COL_8_P_OUTPUT].setVoltage(out.xPresents[7]);
  outputs[COL_9_P_OUTPUT].setVoltage(out.xPresents[8]);
  outputs[COL_10_P_OUTPUT].setVoltage(out.xPresents[9]);
  // outputs presents lights row
  lights[COL_1_LIGHT].setBrightness(out.xPresents[0] == 0.f ? 0.f : 1.f);
  lights[COL_2_LIGHT].setBrightness(out.xPresents[1] == 0.f ? 0.f : 1.f);
  lights[COL_3_LIGHT].setBrightness(out.xPresents[2] == 0.f ? 0.f : 1.f);
  lights[COL_4_LIGHT].setBrightness(out.xPresents[3] == 0.f ? 0.f : 1.f);
  lights[COL_5_LIGHT].setBrightness(out.xPresents[4] == 0.f ? 0.f : 1.f);
  lights[COL_6_LIGHT].setBrightness(out.xPresents[5] == 0.f ? 0.f : 1.f);
  lights[COL_7_LIGHT].setBrightness(out.xPresents[6] == 0.f ? 0.f : 1.f);
  lights[COL_8_LIGHT].setBrightness(out.xPresents[7] == 0.f ? 0.f : 1.f);
  lights[COL_9_LIGHT].setBrightness(out.xPresents[8] == 0.f ? 0.f : 1.f);
  lights[COL_10_LIGHT].setBrightness(out.xPresents[9] == 0.f ? 0.f : 1.f);
}

void Mitosis::processBuffers() {
  while (!clickedCells.empty()) {
    std::pair<int, int> *c = clickedCells.shift();
    bool alive = golGrid->isAlive(c);
    golGrid->setCellState(c, !alive);
  }
  while (!loopParam.empty()) {
    bool loop = loopParam.shift();
    golGrid->setLoop(loop);
  }
  while (!muteRowsBuffer.empty()) {
    int row = muteRowsBuffer.shift();
    dsp->muteUnmuteRow(row, false);
    audibilityChanged = true;
  }
  while (!unmuteRowsBuffer.empty()) {
    int row = unmuteRowsBuffer.shift();
    dsp->muteUnmuteRow(row, true);
    audibilityChanged = true;
  }
  while (!muteColsBuffer.empty()) {
    int col = muteColsBuffer.shift();
    dsp->muteUnmuteCol(col, false);
    audibilityChanged = true;
  }
  while (!unmuteColsBuffer.empty()) {
    int col = unmuteColsBuffer.shift();
    dsp->muteUnmuteCol(col, true);
    audibilityChanged = true;
  }
  while (!muteUnmuteColsBuffer.empty()) {
    int col = muteUnmuteColsBuffer.shift();
    dsp->muteUnmuteCol(col, !dsp->isColAudible(col));
    audibilityChanged = true;
  }
  while (!muteUnmuteRowsBuffer.empty()) {
    int row = muteUnmuteRowsBuffer.shift();
    dsp->muteUnmuteRow(row, !dsp->isRowAudible(row));
    audibilityChanged = true;
  }
  while (!seedInitBuffer.empty()) {
    seedInitBuffer.shift();
    std::vector<std::pair<int, int> *> cells =
        GameOfLifeGrid::createRandomGrid();
    golGrid->init(cells);
  }
  while (!colStartBuffer.empty() && !colSizeBuffer.empty()) {
    colStart = colStartBuffer.shift();
    colSize = colSizeBuffer.shift();
    if (colLimitUp) {
      golGrid->setColLimit(colStart, colSize);
    }
  }
  while (!rowStartBuffer.empty() && !rowSizeBuffer.empty()) {
    rowStart = rowStartBuffer.shift();
    rowSize = rowSizeBuffer.shift();
    if (rowLimitUp) {
      golGrid->setRowLimit(rowStart, rowSize);
    }
  }
}

json_t *Mitosis::dataToJson() {

  json_t *rootJ = json_object();
  json_t *gridJ = json_array();
  json_t *grid1J = json_array();
  json_t *grid2J = json_array();
  json_t *gridLLJ = json_array();
  json_t *colAudibleJ = json_array();
  json_t *rowAudibleJ = json_array();
  json_t *miscValuesJ = json_array();
  // json_t *gridParams = json_array();
  // json_array_append_new(gridParams, json_boolean(loop));
  std::vector<std::pair<int, int> *> ca = golGrid->getCurrentlyAlive();
  for (std::vector<std::pair<int, int> *>::iterator it = ca.begin();
       it != ca.end(); ++it) {
    std::pair<int, int> *c = *it;
    json_array_append_new(gridJ, json_integer(c->first));
    json_array_append_new(gridJ, json_integer(c->second));
  }
  ca = saves[0];
  for (std::vector<std::pair<int, int> *>::iterator it = ca.begin();
       it != ca.end(); ++it) {
    std::pair<int, int> *c = *it;
    json_array_append_new(grid1J, json_integer(c->first));
    json_array_append_new(grid1J, json_integer(c->second));
  }
  ca = saves[1];
  for (std::vector<std::pair<int, int> *>::iterator it = ca.begin();
       it != ca.end(); ++it) {
    std::pair<int, int> *c = *it;
    json_array_append_new(grid2J, json_integer(c->first));
    json_array_append_new(grid2J, json_integer(c->second));
  }
  ca = lastLoadedData;
  for (std::vector<std::pair<int, int> *>::iterator it = ca.begin();
       it != ca.end(); ++it) {
    std::pair<int, int> *c = *it;
    json_array_append_new(gridLLJ, json_integer(c->first));
    json_array_append_new(gridLLJ, json_integer(c->second));
  }
  for (int i = 0; i < NUMCELLS_X; i++) {
    json_array_append_new(colAudibleJ, json_boolean(dsp->isColAudible(i)));
  }
  for (int i = 0; i < NUMCELLS_Y; i++) {
    json_array_append_new(rowAudibleJ, json_boolean(dsp->isRowAudible(i)));
  }
  json_array_append_new(miscValuesJ, json_boolean(golGrid->getLoop()));
  json_array_append_new(miscValuesJ, json_integer(lastLoaded));
  json_object_set_new(rootJ, "golGrid", gridJ);
  json_object_set_new(rootJ, "save1", grid1J);
  json_object_set_new(rootJ, "save2", grid2J);
  json_object_set_new(rootJ, "last loaded", gridLLJ);
  json_object_set_new(rootJ, "rowAudible", rowAudibleJ);
  json_object_set_new(rootJ, "colAudible", colAudibleJ);
  json_object_set_new(rootJ, "misc values", miscValuesJ);
  return rootJ;
}

void Mitosis::dataFromJson(json_t *rootJ) {
  // running
  json_t *gridJ = json_object_get(rootJ, "golGrid");
  json_t *grid1J = json_object_get(rootJ, "save1");
  json_t *grid2J = json_object_get(rootJ, "save2");
  json_t *gridLLJ = json_object_get(rootJ, "last loaded");
  json_t *rowAudibleJ = json_object_get(rootJ, "rowAudible");
  json_t *colAudibleJ = json_object_get(rootJ, "colAudible");
  json_t *switchesJ = json_object_get(rootJ, "misc values");
  golGrid->setLoop(json_is_true(json_array_get(switchesJ, 0)));
  lastLoaded = json_integer_value(json_array_get(switchesJ, 1));
  saves.clear();
  saves.push_back(std::vector<std::pair<int, int> *>());
  saves.push_back(std::vector<std::pair<int, int> *>());
  lastLoadedData.clear();
  if (gridJ && rowAudibleJ && colAudibleJ && grid1J && grid2J && gridLLJ) {
    int x;
    int y;
    for (unsigned int i = 0; i < json_array_size(gridJ) / 2; i++) {
      x = json_integer_value(json_array_get(gridJ, i * 2));
      y = json_integer_value(json_array_get(gridJ, i * 2 + 1));
      golGrid->setCellState(x, y, true);
    }
    for (unsigned int i = 0; i < json_array_size(grid1J) / 2; i++) {
      x = json_integer_value(json_array_get(grid1J, i * 2));
      y = json_integer_value(json_array_get(grid1J, i * 2 + 1));
      saves[0].push_back(GameOfLifeGrid::getCell(x, y));
    }
    for (unsigned int i = 0; i < json_array_size(grid2J) / 2; i++) {
      x = json_integer_value(json_array_get(grid2J, i * 2));
      y = json_integer_value(json_array_get(grid2J, i * 2 + 1));
      saves[1].push_back(GameOfLifeGrid::getCell(x, y));
    }
    for (unsigned int i = 0; i < json_array_size(gridLLJ) / 2; i++) {
      x = json_integer_value(json_array_get(gridLLJ, i * 2));
      y = json_integer_value(json_array_get(gridLLJ, i * 2 + 1));
      lastLoadedData.push_back(GameOfLifeGrid::getCell(x, y));
    }
    for (int i = 0; i < NUMCELLS_X; i++) {
      bool colAudible = json_is_true(json_array_get(colAudibleJ, i));
      dsp->muteUnmuteCol(i, colAudible);
    }
    for (int i = 0; i < NUMCELLS_Y; i++) {
      bool rowAudible = json_is_true(json_array_get(rowAudibleJ, i));
      dsp->muteUnmuteRow(i, rowAudible);
    }
  }
}

struct MitosisWidget : ModuleWidget {
  MitosisWidget(Mitosis *module) {
    setModule(module);
    setPanel(
        APP->window->loadSvg(asset::plugin(pluginInstance, "res/Mitosis.svg")));

    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(
        createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(
        Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<ScrewSilver>(Vec(
        box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    auto knob = createParamCentered<RoundSmallBlackKnob>(
        mm2px(Vec(28.6, 23.332)), module, Mitosis::DEAD_PARAM);
    knob->snap = true;
    knob->snapValue = 0.1f;
    addParam(knob);

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(59.591, 9.0)), module,
                                             Mitosis::CLOCK_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(90.602, 9.0)), module,
                                             Mitosis::RESET_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(173.116, 9.0)), module,
                                             Mitosis::ROW_LIMIT_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(194.116, 9.0)), module,
                                             Mitosis::COL_LIMIT_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.1, 98.0)), module,
                                             Mitosis::SAVE_1_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.1, 98.0)), module,
                                             Mitosis::LOAD_1_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(33.6, 99.75)), module,
                                             Mitosis::SEED_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.1, 114.0)), module,
                                             Mitosis::SAVE_2_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.1, 114.0)), module,
                                             Mitosis::LOAD_2_INPUT));

    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(119.865, 9.0)), module,
                                               Mitosis::DEAD_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(164.1, 29.45)), module,
                                               Mitosis::ROW_1_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(174.1, 29.45)), module,
                                               Mitosis::ROW_1_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(185.1, 29.45)), module,
                                               Mitosis::COL_1_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(195.1, 29.45)), module,
                                               Mitosis::COL_1_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(164.1, 39.35)), module,
                                               Mitosis::ROW_2_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(174.1, 39.35)), module,
                                               Mitosis::ROW_2_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(185.1, 39.35)), module,
                                               Mitosis::COL_2_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(195.1, 39.35)), module,
                                               Mitosis::COL_2_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(164.1, 49.25)), module,
                                               Mitosis::ROW_3_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(174.1, 49.25)), module,
                                               Mitosis::ROW_3_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(185.1, 49.25)), module,
                                               Mitosis::COL_3_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(195.1, 49.25)), module,
                                               Mitosis::COL_3_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(164.1, 59.15)), module,
                                               Mitosis::ROW_4_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(174.1, 59.15)), module,
                                               Mitosis::ROW_4_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(185.1, 59.15)), module,
                                               Mitosis::COL_4_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(195.1, 59.15)), module,
                                               Mitosis::COL_4_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(164.1, 69.05)), module,
                                               Mitosis::ROW_5_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(174.1, 69.05)), module,
                                               Mitosis::ROW_5_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(185.1, 69.05)), module,
                                               Mitosis::COL_5_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(195.1, 69.05)), module,
                                               Mitosis::COL_5_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(164.1, 78.95)), module,
                                               Mitosis::ROW_6_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(174.1, 78.95)), module,
                                               Mitosis::ROW_6_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(185.1, 78.95)), module,
                                               Mitosis::COL_6_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(195.1, 78.95)), module,
                                               Mitosis::COL_6_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(164.1, 88.85)), module,
                                               Mitosis::ROW_7_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(174.1, 88.85)), module,
                                               Mitosis::ROW_7_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(185.1, 88.85)), module,
                                               Mitosis::COL_7_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(195.1, 88.85)), module,
                                               Mitosis::COL_7_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(164.1, 98.75)), module,
                                               Mitosis::ROW_8_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(174.1, 98.75)), module,
                                               Mitosis::ROW_8_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(185.1, 98.75)), module,
                                               Mitosis::COL_8_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(195.1, 98.75)), module,
                                               Mitosis::COL_8_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(164.1, 108.65)),
                                               module, Mitosis::ROW_9_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(
        mm2px(Vec(174.1, 108.65)), module, Mitosis::ROW_9_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(185.1, 108.65)),
                                               module, Mitosis::COL_9_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(
        mm2px(Vec(195.1, 108.65)), module, Mitosis::COL_9_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(164.1, 118.55)),
                                               module, Mitosis::ROW_10_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(
        mm2px(Vec(174.1, 118.55)), module, Mitosis::ROW_10_P_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(185.1, 118.55)),
                                               module, Mitosis::COL_10_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(
        mm2px(Vec(195.1, 118.55)), module, Mitosis::COL_10_P_OUTPUT));

    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(161.1, 17.0)), module, Mitosis::ROW_1_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(165.1, 17.0)), module, Mitosis::ROW_3_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(169.1, 17.0)), module, Mitosis::ROW_5_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(173.1, 17.0)), module, Mitosis::ROW_7_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(177.1, 17.0)), module, Mitosis::ROW_9_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(182.1, 17.0)), module, Mitosis::COL_1_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(186.1, 17.0)), module, Mitosis::COL_3_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(190.1, 17.0)), module, Mitosis::COL_5_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(194.1, 17.0)), module, Mitosis::COL_7_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(198.1, 17.0)), module, Mitosis::COL_9_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(161.1, 21.0)), module, Mitosis::ROW_2_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(165.1, 21.0)), module, Mitosis::ROW_4_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(169.1, 21.0)), module, Mitosis::ROW_6_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(173.1, 21.0)), module, Mitosis::ROW_8_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(177.1, 21.0)), module, Mitosis::ROW_10_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(182.1, 21.0)), module, Mitosis::COL_2_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(186.1, 21.0)), module, Mitosis::COL_4_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(190.1, 21.0)), module, Mitosis::COL_6_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(194.1, 21.0)), module, Mitosis::COL_8_LIGHT));
    addChild(createLightCentered<MediumLight<GreenLight>>(
        mm2px(Vec(198.1, 21.0)), module, Mitosis::COL_10_LIGHT));

    addChild(createLightCentered<SmallLight<GreenLight>>(
        mm2px(Vec(15.1, 100.0)), module, Mitosis::LS_1_LIGHT));
    addChild(createLightCentered<SmallLight<GreenLight>>(
        mm2px(Vec(15.1, 116.0)), module, Mitosis::LS_2_LIGHT));
    addChild(createLightCentered<SmallLight<GreenLight>>(
        mm2px(Vec(33.6, 108.0)), module, Mitosis::SEED_LIGHT));

    addChild(createLightCentered<SmallLight<RedLight>>(
        mm2px(Vec(23.637, 22.722)), module, Mitosis::DEAD_3_LIGHT));
    addChild(createLightCentered<SmallLight<RedLight>>(
        mm2px(Vec(33.563, 22.722)), module, Mitosis::DEAD_8_LIGHT));
    addChild(createLightCentered<SmallLight<RedLight>>(
        mm2px(Vec(24.114, 25.539)), module, Mitosis::DEAD_2_LIGHT));
    addChild(createLightCentered<SmallLight<RedLight>>(
        mm2px(Vec(33.086, 25.54)), module, Mitosis::DEAD_9_LIGHT));
    addChild(createLightCentered<SmallLight<RedLight>>(
        mm2px(Vec(26.055, 27.636)), module, Mitosis::DEAD_1_LIGHT));
    addChild(createLightCentered<SmallLight<RedLight>>(
        mm2px(Vec(31.145, 27.636)), module, Mitosis::DEAD_10_LIGHT));
    addChild(createLightCentered<SmallLight<RedLight>>(
        mm2px(Vec(27.172, 18.54)), module, Mitosis::DEAD_5_LIGHT));
    addChild(createLightCentered<SmallLight<RedLight>>(
        mm2px(Vec(30.028, 18.54)), module, Mitosis::DEAD_6_LIGHT));
    addChild(createLightCentered<SmallLight<RedLight>>(
        mm2px(Vec(24.781, 20.105)), module, Mitosis::DEAD_4_LIGHT));
    addChild(createLightCentered<SmallLight<RedLight>>(
        mm2px(Vec(32.419, 20.105)), module, Mitosis::DEAD_7_LIGHT));

    // mute rows
    std::vector<float> xs;
    std::vector<float> ys;
    xs.push_back(4.1);
    xs.push_back(13.1);
    xs.push_back(22.1);
    xs.push_back(31.1);
    ys.push_back(37.5);
    ys.push_back(46.5);
    ys.push_back(55.5);
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 3; j++) {
        ScaleButton *button = new ScaleButton();
        button->module = module;
        button->box.pos = mm2px(Vec(xs[i], ys[j]));
        button->box.size = mm2px(Vec(7.0, 7.0));
        button->semitone = j * 4 + i;
        button->muteRb = &(module->muteRowsBuffer);
        button->unmuteRb = &(module->unmuteRowsBuffer);
        button->cols = false;
        addChild(button);
      }
    }
    // mute cols
    xs.clear();
    ys.clear();
    xs.push_back(4.1);
    xs.push_back(13.1);
    xs.push_back(22.1);
    xs.push_back(31.1);
    ys.push_back(65.5);
    ys.push_back(74.5);
    ys.push_back(83.5);
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 3; j++) {
        ScaleButton *button = new ScaleButton();
        button->module = module;
        button->box.pos = mm2px(Vec(xs[i], ys[j]));
        button->box.size = mm2px(Vec(7.0, 7.0));
        button->semitone = j * 4 + i;
        button->muteRb = &(module->muteColsBuffer);
        button->unmuteRb = &(module->unmuteColsBuffer);
        button->cols = true;
        addChild(button);
      }
    }
    GoLDisplay *display = new GoLDisplay();
    display->module = module;
    display->box.pos = mm2px(Vec(41.1, 15.5));
    display->box.size = mm2px(Vec(116.0, 107.0));
    addChild(display);

    SliderParam *lpWidget = new SliderParam();
    lpWidget->rb = &(module->loopParam);
    lpWidget->box.pos = mm2px(Vec(146.428, 7.1));
    lpWidget->box.size = mm2px(Vec(7.8, 3.8));
    addChild(lpWidget);

    SeedButton *seedButton = new SeedButton();
    seedButton->box.pos = mm2px(Vec(30.6, 113.25));
    seedButton->box.size = mm2px(Vec(6, 6));
    seedButton->circular = true;
    seedButton->color = COLOR_GREEN_BUTTON_O;
    seedButton->rb = &(module->seedInitBuffer);
    addChild(seedButton);
  }
};

void SeedButton::buttonReleased() {
  if (!rb->full()) {
    rb->push(1);
  }
}

Model *modelMitosis = createModel<Mitosis, MitosisWidget>("Mitosis");

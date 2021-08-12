#include "Mitosis.hpp"
#include "DSP.hpp"
#include "DataReceiver.hpp"
#include "DataSender.hpp"
#include "GameOfLifeGrid.hpp"
#include "GoLDisplay.hpp"
#include "ScaleButton.hpp"
#include "SliderParam.hpp"
#include "plugin.hpp"
#include <unordered_map>
#include <vector>

Mitosis::Mitosis() {
  config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
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
  float clock = inputs[CLOCK_INPUT].getVoltage();
  float rowVoct = inputs[ROW_VOCT_INPUT].getVoltage();
  float colVoct = inputs[COL_VOCT_INPUT].getVoltage();
  float load1 = inputs[LOAD_1_INPUT].getVoltage();
  float load2 = inputs[LOAD_2_INPUT].getVoltage();
  float load3 = inputs[LOAD_3_INPUT].getVoltage();
  float save1 = inputs[SAVE_1_INPUT].getVoltage();
  float save2 = inputs[SAVE_2_INPUT].getVoltage();
  float save3 = inputs[SAVE_3_INPUT].getVoltage();
  float reset = inputs[RESET_INPUT].getVoltage();
  float loadRnd = inputs[INIT_RND_INPUT].getVoltage();
  // init
  // save and load can only be triggered if all of them are low beforehand.
  if (save1 < 1.5f && save2 < 1.5f && save3 < 1.5f) {
    saveHigh = false;
  }
  if (load1 < 1.5f && load2 < 1.5f && load3 < 1.5f && loadRnd < 1.5f &&
      reset < 1.5f) {
    loadHigh = false;
  }
  // save
  if (!saveHigh && (save1 > 3.5f || save2 > 3.5f || save3 > 3.5f)) {
    saveHigh = true;
    if (save1 > 3.5f) {
      lastLoaded = 1;
    } else if (save2 > 3.5f) {
      lastLoaded = 2;
    } else if (save3 > 3.5f) {
      lastLoaded = 3;
    }
    lastLoadedData = golGrid->getCurrentlyAlive();
    saves[lastLoaded - 1] = lastLoadedData;
  }
  if (!loadHigh && (load1 > 3.5f || load2 > 3.5f || load3 > 3.5f ||
                    loadRnd > 3.5f || reset > 3.5f)) {
    loadHigh = true;
    std::vector<std::pair<int, int> *> cells;
    if (load1 > 3.5f) {
      lastLoaded = 1;
      cells = saves[0];
    } else if (load2 > 3.5f) {
      lastLoaded = 2;
      cells = saves[1];
    } else if (load3 > 3.5f) {
      lastLoaded = 3;
      cells = saves[2];
    } else if (loadRnd > 3.5f) {
      lastLoaded = 4;
      cells = GameOfLifeGrid::createRandomGrid();
    } else if (reset > 3.5f) {
      // reset
      cells = lastLoadedData;
    }
    lastLoadedData = cells;
    golGrid->init(cells);
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
  // DSP outputs
  std::vector<std::pair<int, int> *> state = golGrid->getCurrentlyAlive();
  dsp->paramValues(state, rowVoct, colVoct);
  if (dsp->isOutputChanged()) {
    out = dsp->getOutputs();
    dsp->resetOutputChanged();
  }
  // misc outputs
  outputs[DEAD_OUTPUT].setVoltage(golGrid->isStillEvolving() ? 0.f : 10.f);
  // load/save lights
  lights[LS_1_LIGHT].setBrightness(lastLoaded == 1 ? 1.f : 0.f);
  lights[LS_2_LIGHT].setBrightness(lastLoaded == 2 ? 1.f : 0.f);
  lights[LS_3_LIGHT].setBrightness(lastLoaded == 3 ? 1.f : 0.f);
  lights[INIT_RND_LIGHT].setBrightness(lastLoaded == 4 ? 1.f : 0.f);
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
  }
  while (!unmuteRowsBuffer.empty()) {
    int row = unmuteRowsBuffer.shift();
    dsp->muteUnmuteRow(row, true);
  }
  while (!muteColsBuffer.empty()) {
    int col = muteColsBuffer.shift();
    dsp->muteUnmuteCol(col, false);
  }
  while (!unmuteColsBuffer.empty()) {
    int col = unmuteColsBuffer.shift();
    dsp->muteUnmuteCol(col, true);
  }
  while (!muteUnmuteColsBuffer.empty()) {
    int col = muteUnmuteColsBuffer.shift();
    dsp->muteUnmuteCol(col, !dsp->isColAudible(col));
  }
  while (!muteUnmuteRowsBuffer.empty()) {
    int row = muteUnmuteRowsBuffer.shift();
    dsp->muteUnmuteRow(row, !dsp->isRowAudible(row));
  }
}

json_t *Mitosis::dataToJson() {

  json_t *rootJ = json_object();
  json_t *gridJ = json_array();
  json_t *colAudibleJ = json_array();
  json_t *rowAudibleJ = json_array();
  json_t *switchesJ = json_array();
  // json_t *gridParams = json_array();
  // json_array_append_new(gridParams, json_boolean(loop));
  for (int i = 0; i < NUMCELLS_X; i++) {
    for (int j = 0; j < NUMCELLS_Y; j++) {
      json_array_append_new(gridJ, json_boolean(golGrid->isAlive(i, j)));
    }
  }
  for (int i = 0; i < NUMCELLS_X; i++) {
    json_array_append_new(colAudibleJ, json_boolean(dsp->isColAudible(i)));
  }
  for (int i = 0; i < NUMCELLS_Y; i++) {
    json_array_append_new(rowAudibleJ, json_boolean(dsp->isRowAudible(i)));
  }
  json_array_append_new(switchesJ, json_boolean(golGrid->getLoop()));
  json_object_set_new(rootJ, "golGrid", gridJ);
  json_object_set_new(rootJ, "rowAudible", rowAudibleJ);
  json_object_set_new(rootJ, "colAudible", colAudibleJ);
  json_object_set_new(rootJ, "switches", switchesJ);
  return rootJ;
}

void Mitosis::dataFromJson(json_t *rootJ) {
  // running
  json_t *gridJ = json_object_get(rootJ, "golGrid");
  json_t *rowAudibleJ = json_object_get(rootJ, "rowAudible");
  json_t *colAudibleJ = json_object_get(rootJ, "colAudible");
  if (gridJ && rowAudibleJ && colAudibleJ) {
    for (int i = 0; i < NUMCELLS_X; i++) {
      for (int j = 0; j < NUMCELLS_Y; j++) {
        bool value = json_is_true(json_array_get(gridJ, NUMCELLS_Y * i + j));
        golGrid->setCellState(i, j, value);
      }
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
  json_t *switchesJ = json_object_get(rootJ, "switches");
  golGrid->setLoop(json_is_true(json_array_get(switchesJ, 0)));
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

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(59.591, 9.0)), module,
                                             Mitosis::CLOCK_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(90.602, 9.0)), module,
                                             Mitosis::RESET_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(174.326, 9.0)), module,
                                             Mitosis::ROW_VOCT_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(195.326, 9.0)), module,
                                             Mitosis::COL_VOCT_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(33.585, 21.5)), module,
                                             Mitosis::INIT_RND_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(33.6, 97.5)), module,
                                             Mitosis::SAVE_3_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.1, 98.0)), module,
                                             Mitosis::SAVE_1_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.1, 98.0)), module,
                                             Mitosis::LOAD_1_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.1, 114.0)), module,
                                             Mitosis::SAVE_2_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.1, 114.0)), module,
                                             Mitosis::LOAD_2_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(33.6, 118.5)), module,
                                             Mitosis::LOAD_3_INPUT));

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
        mm2px(Vec(33.585, 31.5)), module, Mitosis::INIT_RND_LIGHT));
    addChild(createLightCentered<SmallLight<GreenLight>>(
        mm2px(Vec(15.1, 100.0)), module, Mitosis::LS_1_LIGHT));
    addChild(createLightCentered<SmallLight<GreenLight>>(
        mm2px(Vec(33.6, 108.0)), module, Mitosis::LS_3_LIGHT));
    addChild(createLightCentered<SmallLight<GreenLight>>(
        mm2px(Vec(15.1, 116.0)), module, Mitosis::LS_2_LIGHT));

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
  }
};

Model *modelMitosis = createModel<Mitosis, MitosisWidget>("Mitosis");

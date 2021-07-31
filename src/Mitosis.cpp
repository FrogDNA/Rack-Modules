#include "Mitosis.hpp"
#include "Coordinate.hpp"
#include "DataReceiver.hpp"
#include "DataSender.hpp"
#include "GameOfLifeGrid.hpp"
#include "GoLDisplay.hpp"
#include "SliderParam.hpp"
#include "plugin.hpp"
#include <unordered_map>
#include <vector>

Mitosis::Mitosis() {
  config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
  configParam(TEMP_PARAM, 0.f, 1.f, 0.5f,
              "Roundness of the harmonic amplitude decay function");
  configParam(FOOD_PARAM, 0.f, 1.f, 0.f, "Wideness of the harmonics range");
  clockUp = false;
  golGrid = new GameOfLifeGrid();
  golGrid->init();
  dataSender = new DataSender();
  dataReceiver = new DataReceiver();
  dsp = new DSP();
  // for debugging
  // setbuf(stdout, NULL);
}

void Mitosis::process(const ProcessArgs &args) {
  float clock = inputs[CLOCK_INPUT].getVoltage();
  float vOct = inputs[VOCT_INPUT].getVoltage();
  float send = inputs[SEND_INPUT].getVoltage();
  float busyIn = inputs[BUSYIN_INPUT].getVoltage();
  float clockIn = inputs[DATACLKIN_INPUT].getVoltage();
  float dataIn = inputs[DATAIN_INPUT].getVoltage();
  float temp_roundness = params[TEMP_PARAM].getValue();
  float food_wideness = params[FOOD_PARAM].getValue();
  float dataOut = 0.f;
  float ClockOut = 0.f;
  bool risingEdge = false;
  // data receive
  dataReceiver->check(busyIn, clockIn, dataIn);
  /* clock and AM. */
  /* todo check if AM really brings something */
  if (busyIn < 1.5f) {
    // ignore clock if busyIn is high
    if (clock > 3.5f) {
      risingEdge = (clockUp == false ? true : false);
      clockUp = true;
    } else if (clock < 1.5f) {
      clockUp = false;
      risingEdge = false;
    }
    if (risingEdge) {
      if (dataReceiver->isNewGridReady()) {
        std::vector<Cell *> v =
            dataReceiver->getGrid()->getCurrentlyAlive().currentlyAlive;
        golGrid->init(v);
      } else {
        golGrid->update(loop);
      }
    }
  }
  // check if GUI has some messages
  if (!clickedCells.empty()) {
    Coordinate *c = clickedCells.shift();
    bool alive = golGrid->getCell(c->getX(), c->getY())->isAlive();
    golGrid->setCellState(c->getX(), c->getY(), !alive);
  }
  if (!loopParam.empty()) {
    loop = loopParam.shift();
  }
  // *** PROCESS AUDIO ***
  GridState gs = golGrid->getCurrentlyAlive();
  golGrid->resetModified();
  dsp->paramValues(gs, food_wideness, temp_roundness, vOct);
  float audio = dsp->nextValue(args.sampleTime);
  // data send
  if (send > 3.5f && hasResetSend) {
    if (!dataSender->isTransferInProgress()) {
      dataSender->init(golGrid->getCurrentlyAlive().currentlyAlive);
      hasResetSend = false;
    }
  }
  if (send < 1.5f) {
    hasResetSend = true;
  }
  dataSender->next();
  dataOut = dataSender->getData();
  ClockOut = dataSender->getClock();
  // vOctOut
  float lf = dsp->getLowestFreq();
  if (lastDspLowestFreq != lf) {
    lastDspLowestFreq = lf;
    vOctOut = log2(lf / 16.35f) + vOct;
    vOctOut = clamp(vOctOut, 0.f, 10.f);
  }
  // outputs
  outputs[BUSY_OUTPUT].setVoltage(dataSender->isTransferInProgress() ? 10.f
                                                                     : 0.f);
  outputs[DEAD_OUTPUT].setVoltage(golGrid->isStillEvolving() ? 0.f : 10.f);
  outputs[DATACLK_OUTPUT].setVoltage(ClockOut);
  outputs[DATA_OUTPUT].setVoltage(dataOut);
  outputs[AUDIO_OUTPUT].setVoltage(5.f * audio);
  outputs[VOCTOUT_OUTPUT].setVoltage(vOctOut);
}

json_t *Mitosis::dataToJson() {

  json_t *rootJ = json_object();
  json_t *gridJ = json_array();
  json_t *rowMutedJ = json_array();
  json_t *colMutedJ = json_array();
  // json_t *gridParams = json_array();
  // json_array_append_new(gridParams, json_boolean(loop));
  for (int i = 0; i < NUMCELLSX; i++) {
    for (int j = 0; j < NUMCELLSY; j++) {
      json_array_append_new(gridJ,
                            json_boolean(golGrid->getCell(i, j)->isAlive()));
      json_array_append_new(rowMutedJ,
                            json_boolean(golGrid->getCell(i, j)->rowMuted));
      json_array_append_new(colMutedJ,
                            json_boolean(golGrid->getCell(i, j)->colMuted));
    }
  }
  json_object_set_new(rootJ, "golGrid", gridJ);
  json_object_set_new(rootJ, "rowMuted", rowMutedJ);
  json_object_set_new(rootJ, "colMuted", colMutedJ);
  // json_object_set_new(rootJ, "gridParams", gridParams);
  return rootJ;
}

void Mitosis::dataFromJson(json_t *rootJ) {
  // running
  json_t *gridJ = json_object_get(rootJ, "golGrid");
  json_t *rowMutedJ = json_object_get(rootJ, "rowMuted");
  json_t *colMutedJ = json_object_get(rootJ, "colMuted");
  // json_t *gridParams = json_object_get(rootJ, "gridParams");
  // loop = json_is_true(json_array_get(gridParams, 0));
  if (gridJ && rowMutedJ && colMutedJ) {
    for (int i = 0; i < NUMCELLSX; i++) {
      for (int j = 0; j < NUMCELLSY; j++) {
        bool value = json_is_true(json_array_get(gridJ, NUMCELLSX * i + j));
        bool rowMuted =
            json_is_true(json_array_get(rowMutedJ, NUMCELLSX * i + j));
        bool colMuted =
            json_is_true(json_array_get(colMutedJ, NUMCELLSX * i + j));
        golGrid->setCellState(i, j, value);
        golGrid->getCell(i, j)->rowMuted = rowMuted;
        golGrid->getCell(i, j)->colMuted = colMuted;
      }
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

    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.89, 28.819)),
                                                 module, Mitosis::TEMP_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(35.67, 28.977)),
                                                 module, Mitosis::FOOD_PARAM));

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(35.67, 44.323)), module,
                                             Mitosis::VOCT_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.89, 45.223)), module,
                                             Mitosis::CLOCK_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.57, 100.267)), module,
                                             Mitosis::SEND_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.633, 113.239)),
                                             module, Mitosis::DATAIN_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.927, 114.077)), module,
                                             Mitosis::BUSYIN_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(23.78, 114.078)), module,
                                             Mitosis::DATACLKIN_INPUT));

    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.927, 81.327)),
                                               module, Mitosis::DEAD_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(
        mm2px(Vec(23.78, 81.327)), module, Mitosis::VOCTOUT_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(37.633, 81.327)),
                                               module, Mitosis::AUDIO_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(16.71, 100.267)),
                                               module, Mitosis::BUSY_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(
        mm2px(Vec(27.85, 100.267)), module, Mitosis::DATACLK_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.99, 100.267)),
                                               module, Mitosis::DATA_OUTPUT));

    // mm2px(Vec(7.786, 3.786))
    // infinity
    // addChild(createWidget<Widget>(mm2px(Vec(134.446, 3.607))));
    // mm2px(Vec(7.985, 3.985))
    /*
    addChild(createWidget<Widget>(mm2px(Vec(4.202, 56.819))));
    // mm2px(Vec(7.985, 3.985))
    addChild(createWidget<Widget>(mm2px(Vec(14.592, 56.82))));
    // mm2px(Vec(7.985, 3.985))
    addChild(createWidget<Widget>(mm2px(Vec(24.982, 56.82))));
    // mm2px(Vec(7.985, 3.985))
    addChild(createWidget<Widget>(mm2px(Vec(35.372, 56.82))));
    // mm2px(Vec(7.985, 3.985))
    addChild(createWidget<Widget>(mm2px(Vec(4.202, 62.05))));
    // mm2px(Vec(7.985, 3.985))
    addChild(createWidget<Widget>(mm2px(Vec(14.592, 62.05))));
    // mm2px(Vec(7.985, 3.985))
    addChild(createWidget<Widget>(mm2px(Vec(24.982, 62.05))));
    // mm2px(Vec(7.985, 3.985))
    addChild(createWidget<Widget>(mm2px(Vec(35.372, 62.05))));
    // mm2px(Vec(7.985, 3.985))
    addChild(createWidget<Widget>(mm2px(Vec(4.202, 67.28))));
    // mm2px(Vec(7.985, 3.985))
    addChild(createWidget<Widget>(mm2px(Vec(14.592, 67.28))));
    // mm2px(Vec(7.985, 3.985))
    addChild(createWidget<Widget>(mm2px(Vec(24.982, 67.28))));
    // mm2px(Vec(7.985, 3.985))
    addChild(createWidget<Widget>(mm2px(Vec(35.372, 67.28))));
*/
    GolDisplay *display = new GolDisplay();
    display->module = module;
    display->box.pos = mm2px(Vec(49.06, 12.5));
    display->box.size = mm2px(Vec(110.0, 110.0));
    addChild(display);

    SliderParam *lpWidget = new SliderParam();
    lpWidget->module = module;
    lpWidget->box.pos = mm2px(Vec(75.061, 3.6));
    lpWidget->box.size = mm2px(Vec(7.8, 3.8));
    addChild(lpWidget);
  }
};

Model *modelMitosis = createModel<Mitosis, MitosisWidget>("Mitosis");

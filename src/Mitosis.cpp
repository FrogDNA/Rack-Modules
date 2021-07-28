#include "Mitosis.hpp"
#include "Coordinate.hpp"
#include "DataReceiver.hpp"
#include "DataSender.hpp"
#include "GameOfLifeGrid.hpp"
#include "GoLDisplay.hpp"
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
  golGrid->defaultInit();
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
  dataReceiver->checkAndUpdateGrid(busyIn, clockIn, dataIn, golGrid);
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
      golGrid->update();
    }
  }
  // process audio
  GridState gs = golGrid->getCurrentlyAlive();
  golGrid->resetModified();
  // todo do it only when clock or click
  dsp->paramValues(gs, food_wideness, temp_roundness, vOct);
  float audio = dsp->nextValue(args.sampleTime);
  // printf("audioReturn %f \n", audio);
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
  // outputs
  outputs[BUSY_OUTPUT].setVoltage(dataSender->isTransferInProgress() ? 10.f
                                                                     : 0.f);
  outputs[DEAD_OUTPUT].setVoltage(golGrid->isStillEvolving() ? 0.f : 10.f);
  outputs[DATACLK_OUTPUT].setVoltage(ClockOut);
  outputs[DATA_OUTPUT].setVoltage(dataOut);
  outputs[AUDIO_OUTPUT].setVoltage(5.f * audio);
  lights[CLOCKLIGHT_LIGHT].setBrightness(clockUp == true ? 1.0f : 0.0f);
}

json_t *Mitosis::dataToJson() {

  json_t *rootJ = json_object();
  json_t *gridJ = json_array();
  for (int i = 0; i < NUMCELLSX; i++) {
    for (int j = 0; j < NUMCELLSY; j++) {
      json_array_append_new(gridJ,
                            json_boolean(golGrid->getCell(i, j)->isAlive()));
    }
  }
  json_object_set_new(rootJ, "golGrid", gridJ);
  return rootJ;
}

void Mitosis::dataFromJson(json_t *rootJ) {
  // running
  json_t *recorderJ = json_object_get(rootJ, "golGrid");
  if (recorderJ) {
    for (int i = 0; i < NUMCELLSX; i++) {
      for (int j = 0; j < NUMCELLSY; j++) {
        bool value = json_is_true(json_array_get(recorderJ, NUMCELLSX * i + j));
        golGrid->setCellState(i, j, value);
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

    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.368, 56.297)),
                                                 module, Mitosis::TEMP_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(35.505, 71.012)),
                                                 module, Mitosis::FOOD_PARAM));

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(95.292, 5.151)), module,
                                             Mitosis::CLOCK_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.966, 33.225)), module,
                                             Mitosis::VOCT_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(29.637, 104.157)),
                                             module, Mitosis::BUSYIN_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.137, 104.157)),
                                             module, Mitosis::DATACLKIN_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.637, 113.333)),
                                             module, Mitosis::SEND_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.046, 113.333)),
                                             module, Mitosis::DATAIN_INPUT));

    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(136.038, 5.151)),
                                               module, Mitosis::AUDIO_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(36.963, 48.73)),
                                               module, Mitosis::DEAD_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.637, 104.157)),
                                               module, Mitosis::BUSY_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(
        mm2px(Vec(20.137, 104.157)), module, Mitosis::DATACLK_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.137, 113.333)),
                                               module, Mitosis::DATA_OUTPUT));

    addChild(createLightCentered<MediumLight<RedLight>>(
        mm2px(Vec(104.288, 5.151)), module, Mitosis::CLOCKLIGHT_LIGHT));
    addChild(createLightCentered<MediumLight<RedLight>>(
        mm2px(Vec(29.637, 113.333)), module, Mitosis::BUSYINLIGHT_LIGHT));

    // mm2px(Vec(110.0, 110.0))
    GolDisplay *display = new GolDisplay();
    display->module = module;
    display->box.pos = mm2px(Vec(49.288, 11.674));
    display->box.size = mm2px(Vec(110.0, 110.0));
    addChild(display);
    // addChild(createWidget<Widget>(mm2px(Vec(49.288, 11.674))));
  }
};

Model *modelMitosis = createModel<Mitosis, MitosisWidget>("Mitosis");

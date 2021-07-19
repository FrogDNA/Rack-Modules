#include "Mitosis.hpp"
#include "Coordinate.hpp"
#include "DataReceiver.hpp"
#include "DataSender.hpp"
#include "GameOfLifeGrid.hpp"
#include "plugin.hpp"
#include <unordered_map>
#include <vector>

struct Mitosis : Module {
  GameOfLifeGrid *golGrid = NULL;
  DataSender *dataSender = NULL;
  DataReceiver *dataReceiver = NULL;
  int baseFreqPos = 10;
  float baseFreq = 440.0;              // todo modulate or configure this
  float enveloppeTotalDuration = .02f; // todo modulate or configure this
  float modulationPhase = .0f;         // todo modulate or configure this
  float time = 0.f;
  float envelopeTime = 0.f;
  bool isEnvelopeActive = false;
  bool golUpdateArmed = true;
  bool clockUp;
  bool hasResetSend = true;

  enum ParamIds { TUNE_PARAM, NUM_PARAMS };
  enum InputIds {
    CLOCK_INPUT,
    VOCT_INPUT,
    BUSYIN_INPUT,
    DATACLKIN_INPUT,
    SEND_INPUT,
    DATAIN_INPUT,
    NUM_INPUTS
  };
  enum OutputIds {
    AUDIO_OUTPUT,
    BUSY_OUTPUT,
    DATACLK_OUTPUT,
    DATA_OUTPUT,
    NUM_OUTPUTS
  };
  enum LightIds {
    CLOCKLIGHT_LIGHT,
    EMPTYLIGHT_LIGHT,
    BUSYINLIGHT_LIGHT,
    NUM_LIGHTS
  };

  Mitosis() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    configParam(TUNE_PARAM, -1.f, 1.f, 0.f, "");
    clockUp = false;
    golGrid = new GameOfLifeGrid();
    golGrid->defaultInit();
    dataSender = new DataSender();
    dataReceiver = new DataReceiver();
    // for debugging
    // setbuf(stdout, NULL);
  }

  void process(const ProcessArgs &args) override {
    float clock = inputs[CLOCK_INPUT].getVoltage();
    float vOct = inputs[VOCT_INPUT].getVoltage();
    float send = inputs[SEND_INPUT].getVoltage();
    float busyIn = inputs[BUSYIN_INPUT].getVoltage();
    float clockIn = inputs[DATACLKIN_INPUT].getVoltage();
    float dataIn = inputs[DATAIN_INPUT].getVoltage();
    float tune = params[TUNE_PARAM].getValue();

    float dataOut = 0.f;
    float ClockOut = 0.f;
    bool risingEdge = false;
    time += args.sampleTime;
    float am = 1.0f;
    // data receive
    dataReceiver->checkAndUpdateGrid(busyIn, clockIn, dataIn, golGrid);
    /* clock and AM. */
    /* todo check if AM really brings something */
    if (clock > 3.5f) {
      risingEdge = (clockUp == false ? true : false);
      clockUp = true;
    } else if (clock < 1.5f) {
      clockUp = false;
      risingEdge = false;
    }
    if (risingEdge && !isEnvelopeActive) {
      envelopeTime = 0.f;
      isEnvelopeActive = true;
    }
    if (isEnvelopeActive) {
      envelopeTime += args.sampleTime;
      am = 0.5f +
           0.5f * std::cos(envelopeTime * 2.f * M_PI / enveloppeTotalDuration);
      if (envelopeTime >= (enveloppeTotalDuration / 2.f) && golUpdateArmed) {
        golUpdateArmed = false;
        golGrid->update();
        time = 0.f;
      }
      if (envelopeTime >= enveloppeTotalDuration) {
        isEnvelopeActive = false;
        golUpdateArmed = true;
      }
    }
    // process audio
    float audio = processAudio(golGrid->getCurrentlyAlive(), tune, vOct);
    // data send
    if (send > 3.5f && hasResetSend) {
      if (!dataSender->isTransferInProgress()) {
        dataSender->init(golGrid->getCurrentlyAlive());
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
    outputs[DATACLK_OUTPUT].setVoltage(ClockOut);
    outputs[DATA_OUTPUT].setVoltage(dataOut);
    outputs[AUDIO_OUTPUT].setVoltage(5.f * am * audio);
    lights[CLOCKLIGHT_LIGHT].setBrightness(clockUp == true ? 1.0f : 0.0f);
  }
  /**
  returns float between 0 and 1
  todo use lookup tables to avoid computing sin each time
  */
  float processAudio(std::set<Cell *> alive, float tune, float vOct) {
    float audio = 0.f;
    float count = 0.f;
    // printf("time %f \n", time);
    for (std::set<Cell *>::iterator it = alive.begin(); it != alive.end();
         ++it) {
      float modulation = ((float)((*it)->getY()) - (float)NUMCELLSY / 2) *
                         tune / (float)NUMCELLSY;
      // base frequency
      float freq =
          baseFreq *
          pow(2.0f, (float)((*it)->getX() - NUMCELLSX / 2) / 12.0f + vOct);
      freq = freq * (1.0f + modulation);
      float partAudio = std::sin(2.0f * M_PI * freq * time);
      audio += partAudio;
      count += 1.f;
    }
    if (count != 0.f) {
      audio = audio / count;
    }
    // printf("audio return %f \n", audio);
    return audio;
  }
};

struct GolDisplay : OpaqueWidget {
  Mitosis *module = NULL;
  float offsetX = 0.f;
  float offsetY = 0.f;
  float sizeX = 0.f;
  float sizeY = 0.f;
  float cellSizeX = 0.f;
  float cellSizeY = 0.f;
  float cellSpaceX = 0.f;
  float cellSpaceY = 0.f;
  float numCellsX = 0.f;
  float numCellsY = 0.f;
  bool firstDraw = true;

  GolDisplay() {
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

  void draw(const DrawArgs &args) override {
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
};

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

    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(17.855, 38.076)),
                                                 module, Mitosis::TUNE_PARAM));

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(115.716, 5.151)), module,
                                             Mitosis::CLOCK_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.219, 26.704)), module,
                                             Mitosis::VOCT_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(29.637, 104.157)),
                                             module, Mitosis::BUSYIN_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.137, 104.157)),
                                             module, Mitosis::DATACLKIN_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.637, 113.333)),
                                             module, Mitosis::SEND_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.046, 113.333)),
                                             module, Mitosis::DATAIN_INPUT));

    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(156.461, 5.151)),
                                               module, Mitosis::AUDIO_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.637, 104.157)),
                                               module, Mitosis::BUSY_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(
        mm2px(Vec(20.137, 104.157)), module, Mitosis::DATACLK_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.137, 113.333)),
                                               module, Mitosis::DATA_OUTPUT));

    addChild(createLightCentered<MediumLight<RedLight>>(
        mm2px(Vec(124.711, 5.151)), module, Mitosis::CLOCKLIGHT_LIGHT));
    addChild(createLightCentered<MediumLight<RedLight>>(
        mm2px(Vec(38.011, 31.03)), module, Mitosis::EMPTYLIGHT_LIGHT));
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

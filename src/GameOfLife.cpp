#include "GameOfLife.hpp"
#include "Coordinate.hpp"
#include "GameOfLifeGrid.hpp"
#include "plugin.hpp"
#include <unordered_map>
#include <vector>

struct GameOfLife : Module {
  GameOfLifeGrid *golGrid;
  int baseFreqPos = 10;
  float baseFreq = 440.0;              // todo modulate or configure this
  float enveloppeTotalDuration = .02f; // todo modulate or configure this
  float modulationPhase = .0f;         // todo modulate or configure this
  float time = 0.f;
  float envelopeTime = 0.f;
  bool isEnvelopeActive = false;
  bool golUpdateArmed = true;
  bool clockUp;

  enum ParamIds { TUNE_PARAM, NUM_PARAMS };
  enum InputIds { SEND_INPUT, VOCT_INPUT, CLOCK_INPUT, NUM_INPUTS };
  enum OutputIds { DATACLK_OUTPUT, DATA_OUTPUT, AUDIO_OUTPUT, NUM_OUTPUTS };
  enum LightIds {
    DATALIGHT_LIGHT,
    AUDIOLIGHT_LIGHT,
    CLOCKLIGHT_LIGHT,
    NUM_LIGHTS
  };

  GameOfLife() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    configParam(TUNE_PARAM, -1.f, 1.f, 0.f, "");
    clockUp = false;
    golGrid = new GameOfLifeGrid();
    golGrid->defaultInit();
  }

  void process(const ProcessArgs &args) override {
    float clock = inputs[CLOCK_INPUT].getVoltage();
    float vOct = inputs[VOCT_INPUT].getVoltage();
    float tune = params[TUNE_PARAM].getValue();
    bool risingEdge = false;
    time += args.sampleTime;
    float am = 1.0f;
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
    float audio = processAudio(golGrid->getCurrentlyAlive(), tune, vOct);
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
  GameOfLife *module;
  float offsetX;
  float offsetY;
  float sizeX;
  float sizeY;
  float cellSizeX;
  float cellSizeY;
  float cellSpaceX;
  float cellSpaceY;
  float numCellsX;
  float numCellsY;
  bool firstDraw;

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
    cell->setAlive(!cell->isAlive());
    e.consume(this);
  }
  Widget::onButton(e);
}

struct GameOfLifeWidget : ModuleWidget {
  GameOfLifeWidget(GameOfLife *module) {
    setModule(module);
    setPanel(APP->window->loadSvg(
        asset::plugin(pluginInstance, "res/GameOfLife.svg")));

    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(
        createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(
        Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<ScrewSilver>(Vec(
        box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    addParam(createParamCentered<RoundBlackKnob>(
        mm2px(Vec(7.971, 99.698)), module, GameOfLife::TUNE_PARAM));

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.956, 47.826)), module,
                                             GameOfLife::SEND_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.971, 88.584)), module,
                                             GameOfLife::VOCT_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(23.913, 99.697)), module,
                                             GameOfLife::CLOCK_INPUT));

    addOutput(createOutputCentered<PJ301MPort>(
        mm2px(Vec(35.869, 32.723)), module, GameOfLife::DATACLK_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(
        mm2px(Vec(35.869, 47.826)), module, GameOfLife::DATA_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(
        mm2px(Vec(39.854, 99.698)), module, GameOfLife::AUDIO_OUTPUT));

    addChild(createLightCentered<MediumLight<RedLight>>(
        mm2px(Vec(11.956, 32.724)), module, GameOfLife::DATALIGHT_LIGHT));
    addChild(createLightCentered<MediumLight<RedLight>>(
        mm2px(Vec(39.854, 88.056)), module, GameOfLife::AUDIOLIGHT_LIGHT));
    addChild(createLightCentered<MediumLight<RedLight>>(
        mm2px(Vec(23.913, 88.584)), module, GameOfLife::CLOCKLIGHT_LIGHT));

    // mm2px(Vec(110.0, 110.0))
    GolDisplay *display = new GolDisplay();
    display->module = module;
    display->box.pos = mm2px(Vec(49.288, 11.674));
    display->box.size = mm2px(Vec(110.0, 110.0));
    addChild(display);
    // addChild(createWidget<Widget>(mm2px(Vec(49.288, 11.674))));
  }
};

Model *modelGameOfLife =
    createModel<GameOfLife, GameOfLifeWidget>("GameOfLife");

#include "GameOfLife.hpp"
#include "plugin.hpp"
#include <unordered_map>
#include <vector>

Coordinate::Coordinate(int aX, int aY) {
  x = aX;
  y = aY;
}

bool Coordinate::isEqual(Coordinate c) { return (c.x == x && c.y == y); }

struct GolState {
  std::vector<Coordinate> aliveCells;
  std::vector<Coordinate> toUpdateNextStep;

  GolState(std::vector<Coordinate> v1, std::vector<Coordinate> v2) {
    aliveCells = v1;
    toUpdateNextStep = v2;
  }

  GolState() {
    aliveCells = std::vector<Coordinate>();
    toUpdateNextStep = std::vector<Coordinate>();
  }
};

struct GameOfLife : Module {

  int baseFreqPos = 10;
  float baseFreq = 440.0;              // todo modulate or configure this
  float enveloppeTotalDuration = .02f; // todo modulate or configure this
  float modulationPhase = .0f;         // todo modulate or configure this
  float time = 0.f;
  float envelopeTime = 0.f;
  bool isEnvelopeActive = false;
  bool golUpdateArmed = true;
  bool clockUp;
  GolState gol;
  float sinLut[1000000];

  enum ParamIds { MODINDEXATV_PARAM, NUM_PARAMS };
  enum InputIds {
    SENDIN_INPUT,
    VOCTIN_INPUT,
    FMIN_INPUT,
    MODINDEXIN_INPUT,
    CLOCK_INPUT,
    NUM_INPUTS
  };
  enum OutputIds {
    DATACLOCKOUT_OUTPUT,
    DATAOUT_OUTPUT,
    AUDIO_OUTPUT,
    NUM_OUTPUTS
  };
  enum LightIds {
    DATALIGHT_LIGHT,
    AUDIOLIGHT_LIGHT,
    CLOCKLIGHT_LIGHT,
    NUM_LIGHTS
  };

  GameOfLife() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    configParam(MODINDEXATV_PARAM, -0.1f, 0.1f, 0.f, "");
    for (int i = 0; i < 1000000; i++) {
      sinLut[i] = std::sin(2 * M_PI * static_cast<float>(i) / 1000000.f);
    }
    clockUp = false;
    std::vector<Coordinate> starter;
    starter.push_back(Coordinate(1, 0));
    starter.push_back(Coordinate(2, 1));
    starter.push_back(Coordinate(0, 2));
    starter.push_back(Coordinate(1, 2));
    starter.push_back(Coordinate(2, 2));
    /*starter.push_back(Coordinate(14, 19));
    starter.push_back(Coordinate(14, 20));
    starter.push_back(Coordinate(14, 21));
    starter.push_back(Coordinate(27, 20));
    starter.push_back(Coordinate(28, 20));
    starter.push_back(Coordinate(28, 21));
    starter.push_back(Coordinate(29, 21));
    starter.push_back(Coordinate(29, 22));
    starter.push_back(Coordinate(28, 19));
    starter.push_back(Coordinate(29, 19));
    starter.push_back(Coordinate(29, 18));
    starter.push_back(Coordinate(3, 3));*/
    gol = addCells(starter);
  }

  void process(const ProcessArgs &args) override {
    float clock = inputs[CLOCK_INPUT].getVoltage();
    float vOct = inputs[VOCTIN_INPUT].getVoltage();
    float vOctFM = inputs[FMIN_INPUT].getVoltage();
    float modIndex = params[MODINDEXATV_PARAM].getValue() *
                     inputs[MODINDEXIN_INPUT].getVoltage();
    bool risingEdge = false;
    time += args.sampleTime;
    float am = 1.0f;
    if (clock > 3.5f) {
      risingEdge = (clockUp == false ? true : false);
      // clockUp == false ? risingEdge = true : risingEdge = false;
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
        gol = updateGol(gol);
        time = 0.f;
      }
      if (envelopeTime >= enveloppeTotalDuration) {
        isEnvelopeActive = false;
        golUpdateArmed = true;
      }
    }
    float audio = processAudio(gol.aliveCells, modIndex, vOct, vOctFM);
    outputs[AUDIO_OUTPUT].setVoltage(5.f * am * audio);
    lights[CLOCKLIGHT_LIGHT].setBrightness(clockUp == true ? 1.0f : 0.0f);
  }
  /**
  returns float between 0 and 1
  todo use lookup tables to avoid computing sin each time
  */
  float processAudio(std::vector<Coordinate> alive, float modIndex, float vOct,
                     float vOctFM) {
    float audio = 0.f;
    float count = 0.f;
    // printf("time %f \n", time);
    for (std::vector<Coordinate>::iterator it = alive.begin();
         it != alive.end(); ++it) {
      float modFreq =
          baseFreq *
          pow(2.0f, (float)((*it).y - NUMCELLSY / 2) / 12.0f + vOctFM);
      // modulation
      double intPart;
      float remainder =
          modf(modFreq * time + modulationPhase / (2.f * M_PI), &intPart);
      int index = (int)(remainder * 1000000);
      float modulation = modIndex * sinLut[index];
      // base frequency
      float freq =
          baseFreq * pow(2.0f, (float)((*it).x - NUMCELLSX / 2) / 12.0f + vOct);
      remainder = modf(time * (freq + modulation), &intPart);
      index = floor(remainder * 1000000);
      float partAudio = sinLut[index];
      // printf("%i %i \n", (*it).x, (*it).y);
      // printf("base %f mod %f \n", freq, modFreq);
      // printf("partAudio %f audio %f \n", partAudio, audio);
      audio += partAudio;
      count += 1.f;
    }
    if (count != 0.f) {
      audio = audio / count;
    }
    // printf("audio return %f \n", audio);
    return audio;
  }

  GolState addCell(Coordinate c) {
    std::vector<Coordinate> v1;
    std::vector<Coordinate> v2;
    v1.push_back(c);
    for (int i = c.x - 1; i < c.x + 2; i++) {
      int im = (i + NUMCELLSX) % NUMCELLSX;
      for (int j = c.y - 1; j < c.y + 2; j++) {
        int jm = (j + NUMCELLSY) % NUMCELLSY;
        if (im != c.x || jm != c.y) {
          v2.push_back(Coordinate(im, jm));
        }
      }
    }
    GolState gs = GolState(v1, v2);
    return gs;
  }

  GolState addCells(std::vector<Coordinate> vc) {
    std::vector<Coordinate> v1;
    std::vector<Coordinate> v2;
    for (std::vector<Coordinate>::iterator it = vc.begin(); it != vc.end();
         ++it) {
      GolState gs = addCell(*it);
      // add to v1 elements that do not already belong todo implement in a more
      // optimized way
      for (std::vector<Coordinate>::iterator it1 = gs.aliveCells.begin();
           it1 != gs.aliveCells.end(); ++it1) {
        bool contains = false;
        for (std::vector<Coordinate>::iterator it2 = v1.begin();
             it2 != v1.end(); ++it2) {
          if (it1->isEqual(*it2)) {
            contains = true;
            break;
          }
        }
        if (!contains) {
          v1.push_back(*it1);
        }
      }
      // same for v2
      for (std::vector<Coordinate>::iterator it1 = gs.toUpdateNextStep.begin();
           it1 != gs.toUpdateNextStep.end(); ++it1) {
        bool contains = false;
        for (std::vector<Coordinate>::iterator it2 = v2.begin();
             it2 != v2.end(); ++it2) {
          if (it1->isEqual(*it2)) {
            contains = true;
            break;
          }
        }
        if (!contains) {
          v2.push_back(*it1);
        }
      }
    }
    GolState toReturn = GolState(v1, v2);
    return toReturn;
  }

  bool isAlive(std::vector<Coordinate> alive, Coordinate c) {
    bool currentState = false;
    int count = 0;
    for (std::vector<Coordinate>::iterator it = alive.begin();
         it != alive.end(); ++it) {
      if (c.isEqual(*it)) {
        currentState = true;
      } else {
        int itx = (*it).x;
        int ity = (*it).y;
        int itxm = (itx - 1 + NUMCELLSX) % NUMCELLSX;
        int itxp = (itx + 1) % NUMCELLSX;
        int itym = (ity - 1 + NUMCELLSY) % NUMCELLSY;
        int ityp = (ity + 1) % NUMCELLSY;
        // case equal already accounted for
        if ((itxm == c.x || itxp == c.x || itx == c.x) &&
            (itym == c.y || ityp == c.y || ity == c.y)) {
          count++;
        }
      }
    }
    if (count == 3) {
      return true;
    } else if (count == 2) {
      return currentState;
    } else {
      return false;
    }
  }

  GolState updateGol(GolState gs) {
    std::vector<Coordinate> alive;
    std::vector<Coordinate> up;
    std::vector<Coordinate> toAdd;
    for (std::vector<Coordinate>::iterator it = gs.toUpdateNextStep.begin();
         it != gs.toUpdateNextStep.end(); ++it) {
      if (isAlive(gs.aliveCells, *it)) {
        toAdd.push_back(*it);
      }
    }
    // printf("size %li \n", toAdd.size());
    return addCells(toAdd);
  }

  void setAlive(Coordinate c) {
    GolState addResult = addCell(c);
    // if cell is not already alive, add it
    bool contains = false;
    for (std::vector<Coordinate>::iterator it = gol.aliveCells.begin();
         it != gol.aliveCells.end(); ++it) {
      if ((*it).isEqual(c)) {
        contains = true;
        break;
      }
    }
    if (!contains) {
      gol.aliveCells.push_back(c);
    }
    // todo factorize + implementation
    for (std::vector<Coordinate>::iterator it1 =
             addResult.toUpdateNextStep.begin();
         it1 != addResult.toUpdateNextStep.end(); ++it1) {
      bool contains = false;
      for (std::vector<Coordinate>::iterator it2 = gol.toUpdateNextStep.begin();
           it2 != gol.toUpdateNextStep.end(); ++it2) {
        if ((*it2).isEqual((*it1))) {
          contains = true;
          break;
        }
      }
      if (!contains) {
        gol.toUpdateNextStep.push_back((*it1));
      }
    }
  }

  void setDead(Coordinate c) {
    GolState addResult =
        addCell(c); // to have a list of cells to update next step
    bool contains = false;
    int position = 0;
    for (std::vector<Coordinate>::iterator it = gol.aliveCells.begin();
         it != gol.aliveCells.end(); ++it) {
      if ((*it).isEqual(c)) {
        contains = true;
        break;
      }
      position++;
    }
    if (contains) {
      gol.aliveCells.erase(gol.aliveCells.begin() + position);
    }
    // todo factorize + implementation
    for (std::vector<Coordinate>::iterator it1 =
             addResult.toUpdateNextStep.begin();
         it1 != addResult.toUpdateNextStep.end(); ++it1) {
      bool contains = false;
      for (std::vector<Coordinate>::iterator it2 = gol.toUpdateNextStep.begin();
           it2 != gol.toUpdateNextStep.end(); ++it2) {
        if ((*it2).isEqual((*it1))) {
          contains = true;
          break;
        }
      }
      if (!contains) {
        gol.toUpdateNextStep.push_back((*it1));
      }
    }
  }
};

struct GolDisplay : OpaqueWidget {
  GameOfLife *module;
  std::unordered_map<int, std::unordered_map<int, GolCell *>> aliveCells;
  std::vector<Coordinate> oldAliveCells;
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
    for (int i = 0; i < numCellsX; i++) {
      for (int j = 0; j < numCellsY; j++) {
        GolCell *golCell = new GolCell();
        golCell->box.pos =
            Vec(i * (cellSizeX + cellSpaceX), j * (cellSizeY + cellSpaceY));
        golCell->box.size = Vec(cellSizeX, cellSizeY);
        golCell->x = i;
        golCell->y = j;
        aliveCells[i][j] = golCell;
        addChild(golCell);
      }
    }
  }

  void draw(const DrawArgs &args) override {
    GolState gol = GolState();
    if (module) {
      gol = module->gol;
    }
    std::vector<Coordinate> alive = gol.aliveCells;
    // naive implementation, todo optimize
    for (std::vector<Coordinate>::iterator itOld = oldAliveCells.begin();
         itOld != oldAliveCells.end(); ++itOld) {
      Coordinate cOld = *itOld;
      bool stillAlive = false;
      for (std::vector<Coordinate>::iterator it = alive.begin();
           it != alive.end(); ++it) {
        Coordinate c = *it;
        if (cOld.isEqual(c)) {
          stillAlive = true;
          break;
        }
        if (!stillAlive) {
          aliveCells[cOld.x][cOld.y]->alive = false;
        }
      }
    }
    for (std::vector<Coordinate>::iterator it = alive.begin();
         it != alive.end(); ++it) {
      Coordinate c = *it;
      aliveCells[c.x][c.y]->alive = true;
    }

    oldAliveCells = alive;
    OpaqueWidget::draw(args);
  }

  // to tell the module that this cell is now to be considered alive or dead
  void setLivingState(int x, int y, bool alive) {
    if (module) {
      if (alive) {
        module->setAlive(Coordinate(x, y));
      } else {
        module->setDead(Coordinate(x, y));
      }
    }
  }
};

void GolCell::draw(const DrawArgs &args) {
  if (alive) {
    nvgFillColor(args.vg, nvgRGBA(0x11, 0x11, 0x11, 0xff));
  } else {
    nvgFillColor(args.vg, nvgRGBA(0xdd, 0xdd, 0xdd, 0xff));
  }
  nvgBeginPath(args.vg);
  nvgRect(args.vg, 0, 0, this->box.size.x, this->box.size.y);
  nvgFill(args.vg);
}

void GolCell::onButton(const event::Button &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
    this->alive = !this->alive;
    GolDisplay *gd = getAncestorOfType<GolDisplay>();
    gd->setLivingState(this->x, this->y, this->alive);
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
        mm2px(Vec(7.971, 99.697)), module, GameOfLife::MODINDEXATV_PARAM));

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.956, 44.122)), module,
                                             GameOfLife::SENDIN_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.956, 75.343)), module,
                                             GameOfLife::VOCTIN_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(35.869, 75.343)), module,
                                             GameOfLife::FMIN_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.971, 110.81)), module,
                                             GameOfLife::MODINDEXIN_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(23.913, 110.81)), module,
                                             GameOfLife::CLOCK_INPUT));

    addOutput(createOutputCentered<PJ301MPort>(
        mm2px(Vec(35.869, 29.019)), module, GameOfLife::DATACLOCKOUT_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(
        mm2px(Vec(35.869, 44.122)), module, GameOfLife::DATAOUT_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(
        mm2px(Vec(39.854, 110.81)), module, GameOfLife::AUDIO_OUTPUT));

    addChild(createLightCentered<MediumLight<RedLight>>(
        mm2px(Vec(11.956, 29.02)), module, GameOfLife::DATALIGHT_LIGHT));
    addChild(createLightCentered<MediumLight<RedLight>>(
        mm2px(Vec(39.854, 99.168)), module, GameOfLife::AUDIOLIGHT_LIGHT));
    addChild(createLightCentered<MediumLight<RedLight>>(
        mm2px(Vec(23.913, 99.697)), module, GameOfLife::CLOCKLIGHT_LIGHT));

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

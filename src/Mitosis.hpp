#include "DSP.hpp"
#include "DataReceiver.hpp"
#include "DataSender.hpp"
#include "GameOfLifeGrid.hpp"
#include "plugin.hpp"

#ifndef MITOSIS_H
#define MITOSIS_H

struct Mitosis : Module {
  DSP *dsp = NULL;
  GameOfLifeGrid *golGrid = NULL;
  DataSender *dataSender = NULL;
  DataReceiver *dataReceiver = NULL;
  bool golUpdateArmed = true;
  bool clockUp;
  bool hasResetSend = true;

  enum ParamIds { PROPHASE_PARAM, METAPHASE_PARAM, NUM_PARAMS };
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
    DEAD_OUTPUT,
    BUSY_OUTPUT,
    DATACLK_OUTPUT,
    DATA_OUTPUT,
    NUM_OUTPUTS
  };
  enum LightIds { CLOCKLIGHT_LIGHT, BUSYINLIGHT_LIGHT, NUM_LIGHTS };

  Mitosis();
  void process(const ProcessArgs &args) override;
  float processAudio(std::set<Cell *> alive, float tune, float vOct);
};

#endif

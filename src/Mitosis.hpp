#include "DataReceiver.hpp"
#include "DataSender.hpp"
#include "GameOfLifeGrid.hpp"
#include "plugin.hpp"

#ifndef MITOSIS_H
#define MITOSIS_H

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

  Mitosis();
  void process(const ProcessArgs &args) override;
  float processAudio(std::set<Cell *> alive, float tune, float vOct);
};

#endif

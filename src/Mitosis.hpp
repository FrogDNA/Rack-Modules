#ifndef MITOSIS_H
#define MITOSIS_H

#include "DSP.hpp"
#include "DataReceiver.hpp"
#include "DataSender.hpp"
#include "GameOfLifeGrid.hpp"
#include "plugin.hpp"

struct Mitosis : Module {
  DSP *dsp = NULL;
  GameOfLifeGrid *golGrid = NULL;
  DataSender *dataSender = NULL;
  DataReceiver *dataReceiver = NULL;
  bool golUpdateArmed = true;
  bool clockUp;
  bool hasResetSend = true;
  float vOctOut = 0.f;
  float lastDspLowestFreq = 0.f;

  dsp::RingBuffer<std::pair<int, int> *, 1> clickedCells;
  // loopParam
  dsp::RingBuffer<bool, 1> loopParam;
  // infinity param
  dsp::RingBuffer<bool, 1> infinityParam;
  // mute columns and rows
  dsp::RingBuffer<int, 64> muteUnmuteColsBuffer;
  dsp::RingBuffer<int, 64> muteUnmuteRowsBuffer;
  dsp::RingBuffer<int, 64> muteColsBuffer;
  dsp::RingBuffer<int, 64> unmuteColsBuffer;

  enum ParamIds { TEMP_PARAM, FOOD_PARAM, NUM_PARAMS };
  enum InputIds {
    VOCT_INPUT,
    CLOCK_INPUT,
    SEND_INPUT,
    DATAIN_INPUT,
    BUSYIN_INPUT,
    DATACLKIN_INPUT,
    NUM_INPUTS
  };
  enum OutputIds {
    DEAD_OUTPUT,
    VOCTOUT_OUTPUT,
    AUDIO_OUTPUT,
    BUSY_OUTPUT,
    DATACLK_OUTPUT,
    DATA_OUTPUT,
    NUM_OUTPUTS
  };
  enum LightIds { NUM_LIGHTS };

  Mitosis();
  void processBuffers();
  void process(const ProcessArgs &args) override;
  float processAudio(std::set<std::pair<int, int> *> alive, float tune,
                     float vOct);
  json_t *dataToJson() override;
  void dataFromJson(json_t *rootJ) override;
};

#endif

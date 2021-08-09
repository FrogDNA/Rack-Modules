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
  // mute columns and rows
  dsp::RingBuffer<int, 64> muteUnmuteColsBuffer;
  dsp::RingBuffer<int, 64> muteUnmuteRowsBuffer;
  dsp::RingBuffer<int, 64> muteRowsBuffer;
  dsp::RingBuffer<int, 64> unmuteRowsBuffer;
  dsp::RingBuffer<int, 64> muteColsBuffer;
  dsp::RingBuffer<int, 64> unmuteColsBuffer;
  enum ParamIds { NUM_PARAMS };
  enum InputIds {
    CLOCK_INPUT,
    SEND_INPUT,
    BUSYIN_INPUT,
    DATACLKIN_INPUT,
    DATAIN_INPUT,
    NUM_INPUTS
  };
  enum OutputIds {
    ROW_1_OUTPUT,
    COL_1_OUTPUT,
    ROW_2_OUTPUT,
    COL_2_OUTPUT,
    COL_3_OUTPUT,
    ROW_3_OUTPUT,
    COL_4_OUTPUT,
    ROW_4_OUTPUT,
    COL_5_OUTPUT,
    ROW_5_OUTPUT,
    ROW_6_OUTPUT,
    COL_6_OUTPUT,
    ROW_7_OUTPUT,
    COL_7_OUTPUT,
    ROW_8_OUTPUT,
    COL_8_OUTPUT,
    ROW_9_OUTPUT,
    COL_9_OUTPUT,
    BUSY_OUTPUT,
    DATACLK_OUTPUT,
    DATA_OUTPUT,
    COL_10_OUTPUT,
    ROW_10_OUTPUT,
    DEAD_OUTPUT,
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

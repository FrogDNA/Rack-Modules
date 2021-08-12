#pragma once

#include "DSP.hpp"
#include "DataReceiver.hpp"
#include "DataSender.hpp"
#include "GameOfLifeGrid.hpp"
#include "plugin.hpp"

struct Mitosis : Module {
  DSP *dsp = NULL;
  Outputs out;
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
    ROW_VOCT_INPUT,
    COL_VOCT_INPUT,
    INIT_SAVE_INPUT,
    INIT_RND_INPUT,
    INIT_LOAD_INPUT,
    BUSYIN_INPUT,
    DATACLKIN_INPUT,
    DATAIN_INPUT,
    NUM_INPUTS
  };
  enum OutputIds {
    DEAD_OUTPUT,
    ROW_1_OUTPUT,
    ROW_1_P_OUTPUT,
    COL_1_OUTPUT,
    COL_1_P_OUTPUT,
    ROW_2_OUTPUT,
    ROW_2_P_OUTPUT,
    COL_2_OUTPUT,
    COL_2_P_OUTPUT,
    ROW_3_OUTPUT,
    ROW_3_P_OUTPUT,
    COL_3_OUTPUT,
    COL_3_P_OUTPUT,
    ROW_4_OUTPUT,
    ROW_4_P_OUTPUT,
    COL_4_OUTPUT,
    COL_4_P_OUTPUT,
    ROW_5_OUTPUT,
    ROW_5_P_OUTPUT,
    COL_5_OUTPUT,
    COL_5_P_OUTPUT,
    ROW_6_OUTPUT,
    ROW_6_P_OUTPUT,
    COL_6_OUTPUT,
    COL_6_P_OUTPUT,
    ROW_7_OUTPUT,
    ROW_7_P_OUTPUT,
    COL_7_OUTPUT,
    COL_7_P_OUTPUT,
    BUSY_OUTPUT,
    DATACLK_OUTPUT,
    DATA_OUTPUT,
    ROW_8_OUTPUT,
    ROW_8_P_OUTPUT,
    COL_8_OUTPUT,
    COL_8_P_OUTPUT,
    ROW_9_OUTPUT,
    ROW_9_P_OUTPUT,
    COL_9_OUTPUT,
    COL_9_P_OUTPUT,
    ROW_10_OUTPUT,
    ROW_10_P_OUTPUT,
    COL_10_OUTPUT,
    COL_10_P_OUTPUT,
    NUM_OUTPUTS
  };
  enum LightIds {
    ROW_1_LIGHT,
    ROW_3_LIGHT,
    ROW_5_LIGHT,
    ROW_7_LIGHT,
    ROW_9_LIGHT,
    COL_1_LIGHT,
    COL_3_LIGHT,
    COL_5_LIGHT,
    COL_7_LIGHT,
    COL_9_LIGHT,
    ROW_2_LIGHT,
    ROW_4_LIGHT,
    ROW_6_LIGHT,
    ROW_8_LIGHT,
    ROW_10_LIGHT,
    COL_2_LIGHT,
    COL_4_LIGHT,
    COL_6_LIGHT,
    COL_8_LIGHT,
    COL_10_LIGHT,
    NUM_LIGHTS
  };

  Mitosis();
  void processBuffers();
  void process(const ProcessArgs &args) override;
  float processAudio(std::set<std::pair<int, int> *> alive, float tune,
                     float vOct);
  json_t *dataToJson() override;
  void dataFromJson(json_t *rootJ) override;
};

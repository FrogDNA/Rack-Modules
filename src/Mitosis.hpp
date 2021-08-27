#pragma once

#include "DSP.hpp"
#include "GameOfLifeGrid.hpp"
#include "InterfaceButton.hpp"
#include "plugin.hpp"

struct SeedButton : InterfaceButton {
  dsp::RingBuffer<int, 1> *rb;
  void buttonReleased() override;
  void whileHovering() override {}
};

struct Mitosis : Module {
  DSP *dsp = NULL;
  Outputs out;
  bool audibilityChanged = true;
  GameOfLifeGrid *golGrid = NULL;
  bool clockUp = false;
  float vOctOut = 0.f;
  // 0 is default, 1-3 are loads/saves 1-2, 3 is random seed
  // lastLoaded variables are modified when load OR save.
  int lastLoaded = 0;
  std::vector<std::pair<int, int> *> lastLoadedData;
  bool loadHigh = false;
  bool saveHigh = false;
  std::vector<std::vector<std::pair<int, int> *>> saves;
  // button to reinit by seeding
  dsp::RingBuffer<int, 1> seedInitBuffer;
  // clicked cells to set alive or dead
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
  // limits
  int colSize = DEFAULT_CELLS_DISPLAYED_X;
  int rowSize = DEFAULT_CELLS_DISPLAYED_Y;
  int colStart = (NUMCELLS_X - DEFAULT_CELLS_DISPLAYED_X) / 2;
  int rowStart = (NUMCELLS_Y - DEFAULT_CELLS_DISPLAYED_Y) / 2;
  dsp::RingBuffer<int, 16> colStartBuffer;
  dsp::RingBuffer<int, 16> colSizeBuffer;
  dsp::RingBuffer<int, 16> rowStartBuffer;
  dsp::RingBuffer<int, 16> rowSizeBuffer;
  bool colLimitUp = false;
  bool rowLimitUp = false;
  enum ParamIds { DEAD_PARAM, NUM_PARAMS };
  enum InputIds {
    CLOCK_INPUT,
    RESET_INPUT,
    ROW_LIMIT_INPUT,
    COL_LIMIT_INPUT,
    SAVE_1_INPUT,
    LOAD_1_INPUT,
    SEED_INPUT,
    SAVE_2_INPUT,
    LOAD_2_INPUT,
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
    DEAD_5_LIGHT,
    DEAD_6_LIGHT,
    DEAD_4_LIGHT,
    DEAD_7_LIGHT,
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
    DEAD_3_LIGHT,
    DEAD_8_LIGHT,
    DEAD_2_LIGHT,
    DEAD_9_LIGHT,
    DEAD_1_LIGHT,
    DEAD_10_LIGHT,
    LS_1_LIGHT,
    SEED_LIGHT,
    LS_2_LIGHT,
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

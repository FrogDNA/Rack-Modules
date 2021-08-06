#pragma once

#include "Mitosis.hpp"
#include "plugin.hpp"

struct SliderParam : TransparentWidget {
  Mitosis *module = NULL;
  bool *switchedOn = NULL;
  bool firstDraw = true;
  dsp::RingBuffer<bool, 1> *rb = NULL;
  void draw(const DrawArgs &args) override;
  void onButton(const event::Button &e) override;
  virtual void onFirstDraw() = 0;
};

struct LoopSliderParam : SliderParam {
  void onFirstDraw() override;
};

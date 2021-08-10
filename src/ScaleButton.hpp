#ifndef SCALEBUTTON_H
#define SCALEBUTTON_H

#include "Mitosis.hpp"
#include "plugin.hpp"

struct ScaleButton : OpaqueWidget {
  Mitosis *module;
  int semitone = 0;
  int status = 0;
  bool cols = false;
  dsp::RingBuffer<int, 64> *muteRb;
  dsp::RingBuffer<int, 64> *unmuteRb;
  void draw(const DrawArgs &args) override;
  void onButton(const event::Button &e) override;
};

#endif

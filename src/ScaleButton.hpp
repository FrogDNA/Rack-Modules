#ifndef SCALEBUTTON_H
#define SCALEBUTTON_H

#include "plugin.hpp"

struct ScaleButton : OpaqueWidget {
  int semitone = 0;
  int status = 0;
  dsp::RingBuffer<int, 64> *muteRb;
  dsp::RingBuffer<int, 64> *unmuteRb;
  void draw(const DrawArgs &args) override;
  void onButton(const event::Button &e) override;
};

#endif

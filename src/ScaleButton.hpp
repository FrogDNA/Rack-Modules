#ifndef SCALEBUTTON_H
#define SCALEBUTTON_H

#include "plugin.hpp"

struct ScaleButton : OpaqueWidget {
  int semitone;
  int status;
  void draw(const DrawArgs &args) override;
  void onButton(const event::Button &e) override;
};

#endif

#ifndef SLIDERPARAM_H
#define SLIDERPARAM_H

#include "Mitosis.hpp"
#include "plugin.hpp"

struct SliderParam : TransparentWidget {
  bool switchedOn = false;
  Mitosis *module = NULL;
  void draw(const DrawArgs &args) override;
  void onButton(const event::Button &e) override;
};

#endif

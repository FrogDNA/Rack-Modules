#include "plugin.hpp"

#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H

static const int NUMCELLSX = 30;
static const int NUMCELLSY = 30;

struct GolCell : Widget {

  int x = -1;
  int y = -1;
  bool alive = false;

  void draw(const DrawArgs &args) override;

  void onButton(const event::Button &e) override;
};

#endif

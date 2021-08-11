#pragma once

#include "Consts.hpp"
#include "plugin.hpp"

struct InterfaceButton : Widget {
  bool pressed = false;
  void draw(const DrawArgs &args) override;
  void onButton(const event::Button &e) override;
  void onDragHover(const event::DragHover &e) override;
  void onDragLeave(const event::DragLeave &e) override;
  virtual void buttonReleased() = 0;
  virtual void whileHovering() = 0;
};

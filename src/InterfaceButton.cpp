#include "InterfaceButton.hpp"

void InterfaceButton::draw(const DrawArgs &args) {
  if (pressed) {
    nvgFillColor(args.vg, color);
    nvgBeginPath(args.vg);
    if (circular) {
      nvgCircle(args.vg, this->box.size.x / 2, this->box.size.y / 2,
                this->box.size.x / 2);
    } else {
      nvgRect(args.vg, 0, 0, this->box.size.x, this->box.size.y);
    }
    nvgFill(args.vg);
  }
}

void InterfaceButton::onButton(const event::Button &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
    if (e.action == GLFW_PRESS) {
      pressed = true;
    } else if (e.action == GLFW_RELEASE && pressed) {
      pressed = false;
      buttonReleased();
    }
    e.consume(this);
    Widget::onButton(e);
  }
}

void InterfaceButton::onDragHover(const event::DragHover &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT && pressed) {
    whileHovering();
    e.consume(this);
    Widget::onDragHover(e);
  }
}

void InterfaceButton::onDragLeave(const event::DragLeave &e) {
  pressed = false;
  e.consume(this);
}

#include "ScaleButton.hpp"

void ScaleButton::draw(const DrawArgs &args) {
  if (status == 1) {
    nvgFillColor(args.vg, nvgRGBA(0xba, 0xba, 0x00, 0x6e));
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0, 0, this->box.size.x, this->box.size.y);
    nvgFill(args.vg);
  } else if (status == 2) {
    nvgFillColor(args.vg, nvgRGBA(0x00, 0xba, 0x00, 0x6e));
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0, 0, this->box.size.x, this->box.size.y);
    nvgFill(args.vg);
  }
}
void ScaleButton::onButton(const event::Button &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
    // thread safe
    // logic
    if (status < 2) {
      status = 2;
    } else {
      status = 0;
    }
    e.consume(this);
  }
  Widget::onButton(e);
}

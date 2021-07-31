#include "ScaleButton.hpp"
#include "Consts.hpp"

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
    if (status < 2) {
      bool isFull = false;
      for (int i = semitone; i < NUMCELLSY; i += 12) {
        if (!muteRb->full()) {
          muteRb->push(i);
        } else {
          isFull = true;
          break;
        }
      }
      if (!isFull) {
        status = 2;
      } else {
        status = 1;
      }
    } else {
      bool isFull = false;
      for (int i = semitone; i < NUMCELLSY; i += 12) {
        if (!unmuteRb->full()) {
          unmuteRb->push(i);
        } else {
          isFull = true;
          break;
        }
      }
      if (!isFull) {
        status = 0;
      } else {
        status = 1;
      }
    }
    e.consume(this);
  }
  Widget::onButton(e);
}

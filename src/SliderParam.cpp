#include "SliderParam.hpp"
#include "plugin.hpp"

SliderParam::SliderParam(bool switchOn, dsp::RingBuffer<bool, 1> *rb) {
  this->rb = rb;
  switchedOn = switchOn;
}

void SliderParam::draw(const DrawArgs &args) {
  // draw the button
  nvgFillColor(args.vg, nvgRGBA(0x11, 0x11, 0x11, 0xff));
  nvgBeginPath(args.vg);
  if (switchedOn) {
    nvgRect(args.vg, this->box.size.x / 2, 0, this->box.size.x / 2,
            this->box.size.y);
  } else {
    nvgRect(args.vg, 0, 0, this->box.size.x / 2, this->box.size.y);
  }
  nvgFill(args.vg);
}

void SliderParam::onButton(const event::Button &e) {
  if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
    // thread safe
    if (!rb->full()) {
      switchedOn = !switchedOn;
      rb->push(switchedOn);
    }
    e.consume(this);
  }
  Widget::onButton(e);
}

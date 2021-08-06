#include "SliderParam.hpp"
#include "plugin.hpp"

void SliderParam::draw(const DrawArgs &args) {
  // draw the button
  if (module && module->golGrid && firstDraw) {
    onFirstDraw();
  }
  if (!firstDraw) {
    nvgFillColor(args.vg, nvgRGBA(0x11, 0x11, 0x11, 0xff));
    nvgBeginPath(args.vg);
    if (*switchedOn) {
      nvgRect(args.vg, this->box.size.x / 2, 0, this->box.size.x / 2,
              this->box.size.y);
    } else {
      nvgRect(args.vg, 0, 0, this->box.size.x / 2, this->box.size.y);
    }
    nvgFill(args.vg);
    // draw the empty area
    nvgBeginPath(args.vg);
    if (*switchedOn) {
      nvgFillColor(args.vg, nvgRGBA(0x00, 0xba, 0x00, 0xff));
      nvgRect(args.vg, 0, 0, this->box.size.x / 2, this->box.size.y);
    } else {
      nvgFillColor(args.vg, nvgRGBA(0xba, 0x00, 0x00, 0xff));
      nvgRect(args.vg, this->box.size.x / 2, 0, this->box.size.x / 2,
              this->box.size.y);
    }
    nvgFill(args.vg);
  }
}

void SliderParam::onButton(const event::Button &e) {
  if (rb) {
    if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
      // thread safe
      if (!rb->full()) {
        rb->push(!*switchedOn);
      }
      e.consume(this);
    }
    Widget::onButton(e);
  }
}

void LoopSliderParam::onFirstDraw() {
  firstDraw = false;
  switchedOn = &(module->golGrid->loop);
}

void InfSliderParam::onFirstDraw() {
  firstDraw = false;
  switchedOn = &(module->golGrid->maxSize);
}

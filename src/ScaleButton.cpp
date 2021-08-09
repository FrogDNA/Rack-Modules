#include "ScaleButton.hpp"
#include "Consts.hpp"
#include "DSP.hpp"
#include "Mitosis.hpp"

void ScaleButton::draw(const DrawArgs &args) {
  // if status == 2 (activated), check if all rows are still mute.
  // else, move to status 1
  bool oneMuted = false;
  bool oneUnmuted = false;
  if (module) {
    DSP *dsp = module->dsp;
    for (int i = semitone; i < NUMCELLS_X; i += 12) {
      if (dsp->isColAudible(i)) {
        oneUnmuted = true;
      } else {
        oneMuted = true;
      }
      if (oneUnmuted && oneMuted) {
        break; // dont go further
      }
    }
    if (oneMuted && !oneUnmuted) {
      status = 2;
    } else if (oneUnmuted && !oneMuted) {
      status = 0;
    } else {
      status = 1;
    }
  }
  // if all cols are unmuted, move to status 0
  if (status == 1) {
    nvgFillColor(args.vg, COLOR_YELLOW_BUTTON_T);
    nvgBeginPath(args.vg);
    nvgRect(args.vg, 0, 0, this->box.size.x, this->box.size.y);
    nvgFill(args.vg);
  } else if (status == 2) {
    nvgFillColor(args.vg, COLOR_GREEN_BUTTON_T);
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
      for (int i = semitone; i < NUMCELLS_X; i += 12) {
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
      for (int i = semitone; i < NUMCELLS_X; i += 12) {
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

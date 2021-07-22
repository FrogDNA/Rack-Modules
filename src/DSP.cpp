#include "DSP.hpp"

DSP::DSP() {
  for (int i = 0; i < LUT_SIZE; i++) {
    float phase = (float)i / (float)LUT_SIZE;
    lut.push_back(std::sin(2.0f * M_PI * phase));
  }
}

void DSP::paramValues(std::vector<Cell *> alive, float wideness, float center,
                      float vOct) {
  bool sthChanged = false;
  if (this->wideness != wideness || this->center != center ||
      this->vOct != vOct || this->alive != alive) {
    sthChanged = true;
  }
  this->wideness = wideness;
  this->center = center;
  this->vOct = vOct;
  if (this->alive != alive) {
    this->alive = alive;
    harmonics.clear();
    for (std::vector<Cell *>::iterator it = alive.begin(); it != alive.end();
         ++it) {
      Cell *c = *it;
      int x = c->getX();
      if (harmonics.find(c->getX()) == harmonics.end()) {
        harmonics[x] = 1;
      } else {
        ++(harmonics[x]);
      }
    }
  }
  if (sthChanged) {
    // todo recompute harmonics
    // compute gaussian
    // todo when wideness is 0 or 1
    // printf("something changed \n");
    amplitudes.clear();
    frequencies.clear();
    phases.clear();
    for (auto harmonic : harmonics) {
      int x = harmonic.first;
      int h = harmonic.second;
      int hCenter = floor(center * (h - 1));
      float f = pow(2.0f, (float)(x - NUMCELLSX / 2) / 12.0f + vOct);
      std::vector<float> fx;
      std::vector<float> px;
      std::vector<float> ax;
      // printf("harmonic %i %i \n", x, h);
      for (int i = 0; i < h; i++) {
        // harmonic i+1 of freq x
        float freq = BASE_FREQ * (float)(i + 1) * f;
        fx.push_back(freq);
        px.push_back(0.f);
        float amplitude = 1.f;
        if (wideness != 1.f && wideness != 0.f) {
          float iNormalized = (h == 1 ? 0.f : (float)i / (float)(h - 1));
          float sd = wideness / (1.f - wideness);
          float var = (iNormalized - center);
          amplitude = std::exp(-(var * var) / (2.0 * sd * sd));
          /*printf("x %i i %i, %f %f %f %f \n", x, i, iNormalized, sd, var,
                 amplitude);*/
        } else if (wideness == 0.f) { // wideness == 0
          if (hCenter == i) {
            amplitude = 1.f;
          } else {
            amplitude = 0.f;
          }
        } // else wideness == 1 and amplitude == 1
        ax.push_back(amplitude);
      }
      frequencies.push_back(fx);
      phases.push_back(px);
      amplitudes.push_back(ax);
    }
  }
}

float DSP::nextValue(float sampleTime) {
  float audio = 0.f;
  float ampSum = 0.f;
  for (unsigned int x = 0; x < frequencies.size(); x++) {
    std::vector<float> fx = frequencies[x];
    std::vector<float> ax = amplitudes[x];
    std::vector<float> px = phases[x];
    for (unsigned int i = 0; i < fx.size(); i++) {
      float amplitude = ax[i];
      float frequency = fx[i];
      px[i] = px[i] += frequency * sampleTime;
      px[i] -= floor(px[i]);
      phases[x] = px;
      int phaseInt = floor(LUT_SIZE * px[i]);
      float partAudio = amplitude * lut[phaseInt];
      audio += partAudio;
      ampSum += amplitude;
    }
  }
  if (ampSum != 0.f) {
    audio = audio / ampSum;
  }
  return audio;
}

#include "DSP.hpp"

DSP::DSP() {}

void DSP::paramValues(std::set<Cell *> alive, float wideness, float center,
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
    for (std::set<Cell *>::iterator it = alive.begin(); it != alive.end();
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
    harmonicAmplitudes.clear();
    frequences.clear();
    phases.clear();
    for (auto harmonic : harmonics) {
      int x = harmonic.first;
      int h = harmonic.second;
      int hCenter = floor(center * (h - 1));
      // printf("harmonic %i %i \n", x, h);
      for (int i = 0; i < h; i++) {
        // harmonic i+1 of freq x
        float freq = BASE_FREQ * (float)(i + 1) *
                     pow(2.0f, (float)(x - NUMCELLSX / 2) / 12.0f + vOct);
        frequences[x][i] = freq;
        phases[x][i] = 0.f;
        if (wideness != 1.f && wideness != 0.f) {
          float iNormalized = (h == 1 ? 0.f : (float)i / (float)(h - 1));
          float sd = wideness / (1.f - wideness);
          float var = (iNormalized - center);
          float amplitude = std::exp(-(var * var) / (2.0 * sd * sd));
          /*printf("x %i i %i, %f %f %f %f \n", x, i, iNormalized, sd, var,
                 amplitude);*/
          harmonicAmplitudes[x][i] = amplitude;
        } else if (wideness == 1.f) {
          harmonicAmplitudes[x][i] = 1.f;
        } else { // wideness == 0
          if (hCenter == i) {
            harmonicAmplitudes[x][i] = 1.f;
          } else {
            harmonicAmplitudes[x][i] = 0.f;
          }
        }
      }
    }
  }
}

float DSP::nextValue(float sampleTime) {
  float audio = 0.f;
  float ampSum = 0.f;
  for (auto harmonic : harmonics) {
    int x = harmonic.first;
    int h = harmonic.second;
    for (int i = 0; i < h; i++) {
      float amplitude = harmonicAmplitudes[x][i];
      phases[x][i] = phases[x][i] += frequences[x][i] * sampleTime;
      phases[x][i] -= floor(phases[x][i]);
      float partAudio = amplitude * std::sin(2.0f * M_PI * phases[x][i]);
      audio += partAudio;
      ampSum += amplitude;
      // printf("audio x %i , %i / %i %f \n", x, i, h, amplitude);
    }
  }
  if (ampSum != 0.f) {
    audio = audio / ampSum;
  }
  return audio;
}

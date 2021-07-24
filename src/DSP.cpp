#include "DSP.hpp"

AMGenerator::AMGenerator() {}
void AMGenerator::stopAndReset() {
  phase = 0.f;
  running = false;
}

void AMGenerator::start() { running = true; }

float AMGenerator::nextValue(float sampleTime) {
  float am = 1.f;
  if (running) {
    phase = phase + sampleTime / ENVELOPE_DURATION;
    phase = phase - floor(phase);
    am = 0.5f + 0.5f * std::cos(phase * 2.f * M_PI / ENVELOPE_DURATION);
  }
  return am;
}
bool AMGenerator::isRunning() { return running; }

DSP::DSP() {
  am = new AMGenerator();
  for (int i = 0; i < LUT_SIZE; i++) {
    float phase = (float)i / (float)LUT_SIZE;
    lut.push_back(std::sin(2.0f * M_PI * phase));
  }
  harmonics.reserve(NUMCELLSX);
  for (int i = 0; i < NUMCELLSX; ++i) {
    harmonics[i] = 0;
  }
}

// todo change to split between the three
void DSP::paramValues(std::vector<Cell *> alive, float wideness, float center,
                      float vOct) {
  bool harmonicsChanged = false;
  bool frequencyChanged = false;
  bool amplitudesChanged = false;
  if (this->vOct != vOct) {
    this->vOct = vOct;
    frequencyChanged = true;
  }
  if (this->wideness != wideness || this->center != center) {
    this->wideness = wideness;
    this->center = center;
    amplitudesChanged = true;
  }

  if (this->alive != alive) {
    this->alive = alive;
    harmonicsChanged = true;
    harmonics.clear();
    harmonics.reserve(NUMCELLSX);
    for (int i = 0; i < NUMCELLSX; ++i) {
      harmonics[i] = 0;
    }
    for (std::vector<Cell *>::iterator it = alive.begin(); it != alive.end();
         ++it) {
      Cell *c = *it;
      int x = c->getX();
      ++(harmonics[x]);
    }
  }
  // todo instead of clear keep and prepare futurePhases futureAmplitudes
  // futureFrequencies ???
  // todo trigger AM instead of resetting phases to remove crackling
  // then trigger AM
  if (harmonicsChanged || amplitudesChanged || frequencyChanged) {
    amplitudes.clear();
    frequencies.clear();
    if (harmonicsChanged) {
      phases.clear();
    }

    for (int i = 0; i < NUMCELLSX; ++i) {
      if (harmonics[i] != 0) {
        int h = harmonics[i];
        int hCenter = floor(center * (h - 1));
        float f = pow(2.0f, (float)(i - NUMCELLSX / 2) / 12.0f + vOct);
        std::vector<float> fx;
        std::vector<float> px;
        std::vector<float> ax;
        // printf("harmonic %i %i \n", x, h);
        for (int j = 0; j < h; j++) {
          // harmonic i+1 of freq x
          float freq = BASE_FREQ * (float)(j + 1) * f;
          fx.push_back(freq);
          if (harmonicsChanged) {
            px.push_back(0.f);
          }
          float amplitude = 1.f;
          if (wideness != 1.f && wideness != 0.f) {
            float iNormalized = (h == 1 ? 0.f : (float)j / (float)(h - 1));
            float sd = wideness / (1.f - wideness);
            float var = (iNormalized - center);
            amplitude = std::exp(-(var * var) / (2.0 * sd * sd));
          } else if (wideness == 0.f) { // wideness == 0
            if (hCenter == j) {
              amplitude = 1.f;
            } else {
              amplitude = 0.f;
            }
          } // else wideness == 1 and amplitude == 1
          ax.push_back(amplitude);
        }
        frequencies.push_back(fx);
        if (harmonicsChanged) {
          phases.push_back(px);
        }
        amplitudes.push_back(ax);
      }
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

#include "DSP.hpp"

std::vector<float> DSP::initSinLut() {
  std::vector<float> sinLut;
  sinLut.reserve(SIN_LUT_SIZE);
  for (int i = 0; i < SIN_LUT_SIZE; i++) {
    float phase = (float)i / (float)SIN_LUT_SIZE;
    sinLut.push_back(std::sin(2.0f * M_PI * phase));
  }
  return sinLut;
}
std::vector<float> sinLut = DSP::initSinLut();

AudibleCell::AudibleCell(float x, float baseFrequency, float harmonicNumber,
                         float amplitude) {
  this->x = x;
  this->baseFrequency = baseFrequency;
  this->harmonicNumber = harmonicNumber;
  this->amplitude = amplitude;
}

float AudibleCell::nextValue(float sampleTime) {
  float f = baseFrequency * harmonicNumber * freqMultiplicator;
  phase = phase + f * sampleTime;
  phase = phase - floor(phase);
  int phaseInt = floor(SIN_LUT_SIZE * phase);
  float rawVal = sinLut[phaseInt];
  float amp = amplitude;
  return rawVal * amp;
}

float AudibleCell::currentAmplitude() { return amplitude; }

DSP::DSP() {
  baseFreqLut.reserve(NUMCELLSX);
  for (int i = 0; i < NUMCELLSX; ++i) {
    baseFreqLut[i] = BASE_FREQ * pow(2.0f, (float)(i - NUMCELLSX / 2) / 12.0f);
  }
}

// todo change to split between the three
void DSP::paramValues(GridState state, float wideness, float roundness,
                      float vOct) {
  roundness = pow(10.f, roundness - 0.5f);
  wideness = wideness * (float)(NUMCELLSY - 1) + 1.f;
  // todo take into account muted rows
  std::vector<Cell *> alive;
  for (std::vector<Cell *>::iterator it = state.currentlyAlive.begin();
       it != state.currentlyAlive.end(); ++it) {
    if ((*it)->isAudible()) {
      alive.push_back(*it);
    }
  }
  if (this->vOct != vOct) {
    this->vOct = vOct;
    float mult = pow(2.0f, vOct);
    for (std::vector<AudibleCell *>::iterator it = audibles.begin();
         it != audibles.end(); ++it) {
      (*it)->freqMultiplicator = mult;
    }
  }
  if (this->wideness != wideness || this->roundness != roundness ||
      this->limitH != limitH) {
    this->wideness = wideness;
    this->roundness = roundness;
    this->limitH = limitH;
    // todo put into separate function
    // todo do the same with oldAudibles
    for (std::vector<AudibleCell *>::iterator it = audibles.begin();
         it != audibles.end(); ++it) {
      AudibleCell *c = *it;
      float numHarmonic = c->harmonicNumber;
      // amplitude
      float amplitude = computeAmplitude(wideness, roundness, numHarmonic);
      c->amplitude = amplitude;
    }
  }

  if (this->alive != alive) {
    this->alive = alive;
    audibles.clear();
    std::vector<int> harmonics;
    harmonics.reserve(NUMCELLSX);
    for (int x = 0; x < NUMCELLSX; ++x) {
      harmonics[x] = 0;
    }

    for (std::vector<Cell *>::iterator it = alive.begin(); it != alive.end();
         ++it) {
      Cell *c = *it;
      int x = c->getX();
      ++(harmonics[x]);
      float baseFrequency = baseFreqLut[x];
      float numHarmonic = harmonics[x];
      // amplitude
      float amplitude = computeAmplitude(wideness, roundness, numHarmonic);
      audibles.push_back(
          new AudibleCell(x, baseFrequency, numHarmonic, amplitude));
    }
  }
}

float DSP::nextValue(float sampleTime) {
  float audio = 0.f;
  float ampSum = 0.f;
  for (std::vector<AudibleCell *>::iterator it = audibles.begin();
       it != audibles.end(); ++it) {
    float val = (*it)->nextValue(sampleTime);
    float amp = (*it)->currentAmplitude();
    audio += val;
    ampSum += amp;
  }
  if (ampSum != 0.f) {
    audio = audio / ampSum;
  }
  return audio;
}

float DSP::computeAmplitude(float wideness, float roundness,
                            float numHarmonic) {
  float iNormalized = (numHarmonic - 1) / (wideness * wideness);
  float amplitude = std::max(1.f - (float)pow(iNormalized, roundness), 0.f);
  return amplitude;
}

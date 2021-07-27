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

float AudibleCell::nextValue(float sampleTime, float vOct) {
  nextEnvelopeValue(sampleTime);
  float f = baseFrequency * harmonicNumber * pow(2.0, vOct);
  phase = phase + f * sampleTime;
  phase = phase - floor(phase);
  int phaseInt = floor(SIN_LUT_SIZE * phase);
  float rawVal = sinLut[phaseInt];
  float amp = amplitude * envelope;
  return rawVal * amp;
}

void AudibleCell::nextEnvelopeValue(float sampleTime) {
  if ((raising && !decaying) || (!raising && decaying)) {
    envelopePhase = envelopePhase + (sampleTime / ENVELOPE_DURATION);
    if (raising) {
      if (envelopePhase > 1.0f) {
        envelope = 1.0f;
        raising = false;
      } else {
        envelope = std::sin(M_PI * envelopePhase / 2.0f);
      }
    } else { // decaying
      if (envelopePhase > 1.0f) {
        envelope = 0.0f;
        decaying = false;
      } else {
        envelope = 1 - std::sin(M_PI * envelopePhase / 2.0f);
      }
    }
  }
}

float AudibleCell::currentAmplitude() { return amplitude * envelope; }

DSP::DSP() {
  baseFreqLut.reserve(NUMCELLSX);
  for (int i = 0; i < NUMCELLSX; ++i) {
    baseFreqLut[i] = BASE_FREQ * pow(2.0f, (float)(i - NUMCELLSX / 2) / 12.0f);
  }
}

// todo change to split between the three
void DSP::paramValues(GridState state, float wideness, float roundness,
                      float vOct) {
  std::vector<Cell *> alive = state.currentlyAlive;
  if (this->vOct != vOct) {
    this->vOct = vOct;
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
      float iNormalized = (numHarmonic - 1) / (wideness * wideness);
      float roundness_adapt = pow(100.f, 0.5f - roundness);
      float amplitude =
          std::max(1.f - (float)pow(iNormalized, roundness_adapt), 0.f);
      c->amplitude = amplitude;
    }
  }

  if (this->alive != alive) {
    this->alive = alive;
    oldAudibles = audibles;
    for (std::vector<AudibleCell *>::iterator it = oldAudibles.begin();
         it != oldAudibles.end(); ++it) {
      (*it)->decaying = true;
      (*it)->envelopePhase = 0.f;
    }
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
      float iNormalized = (numHarmonic - 1) / (wideness * wideness);
      float roundness_adapt = pow(10.f, roundness - 0.5f);
      float amplitude =
          std::max(1.f - (float)pow(iNormalized, roundness_adapt), 0.f);
      audibles.push_back(
          new AudibleCell(x, baseFrequency, numHarmonic, amplitude));
    }
  }
}

float DSP::nextValue(float sampleTime) {
  float audio = 0.f;
  float ampSum = 0.f;
  for (std::vector<AudibleCell *>::iterator it = oldAudibles.begin();
       it != oldAudibles.end(); ++it) {
    audio += (*it)->nextValue(sampleTime, vOct);
    ampSum += (*it)->currentAmplitude();
  }
  for (std::vector<AudibleCell *>::iterator it = audibles.begin();
       it != audibles.end(); ++it) {
    float val = (*it)->nextValue(sampleTime, vOct);
    float amp = (*it)->currentAmplitude();
    // printf("val %f amp %f \n", val, amp);
    audio += val;
    ampSum += amp;
  }
  if (ampSum != 0.f) {
    audio = audio / ampSum;
  }
  return audio;
}

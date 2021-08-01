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
const std::vector<float> SIN_LUT = DSP::initSinLut();

DSP::DSP() {
  baseFreqLut.reserve(NUMCELLS_X);
  audibleCols.reserve(NUMCELLS_X);
  audibleRows.reserve(NUMCELLS_Y);
  for (int i = 0; i < NUMCELLS_X; i++) {
    baseFreqLut[i] =
        REFERENCE_FREQ * pow(2.0f, (float)(i - REFERENCE_POS) / 12.0f);
    // nothing to do with before, but use the same for loop to gain time
    audibleCols[i] = true;
  }
  for (int i = 0; i < NUMCELLS_Y; i++) {
    audibleRows[i] = true;
  }
}

void DSP::paramValues(std::vector<Cell *> state, float wideness,
                      float roundness, float vOct) {
  if (this->vOct != vOct) {
    this->vOct = vOct;
    vOctMult = pow(2.0f, vOct);
  }
  if (this->wideness != wideness || this->roundness != roundness) {
    this->wideness = wideness;
    this->roundness = roundness;
    SoundParams sp = SoundParams(wideness, roundness);
    for (int i = 0; i < vSize; i++) {
      float numHarmonic = harmonicNumbers[i];
      // amplitude
      float amplitude = computeAmplitude(&sp, numHarmonic);
      amplitudes[i] = amplitude;
    }
  }

  if (this->alive != state || audibilityChanged) {
    this->alive = state;
    std::vector<int> harmonics;
    harmonics.reserve(NUMCELLS_X);
    baseFrequencies.clear();
    harmonicNumbers.clear();
    amplitudes.clear();
    vSize = 0;
    baseFrequencies.reserve(NUMCELLS_X * NUMCELLS_Y); // worst case scenario
    harmonicNumbers.reserve(NUMCELLS_X * NUMCELLS_Y);
    amplitudes.reserve(NUMCELLS_X * NUMCELLS_Y);
    phases.reserve(NUMCELLS_X * NUMCELLS_Y);
    for (int x = 0; x < NUMCELLS_X; ++x) {
      harmonics[x] = 0;
    }
    float lf = HIGHEST_FREQUENCY + 1.f;
    for (std::vector<Cell *>::iterator it = alive.begin(); it != alive.end();
         ++it) {
      Cell *c = *it;
      if (isCellAudible(*it)) {
        int x = c->getX();
        ++(harmonics[x]);
        ++vSize;
        float baseFrequency = baseFreqLut[x];
        if (baseFrequency < lf) {
          lf = baseFrequency;
        }
        float numHarmonic = harmonics[x];
        // amplitude
        SoundParams sp = SoundParams(wideness, roundness);
        float amplitude = computeAmplitude(&sp, numHarmonic);
        amplitudes.push_back(amplitude);
        harmonicNumbers.push_back(harmonics[x]);
        baseFrequencies.push_back(baseFrequency);
        phases.push_back(0.f);
      }
    }
    if (lf != HIGHEST_FREQUENCY + 1.f) {
      lowestFreq = lf;
    }
  }
}

float DSP::nextValue(float sampleTime) {
  float audio = 0.f;
  float ampSum = 0.f;
  for (int i = 0; i < vSize; i++) {
    float baseFrequency = baseFrequencies[i];
    float harmonicNumber = harmonicNumbers[i];
    float amplitude = amplitudes[i];
    float phase = phases[i];
    float f = baseFrequency * harmonicNumber * vOctMult;
    phases[i] = phase + f * sampleTime;
    phase = phase - floor(phase);
    int phaseInt = floor(SIN_LUT_SIZE * phase);
    float rawVal = SIN_LUT[phaseInt];
    float val = amplitude * rawVal;
    audio += val;
    ampSum += amplitude;
  }
  if (ampSum != 0.f) {
    audio = audio / ampSum;
  }
  return audio;
}

SoundParams::SoundParams(float wideness, float roundness) {
  wide = wideness * WIDENESS_REF + 1.f;
  round = pow(10.f, roundness - 0.5f);
}

float DSP::computeAmplitude(SoundParams *sp, float numHarmonic) {
  float iNormalized = (numHarmonic - 1) / (sp->wide * sp->wide);
  float amplitude = std::max(1.f - (float)pow(iNormalized, sp->round), 0.f);
  return amplitude;
}

float DSP::getLowestFreq() { return lowestFreq; }

void DSP::muteUnmuteCol(int x, bool muted) {
  if (x < 0 || x > NUMCELLS_X) {
    return;
  }
  audibleCols[x] = muted;
}

void DSP::muteUnmuteRow(int y, bool muted) {
  if (y < 0 || y > NUMCELLS_Y) {
    return;
  }
  audibleRows[y] = muted;
}

bool DSP::isColAudible(int x) {
  if (x < 0 || x > NUMCELLS_X) {
    return false;
  }
  return audibleCols[x];
}

bool DSP::isRowAudible(int y) {
  if (y < 0 || y > NUMCELLS_Y) {
    return false;
  }
  return audibleRows[y];
}

bool DSP::isCellAudible(Cell *c) {
  return (audibleRows[c->getY()] && audibleCols[c->getX()]);
}

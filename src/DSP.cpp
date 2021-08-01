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

void DSP::paramValues(GridState state, float wideness, float roundness,
                      float vOct) {
  std::vector<Cell *> alive;
  for (std::vector<Cell *>::iterator it = state.currentlyAlive.begin();
       it != state.currentlyAlive.end(); ++it) {
    if (isCellAudible(*it)) {
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
  if (this->wideness != wideness || this->roundness != roundness) {
    this->wideness = wideness;
    this->roundness = roundness;
    SoundParams sp = SoundParams(wideness, roundness);
    for (std::vector<AudibleCell *>::iterator it = audibles.begin();
         it != audibles.end(); ++it) {
      AudibleCell *c = *it;
      float numHarmonic = c->harmonicNumber;
      // amplitude
      float amplitude = computeAmplitude(&sp, numHarmonic);
      c->amplitude = amplitude;
    }
  }

  if (this->alive != alive) {
    this->alive = alive;
    audibles.clear();
    std::vector<int> harmonics;
    harmonics.reserve(NUMCELLS_X);
    for (int x = 0; x < NUMCELLS_X; ++x) {
      harmonics[x] = 0;
    }
    float lf;
    if (alive.begin() != alive.end()) {
      lf = HIGHEST_FREQUENCY;
    } else {
      lf = lowestFreq;
    }
    for (std::vector<Cell *>::iterator it = alive.begin(); it != alive.end();
         ++it) {
      Cell *c = *it;
      int x = c->getX();
      ++(harmonics[x]);
      float baseFrequency = baseFreqLut[x];
      if (baseFrequency < lf) {
        lf = baseFrequency;
      }
      float numHarmonic = harmonics[x];
      // amplitude
      SoundParams sp = SoundParams(wideness, roundness);
      float amplitude = computeAmplitude(&sp, numHarmonic);
      audibles.push_back(
          new AudibleCell(x, baseFrequency, numHarmonic, amplitude));
    }
    lowestFreq = lf;
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

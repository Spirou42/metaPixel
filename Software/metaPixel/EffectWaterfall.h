/**
* EffectWaterfall.cpp
* FFT waterfall on display enegie translated to hueSpeed
* update is current delay
* no parameters
*/

#ifndef __EFFECT_WATERFALL_H__
#define __EFFECT_WATERFALL_H__
#include "Effect.h"
#include "Parameter.h"
#include "metaPixel.h"

#if USE_AUDIO_EFFECTS
class AudioInputI2S;
class AudioAnalyzeFFT256;
class AudioAnalyzeFFT1024;
class AudioConnection;
class AudioControlSGTL5000;
extern AudioInputI2S            audioIn;
extern AudioAnalyzeFFT1024       analyzeFFT;
extern AudioConnection          patchCord1;
extern AudioControlSGTL5000     AudioShield;


class EffectWaterfall : public Effect
{
public:
  EffectWaterfall():Effect("waterF"){};
  virtual void startEffect();
  virtual void frame(unsigned long now);
  virtual void stopEffect();
protected:
  void collectBins();
  uint8_t binCollection[20];
};

#endif
#endif

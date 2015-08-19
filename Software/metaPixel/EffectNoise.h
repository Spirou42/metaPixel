/**
* EffectNoise.h
*/

#ifndef __EFFECT_NOISE_H__
#define __EFFECT_NOISE_H__
#include "metaPixel.h"
#include "Effect.h"
#include "Parameter.h"
#include "metaDisplay.h"


#define NOISE_DIMENSION ((DISPLAY_WIDTH>DISPLAY_HEIGHT) ? DISPLAY_WIDTH : DISPLAY_HEIGHT)

class EffectNoise:public Effect
{
public:
  newParameter_t *noiseScale;
  newParameter_t *noiseSpeed;
  newParameter_t *hueSpeed;

  EffectNoise():Effect("Noise"),noiseScale(NULL),noiseSpeed(NULL),hueSpeed(NULL){};
  EffectNoise(newParameter_t* nScale,newParameter_t* nSpeed, newParameter_t* hSpeed):Effect("noise"),noiseScale(nScale),noiseSpeed(nSpeed),hueSpeed(hSpeed){};
  virtual void startEffect();
	virtual void frame(unsigned long now);
protected:
  int16_t noiseX;
  int16_t noiseY;
  int16_t noiseZ;
  uint8_t ihue;
  uint8_t noiseD[NOISE_DIMENSION][NOISE_DIMENSION];
  uint8_t noiseP[NOISE_DIMENSION][NOISE_DIMENSION];
  void fillnoise8();
};


#endif

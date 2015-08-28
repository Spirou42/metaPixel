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
  Parameter16_t *noiseScale;
  Parameter16_t *noiseSpeed;
  Parameter16_t *hueSpeed;
  Parameter16_t *effectMask;

  EffectNoise():Effect("Noise"),noiseScale(NULL),noiseSpeed(NULL),hueSpeed(NULL){};
  EffectNoise(Parameter16_t* nScale,Parameter16_t* nSpeed, Parameter16_t* hSpeed, Parameter16_t *eMask):Effect("Noise"),noiseScale(nScale),noiseSpeed(nSpeed),hueSpeed(hSpeed),effectMask(eMask){};
  virtual void startEffect();
	virtual void frame(unsigned long now);
  virtual void printParameter(Print& stream);
  virtual Parameter16_t* parameterAt(size_t idx);
protected:
  int16_t noiseX;
  int16_t noiseY;
  int16_t noiseZ;
  uint8_t ihue;
  uint8_t noiseH[NOISE_DIMENSION][NOISE_DIMENSION];
  uint8_t noiseS[NOISE_DIMENSION][NOISE_DIMENSION];
  uint8_t noiseV[NOISE_DIMENSION][NOISE_DIMENSION];
  void fillnoise8();
};


#endif

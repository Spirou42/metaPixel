/**
* EffectNoise.h
*/

#ifndef __EFFECT_PLASMA_H__
#define __EFFECT_PLASMA_H__
#include "Effect.h"
#include "Parameter.h"
#include "metaPixel.h"

#define NOISE_DIMENSION ((DISPLAY_WIDTH>DISPLAY_HEIGHT) ? DISPLAY_WIDTH : DISPLAY_HEIGHT)

class EffectPlasma:public Effect
{
public:
  newParameter_t *plasmaScale;
  newParameter_t *plasmaSpeed;
  newParameter_t *hueSpeed;
  newParameter_t *plasmaRadius;
  newParameter_t *plasmaMask;
  EffectPlasma():Effect("Plasma"),plasmaScale(NULL),plasmaSpeed(NULL),hueSpeed(NULL),plasmaRadius(NULL),plasmaMask(NULL){};
  EffectPlasma(newParameter_t* pScale, newParameter_t* pSpeed, newParameter_t* pRadius, newParameter_t* hSpeed, newParameter_t *mask):Effect("Plasma"),plasmaScale(pScale),plasmaSpeed(pSpeed),hueSpeed(hSpeed),plasmaRadius(pRadius),plasmaMask(mask){};
  virtual void startEffect();
  virtual void frame(unsigned long now);
};
#endif

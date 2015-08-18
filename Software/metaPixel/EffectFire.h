/**
      EffectFire.h

**/
#ifndef __EFFECT_FIRE_H__
#define __EFFECT_FIRE_H__
#include "Effect.h"
#include "Parameter.h"
#include "metaPixel.h"

class EffectFire:public Effect
{
public:
  newParameter_t *bottomHeating;
  newParameter_t *topCooling;
  EffectFire():Effect("Fire"),bottomHeating(NULL),topCooling(NULL){};
  EffectFire(newParameter_t* bottomH,newParameter_t* topCool):Effect("Fire"),bottomHeating(bottomH),topCooling(topCool){}
  void initializeEffect();
  virtual void runEffect(unsigned long now);
private:
  byte heatCells[NUM_LEDS];
  int16_t heatAt(int16_t x, int16_t y);
  int16_t  neightbours(int16_t x, int16_t y);
};

#endif // __EFFECT_FIRE_H__

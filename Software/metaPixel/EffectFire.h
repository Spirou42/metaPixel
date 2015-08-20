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
  Parameter16_t *bottomHeating;
  Parameter16_t *topCooling;
  int16_t _initPalette;
  EffectFire():Effect("Fire"),bottomHeating(NULL),topCooling(NULL){};
  EffectFire(Parameter16_t* bottomH,Parameter16_t* topCool):Effect("Fire"),bottomHeating(bottomH),topCooling(topCool){}
  void startEffect();
  virtual void frame(unsigned long now);
  virtual void stopEffect();
private:
  byte heatCells[NUM_LEDS];
  int16_t heatAt(int16_t x, int16_t y);
  int16_t  neightbours(int16_t x, int16_t y);
};

#endif // __EFFECT_FIRE_H__

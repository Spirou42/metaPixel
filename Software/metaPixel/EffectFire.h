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
  Parameter16_t *sparcleFreq;
  int16_t _initPalette;
  EffectFire():Effect("Fire"),bottomHeating(NULL),topCooling(NULL),sparcleFreq(NULL){};
  EffectFire(Parameter16_t* bottomH,Parameter16_t* topCool,Parameter16_t *sFreq):Effect("Fire"),bottomHeating(bottomH),topCooling(topCool),sparcleFreq(sFreq){}
  void startEffect();
  virtual void frame(unsigned long now);
  virtual void stopEffect();
  virtual void printParameter(Print& stream);
  virtual Parameter16_t* parameterAt(size_t idx){
    Parameter16_t * res = NULL;
    switch(idx){
      case 0: res = bottomHeating;  break;
      case 1: res = topCooling;     break;
      case 2: res = sparcleFreq; break;
      default:res = NULL; break;

    }
    return res;
  };
  virtual const char * parameterNameAt(size_t idx){
    const char *result = NULL;
    switch (idx) {
      case 0: result ="bHeat";  break;
      case 1: result ="Cool";  break;
      case 2: result ="Sparks"; break;
      default: result = NULL; break;
    }
    return result;
  };
  virtual size_t numberOfParameters(){return 3;};
private:
  byte heatCells[NUM_LEDS];
  int16_t heatAt(int16_t x, int16_t y);
  int16_t  neightbours(int16_t x, int16_t y);
};

#endif // __EFFECT_FIRE_H__

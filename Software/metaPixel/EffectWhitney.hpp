/**
* EffectWhitney.h
*/

#ifndef __EFFECT_WHITNEY_H__
#define __EFFECT_WHITNEY_H__
#include "Effect.hpp"
#include "Arduino.h"
class EffectWhitney:public Effect
{
public:
  Parameter16_t *cycleLength;
  Parameter16_t *lumenScale;
  Parameter16_t *hueSpeed;
  elapsedMillis millisSinceStart;
  unsigned long frameCounter;
  float speed;
  EffectWhitney():Effect("Drift"),cycleLength(NULL),lumenScale(NULL),hueSpeed(NULL){};
  EffectWhitney(Parameter16_t *pa,Parameter16_t *ls,Parameter16_t *hs):Effect("Drift"),cycleLength(pa),lumenScale(ls),hueSpeed(hs){};
  virtual void startEffect();
  virtual void frame(unsigned long now);
  virtual void stopEffect();
  //virtual void printParameter(Print& stream);
  virtual size_t numberOfParameters(){return 3;};
  virtual Parameter16_t* parameterAt(size_t idx){
    Parameter16_t* result = NULL;
    switch(idx){
      case 0: result = cycleLength; break;
      case 1: result = lumenScale; break;
      case 2: result = hueSpeed; break;
      default: result = NULL; break;
    }
    return result;
  }
  virtual const char * parameterNameAt(size_t idx){
    const char * result = NULL;
    switch(idx){
      case 0: result = "cycleL"; break;
      case 1: result = "lumenS"; break;
      case 2: result = "hueSpeed"; break;
      default: result = NULL; break;
    }
    return result;
  }

};
#endif

/**
* EffectWhitney.h
*/

#ifndef __EFFECT_WHITNEY_H__
#define __EFFECT_WHITNEY_H__
#include "Effect.h"
#include "Arduino.h"
class EffectWhitney:public Effect
{
public:
  Parameter16_t *cycleLength;
  elapsedMillis millisSinceStart;
  unsigned long frameCounter;
  float speed;
  EffectWhitney():Effect("Drift"),cycleLength(NULL){};
  EffectWhitney(Parameter16_t *pa):Effect("Drift"),cycleLength(pa){};
  virtual void startEffect();
  virtual void frame(unsigned long now);
  virtual void stopEffect();
  //virtual void printParameter(Print& stream);
  virtual size_t numberOfParameters(){return 1;};
  virtual Parameter16_t* parameterAt(size_t idx){
    Parameter16_t* result = NULL;
    switch(idx){
      case 0: result = cycleLength; break;
      default: result = NULL; break;
    }
    return result;
  }
  virtual const char * parameterNameAt(size_t idx){
    const char * result = NULL;
    switch(idx){
      case 0: result = "cycleL"; break;
      default: result = NULL; break;
    }
    return result;
  }

};
#endif

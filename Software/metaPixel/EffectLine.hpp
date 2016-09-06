/**
* EffectLine.h
*/

#ifndef __EFFECT_LINE_H__
#define __EFFECT_LINE_H__
#include "Effect.h"
#include "Parameter.h"
#include "metaPixel.h"

class EffectLine:public Effect
{
public:
  Parameter16_t *hueSpeed;
  EffectLine():Effect("Line"),hueSpeed(NULL){};
  EffectLine(Parameter16_t* hSpeed):Effect("Line"),hueSpeed(hSpeed){};
  virtual void startEffect();
  virtual void frame(unsigned long now);

  virtual size_t numberOfParameters(){return 1;};
  virtual void printParameter(Print& stream);
  virtual Parameter16_t* parameterAt(size_t idx){
    Parameter16_t* res = NULL;
    switch(idx){
      case 0: res = hueSpeed; break;
      default: break;
    }
    return res;
  }
  virtual const char* parameterNameAt(size_t idx){
    const char *result = NULL;
    switch(idx){
      case 0:result = "hSpeed"; break;
      default: result = NULL; break;
    }
    return result;
  }
protected:
};



#endif

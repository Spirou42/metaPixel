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
  Parameter16_t *plasmaSpeed;
  Parameter16_t *hueSpeed;
  Parameter16_t *plasmaScale;
  Parameter16_t *plasmaRadius;
  Parameter16_t *plasmaMask;
  EffectPlasma():Effect("Plasma"),plasmaSpeed(NULL),hueSpeed(NULL),plasmaScale(NULL),plasmaRadius(NULL),plasmaMask(NULL){};
  EffectPlasma(Parameter16_t* pScale, Parameter16_t* pSpeed, Parameter16_t* pRadius, Parameter16_t* hSpeed, Parameter16_t *mask):Effect("Plasma"),plasmaSpeed(pSpeed),hueSpeed(hSpeed),plasmaScale(pScale),plasmaRadius(pRadius),plasmaMask(mask){};
  virtual void startEffect();
  virtual void frame(unsigned long now);
  virtual void stopEffect();
  virtual void printParameter(Print& stream);
  virtual size_t numberOfParameters(){return 5;};
  virtual Parameter16_t* parameterAt(size_t idx){
    Parameter16_t* res = NULL;
    switch(idx){
      case 0: res = plasmaSpeed;break;
      case 1: res = hueSpeed;break;
      case 2: res = plasmaScale; break;
      case 3: res = plasmaRadius; break;
      case 4: res = plasmaMask; break;
      default: res = NULL; break;
    }
    return res;
  }
  virtual const char* parameterNameAt(size_t idx){
    const char * result = NULL;
    switch(idx){
      case 0: result = "pSpeed";  break;
      case 1: result = "hSpeed";  break;
      case 2: result = "pScale";  break;
      case 3: result = "pRadius"; break;
      case 4: result = "pMask";   break;
    }
    return result;
  };
};
#endif

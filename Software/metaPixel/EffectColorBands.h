/**
* EffectPlasmaSimple.h
*/

#ifndef __EFECT_COLORBANDS_H__
#define __EFECT_COLORBANDS_H__
#include "Effect.h"
#include "Parameter.h"
#include "metaPixel.h"

class EffectColorBands : public Effect {
public:
  Parameter16_t * windowScale;
  Parameter16_t * hueScale;
  Parameter16_t * plasmaSpeed;
  Parameter16_t * mirrorMask;

  EffectColorBands():Effect("Color Bands"),windowScale(NULL),hueScale(NULL),plasmaSpeed(NULL),mirrorMask(NULL){};
  EffectColorBands(Parameter16_t* wScale, Parameter16_t* hScale, Parameter16_t* pSpeed, Parameter16_t *miMask):Effect("ColorBands"),windowScale(wScale),hueScale(hScale),plasmaSpeed(pSpeed),mirrorMask(miMask){};
  virtual void startEffect();
  virtual void frame(unsigned long now);
  virtual Parameter16_t* parameterAt(size_t idx){
    Parameter16_t* res = NULL;
    switch(idx){
      case 0: res = windowScale;break;
      case 1: res = hueScale; break;
      case 2: res = plasmaSpeed; break;
      case 3: res = mirrorMask; break;
      default: res = NULL; break;
    }
    return res;
  };
  virtual const char * parameterNameAt(size_t idx){
    const char * result = NULL;
    switch(idx){
      case 0: result = "wScale";  break;
      case 1: result = "hScale";  break;
      case 2: result = "pSpeed";  break;
      case 3: result = "mMask";   break;
      default: result = NULL; break;
    }
    return result;
  };
  virtual size_t numberOfParameters(){return 4;};

  virtual void printParameter(Print& stream);

protected:
  void DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8);
};


#endif

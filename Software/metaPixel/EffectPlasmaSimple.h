/**
* EffectPlasmaSimple.h
*/

#ifndef __EFECT_PLASMASIMPLE_H__
#define __EFECT_PLASMASIMPLE_H__
#include "Effect.h"
#include "Parameter.h"
#include "metaPixel.h"

class EffectPlasmaSimple : public Effect {
public:
  Parameter16_t * windowScale;
  Parameter16_t * hueScale;
  Parameter16_t * plasmaSpeed;
  Parameter16_t * mirrorMask;

  EffectPlasmaSimple():Effect("PlaSim"),windowScale(NULL),hueScale(NULL),plasmaSpeed(NULL),mirrorMask(NULL){};
  EffectPlasmaSimple(Parameter16_t* wScale, Parameter16_t* hScale, Parameter16_t* pSpeed, Parameter16_t *miMask):Effect("PlaSim"),windowScale(wScale),hueScale(hScale),plasmaSpeed(pSpeed),mirrorMask(miMask){};
  virtual void startEffect();
  virtual void frame(unsigned long now);
protected:
  void DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8);
};


#endif

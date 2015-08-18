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
  newParameter_t * windowScale;
  newParameter_t * hueScale;
  newParameter_t * plasmaSpeed;
  newParameter_t * mirrorMask;

  EffectPlasmaSimple():Effect("PlaSim"),windowScale(NULL),hueScale(NULL),plasmaSpeed(NULL),mirrorMask(NULL){};
  EffectPlasmaSimple(newParameter_t* wScale, newParameter_t* hScale, newParameter_t* pSpeed, newParameter_t *miMask):Effect("PlaSim"),windowScale(wScale),hueScale(hScale),plasmaSpeed(pSpeed),mirrorMask(miMask){};
  virtual void initializeEffect();
  virtual void runEffect(unsigned long now);
protected:
  void DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8);
};


#endif

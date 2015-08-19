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
  EffectLine():Effect("Line"){};
  virtual void startEffect();
  virtual void frame(unsigned long now);
protected:
};



#endif

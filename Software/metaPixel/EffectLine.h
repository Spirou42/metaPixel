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
  virtual void initializeEffect();
  virtual void runEffect(unsigned long now);
protected:
};



#endif

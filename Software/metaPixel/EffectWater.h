#ifndef __EFFECT_WATER__
#define __EFFECT_WATER__
#include "Effect.h"
#include "metaPixel.h"

class EffectWater : public Effect
{
  public :
  EffectWater():Effect("Water"){};
  virtual void startEffect();
  virtual void frame(unsigned long now);
  virtual void stopEffect();
  virtual void printParameter(Print& stream);

  virtual Parameter16_t* parameterAt(size_t idx){
    Parameter16_t* res = NULL;
    switch(idx){
      default: res = NULL; break;
    }
    return res;
  };

  virtual const char* parameterNameAt(size_t idx){
    const char* result = NULL;
    switch (idx) {
      default: result = NULL; break;
    }
    return result;
  };
  virtual size_t numberOfParameters(){return 0;};
private:
  int16_t buffer1[DISPLAY_WIDTH][DISPLAY_HEIGHT];
  int16_t buffer2[DISPLAY_WIDTH][DISPLAY_HEIGHT];
};
#endif // __EFFECT_WATER__

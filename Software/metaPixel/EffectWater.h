#ifndef __EFFECT_WATER__
#define __EFFECT_WATER__
#include "Effect.h"
#include "metaPixel.h"

class EffectWater : public Effect
{
  public :
  Parameter16_t *damping;           ///< damping in percent

  EffectWater():Effect("Water"),damping(NULL){};
  EffectWater(Parameter16_t *damp):Effect("Water"),damping(damp){};
  virtual void startEffect();
  virtual void frame(unsigned long now);
  virtual void stopEffect();
  virtual void printParameter(Print& stream);

  virtual Parameter16_t* parameterAt(size_t idx){
    Parameter16_t* res = NULL;
    switch(idx){
      case 0: res = damping; break;
      default: res = NULL; break;
    }
    return res;
  };

  virtual const char* parameterNameAt(size_t idx){
    const char* result = NULL;
    switch (idx) {
      case 0: result ="Damping"; break;
      default: result = NULL; break;
    }
    return result;
  };
  virtual size_t numberOfParameters(){return 1;};
private:
  int16_t _initPalette;
  int16_t buffer1[DISPLAY_WIDTH*DISPLAY_HEIGHT];
  int16_t buffer2[DISPLAY_WIDTH*DISPLAY_HEIGHT];
  int16_t *b1,*b2;
  void ProcessWater(int16_t *source, int16_t *dest);
  void dropAdrop(int16_t *data);
};
#endif // __EFFECT_WATER__

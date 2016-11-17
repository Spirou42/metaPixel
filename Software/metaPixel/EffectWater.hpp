#ifndef __EFFECT_WATER__
#define __EFFECT_WATER__
#include "Effect.hpp"
#include "metaPixel.h"

class EffectWater : public Effect
{
  public :
  Parameter16_t *damping;           ///< damping in percent
  Parameter16_t *dropTime;          ///< drop rate
  Parameter16_t *hueBase;           ///<hue for the filter
  Parameter16_t *dropStrength;      ///< strength of the drop
  Parameter16_t *modeMask;         ///< a mode mask

  EffectWater():Effect("Water"),damping(NULL){};
  EffectWater(Parameter16_t *damp, Parameter16_t *drt, Parameter16_t *fs, Parameter16_t *ds, Parameter16_t *mM):
    Effect("Water"),damping(damp),dropTime(drt),hueBase(fs),dropStrength(ds),modeMask(mM){};
  virtual void startEffect();
  virtual void frame(unsigned long now);
  virtual void stopEffect();
  virtual void printParameter(Print& stream);

  virtual Parameter16_t* parameterAt(size_t idx){
    Parameter16_t* res = NULL;
    switch(idx){
      case 0: res = damping;      break;
      case 1: res = dropTime;     break;
      case 2: res = hueBase;      break;
      case 3: res = dropStrength; break;
      case 4: res = modeMask;     break;
      default: res = NULL;        break;
    }
    return res;
  };

  virtual const char* parameterNameAt(size_t idx){
    const char* result = NULL;
    switch (idx) {
      case 0: result ="Damping";      break;
      case 1: result ="DropDelay";    break;
      case 2: result ="Hue";          break;
      case 3: result ="DropStrength"; break;
      case 4: result ="Mode";         break;
      default: result = NULL;         break;
    }
    return result;
  };
  virtual size_t numberOfParameters(){return 5;};
private:
  elapsedMillis dropTimer;
  int16_t _initPalette;
  int16_t buffer1[DISPLAY_WIDTH*DISPLAY_HEIGHT];
  int16_t buffer2[DISPLAY_WIDTH*DISPLAY_HEIGHT];
  int16_t *b1,*b2;
  void ProcessWater(int16_t *source, int16_t *dest);
  void dropAdrop(int16_t *data);
  int16_t valueAt(int16_t* source, int16_t x,int16_t y);
  int16_t sumNeighbours(int16_t *source,int16_t x, int16_t y);
};
#endif // __EFFECT_WATER__

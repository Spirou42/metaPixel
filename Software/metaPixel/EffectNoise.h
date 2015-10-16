/**
* EffectNoise.h
*/

#ifndef __EFFECT_NOISE_H__
#define __EFFECT_NOISE_H__
#include "metaPixel.h"
#include "Effect.h"
#include "Parameter.h"
#include "metaDisplay.h"


#define NOISE_DIMENSION ((DISPLAY_WIDTH>DISPLAY_HEIGHT) ? DISPLAY_WIDTH : DISPLAY_HEIGHT)

class EffectNoise:public Effect
{
public:
  Parameter16_t *noiseScale;
  Parameter16_t *noiseSpeed;
  Parameter16_t *hueSpeed;
  Parameter16_t *effectMask;

  EffectNoise():Effect("Noise"),noiseScale(NULL),noiseSpeed(NULL),hueSpeed(NULL){};
  EffectNoise(Parameter16_t* nScale,Parameter16_t* nSpeed, Parameter16_t* hSpeed, Parameter16_t *eMask):Effect("Noise"),noiseScale(nScale),noiseSpeed(nSpeed),hueSpeed(hSpeed),effectMask(eMask){};
  virtual void startEffect();
	virtual void frame(unsigned long now);
  virtual void printParameter(Print& stream);
  virtual Parameter16_t* parameterAt(size_t idx){
  		Parameter16_t* res = NULL;
  		switch(idx){
  			case 0: res = noiseScale; break;
  			case 1: res = noiseSpeed; break;
  			case 2: res = hueSpeed; break;
        case 3: res = effectMask; break;
  			default: break;
  		}
  		return res;
  };
  virtual const char * parameterNameAt(size_t idx){
    const char *result = NULL;
    switch(idx){
      case 0: result = "nScale";  break;
      case 1: result = "nSpeed";  break;
      case 2: result = "hSpeed";  break;
      case 3: result = "nMask"; break;
      default: result = NULL; break;
    }
    return result;
  };
  virtual size_t numberOfParameters(){return 4;};

protected:
  int16_t noiseX;
  int16_t noiseY;
  int16_t noiseZ;
  uint8_t ihue;
  int8_t noiseH[NOISE_DIMENSION][NOISE_DIMENSION];
  int8_t noiseS[NOISE_DIMENSION][NOISE_DIMENSION];
  int8_t noiseV[NOISE_DIMENSION][NOISE_DIMENSION];
  void fillnoise8();
};


#endif

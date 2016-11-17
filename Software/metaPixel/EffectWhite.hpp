
#ifndef __EFFECT_WHITE_H__
#define __EFFECT_WHITE_H__
#include "Effect.hpp"
#include "metaPixel.h"

class EffectWhite : public Effect
{
public:
	Parameter16_t *colorIndexParam;
	Parameter16_t *saturationParam;
	Parameter16_t *valueParameter;

	EffectWhite():Effect("White"),colorIndexParam(NULL){};

	EffectWhite(Parameter16_t* colIdxPar, Parameter16_t* saturation, Parameter16_t *value):Effect("White"),colorIndexParam(colIdxPar),
	saturationParam(saturation),valueParameter(value){};

	virtual void startEffect();
	virtual void frame(unsigned long now);
	virtual void stopEffect();
	virtual void printParameter(Print& stream);
	virtual Parameter16_t* parameterAt(size_t idx){
		Parameter16_t* res = NULL;
		switch(idx){
			case 0: res = colorIndexParam; break;
			case 1: res = saturationParam; break;
			case 2: res = valueParameter; break;
			default: res = NULL;break;
		}
		return res;
	};
	virtual const char* parameterNameAt(size_t idx){
		const char * result = NULL;
		switch(idx){
			case 0: result = "HueIDX"; break;
			case 1: result = "Saturation"; break;
			case 2: result = "Value"; break;
			default: result = NULL; break;
		}
		return result;
	};
	virtual size_t numberOfParameters(){return 3;};
};


#endif

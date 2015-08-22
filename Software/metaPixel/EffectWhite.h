
#ifndef __EFFECT_WHITE_H__
#define __EFFECT_WHITE_H__
#include "Effect.h"
#include "metaPixel.h"

int white(unsigned long now,void* userdata);

class EffectWhite : public Effect
{
public:
	Parameter16_t *colorIndexParam;

	EffectWhite():Effect("White"),colorIndexParam(NULL){};

	EffectWhite(Parameter16_t* colIdxPar):Effect("White"),colorIndexParam(colIdxPar){};
	virtual void startEffect();
	virtual void frame(unsigned long now);
	virtual void stopEffect();
	virtual void printParameter(Print& stream);
	virtual Parameter16_t* parameterAt(size_t idx){
		Parameter16_t* res = NULL;
		switch(idx){
			case 0: res = colorIndexParam; break;
			default: res = NULL;break;
		}
		return res;
	}
};


#endif

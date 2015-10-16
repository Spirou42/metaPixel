/**
Effect.cpp
**/
#include "Effect.h"
#include "Parameter.h"
#include "metaPixel.h"


int effectRunner(unsigned long now, void* userdata)
{
	static uint16_t lastP = 0;
	uint16_t t = EffectProgram.currentValue()%(newMaxPrograms);
	Effect *effect = effectProgramsN[t].program;
	Effect *lastEffect = effectProgramsN[lastP].program;
	if(t != lastP){
		lastEffect->stopEffect();
		lastP = t;
		effect->startEffect();
		Serial << clearHome;
		if(effectProgramsN[t].parameter){
			commandProcessor((char*)effectProgramsN[t].parameter,true);
		}
		dumpParameters();
		Serial <<endl;
	}
	for(int i=0;i<(parameterArraySize-1);i++){
		AnimationValue *k = parameterArray[i].value;
		if(k->_shouldAnimate){
			k->animateParameter();
		}
	}
	effect->frame(now);
	return 0;
}

/**
Effect.cpp
**/
#include "Effect.h"
#include "Parameter.h"
#include "metaPixel.h"


//typedef int (*queuedFunction)(unsigned long,void* userData);
int16_t Effect::getValueFor(newParameter_t *someValue,int16_t defaultValue)
{
		if(NULL!=someValue){
			return someValue->value.currentValue();
		}else{
			return defaultValue;
		}
}

void Effect::setMaxValueFor(newParameter_t *someValue,int16_t maxVal)
{
		if(NULL!=someValue){
			someValue->maxValue = maxVal;
		}
}

int effectRunner(unsigned long now, void* userdata)
{
	static uint16_t lastP = 0;
	uint16_t t = EffectProgram.currentValue()%3;
	Effect *effect = effectProgramsN[t].program;
	if(t != lastP){
		lastP = t;
		effect->initializeEffect();
	}
	effect->runEffect(now);
	return 0;
}

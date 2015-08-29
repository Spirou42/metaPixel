/**
Effect.cpp
**/
#include "Effect.h"
#include "Parameter.h"
#include "metaPixel.h"


//typedef int (*queuedFunction)(unsigned long,void* userData);
int16_t Effect::getValueFor(Parameter16_t *someValue,int16_t defaultValue)
{
		if(NULL!=someValue){
			return someValue->value->currentValue();
		}else{
			return defaultValue;
		}
}

void Effect::setMaxValueFor(Parameter16_t *someValue,int16_t maxVal)
{
		if(NULL!=someValue){
			someValue->maxValue = maxVal;
		}
}
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
		Serial << clearHome<<effect<<endl;
		if(effectProgramsN[t].parameter){
			commandProcessor((char*)effectProgramsN[t].parameter);
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

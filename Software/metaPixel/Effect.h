/**
	 Effect.h
	 @description: simple wrapper class for effect programs

**/

#ifndef __EFFECT_H__
#define __EFFECT_H__
#include <Arduino.h>
#include <Streaming.h>
#include "Parameter.h"

class Effect
{
public:
	const char *name;
	Effect():name(""){};
	Effect(const char *p):name(p){};

	virtual void startEffect(){}; ///< book keeping on startup initialize max and minvalues
	virtual void frame(unsigned long now)=0;
	virtual void stopEffect(){};
	const char * getName(){return name;}
	int16_t getValueFor(newParameter_t *someValue,int16_t defaultValue=0);
	void setMaxValueFor(newParameter_t *someValue,int16_t maxVal );
	const char getCodeFor(newParameter_t *someValue)
	{
		if(NULL!=someValue){return someValue->code;}else{return 0x00;}
	};
};

//typedef int (*queuedFunction)(unsigned long,void* userData);
int effectRunner(unsigned long now, void* userdata);

#endif

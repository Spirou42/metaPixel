/**
	 Effect.h
	 @description: simple wrapper class for effect programs

**/

#ifndef __EFFECT_H__
#define __EFFECT_H__
#include <Arduino.h>
#include <Streaming.h>
#include "VT100Stream.h"
#include "Parameter.h"

class Effect
{
public:
	const char *name;
	Effect():name(""){};
	Effect(const char *p):name(p){};

	virtual void startEffect(){}; ///< book keeping on startup initialize max and minvalues
	virtual void frame(unsigned long now){};
	virtual void stopEffect(){};
	virtual void printParameter(Print& stream){};
	const char * getName(){return name;}

	virtual size_t 					numberOfParameters(){return 0;};
	virtual Parameter16_t* 	parameterAt(size_t idx){Serial <<"Gnartz";return NULL;};
	virtual const char* 		parameterNameAt(size_t idx){Serial <<"Gnartz2";return NULL;};

	int16_t getValueFor(Parameter16_t *someValue,int16_t defaultValue=0);
	void setMaxValueFor(Parameter16_t *someValue,int16_t maxVal );
	const char getCodeFor(Parameter16_t *someValue)
	{
		if(NULL!=someValue){return someValue->code;}else{return 0x00;}
	};
	friend Print& operator<<(Print &output, Effect *d){
		output<<"Effect: "<<bold<<d->name<<normal<<endl;
		output<<"Parameters: ";
		d->printParameter(output);
		return output;
	}
};

//typedef int (*queuedFunction)(unsigned long,void* userData);
int effectRunner(unsigned long now, void* userdata);

#endif

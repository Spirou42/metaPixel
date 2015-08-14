#include "Effect.h"
#include "EffectWhite.h"
#include <Arduino.h>

void EffectWhite::runEffect(unsigned long now)
{
	Serial <<"EffectWhite ";
	int16_t val = getValueFor(colorIndexParam,0);
	Serial<< "Value ("<<getCodeFor(colorIndexParam)<<"): "<<val<<endl;;
//	return 0;
}

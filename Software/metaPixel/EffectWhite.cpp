#include "Effect.h"
#include "EffectWhite.h"
#include <Arduino.h>


void EffectWhite::startEffect()
{
	Serial <<"White startup"<<endl;
	setMaxValueFor(colorIndexParam,255);
	setMaxValueFor(saturationParam,255);
	setMaxValueFor(valueParameter,255);
	colorIndexParam->value->initTo(0);
	saturationParam->value->initTo(0);
	valueParameter->value->initTo(0);

	colorIndexParam->value->animateTo(255,40000);
}
void EffectWhite::frame(unsigned long now)
{
	CRGB color = ColorFromPalette(colorPalettes[Palette.currentValue()],colorIndexParam->value->currentValue());
	CHSV hcolor = rgb2hsv(color);
	hcolor.saturation = saturationParam->value->currentValue();
	hcolor.value = valueParameter->value->currentValue();
	display.fill(hcolor);
	display.flush();
}

void EffectWhite::stopEffect(){
	Serial << "White Stopped"<<endl;
	colorIndexParam->value->initTo(0);
}

void EffectWhite::printParameter(Print& stream){
	stream <<"ColorIDX: "<<*colorIndexParam<<endl;
}

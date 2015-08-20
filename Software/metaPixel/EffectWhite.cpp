#include "Effect.h"
#include "EffectWhite.h"
#include <Arduino.h>


void EffectWhite::startEffect()
{
	setMaxValueFor(colorIndexParam,255);
	colorIndexParam->value=55;
//	colorIndexParam->value.animateTo(255,5000);
}
void EffectWhite::frame(unsigned long now)
{
	CRGB color = ColorFromPalette(colorPalettes[Palette.currentValue()],colorIndexParam->value.currentValue());
	display.fill(color);
	display.flush();
}

void EffectWhite::stopEffect(){
	colorIndexParam->value.initTo(0);
}

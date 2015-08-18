#include "Effect.h"
#include "EffectWhite.h"
#include <Arduino.h>


void EffectWhite::initializeEffect()
{
	setMaxValueFor(colorIndexParam,255);
	colorIndexParam->value.initTo(128);
}
void EffectWhite::runEffect(unsigned long now)
{
	CRGB color = ColorFromPalette(colorPalettes[Palette.currentValue()],colorIndexParam->value.currentValue());
	display.fill(color);
	display.flush();
}

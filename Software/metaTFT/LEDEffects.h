/** Collection of simple LED-Effects for LED stripes */

#ifndef __LEDEFFECTS_H__
#define __LEDEFFECTS_H__

#include "FastLED.h"

typedef void (*SimplePatternList[])();

extern SimplePatternList simplePatterns;
void rainbow();
void rainbowWithGlitter();
void confetti();
void sinelon();
void bpm();
void juggle();

#endif

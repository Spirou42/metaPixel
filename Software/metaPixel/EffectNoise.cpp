/**
* EffectNoise.cpp
*
*  R15U5V6M1   D100C8Q0Z5
*  R20U8V1M5   D100C0Q0Z12
*  R35U5V253M5 D100C0Q0Z3
*
*
*  R15U5V6M1   D100C8Q0Z5  		&20c0&20@u5,8,30@z5,12,30@r15,20,30@v6,1,15%vm5
* [R20U8V1M5   D100C0Q0Z12]		&20v253@u8,5,30@z12,3,30@r20,35,30%z&15m1c8
* [R35U5V253M5 D100C0Q0Z3]
*/

#include "EffectNoise.h"
#include <Arduino.h>
#include "Streaming.h"
void EffectNoise::fillnoise8() {
	for(int i = 0; i < NOISE_DIMENSION; i++) {
		int ioffset = noiseScale->value->currentValue() * i;
		for(int j = 0; j < NOISE_DIMENSION; j++) {
			int joffset = noiseScale->value->currentValue() * j;
			noiseH[i][j] = inoise8(noiseX + ioffset, noiseY + joffset, noiseZ+j);
			noiseS[i][j] = inoise8(noiseX + ioffset, noiseY + joffset, noiseZ+i);
			noiseV[i][j] = inoise8(noiseX + ioffset, noiseY + joffset, noiseZ+i-j);
		}
	}
	noiseZ += noiseSpeed->value->currentValue();
}

void EffectNoise::startEffect()
{
  noiseX = random16();
  noiseY = random16();
  noiseZ = random16();
	BlendParam.initTo(5);
	Delay.initTo(100);
	MirrorParam.initTo(0);

  initValueFor(noiseSpeed,4);
  initValueFor(noiseScale,15);
  initValueFor(hueSpeed,253);
	initValueFor(effectMask,5);

	setMaxValueFor(noiseSpeed,255);
  setMaxValueFor(noiseScale,255);
  setMaxValueFor(hueSpeed,255);
	setMaxValueFor(effectMask,7);
  //Serial <<noiseSpeed->code<<","<<noiseScale->code<<","<<hueSpeed->code<<endl;
  Palette=(0);
}

void EffectNoise::frame(unsigned long now)
{
  fillnoise8();
  for(int i = 0; i < display.displayWidth(); i++) {
    for(int j = 0; j < display.displayHeight(); j++) {
      // We use the value at the (i,j) coordinate in the noise
      // array for our brightness, and the flipped value from (j,i)
      // for our pixel's hue.
			uint8_t targetIndex = ihue;
			if(effectMask->value->currentValue() & 0x1){
				targetIndex += noiseH[i][j];
			}
      CRGB color = ColorFromPalette(colorPalettes[Palette.currentValue()],targetIndex);
      CHSV hcolor = rgb2hsv(color);
			if(effectMask->value->currentValue() & 0x2){
				hcolor.s += noiseS[j][i];
			}
			if(effectMask->value->currentValue() & 0x4){
				hcolor.v += noiseV[i][j];
			}
      display.setPixel(i,j,hcolor);


      // You can also explore other ways to constrain the hue used, like below
      // leds[XY(i,j)] = CHSV(ihue + (noise[j][i]>>2),255,noise[i][j]);
    }
  }
  ihue+=hueSpeed->value->currentValue();
  display.flush();
}
void EffectNoise::printParameter(Print& stream){
	stream << "Noise scale: "<<*noiseScale<<" \tNoise speed: "<<*noiseSpeed<<" \thue Speed: "<<*hueSpeed<<" \tMask: "<<*effectMask<<endl;
}

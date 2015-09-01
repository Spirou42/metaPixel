/**
* EffectNoise.cpp
*/

#include "EffectNoise.h"
#include <Arduino.h>
#include "Streaming.h"
void EffectNoise::fillnoise8() {
	for(int i = 0; i < NOISE_DIMENSION; i++) {
		int ioffset = noiseScale->value->currentValue() * i;
		for(int j = 0; j < NOISE_DIMENSION; j++) {
			int joffset = noiseScale->value->currentValue() * j;
			noiseH[i][j] = inoise8(noiseX + ioffset, noiseY + joffset, noiseZ);
			noiseS[i][j] = inoise8(noiseY + joffset, noiseX + ioffset, noiseZ);
			noiseV[i][j] = inoise8(noiseX + ioffset, noiseY + joffset, noiseZ);
		}
	}
	noiseZ += noiseSpeed->value->currentValue();
}

void EffectNoise::startEffect()
{
  noiseX = random16();
  noiseY = random16();
  noiseZ = random16();
  noiseSpeed->value->initTo(2);
  noiseScale->value->initTo(1);
	noiseScale->value->bounce(90,600000);
  hueSpeed->value->initTo(0);
	effectMask->value->initTo(1);
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
				hcolor.s += noiseS[i][j];
			}
			if(effectMask->value->currentValue() & 0x4){
				hcolor.v += noiseV[i][j];
			}
      //hcolor.v = noiseP[i][j];

        //          hcolor = CHSV(ihue + (noiseD[i][j]>>1),255/*(noiseP[i][j]<<3)*/,255/*noiseP[i][j]*/);
            //hcolor = CHSV(noiseD[j][i],255,noiseD[i][j]);

            //color = ColorFromPalette(colorPalettes[currentPalette],noiseD[j][i]);
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

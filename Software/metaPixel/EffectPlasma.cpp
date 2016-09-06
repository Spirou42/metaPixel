/**
* EffectPlasma.cpp
* D150 C0 B160 Q0 Z8000 U12 V0 I6 R10 M15
*/

#include "EffectPlasma.hpp"
#include "Arduino.h"
#include "Streaming.h"


#define PSCALE (plasmaScale->value->currentValue())
#define KSCALE (10.0)

void EffectPlasma::startEffect()
{
  plasmaScale->value->initTo(3);
  plasmaSpeed->value->initTo(10);
  hueSpeed->value->initTo(0);
  plasmaRadius->value->initTo(display.displayWidth()/2);
  plasmaMask->value->initTo(0);
  //plasmaMask->value->animateTo(15,60000);
  Palette=0;
  setMaxValueFor(plasmaScale,255);
  setMaxValueFor(plasmaSpeed,255);
  setMaxValueFor(plasmaRadius,255);
  setMaxValueFor(plasmaMask,31);
  setMaxValueFor(hueSpeed,255);
}


void EffectPlasma::frame(unsigned long now)
{
  static MPPixel center = MPPixel();
	center.x = display.displayWidth()/2.0;
	center.y = display.displayHeight()/2.0;
	static float k = 0;
	static int paletteShift = 0;

	int xOffset = int(0 + (plasmaRadius->value->currentValue() * cos(k*KSCALE ))) ;
	int yOffset = int(0 + (plasmaRadius->value->currentValue() *-sin(k*KSCALE )));

//	Serial <<"k:"<<k<<" Xo: "<<xOffset<<" ";
	center.x+=xOffset;
	center.y+=yOffset;

//	Serial<<"cx:"<<center.x<<endl;
	for(int x = 0; x < display.displayWidth(); x++){
		for(int y = 0; y < display.displayHeight(); y++)
		{
			MPPixel current = MPPixel(x/*+xOffset*/,y/*+yOffset*/);
			int div = 0;
			int colorIndex=0;
			// Vertical Stripes
			if(plasmaMask->value->currentValue() & VerticalEffect){
				colorIndex += int(128.0 + (128.0 * sin( (float)(current.x+xOffset) / (PSCALE*1) )));div++;
			}

			// Horzontal Stripes
			if(plasmaMask->value->currentValue() & HorizontalEffect){
				colorIndex += int(128.0 + (128.0 * sin( (current.y+yOffset) / (PSCALE*1)  )));div++;
			}

			// Diagonal Stripes
			if(plasmaMask->value->currentValue() & DiagonalEffect){
				colorIndex += int(128.0 + (128.0 * sin((current.x+yOffset + current.y+xOffset) / (PSCALE*1) )));div++;
			}

			// Circle
			if(plasmaMask->value->currentValue() & CircleEffect){
				colorIndex += int(128.0 + (128.0 * sin(center.distanceTo(current) / (PSCALE*1) )));div++;
			}

			colorIndex /=div;
			CRGB color = ColorFromPalette(colorPalettes[Palette.currentValue()],(colorIndex+paletteShift)%256);
			display.setPixel(x,y,color);
		}
	}
	//	Serial<<"Center: ("<<center.x<<", "<<center.y<<")"<<endl;
//		display.setPixel(center,CRGB::White);
	paletteShift+=hueSpeed->value->currentValue();
	paletteShift%=256;
	k+=(M_PI/360.0)*(plasmaSpeed->value->currentValue()/10.0);
  display.flush();
}

void EffectPlasma::printParameter(Print& stream)
{
   stream << "pSpeed: "<<*plasmaSpeed<<" \thueSpeed: "<<*hueSpeed<<" \tpScale: "<<*plasmaScale<<" \tpRadius: "<<*plasmaRadius<<" \tpMask: "<<*plasmaMask<<endl;
}
void EffectPlasma::stopEffect()
{
    plasmaMask->value->initTo(0);
}

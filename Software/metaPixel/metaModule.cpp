/**
* metaModule
*/

#include "metaModule.h"

uint16_t metaModule::XY( uint8_t x, uint8_t y)
{
	uint16_t i;
	if(this->flipped){
		int tx = this->w-1-x;
		int ty = this->h-1-y;
		x = tx;
		y=ty;
	}
	{
		if (x & 0x01){
			//odd collumns run Backwards
			uint8_t reverseY = (h - 1) - y;
			i = (x * h)+reverseY;
		}else{
			i = (x * h)+y;
		}
	}
	return i;
}
uint16_t metaModule::XYabs(uint8_t x, uint8_t y){
	return baseOffset+XY(x,y);
}

void metaModule::setToMeanColor()
{
	uint16_t rComp=0,gComp=0,bComp=0;
	for(uint8_t xcoord=0;xcoord<w;++xcoord){
		for(uint8_t ycoord=0;ycoord<h;++ycoord){
			CRGB color = baseAddress[XY(xcoord,ycoord)];
			rComp += color.r;
			gComp += color.g;
			bComp += color.b;
		}
	}
	CRGB mcolor = CRGB(rComp/(w*h),gComp/(w*h),bComp/(w*h));
//	Serial <<"Color "<<mcolor.r<<" ";
	for(uint8_t xcoord=0;xcoord<w;++xcoord){
		for(uint8_t ycoord=0;ycoord<h;++ycoord){
			uint16_t offset= XY(xcoord,ycoord);
	 		baseAddress[offset]=mcolor;
	 	}
	 }
}

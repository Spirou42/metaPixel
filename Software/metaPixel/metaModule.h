/*
* metaModule class
* simple class describing a single metaModule Grid

*
*/
#ifndef __metaModule_H__
#define __metaModule_H__
#define FASTLED_INTERNAL
#include "Arduino.h"
#include "Streaming.h"
#include "FastLED.h"

struct MPPixel{
public:
	int16_t x;
	int16_t y;
	MPPixel(int16_t xC=0,int16_t yC=0):x(xC),y(yC){}
	inline double distanceTo(const MPPixel& dest){
		return sqrt((x-dest.x)*(x-dest.x) +(y-dest.y)*(y-dest.y));
	}
};

class metaModule {
protected:
	CRGB *baseAddress;          ///< start address of the first pixel in the frameBuffer this metaModule controlls
	uint16_t baseOffset;        ///< offset to the start of the array
	int8_t w;
	int8_t h;
	boolean flipped;

public:
	metaModule(int8_t width = 4 ,int8_t height = 5, boolean flipped = false) : w(width), h(height),flipped(flipped)
	{
		baseAddress = NULL;
	}

	void setFlipped(boolean isFlipped){flipped = isFlipped;}
	boolean getFlipped(){return flipped;}
	void setBaseAddress(CRGB* address){baseAddress = address;}
	CRGB* getBaseAddress(){return baseAddress;}
	void setBaseOffset(uint16_t offset){baseOffset = offset;}
	uint16_t getBaseOffset(){return baseOffset;}

	uint16_t XY( uint8_t x, uint8_t y);
};

#endif

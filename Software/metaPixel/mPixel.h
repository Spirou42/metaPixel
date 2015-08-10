/*
* mPixel class
* simple class describing a single mPixel Grid

*
*/
#ifndef __MPIXEL_H__
#define __MPIXEL_H__

#include <FastLED.h>

struct MPPixel{
public:
	int16_t x;
	int16_t y;
	MPPixel(int16_t xC=0,int16_t yC=0):x(xC),y(yC){}
	inline double distanceTo(const MPPixel& dest){
		return sqrt((x-dest.x)*(x-dest.x) +(y-dest.y)*(y-dest.y));
	}
};

class mPixel {
protected:
	CRGB *baseAddress;          ///< start address of the first pixel in the frameBuffer this mPixel controlls
	uint16_t baseOffset;        ///< offset to the start of the array
	int8_t w;
	int8_t h;
	boolean flipped;

public:
	mPixel(int8_t width = 4 ,int8_t height = 5, boolean flipped = false) : w(width), h(height),flipped(flipped)
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
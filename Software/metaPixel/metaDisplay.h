/*
* metaDisplay - a rectangular grid of metaModule modules
*/
#ifndef __metaDisplay_H__
#define __metaDisplay_H__

#include "metaModule.h"
typedef enum {noMirror=0,horizontalMirror=1,verticalMirror=2,quadMirror=3} displayMirror;

class metaDisplay {
protected:
	uint8_t 	moduleWidth,moduleHeight;      ///< number of pixels in each module
	uint8_t 	width,height;                  ///< number of modules in display
	CRGB* 		ledBuffer;
	CRGB* 		backbuffer;
	metaModule	  *moduleBuffer;
	displayMirror mirMode;
public:
	metaDisplay():moduleWidth(0),moduleHeight(0),width(0),height(0) {};
	metaDisplay(CRGB* buffer, CRGB* bbuff=NULL, uint8_t w=2,uint8_t h=2);/*:moduleWidth(4),moduleHeight(5),width(w),height(h),ledBuffer(buffer),backbuffer(bbuff);*/

	uint16_t XY(uint8_t x, uint8_t y){ return this->XY(MPPixel(x,y));};

	uint16_t XYModule( uint8_t x, uint8_t y);

	uint16_t XY(MPPixel currentPixel);

	uint16_t displayWidth();
	uint16_t displayHeight();
	void setMirrorMode(displayMirror mirM){
		if(mirMode != mirM){
			mirMode = mirM;
			clear();
		}
	};
	displayMirror mirrorMode(){return mirMode;};


	void setPixel(uint16_t x, uint16_t y, CRGB color);
	void setPixel(MPPixel coord,CRGB color);
	CRGB getPixel(uint16_t x, uint16_t y);
	CRGB getPixel(MPPixel coord);

	void scrollDown(uint8_t scanLines,bool clear=false);
	void clear();

	void clearAll();
	void fill(CRGB color);

	void flush();
	void setBrightness(uint8_t b);
	uint8_t brightness();

	// Bresenham line algorythm
	void line(MPPixel st, MPPixel en, CRGB color);
private:
	void _setPixel(MPPixel coord, CRGB color);
};

int backbufferBlender(unsigned long now, void* userdata);

CHSV rgb2hsv(const CRGB& rgb);
#endif

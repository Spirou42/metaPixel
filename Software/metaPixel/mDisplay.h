/*
* mDisplay - a rectangular grid of mPixel modules
*/
#ifndef __MDISPLAY_H__
#define __MDISPLAY_H__

#include "mPixel.h"

#define MODULES_WIDTH   5												// number of modules in X
#define MODULES_HEIGHT  4											// number of modules in Y
#define MODULE_WIDTH    4												// width of module in pixels
#define MODULE_HEIGHT   5												// height of modules in pixels

#define DISPLAY_WIDTH   (MODULES_WIDTH * MODULE_WIDTH)
#define DISPLAY_HEIGHT  (MODULES_HEIGHT * MODULE_HEIGHT)

typedef enum {lowRes,midRes,hiRes} displayResolution;
typedef enum {noMir,horMir,verMir,quadMir} displayMirror;

class mDisplay {
protected:
	uint8_t 	moduleWidth,moduleHeight;      ///< number of pixels in each module
	uint8_t 	width,height;                  ///< number of modules in display
	CRGB* 		ledBuffer;
	CRGB* 		backbuffer;
	mPixel	  *moduleBuffer;
	displayResolution reso;
	displayMirror mirMode;
public:
	mDisplay():moduleWidth(MODULE_WIDTH),moduleHeight(MODULE_HEIGHT),width(MODULES_WIDTH),height(MODULES_HEIGHT) {};
	mDisplay(CRGB* buffer, CRGB* bbuff=NULL, uint8_t w=2,uint8_t h=2);/*:moduleWidth(4),moduleHeight(5),width(w),height(h),ledBuffer(buffer),backbuffer(bbuff);*/

	uint16_t XY(uint8_t x, uint8_t y){ return this->XY(MPPixel(x,y));};

	uint16_t XYModule( uint8_t x, uint8_t y);

	uint16_t XY(MPPixel currentPixel);

	uint16_t displayWidth();
	uint16_t displayHeight();

	void setPixel(uint16_t x, uint16_t y, CRGB color);

	void setPixel(MPPixel coord,CRGB color);

	void clear();

	void clearAll();
	void fill(CRGB color);

	void flush();
	void setBrightness(uint8_t b);
	uint8_t brightness();
	void setResolution(displayResolution res);
	displayResolution resolution(){return reso;};

	// Bresenham line algorythm
	void line(MPPixel st, MPPixel en, CRGB color);
private:
	void _setPixel(MPPixel coord, CRGB color);
};

CHSV rgb2hsv(const CRGB& rgb);
#endif

/**
* metaDisplay.cpp
*/
#include "metaDisplay.h"
#include "Streaming.h"
#include "Arduino.h"
metaModule	  moduleBuffer[MODULES_WIDTH*MODULES_HEIGHT];
metaDisplay::metaDisplay(CRGB* buffer, CRGB* bbuff, uint8_t w,uint8_t h):
moduleWidth(4),moduleHeight(5),width(w),height(h),ledBuffer(buffer),backbuffer(bbuff),mirMode(noMirror)
{
	//moduleBuffer = new metaModule(moduleWidth*moduleHeight);
	for(int x=0;x<width;x++){
		for(int y=0;y<height;y++){
			int i = XYModule(x,y);
			boolean flipped = false;
			if(y&0x01){
				flipped = true;
			}
			moduleBuffer[i] = metaModule(moduleWidth, moduleHeight,flipped);
			moduleBuffer[i].setBaseAddress(buffer+(i*moduleWidth*moduleHeight));
			moduleBuffer[i].setBaseOffset(i*moduleWidth*moduleHeight);
		}
	}
};

uint16_t metaDisplay::XYModule( uint8_t x, uint8_t y)
{
	uint16_t i;
	if (y & 0x01){
		//odd rows run Backwards
		uint8_t reverseX = (width - 1) - x;
		i = (y * width)+reverseX;
	}else{
		i = (y * width)+x;
	}
	return i;
};

uint16_t metaDisplay::XY(MPPixel currentPixel)
{
	uint8_t modX=(currentPixel.x / moduleWidth),modY=(currentPixel.y/moduleHeight);
	uint8_t xMod = (currentPixel.x % moduleWidth), yMod=(currentPixel.y%moduleHeight);
	uint16_t moduleOffset = XYModule(modX,modY);
	uint16_t pixelInModule = moduleBuffer[moduleOffset].XY(xMod,yMod);
	uint16_t pixelInBuffer = pixelInModule+ moduleBuffer[moduleOffset].getBaseOffset();
	return pixelInBuffer;
};

uint16_t metaDisplay::displayWidth()
{
	uint16_t wid = 0;
	wid= moduleWidth*width;
	switch(mirMode){
		case noMirror:
		case horizontalSwapMirror:
		case horizontalMirror:
			wid= moduleWidth*width; break;
		case verticalMirror:
		case quadMirror:
		case quadSwapMirror:
			wid = (moduleWidth*width)/2;
	}
	return wid;
}

uint16_t metaDisplay::displayHeight()
{
	uint16_t h=0;
	h = moduleHeight*height;
	switch(mirMode){
		case noMirror:
		case verticalMirror:
			h = moduleHeight*height; break;
		case horizontalSwapMirror:
		case horizontalMirror:
		case quadMirror:
		case quadSwapMirror:
			h = (moduleHeight * height)/2;
		break;
	}

	return h;
}

void metaDisplay::setPixel(uint16_t x, uint16_t y, CRGB color)
{
	return setPixel(MPPixel(x,y),color);
}

void metaDisplay::_setPixel(MPPixel coord,CRGB color)
{
	if( (coord.x<0) || coord.x>((moduleWidth*width)-1) || (coord.y<0) || (coord.y>((moduleHeight*height)-1)))
	{
		return;
	}
	uint16_t offset = XY(coord);
	if(backbuffer){
		backbuffer[offset]=color;
	}else{
		ledBuffer[offset]=color;
	}
}


void metaDisplay::setPixel(MPPixel coord, CRGB color)
{
	MPPixel tempCoord = coord;
	int16_t displayWidth = (moduleWidth * width)-1;
	int16_t displayHeight = (moduleHeight * height)-1;
	switch(mirMode){
		case horizontalSwapMirror:
			coord.y = this->displayHeight()-coord.y;
		case horizontalMirror:
			tempCoord = coord;
			tempCoord.y = displayHeight - coord.y;
			_setPixel(tempCoord,color);
			break;
		case verticalMirror:
			tempCoord = coord;
			tempCoord.x = displayWidth - coord.x;
			_setPixel(tempCoord,color);
			break;
		case quadSwapMirror:
			coord.y = this->displayHeight()-coord.y;
		case quadMirror:
			tempCoord = coord;
			tempCoord.x = displayWidth-coord.x;
			_setPixel(tempCoord,color);
			tempCoord.y = displayHeight-coord.y;
			_setPixel(tempCoord,color);
			tempCoord.x = coord.x;
			_setPixel(tempCoord,color);
			break;
		case noMirror:
			break;
	}
	_setPixel(coord,color);
}

CRGB metaDisplay::getPixel(MPPixel coord)
{
	uint16_t offset = XY(coord);
	if(backbuffer){
		return backbuffer[offset];
	}else{
		return ledBuffer[offset];
	}
}


CRGB metaDisplay::getPixel(uint16_t x, uint16_t y){
	MPPixel k(x,y);
	return getPixel(k);
}

void metaDisplay::scrollDown(uint8_t lines, bool clear)
{
	for(uint16_t y = lines; y<displayHeight();y++){
		for(uint16_t x = 0; x<displayWidth();++x){
			setPixel(x,y-lines,getPixel(x,y));
		}
	}
	if(clear){
		for(uint16_t y=displayHeight()-lines;y<displayHeight();y++){
			for(uint16_t x=0; x<displayWidth();x++){
				setPixel(x,y,CRGB::Black);
			}
		}
	}
}
void metaDisplay::clear()
{
	CRGB* p = (backbuffer)?backbuffer:ledBuffer;
	for(uint16_t i=0;i<(moduleWidth*width*moduleHeight*height);i++){
		p[i]=CRGB::Black;
	}
}

void metaDisplay::clearAll()
{
	clear();
	for(uint16_t i=0;i<(moduleWidth*width*moduleHeight*height);i++){
		ledBuffer[i]=CRGB::Black;
	}
}

void metaDisplay::fill(CRGB color)
{
	CRGB* p = (backbuffer)?backbuffer:ledBuffer;
	for(uint16_t i=0;i<(moduleWidth*width*moduleHeight*height);i++){
		p[i]=color;
	}
}

void metaDisplay::flush()
{
	if(!backbuffer){
		FastLED.show();
	}
}

void metaDisplay::setBrightness(uint8_t b)
{
	FastLED.setBrightness(b);
}

uint8_t metaDisplay::brightness()
{
	return FastLED.getBrightness();
}
	// Bresenham line algorythm
void metaDisplay::line(MPPixel st, MPPixel en, CRGB color) {
	int dx = abs(en.x-st.x), sx = st.x < en.x ? 1 : -1;
	int dy = -abs(en.y-st.y), sy = st.y < en.y ? 1 : -1;
	int err = dx + dy, e2;
	for(;;) {
		setPixel(st,color);

		if (st.x == en.x && st.y == en.y) break;
		e2 = 2 * err;
		if (e2 > dy) {
			err += dy;
			st.x += sx;
		}
		if (e2 < dx) {
			err += dx;
			st.y += sy;
		}
	}
}

CHSV rgb2hsv(const CRGB& rgb)
{
	CHSV result = CHSV();
	double H,S,V;
	double min,max,delta;
	min = rgb.r<rgb.g?rgb.r:rgb.g;
	min = min < rgb.b?min:rgb.b;

	max = rgb.r>rgb.g?rgb.r:rgb.g;
	max = max  >rgb.b?max  :rgb.b;
	V = max;
	delta = max-min;
	if(max>0){
		S = (delta/max)*255;
	}else{
		S=0;
		H=0;
		V=0;
	}
	if(rgb.r>=max){
		H = (rgb.g - rgb.b)/delta;
	}else if(rgb.g>=max){
		H = 2.0+(rgb.b-rgb.r)/delta;
		}else
			H = 4.0+(rgb.r - rgb.g)/delta;

		H *=43.0;
//	Serial <<"H:"<<H<<" S:"<<S<<" V:"<<V<<endl;
		result.h = (uint8_t)H;
		result.s = (uint8_t)S;
		result.v = (uint8_t)V;
//		Serial <<"rH:"<<result.h<<" rS:"<<result.s<<" rV:"<<result.v<<endl;
		return result;
	}

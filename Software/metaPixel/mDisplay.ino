
void mDisplay::setResolution(displayResolution res)
	{
		reso = res;
		this->clear();
	}
mDisplay::mDisplay(CRGB* buffer, CRGB* bbuff, uint8_t w,uint8_t h):moduleWidth(4),moduleHeight(5),width(w),height(h),ledBuffer(buffer),backbuffer(bbuff),reso(hiRes)
{
	moduleBuffer = new mPixel(MODULES_WIDTH*MODULES_HEIGHT);
	for(int x=0;x<width;x++){
		for(int y=0;y<height;y++){
			int i = XYModule(x,y);
			boolean flipped = false;
			if(y&0x01){
				flipped = true;
			}
			moduleBuffer[i] = mPixel(moduleWidth, moduleHeight,flipped);
			moduleBuffer[i].setBaseAddress(buffer+(i*moduleWidth*moduleHeight));
			moduleBuffer[i].setBaseOffset(i*moduleWidth*moduleHeight);
		}
	}
};

uint16_t mDisplay::XYModule( uint8_t x, uint8_t y)
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

uint16_t mDisplay::XY(MPPixel currentPixel)
{
	uint8_t modX=(currentPixel.x / moduleWidth),modY=(currentPixel.y/moduleHeight);
	uint8_t xMod = (currentPixel.x % moduleWidth), yMod=(currentPixel.y%moduleHeight);
	uint16_t moduleOffset = XYModule(modX,modY);
	uint16_t pixelInModule = moduleBuffer[moduleOffset].XY(xMod,yMod);
	uint16_t pixelInBuffer = pixelInModule+ moduleBuffer[moduleOffset].getBaseOffset();
	return pixelInBuffer;
};

uint16_t mDisplay::displayWidth()
{
	uint16_t width = 0;
	switch(this->resolution()){
		case lowRes:
			width = MODULES_WIDTH;
			break;
		case midRes:
			width = DISPLAY_WIDTH/2;
			break;
		case hiRes:
			width= DISPLAY_WIDTH;
			break;
	}
	return width;
}

uint16_t mDisplay::displayHeight()
{
	uint16_t height=0;
	switch(this->resolution()){
		case lowRes:
			height = MODULES_HEIGHT;
			break;
		case midRes:
			height = (DISPLAY_HEIGHT / 2.0) - MODULES_HEIGHT/2.0;
			break;
		case hiRes:
			height = DISPLAY_HEIGHT;
			break;
	}
	return height;
}

void mDisplay::setPixel(uint16_t x, uint16_t y, CRGB color)
{
	return setPixel(MPPixel(x,y),color);
}

void mDisplay::_setPixel(MPPixel coord,CRGB color)
{
	if( (coord.x<0) || coord.x>(DISPLAY_WIDTH-1) || (coord.y<0) || (coord.y>(DISPLAY_HEIGHT-1)))
	{
		return;
	}
	uint16_t offset = XY(coord);
	if(backbuffer){
		switch(this->resolution()){
			case lowRes:
			{
				MPPixel ncoord = coord;
				for(int x=0;x<MODULE_WIDTH;x++){
					for(int y=0;y<MODULE_HEIGHT;y++){
						ncoord.x = coord.x + x;
						ncoord.y = coord.y + y;
						offset = XY(ncoord);
						backbuffer[offset]=color;
					}
				}
			}
			break;
			case midRes:
			{
				MPPixel ncoord = coord;
				for(int x=0;x<2;x++){
					for(int y=0;y<2;y++){
						ncoord.x = coord.x + x;
						ncoord.y = coord.y + y;
						offset = XY(ncoord);
						backbuffer[offset]=color;
					}
				}
			}
				break;
			case hiRes:

					backbuffer[offset]=color;
			break;
		}
	}else{
		switch(this->resolution()){
			case lowRes:

			case midRes:

			case hiRes:
			uint16_t offset = XY(coord);
			ledBuffer[offset]=color;
			break;
		}
	}
}


void mDisplay::setPixel(MPPixel coord, CRGB color)
{
	switch(resolution()){
		case lowRes:
		coord.x*=MODULE_WIDTH;
		coord.y*=MODULE_HEIGHT;
		break;
		case midRes:
		coord.x*=2.0;
		coord.y=( (coord.y*2.0)+(coord.y%2)+(coord.y/2) );
		break;
		case hiRes:
		break;
	}
	_setPixel(coord,color);
}

void mDisplay::clear()
{
	CRGB* p = (backbuffer)?backbuffer:ledBuffer;
	for(uint16_t i=0;i<DISPLAY_WIDTH*DISPLAY_HEIGHT;i++){
		p[i]=CRGB::Black;
	}
}

void mDisplay::clearAll()
{
	clear();
	for(uint16_t i=0;i<DISPLAY_WIDTH*DISPLAY_HEIGHT;i++){
		ledBuffer[i]=CRGB::Black;
	}
}

void mDisplay::fill(CRGB color)
{
	CRGB* p = (backbuffer)?backbuffer:ledBuffer;
	for(uint16_t i=0;i<DISPLAY_WIDTH*DISPLAY_HEIGHT;i++){
		p[i]=color;
	}
}

void mDisplay::flush()
{
	if(!backbuffer){
		FastLED.show();
	}
}

void mDisplay::setBrightness(uint8_t b)
{
	FastLED.setBrightness(b);
}

uint8_t mDisplay::brightness()
{
	return FastLED.getBrightness();
}
	// Bresenham line algorythm
void mDisplay::line(MPPixel st, MPPixel en, CRGB color) {
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
	// hsv rgb2hsv(rgb in)
	// {
	//     hsv         out;
	//     double      min, max, delta;
	//
	//     min = in.r < in.g ? in.r : in.g;
	//     min = min  < in.b ? min  : in.b;
	//
	//     max = in.r > in.g ? in.r : in.g;
	//     max = max  > in.b ? max  : in.b;
	//
	//     out.v = max;                                // v
	//     delta = max - min;
	//     if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
	//         out.s = (delta / max);                  // s
	//     } else {
	//         // if max is 0, then r = g = b = 0
	//             // s = 0, v is undefined
	//         out.s = 0.0;
	//         out.h = NAN;                            // its now undefined
	//         return out;
	//     }
	//     if( in.r >= max )                           // > is bogus, just keeps compilor happy
	//         out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
	//     else
	//     if( in.g >= max )
	//         out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
	//     else
	//         out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan
	//
	//     out.h *= 60.0;                              // degrees
	//
	//     if( out.h < 0.0 )
	//         out.h += 360.0;
	//
	//     return out;
	// }

	}

	/**********************************************************
	**
	** backbuffer
	**
	**********************************************************/
	#if USE_DOUBLE_BUFFER
	int backbufferBlender(unsigned long now, void* userdata)
	{
		uint8_t frac = BlendParam.currentValue()/Delay.currentValue();
		static uint8_t lastFrac =0;
		if(frac != lastFrac){
			Serial << "frac"<<frac<<endl;
			lastFrac = frac;
		}

	#if DEBUG_EFFECTS
		Serial <<".";
	#endif
		if(frac < 4){
	#if DEBUG_EFFECTS
			Serial << "Frac cliped to 4, was "<<frac<<endl;

	#endif
			frac = 4;
		}
		for(uint16_t i=0;i<NUM_LEDS;i++){
			leds[i]=nblend(leds[i],led_backbuffer[i],frac);
		}
		if( millis() < 5000 ) {
			FastLED.setBrightness( scale8( Brightness.currentValue(), (millis() * 256) / 5000));
		} else {
			FastLED.setBrightness(Brightness.currentValue());
		}

		FastLED.show();
		return 0;
	}
	#endif

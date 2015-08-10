#if USE_PLASMA
/************************
PLASMA
************************/
#define PSCALE (noiseScaleN.currentValue())

int plasma2(unsigned long now, void* userdata)

{
	static MPPixel center = MPPixel();
	center.x = display.displayWidth()/2.0;
	center.y = display.displayHeight()/2.0;
	static float k = 0;
	static int paletteShift = 0;


	int xOffset = int(0 + (display.displayWidth()/1.0 *cos(k* 5.0 ))) ;
	int yOffset = int(0 + (display.displayHeight()/1.0 *-sin(k* 5.0 )));
//	Serial <<"k:"<<k<<" Xo: "<<xOffset<<endl;
	center.x+=xOffset;
	center.y+=yOffset;
//	Serial<<"cx:"<<center.x<<endl;
	for(int x = 0; x < display.displayWidth(); x++){
		for(int y = 0; y < display.displayHeight(); y++)
		{
			MPPixel current = MPPixel(x+xOffset,y+yOffset);
			int div = 0;
			int colorIndex=0;
						colorIndex += int(128.0 + (128.0 * sin( (float)(current.x) / (PSCALE*1) )));div++;				// vertical stripes

//			colorIndex += int(128.0 + (128.0 * sin( (current.y) / (PSCALE*1)  )));div++;				// vertical stripes

									colorIndex += int(128.0 + (128.0 * sin((current.x + current.y) / (PSCALE*1) )));div++;
						colorIndex += int(128.0 + (128.0 * sin(center.distanceTo(current) / (PSCALE*1) )));div++;			// circle

			colorIndex /=div;

			CRGB color = ColorFromPalette(colorPalettes[Palette.currentValue()],(colorIndex+paletteShift)%256);
			display.setPixel(x,y,color);
		}
	}
//	Serial<<"Center: ("<<center.x<<", "<<center.y<<")"<<endl;
	display.setPixel(center,CRGB::White);
	 paletteShift+=noiseHueSpeedN.currentValue();
	  paletteShift%=256;
	k+=M_PI/360.0;
	return 0;

}

#endif

#if USE_PLASMA
/************************
PLASMA
************************/
#define PSCALE (noiseScaleN.currentValue())
#define KSCALE (10.0)
int plasma2(unsigned long now, void* userdata)

{
	static MPPixel center = MPPixel();
	center.x = display.displayWidth()/2.0;
	center.y = display.displayHeight()/2.0;
	static float k = 0;
	static int paletteShift = 0;

	if(effectStarted){
		noiseScaleN.initTo(3);
		noiseSpeedN.initTo(1);
		noiseHueSpeedN.initTo(0);
		plasmaCircleRadiusN.initTo(display.displayWidth()/2);
		effectStarted=false;
	}

	int xOffset = int(0 + (plasmaCircleRadiusN.currentValue() * cos(k*KSCALE ))) ;
	int yOffset = int(0 + (plasmaCircleRadiusN.currentValue() *-sin(k*KSCALE )));
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
			if(plasmaEffectMaskN.currentValue() & VerticalEffect){
				colorIndex += int(128.0 + (128.0 * sin( (float)(current.x+xOffset) / (PSCALE*1) )));div++;
			}	

			// Horzontal Stripes
			if(plasmaEffectMaskN.currentValue() & HorizontalEffect){
				colorIndex += int(128.0 + (128.0 * sin( (current.y+yOffset) / (PSCALE*1)  )));div++;
			}

			// Diagonal Stripes
			if(plasmaEffectMaskN.currentValue() & DiagonalEffect){
				colorIndex += int(128.0 + (128.0 * sin((current.x+yOffset + current.y+xOffset) / (PSCALE*1) )));div++;
			}

			// Circle
			if(plasmaEffectMaskN.currentValue() & CircleEffect){
				colorIndex += int(128.0 + (128.0 * sin(center.distanceTo(current) / (PSCALE*1) )));div++;
			}

			colorIndex /=div;
			CRGB color = ColorFromPalette(colorPalettes[Palette.currentValue()],(colorIndex+paletteShift)%256);
			display.setPixel(x,y,color);
		}
	}
	//	Serial<<"Center: ("<<center.x<<", "<<center.y<<")"<<endl;
		display.setPixel(center,CRGB::White);
	paletteShift+=noiseHueSpeedN.currentValue();
	paletteShift%=256;
	k+=(M_PI/360.0)*noiseSpeedN.currentValue()/10.0;
	if(k<0){
		k=360;
	}
	if(k>360){
		k=0;
	}
	return 0;

}

#endif

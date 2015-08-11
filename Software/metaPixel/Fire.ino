#if USE_FIRE
/************************
Fire
************************/
#define COOLING  500
#define SPARKING 400

static byte heatCells[NUM_LEDS];

int16_t heatAt(int16_t x,int16_t y)
{
	if( (x<0) || (x > DISPLAY_WIDTH-1)){
		return -20;
	}
	if(y<0)
		return noiseHueSpeedN.currentValue();
	if( (y<0) || (y > DISPLAY_HEIGHT-1)){
		return -plasmaCircleRadiusN.currentValue();
	}
	return heatCells[display.XY(x,y)];
}


int16_t neightbours(int16_t x, int16_t y)
{
	int16_t result = 0;
	int16_t div=0;
	result += heatAt(x,y-1)*3.0/2.0;div++;
	result += heatAt(x-1,y-1)/3.0;div++;
	result += heatAt(x+1,y-1)/3.0;div++;
	//	result += heatAt(x-1,y+1)/6;div++;
	//	result += heatAt(x+1,y+1)/6;div++;
	//	result += heatAt(x,y)/10;div++;
	if(random8(100)<10){
		//		result += heatAt(x+1,y)/2;div++;
	}
	if(random8(100)>90){
		//		result += heatAt(x-1,y)/2;div++;
	}
	if (result <0){
		result =0;
	}
	return result / div;
}
int fire(unsigned long now, void* userdata)
{
	static int8_t sparkleOffset = 4;
	static uint8_t sparkleSpan = 12;
	//	static unsigned long lastNow = 0;
	//	static bool dir = true;
	if(effectStarted){
		display.fill(CRGB::Black);
		for(int i=0;i<NUM_LEDS;i++){
			heatCells[i]=0;
		}
		noiseSpeedN.initTo(250);
		Delay = 60;
		BlendParam =7000;
		noiseHueSpeedN = 220;
		plasmaCircleRadiusN.initTo(255);
		noiseScaleN.initTo(0);
		//    fill_solid(led_backbuffer,NUM_LEDS,CRGB::Black);
		effectStarted = false;
		Palette.initTo(7);
		//		lastNow = now;
	}

	// Step 1.  Cool down every cell a little
	// this works without mapping from XY coordinates to cells
#if 1
	for(uint8_t y=0;y<DISPLAY_HEIGHT;y++){
		for(uint8_t x=0;x<DISPLAY_WIDTH;x++){
			uint16_t offset = display.XY(x,y);
			uint8_t coolBase = ((DISPLAY_HEIGHT+y)*noiseSpeedN.currentValue())/10;
			uint8_t cooling = random8(0, (noiseSpeedN.currentValue()));// / display.displayHeight()) + 2);
#if DEBUG_EFFECTS
			Serial <<"Cooling ("<<x<<", "<<y<<") "<<cooling<<endl;
#endif
			heatCells[offset] =qsub8(heatCells[offset],cooling);
		}
	}
#else

	for( int i = 0; i < NUM_LEDS; i++) {
	uint8_t k = random8( (255-heatCells[i])/3.0,(noiseSpeedN.currentValue()*2)/(DISPLAY_HEIGHT*100.0) ); 
	heatCells[i] = qsub8( heatCells[i],  k   );
	}
#endif
	// Step 2.  Heat from each cell drifts 'up' and diffuses a little
	// for( int k= NUM_LEDS - 1; k >= 2; k--) {
	//   		heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
	// }
	for(int y=0;y<DISPLAY_HEIGHT;y++){
		for(int x=0;x<DISPLAY_WIDTH;x++){
			uint16_t offset = display.XY(x,y);

			int16_t h = 0;
			//				byte div = 0;

			h = neightbours(x,y);
			heatCells[offset]+=h;
		}
	}

	// Step 3.  Randomly ignite new 'sparks' of heat near the bottom
	if( random8() < noiseScaleN.currentValue() ) {
		int x = random8(sparkleSpan)+sparkleOffset;
		uint16_t offset = display.XY(x,0);
		heatCells[offset] = qadd8( heatCells[offset], random8(160,255) );
	}

	// if((now - lastNow)>500){
	// 	lastNow = now;
	// 	if(dir)
	// 		sparkleOffset ++;
	// 	else
	// 		sparkleOffset --;
	// 
	// 	if (sparkleOffset >(DISPLAY_WIDTH-sparkleSpan)){
	// 		dir = false;
	// 		sparkleOffset = DISPLAY_WIDTH-sparkleSpan;
	// 	}
	// 	if(sparkleOffset<0 ){
	// 		dir=true;
	// 		sparkleOffset = 0;
	// 	}
	// }

	// Step 4.  Map from heat cells to LED colors
	for( int j = 0; j < NUM_LEDS; j++) {
		int k=heatCells[j];
		if (k>180){
			k=180;
		}
		// if(k<30){
		// 	k=0;
		// }
#if USE_DOUBLE_BUFFER
		led_backbuffer[j] = ColorFromPalette(colorPalettes[Palette.currentValue()],k);
#else
		leds[j] = ColorFromPalette(colorPalettes[Palette.currentValue()],k);
#endif
	}
#if !USE_DOUBLE_BUFFER
	FastLED.show();
#endif
	return 0;
}
#endif

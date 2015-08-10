#if USE_PLASMA
/************************
PLASMA
************************/
void DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8,PlasmaData_t* bp)
{
	byte lineStartHue = startHue8;
	PlasmaData_t * data = (PlasmaData_t*)bp;
	uint8_t h= display.displayHeight()/(data->shouldMirrow?2:1);
	uint8_t w= display.displayWidth()/(data->shouldMirrow?2:1);

	for( byte y = 0; y < h; y++) {
		lineStartHue += yHueDelta8;
		byte pixelHue = lineStartHue;      
		for( byte x = 0; x < w; x++) {
			pixelHue += xHueDelta8;
//      uint16_t pixelInBuffer = display.XY(x,y);
			CRGB color = ColorFromPalette(colorPalettes[Palette.currentValue()],pixelHue);
			display.setPixel(MPPixel(x,y),color);


			if(data->shouldMirrow){
				display.setPixel(x,display.displayHeight()-y-1,color);
				display.setPixel(x,display.displayHeight()-y-1,color);
				display.setPixel(display.displayWidth()-x-1,y,color);
				display.setPixel(display.displayWidth()-x-1,display.displayHeight()-y-1,color);
			}
		}
	}
	FastLED.show();
}

int plasmaSimple(unsigned long now, void* userdata)
{
	static uint32_t frame = 0;

		#if DEBUG_EFFECTS
	    static long lastCall = 0;
			 Serial << now-lastCall<<endl;
			 lastCall = now;
		#endif

	PlasmaData_t *data = (PlasmaData_t*) userdata;
	uint8_t w = display.displayWidth() * data->scale;
	uint8_t h = display.displayHeight() * data->scale;

	if(effectStarted){
		display.fill(CRGB::Black);
		effectStarted = false;
	}

	int32_t yHueDelta32 = ((int32_t)cos16( frame * (27/1) ) * (data->wBase / w));
	int32_t xHueDelta32 = ((int32_t)sin16( frame * (39/1) ) * (data->hBase / h));
	DrawOneFrame( frame / 0xffff, yHueDelta32 / 32768, xHueDelta32 / 32768,data);
		#if !USE_DOUBLE_BUFFER
	FastLED.show();
		#endif
	frame +=data->frameSteps;
#if DEBUG_EFFECTS
	Serial << "Took: "<<(millis()-lastCall)<<" millisec "<<frame<<endl;  
#endif
	return 0;
}
#endif

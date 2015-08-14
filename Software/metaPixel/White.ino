#if USE_WHITE

/*****************************************
White
****************************************/
int white(unsigned long now,void* userdata)
{
//	static int offset = 2;
	if(effectStarted){
		display.fill(CRGB::Orange);
		display.flush();
		effectStarted = false;
		Brightness = 128;
		return 1;
	}
	if(DMX.newFrame()){
		CRGB color = CRGB::Orange;
		const volatile uint8_t* buffer = DMX.getBuffer();
		DMXChannels_t *DMXBlock = (DMXChannels_t*)(buffer + DMXBase);
		color.r = DMXBlock->red;
		color.g = DMXBlock->green;
		color.b = DMXBlock->blue;
		display.fill(color);
		Brightness =DMXBlock->master;
		Serial << "R("<<DMXBlock->red<<") G("<<DMXBlock->green<<") B("<<DMXBlock->blue<<")"<<endl;
		Serial << "Macro("<<DMXBlock->macro<<") Strobe("<<DMXBlock->strobe<<") Mode("<<DMXBlock->mode<<") Master("<<DMXBlock->master<<")"<<endl;
	}

	display.flush();
	return 1;
}



#endif

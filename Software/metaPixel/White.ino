#if USE_WHITE
/*****************************************
White
****************************************/
int white(unsigned long now,void* userdata)
{
//	static int offset = 2;
	if(effectStarted){
		display.fill(CRGB::White);
		display.flush();
		effectStarted = false;
		nextBrightness = 128;
		return 1;
	}
	if(DMX.newFrame()){
		CRGB color = CRGB::White;
		const volatile uint8_t* buffer = DMX.getBuffer();
		DMXChannels_t *DMXBlock = (DMXChannels_t*)(buffer + DMXBase);
		color.r = DMXBlock->red;
		color.g = DMXBlock->green;
		color.b = DMXBlock->blue;
		display.fill(color);
		nextBrightness =DMXBlock->master;
		Serial << "R("<<DMXBlock->red<<") G("<<DMXBlock->green<<") B("<<DMXBlock->blue<<")"<<endl;
		Serial << "Macro("<<DMXBlock->macro<<") Strobe("<<DMXBlock->strobe<<") Mode("<<DMXBlock->mode<<") Master("<<DMXBlock->master<<")"<<endl;
		// for(int i=0;i<514;++i){
		// 	Serial << i<<":"<<_HEX(buffer[i])<<"\t";
		// 	if(!(i%10) && (i>0) ){
		// 		Serial << endl;
		// 	}
		// }
		// Serial <<endl<< "-----------"<<endl<<endl;
	}
	
	// nextBrightness = currentBrightness +offset;
	// if(nextBrightness>0xff){
	// 	nextBrightness = 255;
	// 	offset = -offset;
	// }
	// if(nextBrightness<0){
	// 	nextBrightness = 0;
	// 	offset = -offset;
	// }
	display.flush();
	return 1;
}
#endif
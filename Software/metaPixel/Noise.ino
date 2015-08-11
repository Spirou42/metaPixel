#if USE_NOISE

#define NOISE_DIMENSION ((DISPLAY_WIDTH>DISPLAY_HEIGHT) ? DISPLAY_WIDTH : DISPLAY_HEIGHT)

static int16_t noiseX;
static int16_t noiseY;
static int16_t noiseZ;

Parameter<int16_t> noiseSpeedN(8);
Parameter<int16_t> noiseScaleN(50);
Parameter<int16_t> noiseHueSpeedN(1);


uint8_t noiseD[NOISE_DIMENSION][NOISE_DIMENSION];
uint8_t noiseP[NOISE_DIMENSION][NOISE_DIMENSION];

void fillnoise8() {
	for(int i = 0; i < NOISE_DIMENSION; i++) {
		int ioffset = noiseScaleN.currentValue() * i;
		for(int j = 0; j < NOISE_DIMENSION; j++) {
			int joffset = noiseScaleN.currentValue() * j;
			noiseD[i][j] = inoise8(noiseX + ioffset,noiseY + joffset,noiseZ);
			noiseP[i][j] = inoise8(noiseY+joffset,noiseX+ioffset,noiseZ);
			
		}
	}
	noiseZ += noiseSpeedN.currentValue();
}

int noise(unsigned long now, void* userdata)
{
	static uint8_t ihue=0;
	if(effectStarted){
		effectStarted = false;
		noiseX = random16();
		noiseY = random16();
		noiseZ = random16();
	}
	fillnoise8();

	for(int i = 0; i < display.displayWidth(); i++) {
		for(int j = 0; j < display.displayHeight(); j++) {
			// We use the value at the (i,j) coordinate in the noise
			// array for our brightness, and the flipped value from (j,i)
			// for our pixel's hue.
			CRGB color = ColorFromPalette(colorPalettes[Palette.currentValue()],noiseD[i][j]+ihue);
			CHSV hcolor = rgb2hsv(color);
			hcolor.v = noiseP[i][j];
			
									hcolor = CHSV(ihue + (noiseD[i][j]>>1),255/*(noiseP[i][j]<<3)*/,255/*noiseP[i][j]*/);
						//hcolor = CHSV(noiseD[j][i],255,noiseD[i][j]);

						//color = ColorFromPalette(colorPalettes[currentPalette],noiseD[j][i]);
			display.setPixel(i,j,color);


			// You can also explore other ways to constrain the hue used, like below
			// leds[XY(i,j)] = CHSV(ihue + (noise[j][i]>>2),255,noise[i][j]);
		}
	}
	ihue+=noiseHueSpeedN.currentValue();
	display.flush();
	return 0;
}
#endif

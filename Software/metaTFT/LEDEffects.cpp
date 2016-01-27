/** simple collection of LED effects
*/

#include "Arduino.h"
#include "Streaming.h"
#include "metaTFT.h"
#include "LEDEffects.h"

//SimplePatternList patterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
//const char* patternNames[]={"Rainbow","Rainbow glitter", "Confetti", "Sinelon", "Juggle", "BPM"};
//uint8_t currentPatternNumber = 0; // Index number of which pattern is current
//size_t numberOfPatterns = ARRAY_SIZE(patterns);
uint8_t gHue = 0;
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
	}else{
		H = 4.0+(rgb.r - rgb.g)/delta;
  }
	H *=43.0;
  //	Serial <<"H:"<<H<<" S:"<<S<<" V:"<<V<<endl;
	result.h = (uint8_t)H;
	result.s = (uint8_t)S;
	result.v = (uint8_t)V;
  //		Serial <<"rH:"<<result.h<<" rS:"<<result.s<<" rV:"<<result.v<<endl;
	return result;
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  CRGBPalette16 p = (*currentSystemPalette)->second;
  uint8_t step = 1;
  if(NUM_LEDS < 255){
    step = 255/NUM_LEDS;
  }

  fill_palette(leds,NUM_LEDS,gHue,step,p,255,LINEARBLEND);
  //fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  CRGB color = ColorFromPalette((*currentSystemPalette)->second,gHue + random8(64),255);
  leds[pos] += color;
}

#define ADD_COLOR 0
void blendColor(const CRGBSet &set, CRGB color){
    //CHSV hcolor = rgb2hsv(color);

    CRGBSet::iterator pixel =set.begin();
    CRGBSet::iterator end = set.end();
    for(;pixel != end;++pixel){
      //CRGB r;
      CRGB p = *pixel;
      //r.r = ((int)p.r+(int)color.r)/2;
      //r.g = ((int)p.g+(int)color.g)/2;
      //r.b = ((int)p.b+(int)color.b)/2;
      *pixel += color;

//       //CHSV tcolor = rgb2hsv(*pixel);
//       if(tcolor.v <2){
// //        Serial << "Set"<<endl;
//         tcolor = hcolor;
//       }else if(abs(tcolor.h - hcolor.h)>10){
// //        Serial << "Mean"<<endl;
//         tcolor.h+=((int16_t)hcolor.h-(int16_t)tcolor.h)/2;
//         tcolor.s= ((int16_t)hcolor.s+(int16_t)tcolor.s)/2;
//         tcolor.v =hcolor.v;
//       }else{
//         tcolor = hcolor;
//       }
//       *pixel = CRGB(tcolor);
    }
    //Serial << endl;
}
void minelon(){
// a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 255);
  int maxPhase =12;
  float hueStep = 256.0/(maxPhase+1);
  uint16_t phaseOffset = (65536/maxPhase);

  for(int phase = 1;phase<=maxPhase;phase++){
    //int result = beatsin16(1,0,NUM_LEDS,phase*100,phase*100);

    uint16_t pos = beat88(phase*64,phase*phaseOffset);//
    uint16_t rangewidth = NUM_LEDS - 0;
    uint16_t scaledbeat = scale16( pos+ 32768, rangewidth);
    uint16_t result = 0 + scaledbeat;
    int hue = gHue + hueStep*(phase-1);
    int blobLength = 8;
    CRGB color = ColorFromPalette((*currentSystemPalette)->second,hue,255);
    if((result+blobLength)<NUM_LEDS){
      const CRGBSet p(&leds[result],blobLength);
      blendColor(p,color);
    }else{
      int flen = NUM_LEDS-result;
      int elen = blobLength - flen;
      const CRGBSet p(&leds[result],flen);
      const CRGBSet q(&leds[0],elen);
      blendColor(p,color);
      blendColor(q,color);
    }
    blur1d(leds,NUM_LEDS,120);
  }
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 15);

  int pos = beatsin16((2<<8),0,NUM_LEDS);
  CRGB color = ColorFromPalette((*currentSystemPalette)->second,gHue,255);
  leds[pos] += color;
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette((*currentSystemPalette)->second, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    CRGB color = ColorFromPalette((*currentSystemPalette)->second,dothue + random8(64),255);
    leds[beatsin16(i+7,0,NUM_LEDS)] |= color;
    dothue += 32;
  }
}

void nextPattern(){
  // add one to the current pattern number, and wrap around at the end
  EffectList::iterator l = currentSystemEffect+1;
  if(l==systemEffects.end()){
    l=systemEffects.begin();
  }
  currentSystemEffect = l;
  Serial << "Pattern: "<<(currentSystemEffect - systemEffects.begin())<<" "<<((*currentSystemEffect)->first)<<endl;
}

void nextPalette(){
  PaletteList::iterator l = currentSystemPalette+1;
  if(l==systemPalettes.end()){
    l=systemPalettes.begin();
  }
  currentSystemPalette = l;
  Serial << "Palette: "<<(currentSystemPalette - systemPalettes.begin())<<" "<<((*currentSystemPalette)->first)<<endl;
}

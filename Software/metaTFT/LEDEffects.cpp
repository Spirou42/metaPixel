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

void rainbow()
{
  // FastLED's built-in rainbow generator
  CRGBPalette16 p = (*currentSystemPalette)->second;
  fill_palette(leds,NUM_LEDS,gHue,255/NUM_LEDS,p,255,LINEARBLEND);
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

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  CRGB color = ColorFromPalette((*currentSystemPalette)->second,gHue,192);
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

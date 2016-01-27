/**
*/

#ifndef __METATFT_H__
#define __METATFT_H__
#define FASTLED_INTERNAL
//#define FASTLED_ALLOW_INTERRUPTS 0
#include <stack>
#include <deque>
#include <vector>
#include <tuple>
#include <list>
#include <deque>
#include "UIHelpers.h"

#include "FastLED.h"

/** TFT Configuration **/
#define TFT_RST   2
#define TFT_DC    9
#define TFT_CS    10
#define TFT_MISO  12
#define TFT_MOSI  11
#define TFT_SCK   13
#define TFT_LED   6
//A14 || 6

/** FastLed**/
#define COLOR_ORDER       GRB
#define CHIPSET           WS2812
#define COLOR_CORRECTION  0xffeeff
#define LED_BRIGHTNESS    180

#define LED_PIN           4
#define NUM_LEDS 158

#define DEBUG_RESPONDER 0

extern CRGB leds[];

typedef void(*effectHandler)(void);

typedef std::pair<const String,CRGBPalette16> PalettePair;
typedef std::vector<PalettePair*> PaletteList;

typedef std::pair<const String, effectHandler> EffectPair;
typedef std::vector<EffectPair*> EffectList;


extern PaletteList systemPalettes;
extern PaletteList::iterator currentSystemPalette;

extern EffectList systemEffects;
extern EffectList::iterator currentSystemEffect;

typedef std::stack<metaView*,std::vector<metaView*>> ResponderStack;
extern ResponderStack responderStack;

//extern CHSV rgb2hsv(const CRGB& rgb);
#endif

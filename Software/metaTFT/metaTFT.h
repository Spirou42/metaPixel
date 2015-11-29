/**
*/

#ifndef __METATFT_H__
#define __METATFT_H__
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
#define BRIGHTNESS        160

#define LED_PIN                   4
#define NUM_LEDS 60
#include <FastLED.h>
extern CRGB leds[];
#endif

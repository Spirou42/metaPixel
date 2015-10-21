/**
@file: metaPixel.h
@description: MasterHeaderfile containing all global definitions
*/
#ifndef __METAPIXEL_H__
#define __METAPIXEL_H__
#define FASTLED_INTERNAL
/*#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>*/
#include <Queue.h>
#include <FastLED.h>
#include <Streaming.h>
#include "VT100Stream.h"
#include <TeensyDmx.h>
#include <AudioStream.h>
#include <rdm.h>
#include "ILI9341_t3.h"
#include "SPI.h"

/** Feature Switsches **/
#define USE_EFFECT_SCEDULER 1
#define USE_SERIAL_COMMANDS 1
#define USE_DOUBLE_BUFFER 1
#define USE_ILI9341_DISPLAY 1

/** display configuration **/
#define MODULES_WIDTH   5												// number of modules in X
#define MODULES_HEIGHT  4											  // number of modules in Y
#define MODULE_WIDTH    4												// width of module in pixels
#define MODULE_HEIGHT   5												// height of modules in pixels
#define DISPLAY_WIDTH   (MODULES_WIDTH * MODULE_WIDTH)
#define DISPLAY_HEIGHT  (MODULES_HEIGHT * MODULE_HEIGHT)

/** TFT Configuration **/
#define TFT_RST   2
#define TFT_DC    9
#define TFT_CS    10
#define TFT_MISO  12
#define TFT_MOSI  11
#define TFT_SCK   13
#define TFT_LED   A14

/** Local Libraries **/
#include "Types.h"
#if USE_LEGACY_MENU
#include "Menu.h"
#include "Encoder.h"
#endif

#include "metaModule.h"
#include "metaDisplay.h"

#define NUM_LEDS (DISPLAY_WIDTH * DISPLAY_HEIGHT)
#include "Commands.h"
#include "Effect.h"
#include "Parameter.h"
#include "EffectWhite.h"
#include "EffectFire.h"
#include "EffectNoise.h"
#include "EffectPlasma.h"
#include "EffectPlasmaSimple.h"
#include "EffectLine.h"
#include "EffectWhitney.h"



/** Configuration  & global defaults**/
#define COLOR_ORDER       GRB
#define CHIPSET           WS2812
#define COLOR_CORRECTION  0xffeeff
#define BRIGHTNESS        160


#define DMXBase	          17
#define DMXSpan           7

#define LED_PIN                   4

/** Audio configuration **/
#define AUDIO_INPUT AUDIO_INPUT_MIC

/** DEBUG CONFIGURATION **/
#define DEBUG (1)
#define DEBUG_BLINK   (0 & DEBUG & !USE_TEENSY_AUDIO_LAYOUT)
#define DEBUG_EFFECTS (0 & DEBUG)
#define DEBUG_ENCODER (0 & DEBUG)
#define DEBUG_MENU    (0 & DEBUG)
#define DEBUG_PARSER	(0 & DEBUG)
#define DEBUG_SERIAL  (0 & DEBUG)
#define DEBUG_COMMAND (0 & DEBUG)
#define DEBUG_LOOP		(0 & DEBUG)
#define DEBUG_EFFECT_SCEDULER (1 & DEBUG)

/** settings serial masks **/
#define NAME_CELL_LENGTH      10
#define TFT_NAME_CELL_LENGTH   8
#define PARAMETER_CELL_LENGTH 20
#define LINE_LENGTH           75
/** exporting globals **/
extern CRGB leds[];               ///< primary framebuffer
#if USE_DOUBLE_BUFFER
extern CRGB  led_backbuffer[];    ///< backbuffer
#endif
class metaDisplay;
extern metaDisplay display;          ///< global display class

extern TeensyDmx DMX;             ///< DMX Interface
extern Queue taskQueue;           ///< task queue scheduler
extern CRGBPalette16 colorPalettes[];
/** global effects parameter **/

extern AnimationValue EffectProgram;     ///< current running effect program
extern AnimationValue Delay;             ///< frame delay for the running effect
extern AnimationValue Palette;           ///< actual used palette
extern AnimationValue Brightness;        ///< global brightness of the display
extern AnimationValue BlendParam;        ///< factor for double buffer blending
extern AnimationValue MirrorParam;       ///< global display mirror mode

/** effect specific parameter **/
extern AnimationValue genericSpeed1;       ///< generic global parameter
extern AnimationValue genericSpeed2;       ///< generic global parameter
extern AnimationValue genericScale1;       ///< generic global parameter
extern AnimationValue genericScale2;       ///< generic global Parameter

extern AnimationValue genericParam1;       ///< generic global parameter
extern AnimationValue genericParam2;       ///< generic globa Parameter

extern AnimationValue genericEffectMask1;     ///< generic global mask parameter
extern AnimationValue genericEffectMask2;     ///< generic global Mask 2

extern effectProgramN_t effectProgramsN[];
extern uint8_t newMaxPrograms;

extern Parameter16_t parameterArray[];
extern int16_t parameterArraySize;

extern void dumpParameters();
extern CommandQueue commandQueue;
extern bool parameterHasChanged;

typedef enum{param_P,param_D,param_C,param_B,param_Q, param_Z,param_A,param_U,param_V,param_R,param_I,param_O,param_H,param_M,param_N,param_StartEffect=param_U } paramID;

extern ILI9341_t3 tft;

class tftSerial : public Print
{
public:
	tftSerial(){};
	size_t write(uint8_t c){
		Serial.write(c);
		#if USE_ILI9341_DISPLAY
			tft.write(c);
		#endif
		return 1;
	}
};
extern tftSerial TFTSerial;
#endif

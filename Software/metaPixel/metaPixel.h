/**
@file: metaPixel.h
@description: MasterHeaderfile containing all global definitions
*/
#ifndef __METAPIXEL_H__
#define __METAPIXEL_H__
#define FASTLED_INTERNAL
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Queue.h>
#include <FastLED.h>
#include <Streaming.h>
#include "VT100Stream.h"
#include <TeensyDmx.h>
#include <AudioStream.h>
#include <rdm.h>

/** Feature Switsches **/
#define USE_AUDIO_EFFECTS 0
#define USE_TEENSY_AUDIO 1
#define USE_LEGACY_MENU (!USE_TEENSY_AUDIO)
#define USE_EFFECT_SCEDULER 1
#define USE_OLD_EFFECTS !USE_EFFECT_SCEDULER
#define USE_SERIAL_COMMANDS 1
#define USE_DOUBLE_BUFFER 1

/** display configuration **/
#define MODULES_WIDTH   5												// number of modules in X
#define MODULES_HEIGHT  4											// number of modules in Y
#define MODULE_WIDTH    4												// width of module in pixels
#define MODULE_HEIGHT   5												// height of modules in pixels
#define DISPLAY_WIDTH   (MODULES_WIDTH * MODULE_WIDTH)
#define DISPLAY_HEIGHT  (MODULES_HEIGHT * MODULE_HEIGHT)

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
#include "EffectWaterfall.h"



/** Configuration  & global defaults**/
#define COLOR_ORDER       GRB
#define CHIPSET           WS2812
#define COLOR_CORRECTION  0xffeeff
#define BRIGHTNESS        160


#define DMXBase	          17
#define DMXSpan           7

/** Hardware  **/
#if USE_LEGACY_MENU
#define ENC_PULSE_PIN             6
#define ENC_STEP_PIN              5
#define ENC_DIR_PIN               4
#define LC_DATA_PIN               3
#define LC_CLK_PIN                2
#define LC_CS_PIN                 1
#endif

#if !USE_TEENSY_AUDIO
#define LED_BLINK_PIN            13
#endif
#define LED_PIN                   4

/** Audio configuration **/
#define AUDIO_INPUT AUDIO_INPUT_MIC

/** DEBUG CONFIGURATION **/
#define DEBUG (1)
#define DEBUG_BLINK   (0 & DEBUG & !USE_TEENSY_AUDIO)
#define DEBUG_EFFECTS (0 & DEBUG)
#define DEBUG_ENCODER (0 & DEBUG)
#define DEBUG_MENU    (0 & DEBUG)
#define DEBUG_COMMAND	(1 & DEBUG)
#define DEBUG_LOOP		(0 & DEBUG)
#define DEBUG_EFFECT_SCEDULER (1 & DEBUG)

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

#endif

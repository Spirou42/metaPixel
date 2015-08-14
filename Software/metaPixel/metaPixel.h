/**
@file: metaPixel.h
@description: MasterHeaderfile containing all global definitions
*/
#ifndef __METAPIXEL_H__
#define __METAPIXEL_H__
#define FASTLED_INTERNAL
#include <Queue.h>
#include <FastLED.h>
#include <Streaming.h>
#include <TeensyDmx.h>
#include <rdm.h>


/** Local Libraries **/
#include "Types.h"
#include "Noise.h"
#include "Menu.h"
#include "Encoder.h"
#include "mPixel.h"
#include "mDisplay.h"
#define NUM_LEDS (DISPLAY_WIDTH * DISPLAY_HEIGHT)

#include "Effect.h"
#include "Parameter.h"
#include "EffectWhite.h"
#include "EffectFire.h"

/** Feature Switsches **/
#define USE_EFFECT_SCEDULER 1
#define USE_SERIAL_COMMANDS 1
#define USE_DOUBLE_BUFFER 1

/** Configuration  & global defaults**/
#define COLOR_ORDER       GRB
#define CHIPSET           WS2812
#define COLOR_CORRECTION  0xffeeff
#define BRIGHTNESS        160


#define DMXBase	          17
#define DMXSpan           7

/** Hardware  **/
#define ENC_PULSE_PIN             6
#define ENC_STEP_PIN              5
#define ENC_DIR_PIN               4
#define LC_DATA_PIN               3
#define LC_CLK_PIN                2
#define LC_CS_PIN                 1
#define LED_PIN                   0
#define LED_BLINK_PIN            13

/** DEBUG CONFIGURATION **/
#define DEBUG (1)
#define DEBUG_BLINK   (1 & DEBUG)
#define DEBUG_EFFECTS (0 & DEBUG)
#define DEBUG_ENCODER (0 & DEBUG)
#define DEBUG_MENU    (0 & DEBUG)
#define DEBUG_COMMAND	(1 & DEBUG)
#define DEBUG_LOOP		(1 & DEBUG)
#define DEBUG_EFFECT_SCEDULER (1 & DEBUG)

/** exporting globals **/
extern CRGB leds[];               ///< primary framebuffer
#if USE_DOUBLE_BUFFER
extern CRGB  led_backbuffer[];    ///< backbuffer
#endif
extern mDisplay display;          ///< global display class

extern TeensyDmx DMX;             ///< DMX Interface
extern Queue taskQueue;           ///< task queue scheduler

/** global effects parameter **/

extern Parameter<int16_t>EffectProgram;     ///< current running effect program
extern Parameter<int16_t>Delay;             ///< frame delay for the running effect
extern Parameter<int16_t>Palette;           ///< actual used palette
extern Parameter<int16_t>Brightness;        ///< global brightness of the display
extern Parameter<int16_t>BlendParam;        ///< factor for double buffer blending

/** effect specific parameter **/
extern Parameter<int16_t>genericSpeed1;       ///< generic global parameter
extern Parameter<int16_t>genericSpeed2;       ///< generic global parameter
extern Parameter<int16_t>genericScale1;       ///< generic global parameter
extern Parameter<int16_t>genericScale2;       ///< generic global Parameter

extern Parameter<int16_t>genericParam1;       ///< generic global parameter
extern Parameter<int16_t>genericParam2;       ///< generic globa Parameter

extern Parameter<int16_t>genericEffectMask1;     ///< generic global mask parameter
extern Parameter<int16_t>genericEffectMask2;     ///< generic global Mask 2

extern effectProgramN_t effectProgramsN[];
extern uint8_t newMaxPrograms;

extern newParameter_t parameterArray[];
extern int16_t parameterArraySize;


#endif

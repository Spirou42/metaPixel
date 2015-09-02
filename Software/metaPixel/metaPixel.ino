#define FASTLED_INTERNAL
#define FASTLED_ALLOW_INTERRUPTS 0
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#include <Queue.h>
#include <FastLED.h>
#include <Streaming.h>
#include "VT100Stream.h"
#include <TeensyDmx.h>
#include <rdm.h>
#include <Arduino.h>
#include "metaPixel.h"
#include "Palettes.h"


/* program defined */

#define USE_PLASMA 1
#define USE_FIRE   1
#define USE_LINES  0
#define USE_WHITE  1
#define USE_NOISE  1

#define START_PROG 5

/**********************************************************
**
** Globals
**
**********************************************************/

CRGB  leds[NUM_LEDS];
#if USE_DOUBLE_BUFFER
CRGB  led_backbuffer[NUM_LEDS];
metaDisplay display= metaDisplay(leds,led_backbuffer,MODULES_WIDTH,MODULES_HEIGHT);
#else
metaDisplay display= metaDisplay(leds,NULL,MODULES_WIDTH,MODULES_HEIGHT);
#endif

/** DMX **/

#define DMX_REDE 30
struct RDMINIT rdmData {
	(char*)"TeensyDMX v0.1",
	(char*)"Teensyduino",
	1,  // Device ID
	(char*)"DMX Node",
	1,  // The DMX footprint
	0,  // The DMX startAddress - only used for RDM
	0,  // Additional commands length for RDM
	0   // Definition of additional commands
};
TeensyDmx DMX(Serial3, &rdmData, 30);
Queue taskQueue;
#if USE_LEGACY_MENU
ballState_t currentState;
#endif
/**********************************************************
**
** Parameters
**
**********************************************************/

AnimationValue Delay(10);
AnimationValue EffectProgram(0);
AnimationValue Palette(0);
AnimationValue Brightness(BRIGHTNESS);
AnimationValue BlendParam(5000);
AnimationValue MirrorParam(0);

AnimationValue genericSpeed1(8);
AnimationValue genericSpeed2(1);
AnimationValue genericScale1(50);
AnimationValue genericScale2(50);
AnimationValue genericParam1(display.displayWidth());
AnimationValue genericParam2(display.displayWidth());
AnimationValue genericEffectMask1(HorizontalEffect | VerticalEffect | DiagonalEffect | CircleEffect);
AnimationValue genericEffectMask2(HorizontalEffect | VerticalEffect | DiagonalEffect | CircleEffect);
bool parametersInvalid = false;

CRGBPalette16 colorPalettes[]={
	(CRGBPalette16)RainbowColors_p,
	(CRGBPalette16)CloudColors_p,
	(CRGBPalette16)OceanColors_p,
	(CRGBPalette16)ForestColors_p,
	(CRGBPalette16)PartyColors_p,
	(CRGBPalette16)RainbowStripeColors_p,
	(CRGBPalette16)LavaColors_p,
	(CRGBPalette16)HeatColors_p,
	(CRGBPalette16)arctic_gp,
	(CRGBPalette16)temperature_gp,
	(CRGBPalette16)colombia_gp,
	(CRGBPalette16)cequal_gp,
};
uint8_t numberOfPalettes = sizeof(colorPalettes)/sizeof(CRGBPalette16);

const char * SystemParameterName[]={
	"Program","Delay","Pallete","Brightness","Mirror","Fade"
};

Parameter16_t parameterArray[] = {
	// global scope parameters.
	// these parameters for Program, framerate etc.
	/* 00 */ 	Parameter16_t('P',(int16_t)0,(int16_t)0,&EffectProgram),
	/* 01 */	Parameter16_t('D',(int16_t)1,(int16_t)5000,&Delay),
	/* 02 */	Parameter16_t('C',(int16_t)0,(int16_t)0,&Palette),
	/* 03 */	Parameter16_t('B',(int16_t)0,(int16_t)255,&Brightness),
	/* 04 */  Parameter16_t('Q',(int16_t)0,(int16_t)5,&MirrorParam),
	/* 05 */	Parameter16_t('Z',(int16_t)0,(int16_t)32000,&BlendParam),

	// local parameters. These parameters have a different meening for each  Effect program.
	/* 06 */	Parameter16_t('U',(int16_t)0,(int16_t)0,&genericSpeed1),
	/* 07 */  Parameter16_t('V',(int16_t)0,(int16_t)0,&genericSpeed2),
	/* 08 */	Parameter16_t('R',(int16_t)0,(int16_t)0,&genericScale1),
	/* 09 */	Parameter16_t('I',(int16_t)0,(int16_t)0,&genericScale2),
	/* 10 */	Parameter16_t('O',(int16_t)0,(int16_t)0,&genericParam1),
	/* 11 */ 	Parameter16_t('H',(int16_t)0,(int16_t)0,&genericParam2),
	/* 12 */	Parameter16_t('M',(int16_t)0,(int16_t)255,&genericEffectMask1),
	/* 13 */	Parameter16_t('N',(int16_t)0,(int16_t)255,&genericEffectMask2),

};
typedef enum{param_P,param_D,param_C,param_B,param_Q, param_Z,param_U,param_V,param_R,param_I,param_O,param_H,param_M,param_N } pramId;

int16_t parameterArraySize = sizeof(parameterArray)/sizeof(Parameter16_t);

EffectWhite whiteEffect = EffectWhite(&(parameterArray[param_O]));
EffectFire  fireEffect = EffectFire(&parameterArray[param_O],&parameterArray[param_H],&parameterArray[param_U]);
EffectNoise noiseEffect = EffectNoise(&parameterArray[param_R],&parameterArray[param_U],&parameterArray[param_V],&parameterArray[param_M]);
EffectPlasma plasmaEffect = EffectPlasma(&parameterArray[param_I],&parameterArray[param_U],&parameterArray[param_R],&parameterArray[param_V],&parameterArray[param_M]);
EffectPlasmaSimple simplePlasma = EffectPlasmaSimple(&parameterArray[param_R],&parameterArray[param_I],&parameterArray[param_U],&parameterArray[param_M]);
EffectLine lineEffect = EffectLine();
EffectWhitney whitneyEffect = EffectWhitney(&parameterArray[param_U]);

#if USE_AUDIO_EFFECTS
EffectWaterfall waterfallEffect = EffectWaterfall();
#endif
//EffectWhite dummy = EffectWhite();
//effectProgramN_t h = {dummy,1000,NULL};
effectProgramN_t effectProgramsN[] = {
	{&whiteEffect,100,NULL},
	{&noiseEffect,150,"*rU6C8!R20 80 60"}, //"*rU6C8!R20 80 60"
	{&plasmaEffect,150,"C8"},
	{&simplePlasma,150,NULL},
	{&lineEffect,150,NULL},
	{&fireEffect,60,NULL},
	{&whitneyEffect,60,NULL},
	#if USE_AUDIO_EFFECTS
	{&waterfallEffect,100,NULL},
	#endif
};

uint8_t newMaxPrograms = sizeof(effectProgramsN) / sizeof(effectProgramN_t);




/**********************************************************
**
** DEBUG
**
**********************************************************/

#if DEBUG_BLINK && !USE_TEENSY_AUDIO
int blinker(unsigned long now, void* userData)
{
	static boolean state = HIGH;
	digitalWrite(LED_BLINK_PIN,state);
	state = !state;
	return 0;
}
#endif
/**********************************************************
**
** Arduino
**
**********************************************************/
void dumpParameters()
{

	uint8_t line = 1;
	uint8_t column = 1;
	uint16_t t = EffectProgram.currentValue()%(newMaxPrograms);
	Effect *effect = effectProgramsN[t].program;
	Serial << ScreenPos(line++,column)<<clearLineRight<<effect<<endl;
	Serial << ScreenPos(line++,column)<<clearLineRight<<"Parameter:";
	line = 3;
	Serial<<ScreenPos(line,1)<<clearLineRight;
	size_t params = effect->numberOfParameters();
	for(size_t i=0;i<params;i++){
		Serial << ScreenPos(line,column)<<effect->parameterNameAt(i);
		column +=NAME_CELL_LENGTH;
		Serial << ScreenPos(line,column)<<*effect->parameterAt(i);
		column +=PARAMETER_CELL_LENGTH;
		if(column > LINE_LENGTH){
			line ++;
			column = 1;
			Serial << ScreenPos(line,column)<<clearLineRight;
		}
	}
	//effect->printParameter(Serial);
	line = 6;
	column =1;
	for(int i=0;i<6;i++){
		Serial << ScreenPos(line,column)<<SystemParameterName[i];
		column +=NAME_CELL_LENGTH;
		Serial <<ScreenPos(line,column)<<parameterArray[i]<<endl;
		column +=PARAMETER_CELL_LENGTH;
		if(column>LINE_LENGTH){
			line ++;
			column = 1;
			Serial<<ScreenPos(line,column)<<clearLineRight;
		}
	}
	// collect parameter String
	// String paramString = String(" ");
	// for(size_t i=0;i<6;i++){
	// 	String * k = parameterArray[i].getCommandString();
	// 	paramString += (*k);
	// }
	// for(size_t i=0;i<params;i++){
	// 	String *k = (effect->parameterAt(i))->getCommandString();
	// 	paramString +=(*k);
	// }
	// Serial <<ScreenPos(line++,0)<<clearLineRight<<paramString;
	line ++;
	Serial <<ScreenPos(line,1)<<clearLineRight<<">";
}

elapsedMillis commandQueueTimer = 0;
void setup()
{
	FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(COLOR_CORRECTION);
	FastLED.setBrightness( BRIGHTNESS );
	FastLED.show();
	Serial.begin(115200);
	delay(2000);
	Serial <<"Startup"<<endl;
	// tweak global parameter max for Programs and pallettes
	parameterArray[0].maxValue = newMaxPrograms-1;
	parameterArray[2].maxValue = numberOfPalettes-1;

	#if USE_DEBUG_BLINK
	pinMode(LED_BLINK_PIN, INPUT);
	#endif
	#if USE_LEGACY_MENU
	currentState = stateNone;
	initEncoderUI();
	#endif


	/** Setup Audio **/
	#if USE_AUDIO_EFFECTS
	AudioMemory(20);
	AudioShield.enable();
	AudioShield.inputSelect(AUDIO_INPUT);
	AudioShield.volume(0.5);
	AudioShield.micGain(200);
	analyzeFFT.averageTogether(255);
	audioIn.setActive(false);
	analyzeFFT.setActive(false);
	#endif
	/** Setup DMX **/
	DMX.setMode(TeensyDmx::Mode::DMX_IN);
	Serial << "Init Parameters"<<endl;
	/** initialize Effects **/
	EffectProgram.initTo(START_PROG);
	int16_t cP = EffectProgram.currentValue();
	Delay.initTo(effectProgramsN[cP].delay);
	Brightness.initTo(BRIGHTNESS);
	Palette.initTo(0);
	Serial << "Starting Tasks"<<endl;
	/** Double buffering **/
	#if USE_DOUBLE_BUFFER
	taskQueue.scheduleFunction(backbufferBlender,NULL,"BBB ",0,66);
	#endif

	/** Debug blinker (legacy) **/
	#if DEBUG_BLINK
	taskQueue.scheduleFunction(blinker,NULL,"BLNK",1000,1000);
	#endif

	/** Effect scheduler **/
	#if USE_EFFECT_SCEDULER
	taskQueue.scheduleFunction(effectRunner,NULL,"EFFC",0,Delay.currentValue());
	#endif

	Serial << "Init Commandline Interface"<<endl;
	/** Command line interface **/
	#if USE_SERIAL_COMMANDS
	taskQueue.scheduleFunction(serialReader,NULL,"SERI",200,200);
	#endif

	/** Legacy menu system **/
	#if USE_LEGACY_MENU
	encoderClickCallback = clickHandler;
	lc.clearDisplay(0);
	#endif

	randomSeed(millis());

	Serial<<"metaPixel initialized"<<endl;
	Serial<<"Parameters: "<<parameterArraySize<<endl;
	Serial<<"Programms: "<<newMaxPrograms<<endl;
	Serial <<"Current Tasks:"<<taskQueue._itemsInQueue<<endl<<endl;
	delay(3000);
}

void loop()
{

	random16_add_entropy( random());
	//
	// switch program Slot 0
	//
	if(EffectProgram.hasChanged()){
		int16_t nextP = EffectProgram.nextValue();
		#if DEBUG_LOOP
		int16_t currentP = EffectProgram.currentValue();
		Serial <<"CP: "<< currentP<<" NP:"<<nextP<<" ";
		#endif
		Delay = effectProgramsN[nextP].delay;
		EffectProgram.syncValue();
		#if 0 && DEBUG_LOOP
		Serial << " PQueue" << taskQueue._itemsInQueue<<endl;
		#endif
		parametersInvalid = true;
	}

	//
	// switch delay (Slot 1)
	//
	if(Delay.hasChanged()){
		#if DEBUG_LOOP
		Serial<<"Delay from:"<<Delay.currentValue()<<" To:"<<Delay.nextValue()<<" ";
		#endif
		#if USE_EFFECT_SCEDULER
		taskQueue.scheduleChangeFunction("EFFC",Delay.nextValue(),Delay.nextValue());
		#endif
		Delay.syncValue();
		#if 0 & DEBUG_LOOP
		Serial << "DQueue" << taskQueue._itemsInQueue<<endl;
		#endif
		parametersInvalid = true;
	}
	//
	// switch palette Slot(2)
	//
	if(Palette.hasChanged()){
		parametersInvalid = true;
		Palette.syncValue();
		#if DEBUG_LOOP
		Serial << "Palette changed to "<<Palette.currentValue()<<endl;
		#endif
	}

	//
	// switch brightness Slot(3)
	//
	if(Brightness.hasChanged()){
		#if DEBUG_LOOP
		Serial <<"B:"<<Brightness.currentValue()<<endl;
		#endif
		FastLED.setBrightness(Brightness.nextValue());
		Brightness.syncValue();
		parametersInvalid=true;
	}

	//
	// switch mirror mode Slot(4)
	//
	if(MirrorParam.hasChanged()){
		MirrorParam.syncValue();
		display.setMirrorMode((displayMirror)MirrorParam.currentValue());
		parametersInvalid=true;
	}

	/** check all remaining parameters */
	{
		bool t = false;
		for(int i=5;i<parameterArraySize-1;++i){
			t = t || parameterArray[i].value->syncValue();
		}
		if(t || parametersInvalid){
			dumpParameters();
			parametersInvalid = false;
			t= false;
		}
	}
	commandQueue.processQueue();
	/** Create output on command processing **/
	if(commandQueueTimer>1000){
		if(commandQueue.waiting){
			Serial << ScreenPos(12,1)<<clearLineRight<<commandQueue<<endl;
		}
		Serial << clearDown;
		commandQueueTimer = 0;
	}
	/** run all sequence tasks */
	taskQueue.Run(millis());
	#if DEBUG_LOOP
	{
		static int p = 0;
		Serial <<ScreenPos(32,0)<<clearLineRight<<ScreenPos(22,p++)<< " . ";
		p= p%30;
	}
	#endif
	delay(2);
}

/**********************************************************
**
** backbuffer
**
**********************************************************/
int backbufferBlender(unsigned long now, void* userdata)
{
	uint8_t frac = BlendParam.currentValue()/Delay.currentValue();
	static uint8_t lastFrac =0;
	if(frac != lastFrac){
		//		Serial << "frac"<<frac<<endl;
		lastFrac = frac;
	}

	#if DEBUG_EFFECTS
	Serial <<".";
	#endif
	if(frac < 4){
		#if DEBUG_EFFECTS
		Serial << "Frac cliped to 4, was "<<frac<<endl;

		#endif
		frac = 4;
	}
	for(uint16_t i=0;i<NUM_LEDS;i++){
		leds[i]=nblend(leds[i],led_backbuffer[i],frac);
	}
	if( millis() < 5000 ) {
		FastLED.setBrightness( scale8( Brightness.currentValue(), (millis() * 256) / 5000));
	} else {
		FastLED.setBrightness(Brightness.currentValue());
	}
	FastLED.show();
	return 0;
}

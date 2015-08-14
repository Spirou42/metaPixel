#define FASTLED_INTERNAL
#include <Queue.h>
#include <FastLED.h>
#include <Streaming.h>
#include <TeensyDmx.h>
#include <rdm.h>
#include <Arduino.h>
#include "EffectWhite.h"
#include "metaPixel.h"


/* program defined */

#define USE_PLASMA 1
#define USE_FIRE   1
#define USE_LINES  0
#define USE_WHITE  1
#define USE_NOISE  1

#define START_PROG 1

/**********************************************************
**
** Globals
**
**********************************************************/



CRGB  leds[NUM_LEDS];
#if USE_DOUBLE_BUFFER
CRGB  led_backbuffer[NUM_LEDS];
mDisplay display= mDisplay(leds,led_backbuffer,MODULES_WIDTH,MODULES_HEIGHT);
#else
mDisplay display= mDisplay(leds,NULL,MODULES_WIDTH,MODULES_HEIGHT);
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
ballState_t currentState;
/**********************************************************
**
** Parameters
**
**********************************************************/

Parameter<int16_t>Delay(10);
Parameter<int16_t>EffectProgram(0);
Parameter<int16_t>Palette(0);
Parameter<int16_t>Brightness(BRIGHTNESS);
Parameter<int16_t>BlendParam(5000);

Parameter<int16_t>genericSpeed1(8);
Parameter<int16_t>genericSpeed2(1);
Parameter<int16_t>genericScale1(50);
Parameter<int16_t>genericScale2(50);
Parameter<int16_t>genericParam1(display.displayWidth());
Parameter<int16_t>genericParam2(display.displayWidth());
Parameter<int16_t>genericEffectMask1(HorizontalEffect | VerticalEffect | DiagonalEffect | CircleEffect);
Parameter<int16_t>genericEffectMask2(HorizontalEffect | VerticalEffect | DiagonalEffect | CircleEffect);

int16_t currentResolution ;
volatile int16_t nextResolution;


boolean effectStarted = false;

CRGBPalette16 colorPalettes[]={
	(CRGBPalette16)RainbowColors_p,
	(CRGBPalette16)CloudColors_p,
	(CRGBPalette16)OceanColors_p,
	(CRGBPalette16)ForestColors_p,
	(CRGBPalette16)PartyColors_p,
	(CRGBPalette16)RainbowStripeColors_p,
	(CRGBPalette16)LavaColors_p,
	(CRGBPalette16)HeatColors_p
};
uint8_t numberOfPalettes = sizeof(colorPalettes)/sizeof(CRGBPalette16);

#if USE_PLASMA
PlasmaData_t normalPla = {false,false,1,400,400,15};
PlasmaData_t mirrowPla = {true,false,1,200,200,15};
PlasmaData_t scalePla 	= {false,false,1,100,100,15};
PlasmaData_t biggerPla = {false,false,1,600,600,15};
#endif

effectProgram_t effectPrograms[]  ={
#if USE_WHITE
	{"WHIT",white,100,NULL},
#endif
#if USE_NOISE
	{"NOIS",noise,150,NULL},
#endif
#if USE_LINES
	{"LINE",line,100,NULL},
#endif
#if USE_PLASMA
	{"PLSA",plasma2,600,(void*)&normalPla},
	{"PLSM",plasmaSimple,300,(void*)&mirrowPla},
	{"PLSS",plasmaSimple,250,(void*)&scalePla},
	{"PLSG",plasmaSimple,300,(void*)&biggerPla},
#endif
#if USE_FIRE
	{"FIRE",fire,60,NULL}
#endif
};


uint8_t maxPrograms = sizeof(effectPrograms) / sizeof(effectProgram_t);



newParameter_t parameterArray[] = {
	// global scope parameters.
	// these parameters for Program, framerate etc.
/* 00 */ 	newParameter_t('P',(int16_t)0,(int16_t)0,EffectProgram),
/* 01 */	newParameter_t('D',(int16_t)1,(int16_t)5000,Delay),
/* 02 */	newParameter_t('C',(int16_t)0,(int16_t)0,Palette),
/* 03 */	newParameter_t('B',(int16_t)0,(int16_t)255,Brightness),
/* 04 */	newParameter_t('Z',(int16_t)0,(int16_t)32000,BlendParam),

	// local parameters. These parameters have a different meening for each  Effect program.
/* 05 */	newParameter_t('U',(int16_t)0,(int16_t)0,genericSpeed1),
/* 06 */  newParameter_t('V',(int16_t)0,(int16_t)0,genericSpeed2),
/* 07 */	newParameter_t('R',(int16_t)0,(int16_t)0,genericScale1),
/* 08 */	newParameter_t('I',(int16_t)0,(int16_t)0,genericScale2),
/* 09 */	newParameter_t('O',(int16_t)0,(int16_t)0,genericParam1),
/* 10 */ 	newParameter_t('H',(int16_t)0,(int16_t)0,genericParam2),
/* 11 */	newParameter_t('M',(int16_t)0,(int16_t)255,genericEffectMask1),
/* 12 */	newParameter_t('N',(int16_t)0,(int16_t)255,genericEffectMask2),
};
int16_t parameterArraySize = sizeof(parameterArray)/sizeof(newParameter_t);

EffectWhite whiteEffect = EffectWhite(&(parameterArray[9]));
EffectFire  fireEffect = EffectFire(&parameterArray[9],&parameterArray[10]  );
//EffectWhite dummy = EffectWhite();
//effectProgramN_t h = {dummy,1000,NULL};
effectProgramN_t effectProgramsN[] = {
	 	{&whiteEffect,1000,NULL},
		{&fireEffect,1000,NULL},
		{&whiteEffect,1000,NULL}
};

uint8_t newMaxPrograms = sizeof(effectProgramsN) / sizeof(effectProgramN_t);




/**********************************************************
**
** DEBUG
**
**********************************************************/

#if DEBUG_BLINK
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
	for(int i=0;i<parameterArraySize;i++){
		Serial << parameterArray[i].code<<" "<<parameterArray[i].value<<" ("<<parameterArray[i].maxValue<<")"<<endl;
	}
}

void setup()
{
	FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(COLOR_CORRECTION);
	FastLED.setBrightness( BRIGHTNESS );
	FastLED.show();
	Serial.begin(115200);

	// tweak global parameter max for Programs and pallettes
	parameterArray[0].maxValue = maxPrograms-1;
	parameterArray[2].maxValue = numberOfPalettes-1;

	pinMode(LED_BLINK_PIN, INPUT);
	currentState = stateNone;

	initEncoderUI();

	DMX.setMode(TeensyDmx::Mode::DMX_IN);

	//	Serial << "LedBuffer: "<<(unsigned long)leds<<" Backbuffer: "<<(unsigned long) led_backbuffer<<endl;

	EffectProgram.initTo(START_PROG);
	int16_t cP = EffectProgram.currentValue();
	Delay.initTo(effectPrograms[cP].delay);
	Brightness.initTo(BRIGHTNESS);
	Palette.initTo(0);
	taskQueue.scheduleFunction(effectPrograms[cP].function,effectPrograms[cP].userData,effectPrograms[cP].id,0,Delay.currentValue());
#if USE_DOUBLE_BUFFER
	taskQueue.scheduleFunction(backbufferBlender,NULL,"BBB ",0,66);
#endif
#if DEBUG_BLINK
	taskQueue.scheduleFunction(blinker,NULL,"BLNK",1000,1000);
#endif

#if USE_EFFECT_SCEDULER
	taskQueue.scheduleFunction(effectRunner,NULL,"EFFC",0,Delay.currentValue());
#endif

	effectStarted = true;

	encoderClickCallback = clickHandler;

	lc.clearDisplay(0);
	randomSeed(millis());
	delay(1000);
	Serial<<"metaPixel initialized"<<endl;
	Serial<<"Parameters: "<<parameterArraySize<<endl;
#if USE_SERIAL_COMMANDS
	taskQueue.scheduleFunction(serialReader,NULL,"SERI",200,200);
#endif

	Serial <<" Current Tasks:"<<taskQueue._itemsInQueue<<endl<<endl;
}

void loop()
{
	bool parameterChanged = false;
//	random16_add_entropy( random());

	//
  // switch program Slot 0
	//
	if(EffectProgram.hasChanged()){
		int16_t nextP = EffectProgram.nextValue();
		int16_t currentP = EffectProgram.currentValue();
#if DEBUG_LOOP
		Serial <<"CP: "<< currentP<<" NP:"<<nextP<<" ";
#endif

		effectStarted = true;
		Delay.initTo(effectPrograms[nextP].delay);
		taskQueue.scheduleRemoveFunction(effectPrograms[currentP].id);
		taskQueue.scheduleFunction(effectPrograms[nextP].function,effectPrograms[nextP].userData,effectPrograms[nextP].id,Delay.currentValue(),Delay.currentValue());
		EffectProgram.syncValue();
#if DEBUG_LOOP
		Serial << " PQueue" << taskQueue._itemsInQueue<<endl;
#endif
		parameterChanged = true;
	}

	//
	// switch delay (Slot 1)
	//
	if(Delay.hasChanged()){
#if DEBUG_LOOP
		Serial<<"Delay from:"<<Delay.currentValue()<<" To:"<<Delay.nextValue()<<" ";
#endif
		taskQueue.scheduleChangeFunction(effectPrograms[EffectProgram.currentValue()].id,Delay.nextValue(),Delay.nextValue());
#if USE_EFFECT_SCEDULER
		taskQueue.scheduleChangeFunction("EFFC",Delay.nextValue(),Delay.nextValue());
#endif
		Delay.syncValue();
#if DEBUG_LOOP
		Serial << "DQueue" << taskQueue._itemsInQueue<<endl;
#endif
		parameterChanged = true;
	}
	//
	// switch palette Slot(2)
	//
	if(Palette.hasChanged()){
		parameterChanged = true;
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
		parameterChanged=true;
	}
	if(currentResolution != nextResolution){
		currentResolution = nextResolution;
		display.setResolution( (displayResolution)currentResolution);
#if USE_DOUBLE_BUFFER
		//		display.clearAll();
#endif
	}
	{
		bool t = false;
		for(int i=4;i<parameterArraySize-1;++i){
			t = t || parameterArray[i].value.syncValue();
		}
		if(t || parameterChanged){
			Serial << "---"<<endl;
			dumpParameters();
			parameterChanged = false;
			t= false;
		}
	}
	taskQueue.Run(millis());
	delay(2);
}

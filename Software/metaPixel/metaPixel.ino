#define FASTLED_INTERNAL
//#define FASTLED_ALLOW_INTERRUPTS 1
/* #include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>*/

#include <Queue.h>
#include <FastLED.h>
#include <Streaming.h>
#include "VT100Stream.h"
#include <TeensyDmx.h>
#include <rdm.h>
#include <Arduino.h>
#include "metaPixel.h"
#include "Palettes.h"
#include "SPI.h"
#include "ILI9341_t3.h"

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

/** the global task queue */
Queue taskQueue;

#if USE_ILI9341_DISPLAY
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC,TFT_RST,TFT_MOSI,TFT_SCK,TFT_MISO);
#endif

tftSerial TFTSerial;

/**********************************************************
**
** Parameters
**
**********************************************************/

/** these are the parameters controlling global states */
AnimationValue Delay(10);												///< delay in ms between two calls to the Effect::frame method
AnimationValue EffectProgram(0);								///< index of the currently running effect program
AnimationValue Palette(0);											///< active palette
AnimationValue Brightness(BRIGHTNESS);					///< current brithness of the display
AnimationValue BlendParam(5);										///< detemines how fast the backbuffer is blended into the display buffer
AnimationValue MirrorParam(0);									///< current display mirror state
AnimationValue ResolutionParam(1);							///< current display resolution

/** these parameters are used by the Effects */
AnimationValue genericSpeed1(8);												///< generic parameter for Effects
AnimationValue genericSpeed2(1);												///< generic parameter for Effects
AnimationValue genericScale1(50);												///< generic parameter for Effects
AnimationValue genericScale2(50);												///< generic parameter for Effects
AnimationValue genericParam1(display.displayWidth());		///< generic parameter for Effects
AnimationValue genericParam2(display.displayWidth());		///< generic parameter for Effects
AnimationValue genericEffectMask1(HorizontalEffect | VerticalEffect | DiagonalEffect | CircleEffect);		///< generic parameter for Effects
AnimationValue genericEffectMask2(HorizontalEffect | VerticalEffect | DiagonalEffect | CircleEffect);		///< generic parameter for Effects
bool parameterHasChanged = false;

/**********************************************************
**
** Palettes
**
**********************************************************/

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
	"Program","Delay","Palette","Bright","Mirror","Fade","Res"
};
/** All of the accessible parameters */
Parameter16_t parameterArray[] = {
	// global scope parameters.
	// these parameters for Program, framerate etc.
	/* 00 */ 	Parameter16_t('P',(int16_t)0,(int16_t)0,		&EffectProgram,String("Program")),
	/* 01 */	Parameter16_t('D',(int16_t)1,(int16_t)5000,	&Delay,String("Delay")),
	/* 02 */	Parameter16_t('C',(int16_t)0,(int16_t)0,		&Palette,String("Palette")),
	/* 03 */	Parameter16_t('B',(int16_t)0,(int16_t)255,	&Brightness,String("Brightness")),
	/* 04 */  Parameter16_t('Q',(int16_t)0,(int16_t)5,		&MirrorParam,String("Mirror")),
	/* 05 */	Parameter16_t('Z',(int16_t)1,(int16_t)14,		&BlendParam,String("Blend")),
	/* 06 */	Parameter16_t('A',(int16_t)0,(int16_t)1,		&ResolutionParam,String("Resolution")),

	// local parameters. These parameters have a different meening for each  Effect program.
	/* 07 */	Parameter16_t('U',(int16_t)0,(int16_t)0,		&genericSpeed1,String()),
	/* 08 */  Parameter16_t('V',(int16_t)0,(int16_t)0,		&genericSpeed2,String()),
	/* 09 */	Parameter16_t('R',(int16_t)0,(int16_t)0,		&genericScale1,String()),
	/* 10 */	Parameter16_t('I',(int16_t)0,(int16_t)0,		&genericScale2,String()),
	/* 11 */	Parameter16_t('O',(int16_t)0,(int16_t)0,		&genericParam1,String()),
	/* 12 */ 	Parameter16_t('H',(int16_t)0,(int16_t)0,		&genericParam2,String()),
	/* 13 */	Parameter16_t('M',(int16_t)0,(int16_t)255,	&genericEffectMask1,String()),
	/* 14 */	Parameter16_t('N',(int16_t)0,(int16_t)255,	&genericEffectMask2,String()),

};


int16_t parameterArraySize = sizeof(parameterArray)/sizeof(Parameter16_t);

/**********************************************************
**
** Effects
**
**********************************************************/
EffectWhite whiteEffect = EffectWhite(&(parameterArray[param_O]));
EffectFire  fireEffect = EffectFire(&parameterArray[param_O],&parameterArray[param_H],&parameterArray[param_U]);
EffectNoise noiseEffect = EffectNoise(&parameterArray[param_R],&parameterArray[param_U],&parameterArray[param_V],&parameterArray[param_M]);
EffectPlasma plasmaEffect = EffectPlasma(&parameterArray[param_I],&parameterArray[param_U],&parameterArray[param_R],&parameterArray[param_V],&parameterArray[param_M]);
EffectPlasmaSimple simplePlasma = EffectPlasmaSimple(&parameterArray[param_R],&parameterArray[param_I],&parameterArray[param_U],&parameterArray[param_M]);
EffectLine lineEffect = EffectLine(&parameterArray[param_V]);
EffectWhitney whitneyEffect = EffectWhitney(&parameterArray[param_U],&parameterArray[param_R],&parameterArray[param_V]);

//EffectWhite dummy = EffectWhite();
//effectProgramN_t h = {dummy,1000,NULL};
effectProgramN_t effectProgramsN[] = {
	{&whiteEffect,100,NULL},
	{&noiseEffect,150,NULL}, //"*rU6C8!R20 80 60"
	{&plasmaEffect,150,"D150B160Z8U12V2I6R10M15"},
	{&simplePlasma,150,"I800R2U30Z6"},
	{&lineEffect,65,"v5z2"},
	{&fireEffect,60,"O70H150U70D60Z12000"},
	{&whitneyEffect,67,NULL},
};

uint8_t newMaxPrograms = sizeof(effectProgramsN) / sizeof(effectProgramN_t);

/**********************************************************
**
** DEBUG & Helpers
**
**********************************************************/

int16_t TFT_LogoEnd = 0;

void drawLogo()
{
	tft.setTextSize(5);
	tft.setCursor(25,15);
	tft.setTextColor(ILI9341_GREEN);
	tft<<"metaPixel";
	tft.drawFastHLine(tft.getCursorX(),tft.getCursorY()+10,tft.width(),ILI9341_GREEN);
	tft.setTextColor(ILI9341_YELLOW);
	tft.setTextSize(2);
	tft<<endl;
	TFT_LogoEnd =0; //tft.cursor_y;
}

void initializeTFT()
{
	tft.begin();

	tft.setRotation(3);
	tft.fillScreen(ILI9341_BLACK);
	tft.setTextWrap(true);
	analogWrite(TFT_LED,100);
	drawLogo();

}
bool runningTFT;
unsigned long testText() {
	runningTFT = true;
  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
	runningTFT = false;
  return micros() - start;
}
void dumpTFTParameters()
{
	tft.setTextWrap(false);
	//uint16_t line = 1;
	uint16_t column = 0;
	uint16_t t = EffectProgram.currentValue()%(newMaxPrograms);
	Effect *effect = effectProgramsN[t].program;
//	tft.fillRect(0,TFT_LogoEnd,tft.width(),tft.height()-TFT_LogoEnd,ILI9341_BLACK);
	tft.setCursor(0,TFT_LogoEnd);
	tft.setTextSize(2);
	tft.setTextColor(ILI9341_YELLOW);
	tft.fillRect(0,tft.getCursorY(),tft.width(),2*7,ILI9341_BLACK); tft<<"Effect: "<<effect->getName()<<endl;tft.setCursor(tft.getCursorX(),tft.getCursorY()+3);
	tft.fillRect(0,tft.getCursorY(),tft.width(),2*7,ILI9341_BLACK);tft<<"Parameter:"<<endl;tft.setCursor(tft.getCursorX(),tft.getCursorY()+4);
	size_t params = effect->numberOfParameters();
	tft.fillRect(0,tft.getCursorY(),tft.width(),2*7,ILI9341_BLACK);
	for(size_t i=0;i<params;i++){
		tft << effect->parameterNameAt(i);
		column +=TFT_NAME_CELL_LENGTH*6*tft.getTextSize();
		tft.setCursor(column,tft.getCursorY());
		tft << effect->parameterAt(i)->value->currentValue();
		column +=6*5*tft.getTextSize();
		tft.setCursor(column,tft.getCursorY());
		if(column > (tft.width()-40)){
			tft<<endl;tft.setCursor(tft.getCursorX(),tft.getCursorY()+4);
			tft.fillRect(0,tft.getCursorY(),tft.width(),2*7,ILI9341_BLACK);
			column = 0;
			tft.setCursor(column,tft.getCursorY());
		}
	}
	if(column!=0){
		tft<<endl;tft.setCursor(0,tft.getCursorY()+6);
	}else{
		tft.setCursor(0,tft.getCursorY()+6);
	}
	column=0;
	tft.fillRect(0,tft.getCursorY(),tft.width(),2*7,ILI9341_BLACK);
	for(int i=0;i<7;i++){
		tft<< SystemParameterName[i];
		column +=TFT_NAME_CELL_LENGTH*6*tft.getTextSize();
		tft.setCursor(column,tft.getCursorY());
		tft<< parameterArray[i].value->currentValue();
		column +=6*5*tft.getTextSize();
		tft.setCursor(column,tft.getCursorY());
		if(column>(tft.width()-40)){
			tft<<endl;tft.setCursor(tft.getCursorX(),tft.getCursorY()+4);
			tft.fillRect(0,tft.getCursorY(),tft.width(),2*7,ILI9341_BLACK);
			column = 0;
			tft.setCursor(column,tft.getCursorY());		}
	}

}

void dumpParameters()
{
	#if USE_ILI9341_DISPLAY
	dumpTFTParameters();
	#endif
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
	for(int i=0;i<7;i++){
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
	Serial <<ScreenPos(++line,1)<<clearLineRight<<">";
}


/**********************************************************
**
** Arduino
**
**********************************************************/

elapsedMillis commandQueueTimer = 0;
void setup()
{
	/** Setup TFT **/
	#if USE_ILI9341_DISPLAY
	initializeTFT();
	#endif

	FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(COLOR_CORRECTION);
	FastLED.clear(true);
	FastLED.setBrightness( BRIGHTNESS );
	FastLED.show();
	Serial.begin(115200);
	delay(100);


	TFTSerial <<"Startup"<<endl;
	// tweak global parameter max for Programs and pallettes
	parameterArray[0].maxValue = newMaxPrograms-1;
	parameterArray[2].maxValue = numberOfPalettes-1;

	/** Setup DMX **/
	DMX.setMode(TeensyDmx::Mode::DMX_IN);

	TFTSerial << "Init Parameters"<<endl;
//	tft<<"Init Parameters"<<endl;

	/** initialize Effects **/
	EffectProgram.initTo(START_PROG);
	int16_t cP = EffectProgram.currentValue();
	Delay.initTo(effectProgramsN[cP].delay);
	Brightness.initTo(BRIGHTNESS);
	Palette.initTo(0);

	/** initialize Queued Task */
	TFTSerial << "Starting Tasks"<<endl;


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


	/** Command line interface **/
	TFTSerial << "Init Commandline Interface"<<endl;
	#if USE_SERIAL_COMMANDS
	taskQueue.scheduleFunction(serialReader,NULL,"SERI",200,200);
	#endif


	randomSeed(millis());

	TFTSerial<<"metaPixel initialized"<<endl;
	TFTSerial<<"Parameters: "<<parameterArraySize<<endl;
	TFTSerial<<"Programms: "<<newMaxPrograms<<endl;
	TFTSerial <<"Current Tasks:"<<taskQueue._itemsInQueue<<endl<<endl;
	delay(3000);
	tft.fillRect(0,TFT_LogoEnd,tft.width(),tft.height()-TFT_LogoEnd,ILI9341_BLACK);
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
		parameterHasChanged = true;
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
		parameterHasChanged = true;
	}
	//
	// switch palette Slot(2)
	//
	if(Palette.hasChanged()){
		parameterHasChanged = true;
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
		parameterHasChanged=true;
	}

	//
	// switch mirror mode Slot(4)
	//
	if(MirrorParam.hasChanged()){
		MirrorParam.syncValue();
		display.setMirrorMode((displayMirror)MirrorParam.currentValue());
		parameterHasChanged=true;
	}
	//
	// switch BlendParam
	//
	if(BlendParam.hasChanged()){
		BlendParam.syncValue();
		parameterHasChanged = true;
	}

	//
	// switch resolution
	//
	if(ResolutionParam.hasChanged()){
		ResolutionParam.syncValue();
		display.setResolution((ResolutionParam.currentValue()==0)?lowRes:highRes);
		parameterHasChanged=true;
	}

	/** check all remaining parameters */
	{
		bool t = false;
		for(int i=param_StartEffect;i<parameterArraySize-1;++i){
			t = t || parameterArray[i].value->syncValue();
		}
		if(t || parameterHasChanged){
			dumpParameters();
			parameterHasChanged = false;
			t= false;
		}
	}
	/** run all sequence tasks */
	taskQueue.Run(millis());

	commandQueue.processQueue();
	/** Create output on command processing **/
	if(commandQueueTimer>1000){
		if(commandQueue.waiting){
			Serial << ScreenPos(12,1)<<clearLineRight<<commandQueue<<endl;
		}
		Serial << clearLineRight;
		Serial << clearDown;
		commandQueueTimer = 0;
//		dumpTFTParameters();
	}
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
	if(runningTFT)
		return 0;
	uint8_t frac = (BlendParam.currentValue()*1000)/Delay.currentValue();
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

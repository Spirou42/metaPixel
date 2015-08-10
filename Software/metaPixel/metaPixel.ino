#include <Queue.h>
#include <FastLED.h>
#include <Streaming.h>
#include <rdm.h>
#include <TeensyDmx.h>
#include "Parameter.h"

#include "PlasmaTypes.h"
#include "Noise.h"
#include "Menu.h"
#include "Encoder.h"
#include "mDisplay.h"


#define USE_SERIAL_COMMANDS 1
#define USE_DOUBLE_BUFFER 1
#define COLOR_ORDER     GRB
#define CHIPSET         WS2812

#define DMXBase	17
#define DMXSpan 7

#define BRIGHTNESS      160
//#define COLOR_CORRECTION 0xe0ffe0
//#define COLOR_CORRECTION 0xffffff
#define COLOR_CORRECTION 0xffeeff

#define ENC_PULSE_PIN             6
#define ENC_STEP_PIN              5
#define ENC_DIR_PIN               4
#define LC_DATA_PIN               3
#define LC_CLK_PIN                2
#define LC_CS_PIN                 1
#define LED_PIN                   0
#define LED_BLINK_PIN            13

/* DEBUG CONFIGURATION */
#define DEBUG (1)
#define DEBUG_BLINK   (1 & DEBUG)
#define DEBUG_EFFECTS (0 & DEBUG)
#define DEBUG_ENCODER (0 & DEBUG)
#define DEBUG_MENU    (0 & DEBUG)
#define DEBUG_COMMAND	(0 & DEBUG)
#define DEBUG_LOOP		(0 & DEBUG)


/* program defined */

#define USE_PLASMA 1
#define USE_FIRE   1
#define USE_LINES  0
#define USE_WHITE  1
#define USE_NOISE  1

#define START_PROG 1

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t macro;
	uint8_t strobe;
	uint8_t mode;
	uint8_t master;
}DMXChannels_t;


/**********************************************************
**
** Globals
**
**********************************************************/

#define NUM_LEDS (DISPLAY_WIDTH * DISPLAY_HEIGHT)


CRGB  leds[NUM_LEDS];
#if USE_DOUBLE_BUFFER
CRGB  led_backbuffer[NUM_LEDS];
mDisplay display= mDisplay(leds,led_backbuffer,MODULES_WIDTH,MODULES_HEIGHT);
#else
mDisplay display= mDisplay(leds,NULL,MODULES_WIDTH,MODULES_HEIGHT);
#endif
static MPPixel currentPixel(0,0);

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

Parameter<int16_t> Delay(10);
Parameter<int16_t>EffectProgram(0);

Parameter<int16_t>Palette(0);
Parameter<int16_t>Brightness(BRIGHTNESS);
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
	{"FIRE",fire,90,NULL}
#endif
};
uint8_t maxPrograms = sizeof(effectPrograms) / sizeof(effectProgram_t);

newParameter_t parameterArray[] = {
	newParameter_t('P',(int16_t)0,(int16_t)(maxPrograms-1),EffectProgram),
	newParameter_t('D',(int16_t)1,(int16_t)800,Delay),
	newParameter_t('C',(int16_t)0,(int16_t)(numberOfPalettes-1),Palette),
	newParameter_t('B',(int16_t)0,(int16_t)255,Brightness),
	newParameter_t('U',(int16_t)0,(int16_t)1000,noiseSpeedN),
	newParameter_t('R',(int16_t)0,(int16_t)255,noiseScaleN),
	newParameter_t('I',(int16_t)0,(int16_t)100,noiseHueSpeedN)
};
int16_t parameterArraySize = sizeof(parameterArray)/sizeof(newParameter_t);

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
** backbuffer
**
**********************************************************/
#if USE_DOUBLE_BUFFER
int backbufferBlender(unsigned long now, void* userdata)
{
	uint8_t frac = 4000/Delay.currentValue(); 
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
#endif
/**********************************************************
**
** Arduino
**
**********************************************************/
void dumpParameters()
{
	for(int i=0;i<parameterArraySize;i++){
		Serial << parameterArray[i].code<<" "<<parameterArray[i].value<<endl;
	}
}


void setup() 
{
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(COLOR_CORRECTION);
	FastLED.setBrightness( BRIGHTNESS );
	FastLED.show();
	Serial.begin(115200);


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
	dumpParameters();
}

void loop()
{
	bool parameterChanged = false;
//	random16_add_entropy( random());
	if(EffectProgram.hasChanged()){          // switch program 
		int16_t nextP = EffectProgram.nextValue();
		int16_t currentP = EffectProgram.currentValue();
#if DEBUG_LOOP
		Serial <<"CP: "<< currentP<<" NP:"<<nextP<<" ";
#endif

		effectStarted = true;
		Delay =effectPrograms[nextP].delay;
		taskQueue.scheduleRemoveFunction(effectPrograms[currentP].id);
		taskQueue.scheduleFunction(effectPrograms[nextP].function,effectPrograms[nextP].userData,effectPrograms[nextP].id,Delay.nextValue(),Delay.nextValue());
		EffectProgram.syncValue();
		#if DEBUG_LOOP
		Serial << " PQueue" << taskQueue._itemsInQueue<<endl;
		#endif
		parameterChanged = true;
	}
	// switch delay
	if(Delay.hasChanged()){
		#if DEBUG_LOOP
		Serial<<"Delay from:"<<Delay.currentValue()<<" To:"<<Delay.nextValue()<<" ";
		#endif
		taskQueue.scheduleChangeFunction(effectPrograms[EffectProgram.currentValue()].id,Delay.nextValue(),Delay.nextValue());
		Delay.syncValue();
		#if DEBUG_LOOP
		Serial << "DQueue" << taskQueue._itemsInQueue<<endl;
		#endif
		parameterChanged = true;
	}
	// switch brightness
	if(Brightness.hasChanged()){
		#if DEBUG_LOOP
		    Serial <<"B:"<<currentBrightness<<endl;
		#endif
		FastLED.setBrightness(Brightness.nextValue());
		Brightness.syncValue();
		parameterChanged=true;
	}
	// switch palette
	if(Palette.hasChanged()){
		parameterChanged = true;
		Palette.syncValue();

	}
	if(currentResolution != nextResolution){
		currentResolution = nextResolution;
		display.setResolution( (displayResolution)currentResolution);
#if USE_DOUBLE_BUFFER
//		display.clearAll();
#endif
	}
	taskQueue.Run(millis());
	if(parameterChanged){
			dumpParameters();
	}
	delay(2);
}

/****************************
Serial Interface
****************************/
#define SERIAL_BUFFER_LENGTH 20
char serial_buffer[SERIAL_BUFFER_LENGTH];
uint8_t currentChar=0;

int serialReader(unsigned long now, void* userData)
{
	bool endLine=false;
	while(Serial.available()){
		char c = Serial.read();
		if(c==0x0d){
			c = 0x00;
			endLine = true;
		}
		serial_buffer[currentChar++]=c;
		if(currentChar >(SERIAL_BUFFER_LENGTH-1)){
			currentChar =0;
		}
		if(endLine){
#if DEBUG_COMMAND
			Serial << "Command:"<<serial_buffer <<endl;
#endif
			commandProcessor(serial_buffer);
			Serial << endl;
			memset(serial_buffer,0x00,SERIAL_BUFFER_LENGTH);
			currentChar = 0;
		}
		
	}
	return 0;
}







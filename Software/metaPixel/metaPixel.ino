#include <Queue.h>
#include <FastLED.h>
#include <Streaming.h>
#include <rdm.h>
#include <TeensyDmx.h>
#include "PlasmaTypes.h"
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
int16_t currentDelay = 10;
volatile int16_t  nextDelay = currentDelay;

int16_t currentProgram ;
volatile int16_t nextProgram;

int16_t currentPalette = 0;
volatile int16_t nextPalette;

int16_t currentBrightness;
volatile int16_t nextBrightness;

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
//	{"TIML",timelinePlayer,100,NULL},
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
	uint8_t frac = 4000/currentDelay; 

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
		FastLED.setBrightness( scale8( currentBrightness, (millis() * 256) / 5000));
	} else {
		FastLED.setBrightness(currentBrightness);
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


void setup() 
{

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(COLOR_CORRECTION);
	FastLED.setBrightness( BRIGHTNESS );
	FastLED.show();
	Serial.begin(115200);
	
  Serial<<"Started"<<endl;

	pinMode(LED_BLINK_PIN, INPUT);
	currentState = stateNone;

	initEncoderUI();
	
	DMX.setMode(TeensyDmx::Mode::DMX_IN);
	
//	Serial << "LedBuffer: "<<(unsigned long)leds<<" Backbuffer: "<<(unsigned long) led_backbuffer<<endl;
	currentProgram = nextProgram = START_PROG;
	currentDelay = nextDelay = effectPrograms[currentProgram].delay;
	currentBrightness = nextBrightness = BRIGHTNESS;
	currentPalette = nextPalette = 0;
	taskQueue.scheduleFunction(effectPrograms[currentProgram].function,effectPrograms[currentProgram].userData,effectPrograms[currentProgram].id,0,currentDelay);
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
	Serial << "Running"<<endl;
#if USE_SERIAL_COMMANDS
	taskQueue.scheduleFunction(serialReader,NULL,"SERI",200,200);
#endif

}

void loop()
{
//	random16_add_entropy( random());
	if(nextProgram != currentProgram){          // switch program 
#if DEBUG
		Serial << currentProgram<<" "<<nextProgram<<" ";
#endif
		effectStarted = true;
		nextDelay=currentDelay =effectPrograms[nextProgram].delay;
		taskQueue.scheduleRemoveFunction(effectPrograms[currentProgram].id);
		taskQueue.scheduleFunction(effectPrograms[nextProgram].function,effectPrograms[nextProgram].userData,effectPrograms[nextProgram].id,currentDelay,currentDelay);
		currentProgram = nextProgram;
		#if DEBUG
		Serial << "PQueue" << taskQueue._itemsInQueue<<endl;
		#endif
	}
	// switch delay
	if(currentDelay != nextDelay){
		#if DEBUG
		Serial<<currentDelay<<" "<<nextDelay<<" ";
		#endif
		taskQueue.scheduleChangeFunction(effectPrograms[currentProgram].id,nextDelay,nextDelay);
		currentDelay = nextDelay;
		#if DEBUG
		Serial << "DQueue" << taskQueue._itemsInQueue<<endl;
		#endif
	}
	// switch brightness
	if(currentBrightness != nextBrightness){
		#if DEBUG
//    Serial <<"B:"<<currentBrightness<<endl;
		#endif
		FastLED.setBrightness(nextBrightness);
		currentBrightness = nextBrightness;
	}
	// switch palette
	if(currentPalette != nextPalette){
		currentPalette = nextPalette;
	}
	if(currentResolution != nextResolution){
		currentResolution = nextResolution;
		display.setResolution( (displayResolution)currentResolution);
#if USE_DOUBLE_BUFFER
//		display.clearAll();
#endif
	}
	taskQueue.Run(millis());
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
			Serial << "Command:"<<serial_buffer <<endl;
			commandProcessor(serial_buffer);
			memset(serial_buffer,0x00,SERIAL_BUFFER_LENGTH);
			currentChar = 0;
		}
		
	}
	return 0;
}


/****************************
Timeline Handling
****************************/



TimelineEntry_t timeline[]={
	{{"WHIT",white,100,NULL},5.0},
		{{"LINE",line,100,NULL}, 5.0},
		{{"PLSA",plasmaSimple,300,(void*)&normalPla}, 10.0}
};
int timelinePlayer(unsigned long now,void* userData)
{
	static uint8_t currentTimelineSlot = 0;
	static unsigned long lastNow = 0;
	static  float currentTimeSlice = 0.0;
	// if(effectStarted){
	// 	Serial << "Effect started:"<<endl;
	// 	effectStarted = false;
	// 	lastNow = 0;
	// 	currentTimelineSlot = 0;
	// 	nextDelay=currentDelay =timeline[currentTimelineSlot].program.delay;
	//     taskQueue.scheduleFunction(timeline[currentTimelineSlot].program.function,timeline[currentTimelineSlot].program.userData,timeline[currentTimelineSlot].program.id,currentDelay,currentDelay);
	// 	currentTimeSlice = timeline[currentTimelineSlot].seconds;
	// }

	if( ((now - lastNow)/1000.0f) > currentTimeSlice){
		lastNow = now;
		uint8_t nextTimelineSlot = currentTimelineSlot+1;
		if(nextTimelineSlot>2){
			nextTimelineSlot = 0;
		}
		Serial << "Terinate: "<<timeline[currentTimelineSlot].program.id<<"("<<currentTimelineSlot<<")"<<endl;
		taskQueue.scheduleRemoveFunction(timeline[currentTimelineSlot].program.id);
		nextDelay=currentDelay =timeline[nextTimelineSlot].program.delay;
		Serial << "Start: "<<timeline[nextTimelineSlot].program.id<<"("<<nextTimelineSlot<<")"<<endl;
		effectStarted = true;
		taskQueue.scheduleFunction(timeline[nextTimelineSlot].program.function,timeline[nextTimelineSlot].program.userData,timeline[nextTimelineSlot].program.id,currentDelay,currentDelay);

		currentTimelineSlot = nextTimelineSlot;
		currentTimeSlice = timeline[currentTimelineSlot].seconds;
	}
	return 0;
}





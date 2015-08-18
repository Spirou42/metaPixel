/*
code for encoder and 7 Segment LED UI board
LED Segment display uses MAX7219
*/

#if USE_LEGACY_MENU

#include <LedControl.h>
#include <Streaming.h>
#include <Queue.h>

#include "Encoder.h"



#define doubleClickTime 130
#define longClickTime 700
#define MAXValue 30
#define MINValue -10

int16_t  encoderMaxValue = MAXValue;
int16_t   encoderMinValue = 1;
int8_t    encoderStep = 1;
int16_t  encoderValue;
volatile  int16_t *encoderPos = NULL;
encoderValueChangeFpt encoderValueChangeCallback;
encoderClickFpt       encoderClickCallback;

unsigned long pulseStartTime;
unsigned long pulseEndTime;


LedControl lc = LedControl(LC_DATA_PIN,LC_CLK_PIN,LC_CS_PIN,1);
boolean state = LOW;

void initEncoderUI() {
#if DEBUG_ENCODER
	Serial << "Init encoderUI"<<endl;
#endif

	lc.shutdown(0,false);
	lc.setIntensity(0,8);
	lc.clearDisplay(0);

	//encoderPin Step
	pinMode(ENC_STEP_PIN, INPUT);
	digitalWrite(ENC_STEP_PIN,HIGH);

	// encoderPin Direction
	pinMode(ENC_DIR_PIN, INPUT);
	digitalWrite(ENC_DIR_PIN,HIGH);

	// encoderPin Pulse
	pinMode(ENC_PULSE_PIN,INPUT);
	digitalWrite(ENC_PULSE_PIN,HIGH);

	attachInterrupt(ENC_PULSE_PIN, doPulse,CHANGE);
	attachInterrupt(ENC_STEP_PIN, doEncoder, FALLING);  // encoder pin on interrupt 0 - pin 3
	if(encoderPos){
		*encoderPos = encoderMinValue;
	}
	encoderValue = encoderMinValue;
//  printNumber(encoderPos);
#if DEBUG_ENCODER
	Serial << "EncoderUI initialized"<<endl;                // a personal quirk
#endif
	writeMessage();
}

void writeMessage() {
	unsigned long delaytime=250;
	lc.setChar(0,3,'2',false);
	delay(delaytime);
	lc.setChar(0,2,'H',false);
	delay(delaytime);
	lc.setChar(0,1,'1',false);
	delay(delaytime);
	lc.setChar(0,0,'1',false);
	delay(delaytime);
	lc.clearDisplay(0);
}

void doPulse()
{

	noInterrupts();
	detachInterrupt(ENC_PULSE_PIN);
	static boolean wasLong = false;
	static uint8_t clickCount = 0;
	if (digitalRead(ENC_PULSE_PIN)){
		if((millis() - pulseEndTime)<doubleClickTime){
			clickCount ++;
			#if DEBUG_ENCODER
			Serial.print("Double ");
			#endif
		}

		pulseStartTime = millis();
		#if DEBUG_ENCODER
		Serial.print("DOWN");
		#endif

	}else{
		clickCount ++;
		pulseEndTime = millis();
		if((millis() - pulseStartTime) >longClickTime){
			wasLong = true;

		}
		if(encoderClickCallback){
			encoderClickCallback(clickCount,wasLong);
		}
		wasLong = false;
		clickCount = 0;

	}
	delay(10);
	attachInterrupt(ENC_PULSE_PIN, doPulse,CHANGE);
	interrupts();
}

void doEncoder(){
//	static unsigned long lastTick = 0;
	if(encoderPos == NULL){
		return;
	}
	noInterrupts();
	detachInterrupt(ENC_STEP_PIN);
	uint8_t tickMul=1;
/*	unsigned long tickTime = millis()-lastTick;
	uint8_t tickMul = (200-(tickTime))/20.0;
	if(tickTime>200){
		tickMul = 1;
	}
	Serial << "TickTime: "<<tickTime<<"("<<tickMul<<")"<<endl;*/
//	lastTick = millis();


	if (digitalRead(ENC_DIR_PIN) == HIGH) {  // check channel B to see which way
																						// encoder is turning
		*encoderPos = *encoderPos - encoderStep*tickMul;         // CCW
	}
	else {
		*encoderPos = *encoderPos + encoderStep*tickMul;         // CW
	}
	if(*encoderPos < encoderMinValue){
		*encoderPos = encoderMinValue;
	}

	if(*encoderPos>encoderMaxValue){
		*encoderPos = encoderMaxValue;
	}


	if(encoderValueChangeCallback && encoderPos){
		encoderValueChangeCallback(*encoderPos);
	}
	delay(10);
	attachInterrupt(ENC_STEP_PIN, doEncoder, FALLING);  // encoder pin on interrupt 0 - pin 3
	interrupts();
}

void printNumber(int v) {
	int ones;
	int tens;
	int hundreds;
	boolean negative = false;

	if( (v < -999) || (v > 999))
		return;
	if(v<0) {
		negative=true;
		v=v*-1;
	}
	ones=v%10;
	v=v/10;
	tens=v%10;
	v=v/10;
	hundreds=v;
	if(negative) {
			//print character '-' in the leftmost column
		lc.setChar(0,3,'-',false);
	}
	else {
			//print a blank in the sign column
		lc.setChar(0,3,' ',false);
	}
		//Now print the number digit by digit
	lc.setDigit(0,2,(byte)hundreds,false);
	lc.setDigit(0,1,(byte)tens,false);
	lc.setDigit(0,0,(byte)ones,false);
}

#endif

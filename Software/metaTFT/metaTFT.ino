#define FASTLED_INTERNAL
#include "SPI.h"
#include "ILI9341_t3.h"
#include "Streaming.h"
#include "metaTFTDisplay.h"

#include "Switch.h"
#include "UserEvent.h"
#include "Switches.h"
#include "Encoder.h"
#include "Encoders.h"
#include "FastLED.h"
#include "Palettes.h"
#include "font_Arial.h"
#include "GraphicTests.h"

/** TFT Configuration **/
#define TFT_RST   2
#define TFT_DC    9
#define TFT_CS    10
#define TFT_MISO  12
#define TFT_MOSI  11
#define TFT_SCK   13
#define TFT_LED   A14
//A14 || 6

metaTFT tft = metaTFT(TFT_CS, TFT_DC,TFT_RST,TFT_MOSI,TFT_SCK,TFT_MISO,TFT_LED,3);
UserEventQueue eventQueue = UserEventQueue();

CRGBPalette16 palettes[] = {
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
const char * paletteNames[] = {
	"Rainbow", "Clouds",	"Ocean", "Forest", "Party", "Rainbow Stripes", "Lava", "Heat",
	"GP Arctic", "GP Temperature","GP Colombia", "GP Cequal"
};
size_t numberOfPalettes = sizeof(palettes) / sizeof(CRGBPalette16);

int8_t currentPalette = 0;
//ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC,TFT_RST,TFT_MOSI,TFT_SCK,TFT_MISO);
void initializeTFT()
{
	//tft.setLuminance(10);
	tft.start();
	tft.setLuminance(180);
	//tft.setFont(Arial_16);
}
Adafruit_GFX_Button UpButton = Adafruit_GFX_Button();
Adafruit_GFX_Button DownButton = Adafruit_GFX_Button();
Adafruit_GFX_Button CenterButton = Adafruit_GFX_Button();
Adafruit_GFX_Button LeftButton = Adafruit_GFX_Button();
Adafruit_GFX_Button RightButton = Adafruit_GFX_Button();

void initMask()
{
	UpButton.initButton((ILI9341_t3*)&tft,160,45,100,50,
	ILI9341_YELLOW,ILI9341_BLACK,ILI9341_GREEN,"UP",2);

	DownButton.initButton((ILI9341_t3*)&tft,160,tft.height()-45,100,50,
	ILI9341_YELLOW,ILI9341_BLACK,ILI9341_GREEN,"Down",2);

	CenterButton.initButton((ILI9341_t3*)&tft,160,tft.height()/2,100,50,
	ILI9341_YELLOW,ILI9341_BLACK,ILI9341_GREEN,"Center",2);

	LeftButton.initButton((ILI9341_t3*)&tft,50,tft.height()/2,100,50,
	ILI9341_YELLOW,ILI9341_BLACK,ILI9341_GREEN,"Left",2);

	RightButton.initButton((ILI9341_t3*)&tft,270,tft.height()/2,100,50,
	ILI9341_YELLOW,ILI9341_BLACK,ILI9341_GREEN,"Right",2);

}

elapsedMillis firstTime = elapsedMillis(0);

void setup() {
	//delay(1000);
	initializeTFT();
	pinMode(TFT_LED,OUTPUT);
	Serial.begin(115200);
	enableSwitches();
	enableEncoders();
	initMask();
}
elapsedMillis displayTimer ;


void drawMask(){
	tft.fillScreen(ILI9341_BLACK);
	UpButton.drawButton();
	DownButton.drawButton();
	CenterButton.drawButton();
	LeftButton.drawButton();
	RightButton.drawButton();
}

#define lineDelay 0
#define TextX 80
#define TextY 120

void drawText()
{
	const char * name = paletteNames[currentPalette];
	int16_t x=(tft.width()/2- tft.stringWidth(name)/2), y= (tft.height()/2 - tft.stringHeight(name)/2);
	tft.fillRect(x,y,tft.stringWidth(name),tft.stringHeight(name),ILI9341_BLACK);
	tft.setCursor(x,y);
	tft << name<<endl;

}
void effectMoiree()
{
	uint8_t colorIDX = 0;
	tft.fillScreen(ILI9341_BLACK);
	tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
	tft.setTextSize(4);
	int step = 1;
//	for(int step=10;step>0;--step){
		//		int step = 3;
		for(int x=0;x<tft.width();x+=step){
			CRGB c = ColorFromPalette(palettes[currentPalette],colorIDX++);
			//uint16_t color = tft.color565(c.r, c.g, c.b);
			tft.drawLine(x,0,tft.width()-x,tft.height(),c);
//			drawText();
//			delay(lineDelay);
		}
		for(int y=tft.height();y>=0;y-=step){
			CRGB c = ColorFromPalette(palettes[currentPalette],colorIDX++);
			uint16_t color = tft.color565(c.r, c.g, c.b);
			tft.drawLine(0,y,tft.width(),tft.height()-y,color);
//			drawText();
//			delay(lineDelay);
		}
//	}
	drawText();
	currentPalette = ((++currentPalette) % numberOfPalettes);
	delay(3000);
}

boolean skipMask = false;
typedef void(*effectHandler)();

static uint16_t offset = 0;
void scrollLeft()
{
	offset -= 10;
	tft.setScroll(offset);
	Serial << "Offset: "<<offset<<endl;

}
void scrollCenter()
{
	tft.setScroll(0);
	offset = 0;
}
void scrollRight()
{
	offset +=10;
	tft.setScroll(offset);
	Serial << "Offset: "<<offset<<endl;
}


void adjustBrightness()
{
	static elapsedMillis lastAdjust = elapsedMillis(0);
	tft.fillScreen(ILI9341_BLACK);
	tft.setCursor(20,80);
	tft << "Brightness: "<<tft.getLuminance();

	lastAdjust =0;
	do{
		if(eventQueue.length()){
			UserEvent *evnt = eventQueue.popEvent();
			if(evnt->getType()==UserEvent::EventType::EventTypeKey){
				UserEvent::ButtonData data = evnt->getData().buttonData;
				if(data.id==UserEvent::ButtonID::CenterButton){
					break;
				}
			}else if(evnt->getType() ==UserEvent::EventType::EventTypeEncoder){
				UserEvent::EncoderData data = evnt->getData().encoderData;
				int8_t steps = data.absSteps;
				lastAdjust=0;
				tft.setLuminance(tft.getLuminance()+steps);
				//tft.fillScreen(ILI9341_BLACK);
				tft.setCursor(20,80);
				tft << "Brightness: "<<tft.getLuminance();
				Serial <<"Brightness: "<<tft.getLuminance()<<"    "<<endl;
			}
		}
	}while(lastAdjust<5000);
}

void resetDisplay()
{
	tft.begin();
	tft.start();
}

void loop() {
	// put your main code here, to run repeatedly:
	if(firstTime>1000 && !skipMask){
		drawMask();
		Serial << "Draw"<<endl;
		Serial.flush();
		skipMask = true;
	}
	// if(eventQueue.length()){
	// 	tft.setCursor(0,0);
	// 	tft << eventQueue.length()<<"     ";
	// 	Serial <<eventQueue.length()<<endl;
	// }

	if(displayTimer > 500){
		while(eventQueue.length()){
			UserEvent *evnt = eventQueue.popEvent();
			Serial << evnt << endl;
			if(evnt->getType() == UserEvent::EventType::EventTypeKey){
				UserEvent::ButtonData data = evnt->getData().buttonData;
				Adafruit_GFX_Button *someButton = NULL;
				effectHandler k = NULL;
				switch(data.id){
					case UserEvent::ButtonID::UpButton: someButton = &UpButton; k = effectMoiree; break;
					case UserEvent::ButtonID::CenterButton: someButton = &CenterButton;k=resetDisplay;break;
					case UserEvent::ButtonID::DownButton: someButton = &DownButton; k = testThings; break;
					case UserEvent::ButtonID::LeftButton: someButton = &LeftButton; k=scrollLeft; break;
					case UserEvent::ButtonID::RightButton: someButton = &RightButton; k=scrollRight; break;
					default: break;
				}
				boolean someState = false;
				switch(data.state){
					case UserEvent::ButtonState::ButtonDown: someState = true; break;
					case UserEvent::ButtonState::ButtonUp: someState = false; break;
					default: break;
				}
				if(someButton)
					someButton->drawButton(someState);

				//		Serial << evnt<<", "<<_HEX((long unsigned int)k)<<endl;
				if((NULL != k)){
					switch(data.state){
						case UserEvent::ButtonState::ButtonClick:
						case UserEvent::ButtonState::ButtonLongClick:
						case UserEvent::ButtonState::ButtonDoubleClick:
						k();

						drawMask();
						k=0;
						break;
						default: break;

					}
				}
			}else{
				adjustBrightness();
				drawMask();
			}
			delete evnt;
		}
		displayTimer = 0;
		// tft.setLuminance(tft.getLuminance()+1);
		// Serial << "Brigth "<<tft.getLuminance()<<endl;
		// drawMask();
	}
}

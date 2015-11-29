#define FASTLED_INTERNAL
#include <stack>
#include <vector>
#include "Math.h"

#include "SPI.h"
#include "ILI9341_t3.h"
#include "Streaming.h"
#include "FastLED.h"

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
#include "UIHelpers.h"
#include "font_Montserrat_Regular.h"




/** TFT Configuration **/
#define TFT_RST   2
#define TFT_DC    9
#define TFT_CS    10
#define TFT_MISO  12
#define TFT_MOSI  11
#define TFT_SCK   13
#define TFT_LED   6
//A14 || 6

/** FastLed**/
#define COLOR_ORDER       GRB
#define CHIPSET           WS2812
#define COLOR_CORRECTION  0xffeeff
#define BRIGHTNESS        160

#define LED_PIN                   4
#define NUM_LEDS 60
CRGB  leds[NUM_LEDS];


metaTFT tft = metaTFT(TFT_CS, TFT_DC,TFT_RST,TFT_MOSI,TFT_SCK,TFT_MISO,TFT_LED,3);
UserEventQueue eventQueue = UserEventQueue();
using namespace std;
vector<int> blubber;

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
	//tft.setLuminance(180);
	//tft.setFont(Arial_16);
}

void initializeLEDs()
{
	FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(COLOR_CORRECTION);
	FastLED.clear(true);
	FastLED.setBrightness( BRIGHTNESS );
	FastLED.show();
}
metaButton UpButton = metaButton();
metaButton DownButton = metaButton();
metaButton CenterButton = metaButton();
metaButton LeftButton = metaButton();
metaButton RightButton = metaButton();

void initMask()
{
	UpButton.initButton(&tft,160,45,100,50,
	ILI9341_YELLOW,ILI9341_BLACK,ILI9341_GREEN,"UP",2);

	DownButton.initButton(&tft,160,tft.height()-45,100,50,
	ILI9341_YELLOW,ILI9341_BLACK,ILI9341_GREEN,"Down",2);

	CenterButton.initButton(&tft,160,tft.height()/2,100,50,
	ILI9341_YELLOW,ILI9341_BLACK,ILI9341_GREEN,"Center",2);

	LeftButton.initButton(&tft,50,tft.height()/2,100,50,
	ILI9341_YELLOW,ILI9341_BLACK,ILI9341_GREEN,"Left",2);

	RightButton.initButton(&tft,270,tft.height()/2,100,50,
	ILI9341_YELLOW,ILI9341_BLACK,ILI9341_GREEN,"Right",2);
}

elapsedMillis firstTime = elapsedMillis(0);

void setup() {
	initializeLEDs();
	Serial.begin(115200);
	Serial << "Start"<<endl;
	pinMode(TFT_LED,OUTPUT);
	digitalWriteFast(TFT_LED,1);
	Serial << "Init"<<endl;
	initializeTFT();


	enableSwitches();
	enableEncoders();
	initMask();
	// for(int i=0;i<100;i++){
	// 	blubber.push_back(i);
	// }
	//
	// for(auto i=begin(blubber);i!=end(blubber);i++){
	// 	Serial << "Number: "<<*i<<endl;
	// }
}
elapsedMillis displayTimer ;


void drawMask(){
	tft.fillScreen(ILI9341_BLACK);
	tft.setFontAdafruit();
	tft.setTextSize(2);
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
	GCSize stringSize = tft.stringSize(name);
	int16_t x=(tft.width()/2- stringSize.w/2), y= (tft.height()/2 - stringSize.h/2);
	tft.fillRect(x,y,stringSize.w,stringSize.h,ILI9341_BLACK);
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
	int8_t uValue = log(256-tft.getLuminance())*10;
	tft.fillScreen(ILI9341_BLACK);
	String labelStr = String(" Brightness ");
	String valueStr = String("-55 ");
	String blubberStr = String ("-UU ");
	metaValue bla = metaValue();
	bla.initValue(&tft,GCRect(180,tft.height()/2-25,138,80), &labelStr, &valueStr);
	GCPoint bp = bla.getOrigin();
	bp.x = tft.width()/2.0 - bla.getSize().w/2.0;
	bla.setOrigin(bp);
	bla.redraw();


	static elapsedMillis lastAdjust = elapsedMillis(0);

	// tft.setCursor(20,80);
	// tft.fillRect(20,80,16*6*tft.getTextSize(),7*tft.getTextSize(),ILI9341_BLUE);
	// tft << "Brightness: "<<tft.getLuminance();

	lastAdjust =0;
	do{
		if(eventQueue.length()){
			int8_t kValue = uValue;
			UserEvent *evnt = eventQueue.popEvent();
			if(evnt->getType()==UserEvent::EventType::EventTypeKey){
				UserEvent::ButtonData data = evnt->getData().buttonData;
				if(data.id==UserEvent::ButtonID::CenterButton){
					break;
				}
				if(data.id == UserEvent::ButtonID::UpButton){
					uValue = 55;
					lastAdjust=0;
				}
				if(data.id == UserEvent::ButtonID::DownButton &&
					(data.state == UserEvent::ButtonState::ButtonClick ||
						data.state == UserEvent::ButtonState::ButtonLongClick)){
					uValue = 0;
					lastAdjust=0;
				}

			}else if(evnt->getType() ==UserEvent::EventType::EventTypeEncoder){
				UserEvent::EncoderData data = evnt->getData().encoderData;
				int8_t steps = data.absSteps;
				uValue +=steps;
				lastAdjust=0;
			}
			if(uValue > 55){
				uValue = 55;
			}else if(uValue <0){
				uValue = 0;
			}
			if(uValue != kValue){
				uint8_t k= exp(uValue/10.0);
				k = 256 -k;
				tft.setLuminance(k);
				valueStr.remove(0);
				valueStr += String(uValue);
				bla.valueUpdate();
				bla.redraw();
			//  tft.setCursor(20,20);
			//  tft.fillRect(20,80,16*6*tft.getTextSize(),7*tft.getTextSize(),ILI9341_BLUE);
			//  tft << "Brightness: "<<tft.getLuminance();
				//Serial <<"Brightness: "<<tft.getLuminance()<<"    "<<endl;
			}
		}
	}while(lastAdjust<5000);
}

void resetDisplay()
{
	tft.begin();
	tft.start();
}

void testViews()
{
	tft.fillScreen(ILI9341_BLACK);
	metaView k = metaView();
	Serial <<"TFT: "<<tft.width()<<", "<<tft.height()<<endl;
	k.initView(&tft,10,10,tft.width()-20,tft.height()-20);
	k.setBackgroundColor(ILI9341_BLUE);
	k.setOutlineColor(ILI9341_RED);
	k.setCornerRadius(10);
	k.setDrawsOutline(true);
	k.setOpaque(true);
	String label("Hurga");
	metaLabel l = metaLabel(&label);
	l.initView(&tft,10,10,30,30);
	l.setBackgroundColor(ILI9341_ORANGE);
	l.setOutlineColor(ILI9341_GREEN);
	l.setCornerRadius(5);
	l.setDrawsOutline(true);
	l.setOpaque(false);
	l.setTextSize(3);
	l.setTextColor(ILI9341_BLACK);

	k.addSubview(&l);
	k.redraw();
	delay(800);
	GCPoint t = GCPoint(2,1);
	for(int i=0;i<100;i++){
		l.setOrigin(l.getOrigin()+t);
		k.redraw();
		delay(80);
	}
	l.removeFromSuperview();
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

	if(displayTimer > 100){
		while(eventQueue.length()){
			UserEvent *evnt = eventQueue.popEvent();
			Serial << evnt << endl;
			if(evnt->getType() == UserEvent::EventType::EventTypeKey){
				UserEvent::ButtonData data = evnt->getData().buttonData;
				metaButton *someButton = NULL;
				effectHandler k = NULL;
				switch(data.id){
					case UserEvent::ButtonID::UpButton: someButton = &UpButton; k = effectMoiree; break;
					case UserEvent::ButtonID::CenterButton: someButton = &CenterButton;break;
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

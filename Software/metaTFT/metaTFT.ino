#define FASTLED_INTERNAL
#include <stack>
#include <vector>
#include "Math.h"

#include "SPI.h"
#include "ILI9341_t3.h"
#include "Queue.h"
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
#include "font_ArialBold.h"
#include "font_GillSans.h"
#include "font_GillSans_SemiBold.h"
#include "GraphicTests.h"
#include "UIHelpers.h"
//#include "font_Montserrat_Regular.h"
#include "LEDEffects.h"
#include "metaTFT.h"

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
Queue taskQueue;

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
	FastLED.setBrightness( LED_BRIGHTNESS );
	FastLED.show();
}

typedef struct _valuePair{
	String label,value;
	_valuePair(String l, String v):label(l),value(v){};
	_valuePair(const char* l, const char* v):label(l),value(v){};
}valuePair;

// valuePair upButtonPair("Up", "Palettes");
// valuePair downButtonPair("Down", "Test");
// valuePair centerButtonPair("Center"," Effect");
// valuePair leftButtonPair("Left","");
// valuePair rightButtonPair("Right","");

const String proLabel("Program");
const String palLabel("Palettes");
const String testLabel("Test");

metaList  MaskView;
metaView	SecondView;
metaLabel ProgramLabel(&proLabel);
metaLabel PaletteLabel(&palLabel);
metaLabel TestLabel(&testLabel);

// metaValue UpButton(&upButtonPair.label,&upButtonPair.value);
// metaValue DownButton(&downButtonPair.label,&downButtonPair.value);
// metaValue CenterButton(&centerButtonPair.label, &centerButtonPair.value);
// metaValue LeftButton(&leftButtonPair.label, &leftButtonPair.value);
// metaValue RightButton(&rightButtonPair.label, &rightButtonPair.value);
// metaButton UpButton = metaButton();
// metaButton DownButton = metaButton();
// metaButton CenterButton = metaButton();
// metaButton LeftButton = metaButton();
// metaButton RightButton = metaButton();

void initMask()
{
	GCSize insets(2,2);
	ProgramLabel.setInsets(insets);
	ProgramLabel.setOutlineColor(ILI9341_PURPLE);

	ProgramLabel.initView(&tft,GCRect());
	ProgramLabel.setFont(&Arial_14);
	ProgramLabel.setTextSize(1);
	ProgramLabel.sizeToFit();

	PaletteLabel.setInsets(insets);
	PaletteLabel.setOutlineColor(ILI9341_PURPLE);

	PaletteLabel.initView(&tft,GCRect());
	PaletteLabel.setFont(&Arial_14);
	PaletteLabel.setTextSize(2);
	PaletteLabel.sizeToFit();

	TestLabel.setInsets(insets);
	TestLabel.setOutlineColor(ILI9341_PURPLE);

	TestLabel.initView(&tft,GCRect());
	TestLabel.setFont(&Arial_14);
	TestLabel.setTextSize(3);
	TestLabel.sizeToFit();

	MaskView.initView(&tft,GCRect(10,20,tft.width()/3,tft.height()-40));


	MaskView.setRespondsToEvents(UserEvent::EventMask::EncoderEvents | UserEvent::EventMask::ButtonEvents);
	MaskView.setDrawsOutline(true);
	MaskView.setCornerRadius(3);
	MaskView.setOutlineColor(ILI9341_RED);
	MaskView.addSubview(&ProgramLabel);
	MaskView.addSubview(&PaletteLabel);
	MaskView.addSubview(&TestLabel);
	MaskView.layoutList();

	MaskView.setOpaque(false);

	SecondView.initView(&tft,GCRect(tft.width()/3+10+2,20,(2*tft.width()/3)-20,tft.height()-40));
	SecondView.setDrawsOutline(true);
	SecondView.setCornerRadius(3);
	SecondView.setOutlineColor(ILI9341_NAVY);
	SecondView.setOpaque(false);
}

elapsedMillis firstTime = elapsedMillis(0);

elapsedMillis displayTimer ;
elapsedMillis ledTimer;

void drawMask(){
	//tft.fillScreen(ILI9341_BLACK);
	//MaskView.setNeedsRedraw();
	MaskView.redraw();
	SecondView.redraw();
	//
	// tft.setFontAdafruit();
	// tft.setTextSize(2);
	// UpButton.drawButton();
	// DownButton.drawButton();
	// CenterButton.drawButton();
	// LeftButton.drawButton();
	// RightButton.drawButton();
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

bool skipMask = false;
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

int processLEDEffects(unsigned long now,void* data)
{
	if(ledTimer > (1000/FRAMES_PER_SECOND)){
		patterns[currentPatternNumber]();
		FastLED.show();
		EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
		EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
	}
	return 0;
}

void adjustBrightness()
{
	int8_t uValue = log(256-tft.getLuminance())*10;
	tft.fillScreen(ILI9341_BLACK);
	String labelStr = String("Brightness");
	String valueStr = String("-55 ");
	String blubberStr = String ("-UU ");
	metaValue bla = metaValue();
	metaValue::LayoutDefinition brightnessTheme;
	brightnessTheme.labelFont = &Arial_16;
	brightnessTheme.valueFont = &Arial_40;
	brightnessTheme.labelOutlineCornerRadius = 10;
	brightnessTheme.labelOutlineInset = 8;
	brightnessTheme.labelDrawOutline=false;
	brightnessTheme.verticalValueInset=20;
	brightnessTheme.horizontalLabelInset=10;
	brightnessTheme.horizontalValueInset=30;
	brightnessTheme.valueColor = ILI9341_DARKGREEN;

	bla.setLayout(brightnessTheme);
	bla.initValue(&tft,GCRect(100,00,13,8), &labelStr, &valueStr);
	bla.sizeToFit();
	Serial << "Allign Now"<<endl;
	bla.allignInSuperView(HALLIGN_CENTER | VALLIGN_CENTER);
	valueStr.remove(0);
	valueStr+=String()+uValue;
	bla.redraw();

	static elapsedMillis lastAdjust = elapsedMillis(0);

	lastAdjust =0;

	do{
		taskQueue.Run(millis());
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
	}while(true/*lastAdjust<5000*/);
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

void processListEvents(metaList *list)
{
	while(eventQueue.length()){
		UserEvent *evnt = eventQueue.popEvent();
		uint16_t l = evnt->eventMask();
		uint16_t k = list->respondsToEvents();
		if((l&k) != 0){
			int16_t selIndex = list->processEvent(evnt);
			Serial << "SelectedIndex = "<<selIndex<<endl;
			if( selIndex >=0 ){
				list->redraw();
			}
		}
		delete evnt;
	}
}


void setup() {
	Serial.begin(115200);
	while(!Serial){}
	Serial << "Start"<<endl;
	Serial << "Effects: "<<numberOfPatterns<<endl;
	initializeLEDs();

	// init LED Backlight
	pinMode(TFT_LED,OUTPUT);
	digitalWriteFast(TFT_LED,1);

	Serial << "Init TFT"<<endl;
	initializeTFT();

	// enable UI
	enableSwitches();
	enableEncoders();

	// draw mask
	initMask();

	// initialize tasks
	taskQueue.scheduleFunction(processLEDEffects,NULL,"EFFC",0,1000/FRAMES_PER_SECOND);
}

void loop() {
	// put your main code here, to run repeatedly:
	if(firstTime>1000 && !skipMask){
		tft.fillScreen(ILI9341_BLACK);
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
		processListEvents(&MaskView);
		displayTimer = 0;
	}
	/** run all sequence tasks */
taskQueue.Run(millis());

}

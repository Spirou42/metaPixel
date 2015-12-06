#define FASTLED_INTERNAL
#include <stack>
#include <vector>
#include <tuple>
#include <list>
#include "Math.h"

#include "SPI.h"
#include "ILI9341_t3.h"
#include "Queue.h"
#include "Streaming.h"
#include "FastLED.h"

#include "UserEvent.h"
#include "metaTFTDisplay.h"
#include "Switch.h"

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

PaletteList initializeSystemPalettes(){
	PaletteList tmp;
	tmp.push_back(new PalettePair("Rainbow",RainbowColors_p));
	tmp.push_back(new PalettePair("Clouds",CloudColors_p));
	tmp.push_back(new PalettePair("Ocean",OceanColors_p));
	tmp.push_back(new PalettePair("Forest",ForestColors_p));
	tmp.push_back(new PalettePair("Party",PartyColors_p));
	tmp.push_back(new PalettePair("Rainbow Stripes",RainbowStripeColors_p));
	tmp.push_back(new PalettePair("Lava",LavaColors_p));
	tmp.push_back(new PalettePair("Heat",HeatColors_p));
	tmp.push_back(new PalettePair("Arctic",arctic_gp));
	tmp.push_back(new PalettePair("Temperature",temperature_gp));
	tmp.push_back(new PalettePair("Colombia",colombia_gp));
	tmp.push_back(new PalettePair("Cequal",cequal_gp));
	return tmp;
}

EffectList initializeSystemEffects(){
	EffectList tmp;
	tmp.push_back(new EffectPair("Rainbow",rainbow));
	tmp.push_back(new EffectPair("Rainbow Glitter",&rainbowWithGlitter));
	tmp.push_back(new EffectPair("Confetti",&confetti));
	tmp.push_back(new EffectPair("Sinelon",&sinelon));
	tmp.push_back(new EffectPair("Juggle",&juggle));
	tmp.push_back(new EffectPair("BPM",&bpm));
	return tmp;
}

PaletteList systemPalettes = initializeSystemPalettes();
PaletteList::iterator currentSystemPalette = systemPalettes.begin();

EffectList systemEffects = initializeSystemEffects();
EffectList::iterator currentSystemEffect = systemEffects.begin();

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

metaList  SelectMenu;
metaView	SecondView;

void initSelectMenu()
{
	// visual them definition for a single list entry
	GCSize insets(5,5);
	metaView::ViewLayout viewLayout;
	metaLabel::LabelLayout labelLayout;
	viewLayout.backgroundColor=ILI9341_BLACK;
	viewLayout.outlineColor=ILI9341_ORANGE;
	viewLayout.opaque=false;
	labelLayout.viewLayout = &viewLayout;
	labelLayout.font = &Arial_16;
	labelLayout.insets=insets;
	labelLayout.indicatorSpace = 14;
	labelLayout.indicatorSize = GCSize(6,6);
	labelLayout.textSize=1;
	labelLayout.textColor=ILI9341_GREEN;
	viewLayout.visualizeState=true;


	SelectMenu.initView(&tft,GCRect(2,12,tft.width()/2,tft.height()-4));
	SelectMenu.setBorderInset(GCSize(15,5));
	SelectMenu.setLabelLayout(&labelLayout);
	SelectMenu.setRespondsToEvents(EventMask::EncoderEvents | EventMask::ButtonEvents |
		 EventMask::ButtonEvent_Down |
		 EventMask::ButtonEvent_Up |
		 EventMask::ButtonEvent_Center | EventMask::ButtonState_All);
	SelectMenu.setDrawsOutline(true);
	SelectMenu.setCornerRadius(3);
	SelectMenu.setOutlineColor(ILI9341_RED);
	SelectMenu.setOpaque(false);

	SelectMenu.addEntry( String("Brightness"));
	SelectMenu.addEntry( String("Program"));
	SelectMenu.addEntry( String("Pallette"));
	SelectMenu.addEntry( String("Test"));
	SelectMenu.addEntry( String("Tast"));
	SelectMenu.layoutList();
	SelectMenu.sizeToFit();

	GCPoint selO = SelectMenu.getOrigin();
	GCSize 	selS = SelectMenu.getSize();
	int seVx = selO.x+selS.w + 2;
	SecondView.initView(&tft,GCRect(seVx, selO.y,
	tft.width()-seVx-selO.x,selS.h));
	SecondView.setDrawsOutline(true);
	SecondView.setCornerRadius(3);
	SecondView.setOutlineColor(ILI9341_NAVY);
	SecondView.setOpaque(false);
}

elapsedMillis firstTime = elapsedMillis(0);

elapsedMillis displayTimer ;
elapsedMillis ledTimer;






int processLEDEffects(unsigned long now,void* data)
{
	if(ledTimer > (1000/FRAMES_PER_SECOND)){
		EffectPair *l = *currentSystemEffect;
		effectHandler h = l->second;
		h();
		//patterns[currentPatternNumber]();
		FastLED.show();
		EVERY_N_MILLISECONDS( 20 ) { gHue--; } // slowly cycle the "base color" through the rainbow
		EVERY_N_SECONDS( 120 ) { nextPattern(); } // change patterns periodically
		EVERY_N_SECONDS(30){nextPalette();}
	}
	return 0;
}

#if 0
void adjustBrightness()
{
	int8_t uValue = log(256-tft.getLuminance())*10;
	tft.fillScreen(ILI9341_BLACK);
	String labelStr = String("Brightness");
	String valueStr = String("-55 ");
	String blubberStr = String ("-UU ");
	metaValue bla = metaValue();
	metaValue::ValueLayout brightnessTheme;
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
	bla.initValue(&tft,GCRect(100,00,13,8), labelStr, valueStr);
	bla.sizeToFit();
	Serial << "Allign Now"<<endl;
	bla.allignInSuperView(HALLIGN_CENTER | VALLIGN_CENTER);
	valueStr.remove(0);
	valueStr+=String()+uValue;
	bla.setValue(valueStr);
	bla.redraw();

	static elapsedMillis lastAdjust = elapsedMillis(0);

	lastAdjust =0;

	do{
		taskQueue.Run(millis());
		if(eventQueue.length()){
			int8_t kValue = uValue;
			UserEvent *evnt = eventQueue.popEvent();
			if(evnt->getType()==EventType::EventTypeButton){
				UserEvent::ButtonData data = evnt->getData().buttonData;
				if(data.id==ButtonID::CenterButton &&
					data.state == ButtonState::ButtonClick){
					break;
				}
				if(data.id == ButtonID::UpButton &&
					(data.state == ButtonState::ButtonClick ||
						data.state == ButtonState::ButtonLongClick)){
					uValue = 55;
					lastAdjust=0;
				}
				if(data.id == ButtonID::DownButton &&
					(data.state == ButtonState::ButtonClick ||
						data.state == ButtonState::ButtonLongClick)){
					uValue = 0;
					lastAdjust=0;
				}

			}else if(evnt->getType() ==EventType::EventTypeEncoder){
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
				Serial << "ValueStr: "<<valueStr<<endl;
				bla.setValue(valueStr);
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
#endif


/** todo: re-move this functionalityto a generic implementation */
void processListEvents(metaList *list)
{
	// if(eventQueue.length()){
	// 	vector<metaView*>::iterator iter = list->_subViews.begin();
	// 	Serial << "SubViews: "<<list->_subViews.size()<<endl;
	// 	while(iter != list->_subViews.end()){
	// 		Serial << "SubView: "<<_HEX((long int)(*iter))<<endl;
	// 		iter ++;
	// 	}
	// }
	while(eventQueue.length()){
		UserEvent *evnt = eventQueue.popEvent();
		//uint16_t l = evnt->eventMask();
		uint16_t k = list->respondsToEvents();
		int16_t oldSelectedElement =list->selectedIndex();
		if((evnt->matchesMask(k)) ){
			int16_t result = list->processEvent(evnt);
			if(result > ResponderResult::ChangedSelect){
				Serial << "List changed select x = "<<result<<endl;
				list->redraw();
			}else{
				switch(result){
					case ResponderResult::ChangedNothing:
					Serial << "List did not change"<< endl;
					break;

					case ResponderResult::ChangedVisual:
					Serial << "List changed visualy"<< endl;
					list->redraw();
					break;

					case ResponderResult::ChangedState:
					{
						int16_t idx =list->activeIndex();
						Serial << "List changed state selected "<<idx<<endl;
						list->redraw();
					}
					break;

					case ResponderResult::ResponderExit:
					Serial << "List will exit"<< endl;
					break;

				}
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
	initSelectMenu();
	Serial << "Installed Palettes: ("<<systemPalettes.size()<<")"<<endl;
	PaletteList::iterator iter = systemPalettes.begin();
	while(iter != systemPalettes.end()){
		PalettePair *k = *iter;
		Serial << "Name: "<< k->first<<endl;
		iter ++;
	}
	Serial << "Current: "<<(*currentSystemPalette)->first<<endl;

	Serial << "Installed Effects: ("<<systemEffects.size()<<")"<<endl;
	EffectList::iterator eiter = systemEffects.begin();
	while(eiter != systemEffects.end()){
		EffectPair *k = *eiter;
		Serial << "Name: "<<k->first<<endl;
		eiter ++;
	}
	// initialize tasks
	taskQueue.scheduleFunction(processLEDEffects,NULL,"EFFC",0,1000/FRAMES_PER_SECOND);
}

bool skipMask = false;
void loop() {
	// put your main code here, to run repeatedly:
	if(firstTime>1000 && !skipMask){
		tft.fillScreen(ILI9341_BLACK);
		SelectMenu.redraw();
		SecondView.redraw();

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
		processListEvents(&SelectMenu);
		displayTimer = 0;
	}
	/** run all sequence tasks */
taskQueue.Run(millis());

}

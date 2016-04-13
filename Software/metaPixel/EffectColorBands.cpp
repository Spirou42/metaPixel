/**
* EffectPlasmaSimple
* I800 R2 U30
*/

#include "EffectColorBands.h"
#include "Arduino.h"
#include "Streaming.h"
#include "VT100Stream.h"


void EffectColorBands::startEffect()
{
  Serial << "SimplePlasma Init Effect"<<endl;
  *(windowScale->value) = 1;
  windowScale->value->_shouldAnimate=false;
  *(hueScale->value) = 100;
  *(plasmaSpeed->value) = 15;
  *(mirrorMask->value) = 0;
  setMaxValueFor(windowScale,1000);
  setMaxValueFor(hueScale,1000);
  setMaxValueFor(plasmaSpeed,255);
  setMaxValueFor(mirrorMask,1);
	//display.fill(CRGB::Black);
}

void EffectColorBands::frame(unsigned long now)
{
  static uint32_t frame = 0;

#if DEBUG_EFFECTS
	static long lastCall = 0;
	Serial << now-lastCall<<endl;
	lastCall = now;
#endif

	uint8_t w = display.displayWidth() * windowScale->value->currentValue();
	uint8_t h = display.displayHeight() * windowScale->value->currentValue();


	int32_t yHueDelta32 = ((int32_t)cos16( frame * (27/1) ) * (hueScale->value->currentValue() / w));
	int32_t xHueDelta32 = -((int32_t)sin16( frame * (39/1) ) * (hueScale->value->currentValue() / h));
	DrawOneFrame( frame / 0xffff, yHueDelta32 / 32768, xHueDelta32 / 32768);
  display.flush();
	frame +=plasmaSpeed->value->currentValue();
#if DEBUG_EFFECTS
	Serial << "Took: "<<(millis()-lastCall)<<" millisec "<<frame<<endl;
#endif

}

void EffectColorBands::DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
  byte lineStartHue = startHue8;
	uint8_t h= display.displayHeight()/(mirrorMask->value->currentValue()?2:1);
	uint8_t w= display.displayWidth()/(mirrorMask->value->currentValue()?2:1);

	for( byte y = 0; y < h; y++) {
		lineStartHue += yHueDelta8;
		byte pixelHue = lineStartHue;
		for( byte x = 0; x < w; x++) {
			pixelHue += xHueDelta8;
			//      uint16_t pixelInBuffer = display.XY(x,y);
			CRGB color = ColorFromPalette(colorPalettes[Palette.currentValue()],pixelHue);
			display.setPixel(MPPixel(x,y),color);

			if(mirrorMask->value->currentValue()){
				display.setPixel(x,display.displayHeight()-y-1,color);
				display.setPixel(x,display.displayHeight()-y-1,color);
				display.setPixel(display.displayWidth()-x-1,y,color);
				display.setPixel(display.displayWidth()-x-1,display.displayHeight()-y-1,color);
			}
		}
	}
	FastLED.show();
}

void EffectColorBands::printParameter(Print& stream){
  stream << "wScale: "<<*windowScale<<" \thScale: "<<*hueScale<<" \tpSpeed: "<<*plasmaSpeed<<" \tmMask: "<<*mirrorMask<<endl;
}

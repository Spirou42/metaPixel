/**
* EffectFire.cpp
* O70 H150 U80 D60 d60 Z12000
*/

#include "EffectFire.hpp"
#define COOLING 250
#define SPARKLES 10
void EffectFire::startEffect()
{
  setMaxValueFor(bottomHeating,255);
  setMaxValueFor(topCooling,255);
  setMaxValueFor(sparcleFreq,255);
  bottomHeating->value->initTo(200);
  topCooling->value->initTo(255);
  //bottomHeating->value->animateTo(10,300000);
  sparcleFreq->value->initTo(10);
  display.fill(CRGB::Black);
  for(int i=0;i<NUM_LEDS;i++){
    heatCells[i]=0;
  }
  BlendParam.initTo(12000);
  _initPalette = Palette.currentValue();
  Palette.initTo(7);
  Delay = 60;
}

void EffectFire::frame(unsigned long now)
{
  static int8_t sparkleOffset = 4;
  static uint8_t sparkleSpan = 12;
  //	static unsigned long lastNow = 0;
  //	static bool dir = true;

  // Step 1.  Cool down every cell a little
  // this works without mapping from XY coordinates to cells
  for(uint8_t y=0;y<display.displayHeight();y++){
    for(uint8_t x=0;x<display.displayWidth();x++){
      uint16_t offset = display.XY(x,y);
      //			uint8_t coolBase = ((DISPLAY_HEIGHT+y)*genericSpeed1.currentValue())/10;
      uint8_t cooling = random8((y)*2,topCooling->value->currentValue());// / display.displayHeight()) + 2);
      #if DEBUG_EFFECTS
      Serial <<"Cooling ("<<x<<", "<<y<<") "<<cooling<<endl;
      #endif
      heatCells[offset] =qsub8(heatCells[offset],cooling);
    }
  }
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  // for( int k= NUM_LEDS - 1; k >= 2; k--) {
  //   		heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  // }
  for(int y=0;y<display.displayHeight();y++){
    for(int x=0;x<display.displayWidth();x++){
      uint16_t offset = display.XY(x,y);

      int16_t h = 0;
      //				byte div = 0;

      h = neightbours(x,y);
      heatCells[offset]+=h;
    }
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if( random8() <= sparcleFreq->value->currentValue() ) {
    int x = random8(sparkleSpan)+sparkleOffset;
    uint16_t offset = display.XY(x,0);
    heatCells[offset] = qadd8( heatCells[offset], random8(200,255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for (int y = 0;y < display.displayHeight();y++) {
    for (int x = 0; x < display.displayWidth(); x++) {
      uint16_t offset = display.XY(x,y);
      int k = heatCells[offset];
      if(k>200){
        k=200;
      }
      display.setPixel(x,y,ColorFromPalette(colorPalettes[Palette.currentValue()],k));
    }
  }
  // 	for( int j = 0; j < NUM_LEDS; j++) {
  // 		int k=heatCells[j];
  // 		if (k>200){
  // 			k=200;
  // 		}
  // 		// if(k<30){
  // 		// 	k=0;
  // 		// }
  // #if USE_DOUBLE_BUFFER
  // 		led_backbuffer[j] = ColorFromPalette(colorPalettes[Palette.currentValue()],k);
  // #else
  // 		leds[j] = ColorFromPalette(colorPalettes[Palette.currentValue()],k);
  // #endif
  // 	}
  display.flush();
}

int16_t EffectFire::heatAt(int16_t x,int16_t y)
{
  if( (x<0) || (x > DISPLAY_WIDTH-1)){
    return -20;
  }
  if(y<0)
  return bottomHeating->value->currentValue();
  if( (y<0) || (y > DISPLAY_HEIGHT-1)){
    return -20;
  }
  return heatCells[display.XY(x,y)];
}

int16_t EffectFire::neightbours(int16_t x, int16_t y)
{
  int16_t result = 0;
  int16_t div=0;
  result += heatAt(x,y-1)*3.0/2.0;div++;
  result += heatAt(x-1,y-1)/3.0;div++;
  result += heatAt(x+1,y-1)/3.0;div++;
  //	result += heatAt(x-1,y+1)/6;div++;
  //	result += heatAt(x+1,y+1)/6;div++;
  //	result += heatAt(x,y)/10;div++;
  if(random8(100)<10){
    //		result += heatAt(x+1,y)/2;div++;
  }
  if(random8(100)>90){
    //		result += heatAt(x-1,y)/2;div++;
  }
  if (result <0){
    result =0;
  }
  return result / div;
}
void EffectFire::stopEffect()
{
  Palette.initTo(_initPalette);
  bottomHeating->value->initTo(0);
}
void EffectFire::printParameter(Print& stream)
{
  stream << "Bottom Heat: "<<*bottomHeating<<" \tTop Cool: "<<*topCooling<<" \tSparcles: "<<*sparcleFreq<<endl;
}

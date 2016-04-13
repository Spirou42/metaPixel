#include "Effect.h"
#include "EffectWater.h"
#include <Arduino.h>

void EffectWater::startEffect(){
  Serial<<"Water start"<<endl;
  display.fill(CRGB::Blue);
  setMaxValueFor(damping,100);
  damping->value->initTo(86);
  for(int x = 0;x<DISPLAY_WIDTH;++x){
    for(int y=0;y<DISPLAY_HEIGHT;++y){
      buffer1[display.XY(x,y)] = 0;
      buffer2[display.XY(x,y)] = 0;
    }
  }
  b1 = buffer1;
  b2 = buffer2;
  b1[display.XY(8,8)]=1000;
  _initPalette = Palette.currentValue();
  Palette.initTo(11);
  Serial << "Water started"<<endl;
}

int16_t sumNeighbours(int16_t *source,int16_t x, int16_t y)
{
    int16_t result = 0, nums=0;
    if(y>=1) result += source[display.XY(x,y-1)]; nums ++;
    if(y<display.displayHeight()-1) result += source[display.XY(x,y+1)];nums ++;
    if(x>1) result += source[display.XY(x-1,y)];nums ++;
    if(x<display.displayWidth()-1) result += source[display.XY(x+1,y)];nums ++;
    result /=nums;
    return result;
}

void EffectWater::ProcessWater(int16_t *source, int16_t *dest)
{
  //Serial << "Water process"<<endl;
  for(size_t x=0;x<DISPLAY_WIDTH;++x){
    for(size_t y=0;y<DISPLAY_HEIGHT;++y){
      size_t offset = display.XY(x,y);
      dest[offset] = sumNeighbours(source,x,y) - dest[display.XY(x,y)];
      dest[offset] = (float)dest[offset]*((float)damping->value->currentValue()/100.0);
    }
  }
  //Serial << "Water processed"<<endl;
}
void swap(int16_t **t1, int16_t **t2){
  int16_t *k = *t1;
  *t1 = *t2;
  *t2 = k;
}
void EffectWater::dropAdrop(int16_t *data){
  if( random8() <= 40) {
    //Serial<< "Drop"<<endl;
    int x = random8(display.displayWidth()-2)+1;
    int y = random8(display.displayHeight()-2)+1;
    uint16_t offset = display.XY(x,y);
    data[offset] = 500;
  }
}

void EffectWater::frame(unsigned long now){
  //Serial << "Water Frame"<<endl;
  dropAdrop(b1);
  ProcessWater(b1,b2);
  CRGB t = CRGB::Blue;

  for(int x=0;x<DISPLAY_WIDTH;++x){
    for(int y=0;y<DISPLAY_HEIGHT;++y){
      CHSV c = rgb2hsv(CRGB::Blue);
      c.value = 100;
      int16_t k = b2[display.XY(x,y)];
      if(k>0)
        c.saturation = 255 - k;
      if(k<0)
        c.value +=k;
      //CRGB color = ColorFromPalette(colorPalettes[Palette.currentValue()],k);

      //Serial << k<<" ";
      display.setPixel(MPPixel(x,y),c);
    }
    //Serial << endl;
  }
  //Serial << endl;
  swap(&b1,&b2);

  FastLED.show();
}

void EffectWater::stopEffect(){
  Palette.initTo(_initPalette);
}

void EffectWater::printParameter(Print& stream){

}

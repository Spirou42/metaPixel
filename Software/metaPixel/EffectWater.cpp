
#include "Effect.hpp"
#include "EffectWater.hpp"
#include <Arduino.h>

void EffectWater::startEffect(){
  Serial<<"Water start"<<endl;
  display.fill(CRGB::Blue);
  setMaxValueFor(damping,100);
  setMaxValueFor(hueBase,255);
  setMaxValueFor(dropTime,10000);
  setMaxValueFor(dropStrength,1000);
  setMaxValueFor(modeMask,2);
  initValueFor(damping,25);
  initValueFor(hueBase,0);
  initValueFor(dropTime,1000);
  initValueFor(dropStrength,50);
  initValueFor(modeMask,2);

  dropTimer = 0;
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
  Palette.initTo(8);
  BlendParam.initTo(5);
  Serial << "Water started"<<endl;
}
int16_t EffectWater::valueAt(int16_t* source, int16_t x,int16_t y)
{
  if( (x<0) || (x > DISPLAY_WIDTH-1)){
    return 0;
  }
  if( (y<0) || (y > DISPLAY_HEIGHT-1)){
    return 0;
  }
  return source[display.XY(x,y)];
}

int16_t EffectWater::sumNeighbours(int16_t *source,int16_t x, int16_t y)
{
    int16_t result = 0, nums=0;
    for(int i=- 1;i<=1;i++){
      if(i == 0) continue;
      result += valueAt(source,x,y+i);nums++;
      result += valueAt(source,x+i,y);nums++;
    }
    result /=(nums>>1);
    return result;
}

void EffectWater::ProcessWater(int16_t *source, int16_t *dest)
{
  //Serial << "Water process"<<endl;
  for(size_t x=0;x<DISPLAY_WIDTH;++x){
    for(size_t y=0;y<DISPLAY_HEIGHT;++y){
      size_t offset = display.XY(x,y);
      dest[offset] = sumNeighbours(source,x,y) - dest[display.XY(x,y)];
      dest[offset] = (float)dest[offset]*((100.0-(float)damping->value->currentValue())/100.0);
    }
  }
  //Serial << "Water processed"<<endl;
}

void swapBuffer(int16_t **t1, int16_t **t2){
  int16_t *k = *t1;
  *t1 = *t2;
  *t2 = k;
}

void EffectWater::dropAdrop(int16_t *data){
//  if( random8() <= 40) {
  if(dropTimer>getValueFor(dropTime)){
    dropTimer = 0;
    int x = random8(display.displayWidth()-2)+1;
    int y = random8(display.displayHeight()-2)+1;
    int t = 10*getValueFor(dropStrength);

    uint16_t offset = display.XY(x,y);
    data[offset] = t;
    offset = display.XY(x+1,y);
    if(offset < NUM_LEDS){
      data[offset] = t;
    }
    offset = display.XY(x+1,y+1);
    if(offset < NUM_LEDS){
      data[offset] = t;
    }

  }
}

void EffectWater::frame(unsigned long now){
  //Serial << "Water Frame"<<endl;
  dropAdrop(b1);
  ProcessWater(b1,b2);
  CRGB t = CRGB::Blue;

  int16_t mode = getValueFor(modeMask);
  for(int x=0;x<DISPLAY_WIDTH;++x){
    for(int y=0;y<DISPLAY_HEIGHT;++y){
      int16_t k = b2[display.XY(x,y)];

      CHSV c = (mode & 0x01)?CHSV(getValueFor(hueBase),255,255):rgb2hsv(CRGB::Blue);
      if( mode & 0x02){
        uint8_t l = k&0xff;
        c = rgb2hsv(ColorFromPalette(colorPalettes[Palette.currentValue()],l+getValueFor(hueBase)));
      }else{
        c.value = 127;
        if(k>0){
          if(k>255){
              c.saturation = 0;
          }else{
            c.saturation = 255 - k;
          }
        }
        if(k<0){
          if(abs(k)>c.value){
            c.value = 0;
          }else{
            c.value +=k;
          }
        }
      }

      //CRGB color = ColorFromPalette(colorPalettes[Palette.currentValue()],k);

      //Serial << k<<" ";
      display.setPixel(MPPixel(x,y),c);
    }
    //Serial << endl;
  }
  //Serial << endl;
  swapBuffer(&b1,&b2);

  display.flush();
}

void EffectWater::stopEffect(){
  Palette.initTo(_initPalette);
  Brightness.initTo(160);
}

void EffectWater::printParameter(Print& stream){

}

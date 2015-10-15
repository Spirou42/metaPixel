/**
* EffectWhitney.cpp
* D60 C0 B160 Q0 Z12000 U180
*/

#include "EffectWhitney.h"
#include "metaPixel.h"

void EffectWhitney::startEffect()
{
  display.clear();
  millisSinceStart = 0;
  frameCounter = 0;
  setMaxValueFor(cycleLength,20000);
  setMaxValueFor(lumenScale,100);
  setMaxValueFor(hueSpeed,255);
  hueSpeed->value->initTo(2);
  lumenScale->value->initTo(2);
  *(cycleLength->value) = 60*3;
}

void EffectWhitney::stopEffect()
{

}

void EffectWhitney::frame(unsigned long now)
{
  uint16_t numberOfPixels = display.displayWidth()*display.displayHeight();
  //Serial <<clearLineRight<<"W:"<<display.displayWidth()<<" h:"<<display.displayHeight()<<endl;
  speed = (2*PI*numberOfPixels)/cycleLength->value->currentValue();
  float timer = (millisSinceStart / 1000.0)*speed;
  //float crad = (min(width,height)/2) * 0.95;
  float crad = 1;   //(20/2)*0.95;
  for (uint16_t i = 0; i < numberOfPixels; i++) {
    float r = 1 - i/ ((float)numberOfPixels *1.0);
    float a,len,rad;
    a = timer * r;
    //len = i*crad/(float) numberOfPixels;
    //rad = max(1,len*0.25);
    float lScale = (lumenScale->value->currentValue()/50.0)-1.0;
    float lum = (sin(a)+1.0) / 2 ;
    float hue = (r+timer / (256.0-hueSpeed->value->currentValue())/* *0.001 hueSpeed*/);
    uint8_t h =((hue-int(hue))*255);

    float x = (i%display.displayWidth());
    float y = (i/display.displayWidth());
    //Serial << "Lumen: "<<lum<<" lScale:"<<lScale<<endl;
    CRGB c = ColorFromPalette(colorPalettes[Palette.currentValue()],h);
    CHSV hC = rgb2hsv(c);
    float k = lum*255;
    while(k>255){
      k-=255;
    }

    hC.v= (k);
    display.setPixel(x,y,hC);
  }
  frameCounter++;
}

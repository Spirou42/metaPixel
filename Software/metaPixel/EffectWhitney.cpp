/**
* EffectWhitney.cpp
* D60 C0 B160 Q0 Z12000 U180

sequence
Q0Z1U180R400V1&20 #r400 200 20 #z1 5 60 %r #r200 30 60%r #z5 14 60 #r30 75 30 %z#z14 1 60#r75 400 60 %z
Q4Z1U180R400V1&20 #r400 200 20 #z1 5 60 %r #r200 30 60%r #z5 14 60 #r30 75 30 %z#z14 1 60#r75 400 60 %z

Q0Z1U180R400V1
Q0Z14U180R75V0
Q4Z5U180R60V0


*/

#include "EffectWhitney.h"
#include "metaPixel.h"

void EffectWhitney::startEffect()
{
  //display.clear();
  millisSinceStart = 0;
  frameCounter = 0;
  setMaxValueFor(cycleLength,20000);
  setMaxValueFor(lumenScale,400);
  setMaxValueFor(hueSpeed,190);
  hueSpeed->value->initTo(2);
  lumenScale->value->initTo(75);
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
  double timer = (millisSinceStart / 1000.0)*speed;
  //float crad = (min(width,height)/2) * 0.95;
  //float crad = 1;   //(20/2)*0.95;
  for (uint16_t i = 0; i < numberOfPixels; i++) {
    float r = 1 - i/ ((float)numberOfPixels *1.0);
    float a;
    a = timer * r;
    //len = i*crad/(float) numberOfPixels;
    //rad = max(1,len*0.25);
    float lScale = (lumenScale->value->currentValue()/50.0)-1.0;
    float hSpeed =((200-hueSpeed->value->currentValue()) -100);
    float lum = (sin(a)+lScale) / (1.0+lScale) ;
    float hSpeedT = (100.0 + hSpeed);
    if(hSpeedT == 0.0){
      hSpeedT = 0.0;
    }else{
      hSpeedT = 1/hSpeedT;
    }
    float hueStep = 0.0;
    hueStep = (hueSpeed->value->currentValue() == 0) ? 0.0: (timer * hSpeedT);
    float hue = (r + hueStep);// (256.0-hueSpeed->value->currentValue())/* *0.001 hueSpeed*/);
    uint8_t h =((hue-int(hue))*255);

    float x = (i%display.displayWidth());
    float y = (i/display.displayWidth());
    // if(i==0){
    //   Serial << ScreenPos(15,1)<<clearLineRight<<" hStep:"<<hueStep<<" ("<<hSpeedT<<")"<<endl;
    //   if (hSpeedT != 0.0) {
    //     Serial << "Ba "<<(1-hSpeedT)<<endl;
    //   }
    //   Serial.print(hSpeedT);
    // }
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

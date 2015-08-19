/**
* EffectWaterfall.cpp
* FFT waterfall on display enegie translated to hueSpeed
* update is current delay
* no parameters
*/
#include "EffectWaterfall.h"
#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "metaPixel.h"

// GUItool: begin automatically generated code
AudioInputI2S            audioIn;           //xy=109,65
AudioSynthWaveformSine sinewave;
AudioAnalyzeFFT1024       analyzeFFT;       //xy=408,57
AudioConnection          patchCord1(audioIn, 0, analyzeFFT, 0);
//AudioConnection         patchCord1(sinewave, 0, analyzeFFT, 0);
AudioControlSGTL5000     AudioShield;     //xy=247,216

// GUItool: end automatically generated code

void EffectWaterfall::startEffect()
{
  Serial << "Init waterfall"<<endl;
  audioIn.setActive(true);
  analyzeFFT.setActive(true);
  sinewave.amplitude(1.0);
  sinewave.frequency(8000);
  analyzeFFT.windowFunction(AudioWindowHanning1024);
  analyzeFFT.averageTogether(20);
  Palette=7;
  display.clearAll();
}

void EffectWaterfall::collectBins()
{
  binCollection[0] =  analyzeFFT.read(0) * 255;           // 1
  binCollection[1] =  analyzeFFT.read(1)* 255;            // 1
  binCollection[2] =  analyzeFFT.read(2, 3)* 255;         // 2
  binCollection[3] =  analyzeFFT.read(4, 6)* 255;         // 3
  binCollection[4] =  analyzeFFT.read(7, 10)* 255;        // 4
  binCollection[5] =  analyzeFFT.read(11, 15)* 255;       // 5
  binCollection[6] =  analyzeFFT.read(16, 22)* 255;       // 7
  binCollection[7] =  analyzeFFT.read(23, 32)* 255;       // 10
  binCollection[8] =  analyzeFFT.read(33, 46)* 255;       // 14
  binCollection[9] =  analyzeFFT.read(47, 66)* 255;       // 20
  binCollection[10] = analyzeFFT.read(67, 93)* 255;       // 27
  binCollection[11] = analyzeFFT.read(94, 131)* 255;      // 38
  binCollection[12] = analyzeFFT.read(132, 184)* 255;     // 53
  binCollection[13] = analyzeFFT.read(185, 257)* 255;     // 73
  binCollection[14] = analyzeFFT.read(258, 359)* 255;     // 102
  binCollection[15] = analyzeFFT.read(360, 511)* 255;     // 152
}

void EffectWaterfall::frame(unsigned long now)
{
  static float frequency =500.0;
  static float frequencyStep = 500.0;
  if(analyzeFFT.available()){
    // collect bins
    collectBins();
    // for(uint16_t i=0; i<display.displayWidth();i++){
    //
    //   binCollection[i] = analyzeFFT.read(i*6,6)*255.0;
    //   Serial << binCollection[i]<<", ";
    // }
    //Serial << endl;
    display.scrollDown(1,true);
    for(uint16_t i=0;i<display.displayWidth();i++){
      CRGB color = ColorFromPalette(colorPalettes[Palette.currentValue()],binCollection[i]);
      display.setPixel(i,display.displayHeight()-1,color);
    }

    display.flush();
    frequency+=frequencyStep;
    if((frequency>20000) || (frequency<500.0)){
      frequencyStep =-frequencyStep;
    }
    AudioNoInterrupts();
    sinewave.frequency(frequency);
    AudioInterrupts();
  }
}

void EffectWaterfall::stopEffect()
{
  audioIn.setActive(false);
  analyzeFFT.setActive(false);
}

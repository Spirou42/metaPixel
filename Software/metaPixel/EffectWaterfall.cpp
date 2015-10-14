/**
* EffectWaterfall.cpp
* FFT waterfall on display enegie translated to hueSpeed
* update is current delay
* no parameters
*/
#include "metaPixel.h"
#if USE_AUDIO_EFFECTS
#include "EffectWaterfall.h"
#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#include "Math.h"

// GUItool: begin automatically generated code
AudioInputI2S            audioIn;           //xy=109,65
AudioAnalyzeFFT1024       analyzeFFT;       //xy=408,57
AudioConnection          patchCord1(audioIn, 0, analyzeFFT, 0);
//AudioConnection           patchCord1(waveform, 0, analyzeFFT, 0);
//AudioConnection           patchCord2(waveform,0,audioOut,0);

AudioControlSGTL5000     AudioShield;     //xy=247,216

// GUItool: end automatically generated code

void EffectWaterfall::startEffect()
{
  Serial << "Init waterfall"<<endl;
//  waveform.begin(WAVEFORM_SINE);
  audioIn.setActive(true);
  analyzeFFT.setActive(true);
  analyzeFFT.windowFunction(AudioWindowHanning1024);
  analyzeFFT.averageTogether(20);
  Palette=7;
  display.clearAll();
  AudioShield.unmuteHeadphone();
}

void EffectWaterfall::collectBins()
{
  uint16_t binCount1024[]={1,1,2,2,3,3,4,4,5,6,7,10,14,20,27,38,53,73,102,137};
  uint16_t currentBin=0;
  float p=0;
  for (size_t i = 0;  i < (sizeof(binCount1024)/sizeof(uint16_t)) ; ++i) {
    uint16_t k = binCount1024[i];
    if (k==1) {
      p=analyzeFFT.read(currentBin);
      currentBin+=k;
    }else if(k>1){
      uint16_t stopBin = currentBin+(k-1);
      p = analyzeFFT.read(currentBin,stopBin);
      currentBin+=k;
    }else{
    }
    binCollection[i]=255.0*(log(10*p)+1.0);
  }
}

void EffectWaterfall::frame(unsigned long now)
{
  static float frequency =200.0;
  static float frequencyStep = 10.0;
  if(analyzeFFT.available()){
    // collect bins
    collectBins();
    display.scrollDown(1,true);
    for(uint16_t i=0;i<display.displayWidth();i++){
      CRGB color = ColorFromPalette(colorPalettes[Palette.currentValue()],binCollection[i]);
      display.setPixel(i,display.displayHeight()-1,color);
    }
    display.flush();
    frequency+=frequencyStep;
    if((frequency>18000) || (frequency<10.0)){
      frequencyStep =-frequencyStep;
    }
  }
}

void EffectWaterfall::stopEffect()
{
  audioIn.setActive(false);
  analyzeFFT.setActive(false);
  AudioShield.muteHeadphone();
}

#endif

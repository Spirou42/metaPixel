/**
* EffectLine.cpp
*/

#include "EffectLine.h"
#include "Arduino.h"
#include "Streaming.h"

void EffectLine::initializeEffect()
{
  display.clear();
}

void EffectLine::runEffect(unsigned long now)
{
  static MPPixel start =MPPixel(0,0),end=MPPixel(display.displayWidth()-1,display.displayHeight()-1);
//  static unsigned long lastNow;
  static uint8_t currentColor = 0;
  static bool horizontal = true;

  CRGB color = ColorFromPalette(colorPalettes[Palette.currentValue()],currentColor++);
  display.line(start,end,color);
  if (true/*(now - lastNow) > 200*/){
//    lastNow = now;

    if (horizontal){
      start.x +=1;
      end.x -= 1;
      if(start.x>display.displayWidth()-1){
        start.x = (display.displayWidth()-1);
        end.x = 0;
        horizontal = false;
      }
    }else{
      start.y++;
      end.y--;
      if(start.y>(display.displayHeight()-1)){
        start =MPPixel(0,0);
        end = MPPixel(display.displayWidth()-1,display.displayHeight()-1);
        horizontal = true;
//				currentColor +=15;

      }
    }


  }
#if !USE_DOUBLE_BUFFER
  FastLED.show();
#endif
}

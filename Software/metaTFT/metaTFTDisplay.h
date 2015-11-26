/**
* definition for metaPixel TFT interface
*/

#ifndef __METATFT_H__
#define __METATFT_H__
#define FASTLED_INTERNAL
#include "SPI.h"
#include "ILI9341_t3.h"
#include "FastLED.h"
#include "Streaming.h"

class metaTFT : public ILI9341_t3 {
public:
  metaTFT(uint8_t _CS, uint8_t _DC, uint8_t _RST = 255, uint8_t _MOSI=11, uint8_t _SCLK=13, uint8_t _MISO=12, uint8_t bkg_pin=A14,uint8_t rotation = 3,boolean hasAnalogBackLg=false):
  ILI9341_t3(_CS, _DC, _RST, _MOSI, _SCLK, _MISO),_backlight_pin(bkg_pin),defaultRotation(rotation),_isBacklightAnalog(hasAnalogBackLg),_luminance(80){};
  void start();

  void setLuminance(uint8_t val){_luminance = val;updateBacklight();};
  uint8_t getLuminance(){return _luminance;};
  virtual void drawLogo();

  uint16_t stringWidth(const char* str);
  uint16_t stringHeight(const char* str);
protected:
  int16_t   TFT_LogoEnd = 0;
  uint8_t _backlight_pin;
  uint8_t defaultRotation ;
  boolean _isBacklightAnalog;

  uint8_t _luminance ;                    //< valaue for backlight

  void updateBacklight();
  typedef struct _charDim {
    uint32_t width, height;
  } charDimentions;

  charDimentions fontCharDimentions(unsigned int c);

};

#endif

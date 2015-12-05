/**
* definition for metaPixel TFT interface
*/
#include "metaTFTDisplay.h"
#include "Streaming.h"
//#include "UIHelpers.h"  ///<< i need the metaView definition
void metaTFT::start()
{
  this->begin();
  //delay(1000);
  this->setRotation(defaultRotation);
  this->fillScreen(ILI9341_BLACK);
  this->setTextWrap(true);
  this->updateBacklight();
  drawLogo();

  delay(800);
}

void metaTFT::updateBacklight()
{
  if(_luminance==255){
    Serial << "Ping"<<endl;
    pinMode(_backlight_pin,OUTPUT);
    digitalWriteFast(_backlight_pin,1);
  }else{
    analogWrite(_backlight_pin,_luminance);
  }
}
static uint32_t fetchbits_unsigned(const uint8_t *p, uint32_t index, uint32_t required)
{
	uint32_t val = 0;
	do {
		uint8_t b = p[index >> 3];
		uint32_t avail = 8 - (index & 7);
		if (avail <= required) {
			val <<= avail;
			val |= b & ((1 << avail) - 1);
			index += avail;
			required -= avail;
		} else {
			b >>= avail - required;
			val <<= required;
			val |= b & ((1 << required) - 1);
			break;
		}
	} while (required);
	return val;
}
static uint32_t fetchbits_signed(const uint8_t *p, uint32_t index, uint32_t required)
{
	uint32_t val = fetchbits_unsigned(p, index, required);
	if (val & (1 << (required - 1))) {
		return (int32_t)val - (1 << required);
	}
	return (int32_t)val;
}

GCSize metaTFT::fontCharDimentions(unsigned int c)
{
  uint32_t bitoffset;
  const uint8_t *data;
  GCSize k;
  //Serial.printf("drawFontChar %d\n", c);

  if (c >= font->index1_first && c <= font->index1_last) {
    bitoffset = c - font->index1_first;
    bitoffset *= font->bits_index;
  } else if (c >= font->index2_first && c <= font->index2_last) {
    bitoffset = c - font->index2_first + font->index1_last - font->index1_first + 1;
    bitoffset *= font->bits_index;
  } else if (font->unicode) {
    return k; // TODO: implement sparse unicode
  } else {
    return k;
  }
  //Serial.printf("  index =  %d\n", fetchbits_unsigned(font->index, bitoffset, font->bits_index));
  data = font->data + fetchbits_unsigned(font->index, bitoffset, font->bits_index);

  uint32_t encoding = fetchbits_unsigned(data, 0, 3);
  if (encoding != 0) return k;
  //uint32_t width = fetchbits_unsigned(data, 3, font->bits_width);
  bitoffset = font->bits_width + 3;
  //uint32_t height = fetchbits_unsigned(data, bitoffset, font->bits_height);
  bitoffset += font->bits_height;
  //int32_t xoffset = fetchbits_signed(data, bitoffset, font->bits_xoffset);
  bitoffset += font->bits_xoffset;
  int32_t yoffset = fetchbits_signed(data, bitoffset, font->bits_yoffset);
  bitoffset += font->bits_yoffset;
//Serial.printf("  offset = %d,%d\n", xoffset, yoffset);

  uint32_t delta = fetchbits_unsigned(data, bitoffset, font->bits_delta);
	bitoffset += font->bits_delta;
  k.w = delta;
  k.h = font->cap_height-yoffset;
//  Serial<<"size["<<((char)c)<<"] = "<<width<<", "<< height<<" ("<<delta<<")"<<"Xof: "<<xoffset<<" yOff: "<<yoffset<<" "<<k<<endl;
  return k;
}

GCSize metaTFT::stringSize(const char* str){
  GCSize s=GCSize();
  if(!font){
    s.w = strlen(str)*6 * textsize;
    s.h = 8*textsize;
  }else{
    size_t i=0;
    int32_t width = 0;
    int32_t height = 0;
    //uint32_t height= 0;
    while(str[i]!= 0x00){
      GCSize k = fontCharDimentions(str[i]);
      width += k.w;
      height=(height < k.h)?k.h:height;
      ++i;
    }
    s.w = width;
    s.h = height;//font->cap_height;
  }
  return s;
}

uint16_t metaTFT::stringWidth(const char * str)
{
  return stringSize(str).w;
}
uint16_t metaTFT::stringHeight(const char * str){
  return stringSize(str).h;
}


void metaTFT::drawLogo()
{
  this->setTextSize(5);
  this->setCursor(25,15);
  this->setTextColor(ILI9341_GREEN);
  *this<<"   meta"<<endl<<"  Display"<<endl;
  this->drawFastHLine(this->getCursorX(),this->getCursorY()+10,this->width(),ILI9341_GREEN);
  this->setTextColor(ILI9341_YELLOW);
  this->setTextSize(2);
  *this<<endl;
  TFT_LogoEnd =0; //tft.cursor_y;
}

GraphicsContext::GraphicsContext(const GraphicsContext &gc){
	_base = gc._base;
	_display = gc._display;
	_fillColor = gc._fillColor;
	_strokeColor = gc._strokeColor;
}

size_t GraphicsContext::write(uint8_t c){
  return _display->write(c);
}

void GraphicsContext::initGraphicsContext(metaTFT* display){
  _display = display;
}

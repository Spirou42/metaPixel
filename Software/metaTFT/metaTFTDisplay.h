/**
* definition for metaPixel TFT interface
*/

#ifndef __METATFTDISPLAY_H__
#define __METATFTDISPLAY_H__
#define FASTLED_INTERNAL
#include "SPI.h"
#include "ILI9341_t3.h"
#include "FastLED.h"
#include "Streaming.h"

 /** a collection of structs for basic geometry  handling (int16_t)*/
typedef struct _GCPoint{
  int16_t x,y;
  _GCPoint(int16_t X,int16_t Y):x(X),y(Y){};
  _GCPoint():x(0),y(0){};
  _GCPoint( const _GCPoint& k){x=k.x;y=k.y;};
  bool operator==(_GCPoint p){return (x==p.x)&&(y==p.y);}
  bool operator!=(_GCPoint p){return !(*this == p);}
  _GCPoint operator+=(_GCPoint p){x+=p.x;y+=p.y;return *this;};
  //_GCPoint operator+(_GCPoint &p){_GCPoint r; r.x=p.x+x;r.y=p.y+y;return r;}
  _GCPoint operator+(_GCPoint p){_GCPoint r; r.x=p.x+x; r.y= p.y+y;return r;}
}GCPoint;

typedef struct _GCSize{
  int16_t w,h;
  _GCSize():w(0),h(0){};
  _GCSize(const _GCSize & k){w=k.w;h=k.h;};
  _GCSize(int16_t w, int16_t h):w(w),h(h){};
  bool operator==(_GCSize s){return (s.w == w)&&(s.h == h);}
  bool operator!=(_GCSize s){return !(*this == s);}
  _GCSize operator+=(_GCSize s){w+=s.w;h+=s.h; return *this;};
  _GCSize operator+(_GCSize p){_GCSize r; r.w=p.w+w;r.h=p.h+h;return r;}
  _GCSize operator+=(int t){w+=t;h+=t; return *this;};
  _GCSize operator*(int k){_GCSize l;l.w=k*w;l.h=k*h;return l;}
}GCSize;

typedef struct _GCRect{
  GCPoint origin;
  GCSize  size;
  _GCRect():origin(),size(){};
  _GCRect(const _GCRect &k){origin = k.origin; size = k.size;};
  _GCRect(GCPoint o, GCSize s){origin = o; size = s;};
  _GCRect(int16_t x, int16_t y, int16_t w, int16_t h):origin(x,y),size(w,h){};
}GCRect;

// Streaming operator for geometry classes

inline Print& operator<<(Print& obj, GCPoint &p){
  obj << "{"<<p.x<<", "<<p.y<<"}";
  return obj;
};
inline Print& operator<<(Print& obj, GCSize &p){
  obj << "{"<<p.w<<", "<<p.h<<"}";
  return obj;
};
inline Print& operator<<(Print& obj, GCRect &p){
  obj << "{"<<p.origin<<", "<<p.size<<"}";
  return obj;
};



/** my display subclass */
class metaTFT : public ILI9341_t3 {
public:
  metaTFT(uint8_t _CS, uint8_t _DC, uint8_t _RST = 255, uint8_t _MOSI=11, uint8_t _SCLK=13, uint8_t _MISO=12, uint8_t bkg_pin=A14,uint8_t rotation = 3):
  ILI9341_t3(_CS, _DC, _RST, _MOSI, _SCLK, _MISO),_backlight_pin(bkg_pin),defaultRotation(rotation),_luminance(80){
    updateBacklight();}

  void start();

  void setLuminance(uint8_t val){
    _luminance = val;updateBacklight();}

  uint8_t getLuminance(){
    return _luminance;}

  virtual void drawLogo();

  GCSize stringSize(const char* str);
  uint16_t stringWidth(const char* str);
  uint16_t stringHeight(const char* str);
protected:
  int16_t   TFT_LogoEnd = 0;
  uint8_t _backlight_pin;
  uint8_t defaultRotation ;

  uint8_t _luminance ;                    //< valaue for backlight

  void updateBacklight();
  GCSize fontCharDimentions(unsigned int c);

};

/** abstract base class for all view related classes.
graphics context encapsulates the device information of the TFT class and acts as a proxy for drawing operations
allowing a simple coordinate system hierarchy  without the need for handling real stacks of finite transformation matrices.

*/
class GraphicsContext : public Print{
  friend class metaView;
  friend class metaLabel;
  friend class metaValue;
public:
  GraphicsContext():_base(),_display(NULL),_fillColor(ILI9341_BLACK),_strokeColor(ILI9341_GREEN){};
  GraphicsContext(metaTFT* display):_base(GCPoint(0,0)),_display(display),_fillColor(ILI9341_BLACK),_strokeColor(ILI9341_GREEN){};
  GraphicsContext(const GraphicsContext &gc);

  void initGraphicsContext(metaTFT* display);
  void setBaseCoord(GCPoint p){_base = p;}
  GCPoint getBaseCoord(){return _base;}

  virtual GCPoint getScreenOrigin()=0;

  void setFillColor(uint16_t c){_fillColor = c;}
  uint16_t getFillColor(){return _fillColor;}

  void setStrokeColor(uint16_t c){_strokeColor=c;};
  uint16_t getStrokeColor(){return _strokeColor;};

  void addBase(GCPoint p){_base+=p;};
  void drawPixel(GCPoint p){
    GCPoint k = p+getScreenOrigin();
    _display->drawPixel(k.x,k.y,_strokeColor);
  }

  void drawPixel(int16_t x, int16_t y){
    drawPixel(GCPoint(x,y));
  }

  void drawLine(GCPoint start, GCPoint end){
    GCPoint base = getScreenOrigin();
    GCPoint s1 = start + base;
    GCPoint e1 = end +base;
    _display->drawLine(s1.x,s1.y,e1.x,e1.y,_strokeColor);
  }
  void drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2){
    drawLine(GCPoint(x1,y1),GCPoint(x2,y2));
  }
  void drawRect(GCRect frame){
    GCPoint l = frame.origin + getScreenOrigin();
    _display->drawRect(l.x,l.y,frame.size.w,frame.size.h,_strokeColor);
  }
  void drawRect(GCPoint p, GCSize s){drawRect(GCRect(p,s));}
  void drawRect(int16_t x, int16_t y, int16_t w, int16_t h){drawRect(GCRect(x,y,w,h));}

  void fillRect(GCRect frame){
    GCPoint l = frame.origin + getScreenOrigin();
    _display->fillRect(l.x,l.y,frame.size.w,frame.size.h,_fillColor);
  }
  void fillRect(GCPoint p, GCSize s){fillRect(GCRect(p,s));}
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h){fillRect(GCRect(x,y,w,h));}

  void drawRoundRect(GCRect frame, int16_t r){
    GCPoint l = frame.origin+getScreenOrigin();
    _display->drawRoundRect(l.x,l.y,frame.size.w,frame.size.h,r,_strokeColor);
  }
  void drawRoundRect(GCPoint p, GCSize s, int16_t r){drawRoundRect(GCRect(p,s),r);}
  void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r){drawRoundRect(GCRect(x,y,w,h),r);}

  void fillRoundRect(GCRect frame, int16_t r){
    GCPoint l = frame.origin+getScreenOrigin();
    _display->fillRoundRect(l.x,l.y,frame.size.w,frame.size.h,r,_fillColor);
  }
  void fillRoundRect(GCPoint p, GCSize s, int16_t r){fillRoundRect(GCRect(p,s),r);}
  void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r){fillRoundRect(GCRect(x,y,w,h),r);}

  void setCursor(GCPoint p){
    GCPoint l = p+getScreenOrigin();
    _display->setCursor(l.x,l.y);
  }
  void setCursor(int16_t x, int16_t y){setCursor(GCPoint(x,y));}

  void setTextColor(uint16_t c){_display->setTextColor(c);};
  void setTextColor(uint16_t c, uint16_t b){_display->setTextColor(c,b);};

  void setTextSize(uint8_t s){_display->setTextSize(s);};

  void setFont(const ILI9341_t3_font_t* font){if(font){_display->setFont(*font);}else{_display->setFontAdafruit();}};

  GCSize stringSize(const char* str){return _display->stringSize(str);};
	virtual size_t write(uint8_t);
  GCSize displaySize(){return GCSize(_display->width(),_display->height());};
//protected:
  GCPoint _base;
  metaTFT* _display;
  uint16_t _fillColor;
  uint16_t _strokeColor;
};

#endif

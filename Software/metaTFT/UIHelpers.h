/**
* a here the ui elements go
*/

#ifndef __UIHELPERS_H__
#define __UIHELPERS_H__
#include "Arduino.h"
#include <vector>
#include "metaTFTDisplay.h"

using namespace std;

class metaButton
{
public:

  metaButton(void):_gfx(NULL),_x(-1),_y(-1),_w(0),_h(0),_textsize(1) {}
	void initButton(metaTFT *gfx, int16_t x, int16_t y,
		uint8_t w, uint8_t h,
		uint16_t outline, uint16_t fill, uint16_t textcolor,
		const char *label, uint8_t textsize);
	void drawButton(bool inverted = false);
	bool contains(int16_t x, int16_t y);
	void press(boolean p) {
		laststate = currstate;
		currstate = p;
	}
	bool isPressed() { return currstate; }
	bool justPressed() { return (currstate && !laststate); }
	bool justReleased() { return (!currstate && laststate); }

	void setTextColor(uint16_t tc){_textcolor = tc;}
	uint16_t getTextColor(){return _textcolor;}

	void setFillColor(uint16_t fc){_fillcolor = fc;}
	uint16_t getFillColor(){return _fillcolor;}

	void setOutlineColor(uint16_t oc){_outlinecolor = oc;}
	uint16_t getOutlineColor(){return _outlinecolor;}

	int16_t getWidth(){return _w;}
	int16_t getHeight(){return _h;}
	int16_t getX(){return _x;}
	int16_t getY(){return _y;}

private:
	metaTFT *_gfx;
	int16_t _x, _y;
	uint16_t _w, _h;
	uint8_t _textsize;
	uint16_t _outlinecolor, _fillcolor, _textcolor;
	char _label[10];
	boolean currstate, laststate;
};

/** baseclass for UI elements */

class metaView
{
public:
  metaView(void):_gfx(NULL),_x(-1), _y(-1), _w(0), _h(0),_outlineColor(0),_backgroundColor(0),_cornerRadius(0),_opaque(true),
  _drawsOutline(false),_needsRedraw(false) {};
  void initView(metaTFT *gfx, int16_t x, int16_t y, uint8_t w, uint8_t h);

  void setOutlineColor(uint16_t c){_outlineColor = c;};
  uint16_t getOutlineColor(){return _outlineColor;};

  void setBackgroundColor(uint16_t c){_backgroundColor = c;};
  uint16_t getBackgroundColor(){return _backgroundColor;};

  void setOpaque(boolean f){_opaque = f;};
  boolean getOpaque(){return _opaque;};

  void addSubview(metaView* ptr){_subViews.push_back(ptr);};

  void setNeedsRedraw(){_needsRedraw = true;};


  virtual void redraw();
protected:
  metaTFT *_gfx;
  int16_t _x, _y;                            // upper right corner of the Label!
	uint16_t _w, _h;                           // width and height
	uint16_t _outlineColor, _backgroundColor;  // colorization
  uint8_t _cornerRadius;                     // cornerradius
  boolean _opaque;                           // draws background color
  boolean _drawsOutline;
  boolean _needsRedraw;
  vector<metaView*> _subViews;
};

class metaLabel : public metaView
{
public:
  metaLabel(void):metaView(),_textColor(ILI9341_GREEN),_textSize(1),_font(NULL),_label(NULL){};
  metaLabel(const String* label,uint16_t textColor=ILI9341_GREEN):metaView(),_textColor(textColor),_textSize(1),_font(NULL),_label(label){};
  void setFont(const ILI9341_t3_font_t &f){_font = &f;};
  const ILI9341_t3_font_t *getFont(){return _font;};

  void setTextColor(uint16_t tc){_textColor=tc;};
  uint16_t getTextColor(){return _textColor;};

  void setTextSize(uint8_t s){_textSize = s;};
  uint8_t getTextSize(){return _textSize;};

  virtual void redraw();

protected:
  uint16_t _textColor;        // textColor
  uint8_t _textSize;          // used if font == NULL
  const ILI9341_t3_font_t *_font;   //
  const String *_label;
};
#endif

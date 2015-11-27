/**
* implementation of the UIHelper classes
*/

#include "UIHelpers.h"
#include "metaTFTDisplay.h"
void metaButton::initButton(metaTFT *gfx,
	int16_t x, int16_t y, uint8_t w, uint8_t h,
	uint16_t outline, uint16_t fill, uint16_t textcolor,
	const char *label, uint8_t textsize)
{
	_x = x;
	_y = y;
	_w = w;
	_h = h;
	_outlinecolor = outline;
	_fillcolor = fill;
	_textcolor = textcolor;
	_textsize = textsize;
	_gfx = gfx;
	strncpy(_label, label, 9);
	_label[9] = 0;
}

void metaButton::drawButton(bool inverted)
{
	uint16_t fill, outline, text;

	if (! inverted) {
		fill = _fillcolor;
		outline = _outlinecolor;
		text = _textcolor;
	} else {
		fill =  _textcolor;
		outline = _outlinecolor;
		text = _fillcolor;
	}
	_gfx->fillRoundRect(_x - (_w/2), _y - (_h/2), _w, _h, min(_w,_h)/4, fill);
	_gfx->drawRoundRect(_x - (_w/2), _y - (_h/2), _w, _h, min(_w,_h)/4, outline);
	_gfx->setCursor(_x - strlen(_label)*3*_textsize, _y-4*_textsize);
	_gfx->setTextColor(text);
	_gfx->setTextSize(_textsize);
	_gfx->print(_label);
}

bool metaButton::contains(int16_t x, int16_t y)
{
	if ((x < (_x - _w/2)) || (x > (_x + _w/2))) return false;
	if ((y < (_y - _h/2)) || (y > (_y + _h/2))) return false;
	return true;
}

void metaView::redraw()
{
  if(_opaque){
    if(_cornerRadius==0){
      _gfx->fillRect(_x,_y,_w,_h,_backgroundColor);
    }else{
      _gfx->fillRoundRect(_x,_y,_w,_h,_cornerRadius,_backgroundColor);
    }
  }
  if(_drawsOutline){
    if(_cornerRadius==0){
      _gfx->drawRect(_x,_y,_w,_h,_outlineColor);
    }else{
      _gfx->drawRoundRect(_x,_y,_w,_h,_cornerRadius,_outlineColor);
    }
  }
}

void metaLabel::redraw()
{
  metaView::redraw();
}

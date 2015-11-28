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

/*** metaView **/
void metaView::initView(metaTFT* tft, GCRect frame)
{
	_frame = frame;
	_gc = GraphicsContext(tft);
	_needsRedraw = true;
	_needsLayout = true;
}

void metaView::initView(metaTFT* tft, GCPoint origin, GCSize size)
{
	initView(tft,GCRect(origin,size));
}

void metaView::initView(metaTFT* tft, int16_t x, int16_t y, int16_t w, int16_t h)
{
	initView(tft,GCRect(x,y,w,h));
}

const vector<metaView*>::iterator metaView::findSubview(metaView* subView){
	vector<metaView*>::iterator start = _subViews.begin();
	while(start != _subViews.end()){
		if(*start == subView){
			return start;
		}
		start ++;
	}
	return start;
}
void metaView::removeFromSuperview(){
	if(_superView){
		_superView->removeSubview(this);
	}
//	_superView = NULL;
}

void metaView::removeSubview(metaView* subView)
{
	const vector<metaView*>::iterator subIter = findSubview(subView);
	if(_subViews.end() != subIter){
		_subViews.erase(subIter);
		subView->_superView = NULL;
		subView->_gc._base = GCPoint(0,0);
	}
}
void metaView::addSubview(metaView* ptr){
	_subViews.push_back(ptr);
	ptr->_superView = this;
	ptr->_gc._base = _frame.origin+_gc._base;
}


boolean metaView::childNeedsLayout(){
	vector<metaView*>::iterator iter = _subViews.begin();
	while(iter != _subViews.end()){
		if((*iter++)->_needsLayout){
			return true;
		}
	}
	return false;
}

void metaView::redraw(){
	vector<metaView*>::iterator redrawIter = _subViews.begin();
	boolean cnl = childNeedsLayout();
	if(_needsRedraw || cnl){
		//Serial << "Redraw: "<<_needsRedraw<<" "<<cnl<<endl;
		_gc.setFillColor(_backgroundColor);
		_gc.setStrokeColor(_outlineColor);
  	if(_cornerRadius==0){
    	_gc.fillRect(_frame);
  	}else{
    	_gc.fillRoundRect(_frame,_cornerRadius);
  	}
  	if(_drawsOutline){
    	if(_cornerRadius==0){
      	_gc.drawRect(_frame);
    	}else{
      	_gc.drawRoundRect(_frame,_cornerRadius);
    	}
  	}
		while(redrawIter!=_subViews.end()){
//			Serial << "RedrawChild: "<<endl;
			(*redrawIter)->_needsRedraw = true;
			(*redrawIter)->redraw();
			(*redrawIter)->resetFlags();
			redrawIter++;
		}
	}else{
		if(_subViews.size()){
//			Serial << "Refresh children"<<endl;
			while(redrawIter!=_subViews.end()){
				if((*redrawIter)->_needsRedraw){
					(*redrawIter)->redraw();
					(*redrawIter)->resetFlags();
				}
				redrawIter++;
			}
		}
	}
}

/** metaLabel **/
GCSize metaLabel::intrinsicSize(){
	GCSize s=GCSize();
	s = _gc.stringSize(_label->c_str());
	return s;
}
void metaLabel::redraw()
{
	//Serial << "Label Redraw"<<endl;
  metaView::redraw();
	if(!_allignmentMask){
		_gc.setCursor(_frame.origin+_textPosition);
	}else{
		GCPoint tp=GCPoint();
		GCSize strSize = _gc.stringSize(_label->c_str());

	}
	_gc._display->setTextColor(_textColor,_backgroundColor);
	_gc._display->setTextSize(_textSize);
	_gc << *_label<<endl;
}

/**
* implementation of the UIHelper classes
*/

#include "UIHelpers.h"
#include "metaTFTDisplay.h"
#include "font_Arial.h"
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
void metaView::setOrigin(GCPoint p){
	_frame.origin = p;
	vector<metaView*>::iterator iter = _subViews.begin();
	while(iter != _subViews.end()){
		metaView * ptr = *iter;
		ptr->_gc._base = _frame.origin+_gc._base;
		ptr->setNeedsLayout();
		iter++;
	}
	_needsLayout=true;
};

boolean metaView::childNeedsLayout(){
	vector<metaView*>::iterator iter = _subViews.begin();
	while(iter != _subViews.end()){
		if((*iter++)->_needsLayout){
			return true;
		}
	}
	return false;
}

void metaView::allignInRect(uint8_t allignmentMask, GCRect rect){
	if(!allignmentMask)
		return;
	GCPoint origin = _frame.origin;
	if(allignmentMask & HALLIGN_LEFT){
		origin.x = rect.origin.x;
	}else if(allignmentMask & HALLIGN_RIGHT){
		origin.x = (rect.size.w - _frame.size.w) +rect.origin.x;
	}else if(allignmentMask & HALLIGN_CENTER){
		origin.x = (rect.size.w/2 - _frame.size.w/2) + rect.origin.x;
	}

	if(allignmentMask & VALLIGN_TOP){
		origin.y = rect.origin.y;
	}else if(allignmentMask & VALLIGN_BOTTOM){
		origin.y = (rect.size.h - _frame.size.h) + rect.origin.y;
	}else if(allignmentMask & VALLIGN_CENTER){
		origin.y = (rect.size.h/2 - _frame.size.h/2)+rect.origin.y;
	}
	setOrigin(origin);
	setNeedsLayout();

}
void metaView::allignInSuperView(uint8_t allignmentMask){
	if((allignmentMask == 0))
		return;
	GCRect p;
	if(_superView){
		p.size = _superView->getSize();
	}else{
		p.size = _gc.displaySize();
		Serial << "Allign: "<<p<<endl;
	}
	allignInRect(allignmentMask, p);
}

void metaView::redrawChildren(boolean forceRedraw){
	vector<metaView*>::iterator redrawIter = _subViews.begin();
	while(redrawIter!=_subViews.end()){
		//Serial << "RedrawChild: "<<endl;
		if(forceRedraw){
			(*redrawIter)->_needsRedraw = true;
		}
		(*redrawIter)->redraw();
		(*redrawIter)->resetFlags();
		redrawIter++;
	}
}

void metaView::drawDebug(){
	#if 1
	if (drawDebugRect){
		GCRect dr = debugRect;
		dr.origin += _frame.origin;
		_gc.setStrokeColor(ILI9341_RED);
		_gc.drawRect(dr);
		Serial << "DebugDraw: "<<debugRect<<endl;
	}
	#endif
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
		redrawChildren(true);
		resetFlags();
	}else{
		redrawChildren();
	}
	drawDebug();
}

/** metaLabel **/
GCSize metaLabel::intrinsicSize(){
	GCSize s=GCSize();
	_gc.setFont(_font);
	_gc.setTextSize(_textSize);
	s = _gc.stringSize(_label->c_str());
	return s;
}

void metaLabel::redraw(){
	//Serial << "Label Redraw"<<endl;
  metaView::redraw();
	GCPoint tp=_textPosition;

	if(_allignmentMask){
		GCSize is = intrinsicSize();
		if((_allignmentMask & HALLIGN_LEFT))
			{
				tp.x = 0;
			}
		if((_allignmentMask & HALLIGN_RIGHT))
			{
				tp.x = _frame.size.w - is.w;
			}
		if((_allignmentMask & HALLIGN_CENTER))
			{
				tp.x = _frame.size.w/2 - is.w/2;
			}
		if((_allignmentMask & VALLIGN_TOP))
			{
				tp.y = 0;
			}
		if((_allignmentMask & VALLIGN_BOTTOM))
			{
				tp.y = _frame.size.h - is.h;
			}
		if((_allignmentMask & VALLIGN_CENTER))
			{
				tp.y = _frame.size.h/2 - is.h/2;
			}
	}
	_gc.setCursor(_frame.origin+tp);
	_gc.setFont(_font);
	//Serial << "[Label] ("<<*_label<<") "<<(int)_font<<","<<_textColor<<","<<_backgroundColor<<endl;
	_gc.setTextColor(_textColor,_backgroundColor);
	_gc.setTextSize(_textSize);
	_gc << *_label<<endl;
}


void metaValue::initValue(metaTFT* tft, GCRect frame, const ILI9341_t3_font_t *valueFont, const ILI9341_t3_font_t *labelFont){
	metaView::initView(tft, frame);
	setBackgroundColor(ILI9341_BLACK);
	setOutlineColor(ILI9341_YELLOW);
	setCornerRadius(8);
	setDrawsOutline(false);

	_labelView.initView(tft,0,0,100,100);
	_labelView.setLabel(_label);
	_labelView.setFont(labelFont);
	_labelView.setTextSize(2);
	_labelView.setBackgroundColor(_backgroundColor);
	_labelView.setTextColor(_labelColor);
	_labelView.setOutlineColor(_labelColor);
	_labelView.setCornerRadius(3);
	_labelView.setDrawsOutline(false);
	_labelView.setAllignmentMask(VALLIGN_CENTER | HALLIGN_CENTER);
	_labelView.sizeToFit();

	GCSize ls = _labelView.getSize();
	ls.h = labelFont->line_space;
	if(_labelView.getDrawsOutline()){
		ls.h+=6;
	}
	ls.w+=15;
	_labelView.setSize(ls);

	GCPoint lo =_labelView.getOrigin();
	lo.x = 18;
	_topBorderOffset =   ls.h/2 ;

	_labelView.setOrigin(lo);
	_valueView.initView(tft,0,0,120,120);
	_valueView.setLabel(_value);
	_valueView.setTextSize(3);
	_valueView.setFont(valueFont);
	_valueView.setBackgroundColor(_backgroundColor);
	_valueView.setTextColor(_valueColor);
	_valueView.sizeToFit();
	_valueView.setAllignmentMask(VALLIGN_CENTER | HALLIGN_CENTER);
	_valueView.setTextPosition(1,1);
	_valueView.setNeedsRedraw();
	GCSize s= _valueView.getSize();
	s+=4;
	_valueView.setSize(s);

	GCPoint valOr = GCPoint();
	valOr.x =getSize().w/2 - s.w/2;
	valOr.y = (getSize().h/2 - s.h/2)+_topBorderOffset/2;
	_valueView.setOrigin(valOr);
	//Serial << "ValuePos "<<valOr.x<<", "<<valOr.y<<"("<<s.w<<","<<s.h<<")"<<endl;
	//	value->remove(0);

	addSubview(&_labelView);
	//	Serial << "Label Added"<<endl;
	addSubview(&_valueView);
		//Serial << "Value Added"<<endl;

}

void metaValue::initValue(metaTFT* tft, GCRect frame, String* label, String *value, const ILI9341_t3_font_t *valueFont, const ILI9341_t3_font_t *labelFont){
	_label = label;
	_value = value;
	initValue(tft,frame, valueFont, labelFont);
}

void metaValue::redraw(){
	if(_needsRedraw || childNeedsLayout()){
		GCRect p=_frame;
		p.origin.y+=_topBorderOffset;
		p.size.h-=_topBorderOffset;
		//Serial << "TopBorderOffset: "<<_topBorderOffset<<endl;
		//Serial << "Rect: "<<p<<endl;
		//Serial<< "Base: "<<_gc._base<<endl;
		_gc.setFillColor(_backgroundColor);
		_gc.setStrokeColor(_outlineColor);
		_gc.fillRoundRect(p,_cornerRadius);
		_gc.drawRoundRect(p,_cornerRadius);
		resetFlags();
		redrawChildren(true);
	}else{
		redrawChildren();
	}
	drawDebug();
}
#define VALUE_HORIZONTAL_INSET 18
#define VALUE_VERTICAL_INSET 10
void metaValue::sizeToFit(){
	GCSize valueSize = _valueView.getSize();
	Serial << "valueSize: "<<valueSize<<endl;
	GCSize labelSize = _labelView.getSize();
	Serial << "labelSize: "<<labelSize;
	GCSize ownSize = _frame.size;
	Serial << "ownSize: "<<ownSize<<endl;
	valueSize.h+=(2*VALUE_VERTICAL_INSET)+(2*_topBorderOffset);
	valueSize.w+=2*VALUE_HORIZONTAL_INSET;
	//labelSize.h+=2*VALUE_VERTICAL_INSET;
	labelSize.w+=2*VALUE_HORIZONTAL_INSET;

	ownSize.w = MAX(ownSize.w,MAX(valueSize.w,labelSize.w));
	ownSize.h = MAX(ownSize.h,valueSize.h);
	if(( (float)ownSize.w/(float)ownSize.h)>1.8){
		ownSize.h = ownSize.w/1.6180;
	}
	Serial << "resultSize: "<<ownSize<<endl;
	// relayout the Value<
	GCRect p;
	p.size = ownSize;
	//p.size.w-=2*VALUE_HORIZONTAL_INSET;
	p.size.h=(p.size.h - (2*_topBorderOffset))-(2*VALUE_VERTICAL_INSET);
	p.size.w=p.size.w - (2*VALUE_HORIZONTAL_INSET);
	p.origin.x = VALUE_HORIZONTAL_INSET;
	p.origin.y = 2*_topBorderOffset+VALUE_VERTICAL_INSET-(_valueView.getSize().h/10);
	Serial << "layoutRect: "<<p<<endl;
	this->debugRect =p;
	//this->drawDebugRect = true;
	_valueView.allignInRect(HALLIGN_CENTER | VALLIGN_CENTER,p);

	setSize(ownSize);
}

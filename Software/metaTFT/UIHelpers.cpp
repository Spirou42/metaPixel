/**
* implementation of the UIHelper classes
*/

#include "UIHelpers.h"
#include "metaTFTDisplay.h"
#include "font_Arial.h"
#include "UserEvent.h"

#define DEBUG_LAYOUT_COLOR_BACKGROUND_INNER ILI9341_DARKGREY
#define DEBUG_LAYOUT_COLOR_BACKGROUND_MIDDLE ILI9341_LIGHTGREY
#define DEBUG_LAYOUT_COLOR_BACKGROUND_OUTER ILI9341_NAVY
#define DEBUG_LAYOUT_LINECOLOR ILI9341_WHITE

/*** metaView **/
void metaView::initView(metaTFT* tft, GCRect frame)
{
	initGraphicsContext(tft);
	_frame = frame;
	_needsRedraw = true;
	_needsLayout = true;
}
GCPoint metaView::getBase(){
  GCPoint result;
  result += getOrigin();
  metaView *superView = getSuperview();
  while(superView){
    result += superView->getOrigin();
    superView = superView->getSuperview();
  }
  return result;
}
void metaView::initView(metaTFT* tft, GCPoint origin, GCSize size)
{
	initView(tft,GCRect(origin,size));
}

void metaView::initView(metaTFT* tft, int16_t x, int16_t y, int16_t w, int16_t h)
{
	initView(tft,GCRect(x,y,w,h));
}

void metaView::setLayout(ViewLayout l){
	_outlineColor = l.outlineColor;
	_backgroundColor = l.backgroundColor;
	_cornerRadius = l.cornerRadius;
	_opaque = l.opaque;
	_drawsOutline = l.drawsOutline;
	_visualizeState = l.visualizeState;
	_state = l.state;
	setNeedsRedraw();
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
//		subView->_gc._base = GCPoint(0,0);
	}
}
void metaView::addSubview(metaView* ptr){
	_subViews.push_back(ptr);
	ptr->_superView = this;
//	ptr->_gc._base = _frame.origin+_gc._base;
}

void metaView::sizeToFit()
{
	return;
}
bool metaView::childNeedsLayout(){
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
		p.size = GraphicsContext::displaySize();
		Serial << "Allign: "<<p<<endl;
	}
	allignInRect(allignmentMask, p);
}

void metaView::redrawChildren(bool forceRedraw){
	if(_subViews.size() ==0)
		return;
//	Serial << ">>>>>>>redrawChildren "<< forceRedraw<<endl;
	vector<metaView*>::iterator redrawIter = _subViews.begin();
	while(redrawIter!=_subViews.end()){
		//Serial << "RedrawChild: "<<endl;
		if(forceRedraw){
			(*redrawIter)->_needsRedraw = true;
//			Serial << "Force:"<<endl;
		}
		if((*redrawIter)->_needsRedraw)
			(*redrawIter)->redraw();
		(*redrawIter)->resetFlags();
		redrawIter++;
	}
//	Serial << "<<<<<<<redrawChildren"<<endl<<endl;;
}
#if DEBUG_LAYOUT
void metaView::drawDebug(){
	#if DEBUG_LAYOUT
	if (drawDebugRect){
		GCRect dr = debugRect;
//		dr.origin += _frame.origin;
		GraphicsContext::setStrokeColor(DEBUG_LAYOUT_LINECOLOR);
		GraphicsContext::drawRect(dr);
		Serial << "DebugDraw: "<<debugRect<<endl;
	}
	#endif
}
#endif
void metaView::redraw(){
	vector<metaView*>::iterator redrawIter = _subViews.begin();
	bool cnl = childNeedsLayout();
	if(_needsRedraw || cnl){
//		Serial << "Redraw: "<<_needsRedraw<<" "<<cnl<<endl;
		GraphicsContext::setFillColor(_backgroundColor);
		GraphicsContext::setStrokeColor(_outlineColor);
  	if(_cornerRadius==0){
    	GraphicsContext::fillRect(getBounds());
  	}else{
    	GraphicsContext::fillRoundRect(getBounds(),_cornerRadius);
  	}
  	if(_drawsOutline){
    	if(_cornerRadius==0){
      	GraphicsContext::drawRect(getBounds());
    	}else{
      	GraphicsContext::drawRoundRect(getBounds(),_cornerRadius);
    	}
  	}
		redrawChildren(true);
		resetFlags();
	}else{
		redrawChildren();
	}
	#if DEBUG_LAYOUT
	drawDebug();
	#endif
}

/** metaLabel **/
GCSize metaLabel::intrinsicSize(){
	GCSize s=GCSize();
	GraphicsContext::setFont(_font);
	GraphicsContext::setTextSize(_textSize);
	s = GraphicsContext::stringSize(_label->c_str());
	if(_visualizeState){
		s.w +=_indicatorSpace;
	}
	s.w+=2*_insets.w;
	s.h+=2*_insets.h;
	return s;
}

void metaLabel::setLayout(LabelLayout ll){
	if(NULL != ll.viewLayout){
		metaView::setLayout(*ll.viewLayout);
	}
	setFont(ll.font);
	setTextSize(ll.textSize);
	setAllignmentMask(ll.allignmentMask);
	setTextPosition(ll.textPosition);
	setInsets(ll.insets);
	setIndicatorSize(ll.indicatorSize);
	setIndicatorSpace(ll.indicatorSpace);
}

void metaLabel::redraw(){
	//Serial << ">>>>>>Label Redraw "<<_frame<<_font->line_space<<endl;
  metaView::redraw();
	GCPoint tp=_textPosition;
	tp.x+=_insets.w;
	tp.y+=_insets.h;
	int16_t xOffset = 0;
	if(_visualizeState){
		xOffset = _indicatorSpace;
		tp.x += xOffset;
	}
	if(_allignmentMask){
		GCSize is = intrinsicSize();
		if((_allignmentMask & HALLIGN_LEFT))
			{
				tp.x += 0/*+xOffset*/;
			}
		if((_allignmentMask & HALLIGN_RIGHT))
			{
				tp.x += _frame.size.w - is.w+xOffset;
			}
		if((_allignmentMask & HALLIGN_CENTER))
			{
				tp.x += _frame.size.w/2 - is.w/2 + xOffset;
			}
		if((_allignmentMask & VALLIGN_TOP))
			{
				tp.y = 0;
			}
		if((_allignmentMask & VALLIGN_BOTTOM))
			{
				tp.y += _frame.size.h - is.h;
			}
		if((_allignmentMask & VALLIGN_CENTER))
			{
				tp.y += _frame.size.h/2 - is.h/2;
			}
	}
	if(_visualizeState){
		uint16_t bgColor = ILI9341_BLACK;
		uint16_t olColor = ILI9341_DARKGREY;
		switch(_state){
			case Off: bgColor = ILI9341_BLACK; break;
			case On: bgColor = _outlineColor; break;
			case Mixed: bgColor = ILI9341_NAVY; break;
		}
		GraphicsContext::setFillColor(bgColor);
		GraphicsContext::setStrokeColor(olColor);
		GCRect indicator;
		indicator.origin=GCPoint(0,   (_frame.size.h-2*_insets.h)/2 -_indicatorSize.h/2 -(_font->line_space-_font->cap_height)/2);
		indicator.origin.x += _insets.w;
		indicator.origin.y += _insets.h;
		//indicator.origin = _frame.origin;
		indicator.size =_indicatorSize;
		if(_state != Off){
			GraphicsContext::fillRect(indicator);
		}
		drawRect(indicator);
		//drawRect(_insets.w,_insets.h,_indicatorSpace,_frame.size.h-2*_insets.h);

	}
//	Serial << *_label<<" TP: "<<tp<<endl;
	GraphicsContext::setCursor(tp);
	GraphicsContext::setFont(_font);
	//Serial << "[Label] ("<<*_label<<") "<<(int)_font<<","<<_textColor<<","<<_backgroundColor<<endl;
	if(_opaque){
		GraphicsContext::setTextColor(_textColor,_backgroundColor);
//		Serial << "opaque"<<endl;
	}else{
		GraphicsContext::setTextColor(_textColor);
	}
	GraphicsContext::setTextSize(_textSize);
	(*this) << *_label<<endl;
	resetFlags();
//	Serial << "<<<<<<Label Redraw"<<endl;
}

/** metaValue **/

void metaValue::initValue(metaTFT* tft, GCRect frame){
	metaView::initView(tft, frame);
	setBackgroundColor(ILI9341_BLACK);
	#if DEBUG_LAYOUT_VALUE
	setBackgroundColor(DEBUG_LAYOUT_COLOR_BACKGROUND_MIDDLE);
	setOpaque(true);
	#endif
	setOutlineColor(ILI9341_YELLOW);
	setCornerRadius(_cornerRadius);
	setDrawsOutline(false);

	_labelView.initView(tft,0,0,100,100);
	_labelView.setLabel(_label);
	_labelView.setFont(_labelFont);
	_labelView.setTextSize(2);
	_labelView.setBackgroundColor(_backgroundColor);
	#if DEBUG_LAYOUT_VALUE
	_labelView.setBackgroundColor(DEBUG_LAYOUT_COLOR_BACKGROUND_INNER);
	#endif
	_labelView.setTextColor(_labelColor);
	_labelView.setOutlineColor(_labelColor);
	_labelView.setCornerRadius(_labelOutlineCornerRadius);
	_labelView.setDrawsOutline(_labelDrawOutline);
	_labelView.setAllignmentMask(VALLIGN_CENTER | HALLIGN_CENTER);
	_labelView.sizeToFit();

	GCSize ls = _labelView.getSize();
	ls.h = _labelFont->line_space;
	if(_labelView.getDrawsOutline()){
		ls.h+=2*_labelOutlineInset;
	}
	ls.w+=4*_labelOutlineInset;
	_labelView.setSize(ls);

	GCPoint lo =_labelView.getOrigin();
	lo.x = _horizontalLabelInset;
	_frameInset =   ls.h/2 ;

	_labelView.setOrigin(lo);
	_valueView.initView(tft,0,0,120,120);
	_valueView.setLabel(_value);
	_valueView.setTextSize(3);
	_valueView.setFont(_valueFont);
	_valueView.setBackgroundColor(_backgroundColor);
	#if DEBUG_LAYOUT_VALUE
	_valueView.setBackgroundColor(DEBUG_LAYOUT_COLOR_BACKGROUND_INNER);
	#endif
	_valueView.setTextColor(_valueColor);
	_valueView.sizeToFit();
	_valueView.setAllignmentMask(VALLIGN_CENTER | HALLIGN_CENTER);
	_valueView.setTextPosition(1,1);
	_valueView.setNeedsRedraw();
	GCSize s= _valueView.getSize();
	s+=4;
	_valueView.setSize(s);

	GCPoint valOr = GCPoint();
	valOr.x =((getSize().w-2*_horizontalValueInset)/2 - s.w/2)+_horizontalValueInset;
	valOr.y = (getSize().h/2 - s.h/2)+_frameInset/2;
	_valueView.setOrigin(valOr);
	//Serial << "ValuePos "<<valOr.x<<", "<<valOr.y<<"("<<s.w<<","<<s.h<<")"<<endl;
	//	value->remove(0);

	addSubview(&_labelView);
	//	Serial << "Label Added"<<endl;
	addSubview(&_valueView);
		//Serial << "Value Added"<<endl;

}

void metaValue::initValue(metaTFT* tft, GCRect frame, String* label, String *value){
	_label = label;
	_value = value;
	initValue(tft,frame);
}

void metaValue::redraw(){
	if(_needsRedraw || childNeedsLayout()){
		GCRect p=getBounds();
		p.origin.y+=_frameInset;
		p.size.h-=_frameInset;
		//Serial << "TopBorderOffset: "<<_frameInset<<endl;
		//Serial << "Rect: "<<p<<endl;
		//Serial<< "Base: "<<_gc._base<<endl;
		#if DEBUG_LAYOUT_VALUEBACKGROUND
		GraphicsContext::setFillColor(DEBUG_LAYOUT_COLOR_BACKGROUND_OUTER);
		GraphicsContext::fillRoundRect(getBounds(),_cornerRadius);
		#endif
		GraphicsContext::setFillColor(_backgroundColor);
		GraphicsContext::setStrokeColor(_outlineColor);

		GraphicsContext::fillRoundRect(p,_cornerRadius);
		GraphicsContext::drawRoundRect(p,_cornerRadius);
		resetFlags();
		redrawChildren(true);
	}else{
		redrawChildren();
	}
	#if DEBUG_LAYOUT
	drawDebug();
	#endif
}

void metaValue::sizeToFit(){
	GCSize valueSize = _valueView.getSize();
	Serial << "valueSize: "<<valueSize<<endl;
	GCSize labelSize = _labelView.getSize();
	int16_t baslineCorrection = (_labelView.getFont()->line_space - _labelView.getFont()->cap_height-3);
	//	baslineCorrection = 0;
	Serial << "labelSize: "<<labelSize;
	Serial << "baseLine Correction "<<baslineCorrection<<endl;
	GCSize ownSize = _frame.size;
	Serial << "ownSize: "<<ownSize<<endl;
	valueSize.h+=(2*_verticalValueInset)+(2*_frameInset);//baslineCorrection;
	valueSize.w+=2*_horizontalValueInset;
	//labelSize.h+=2*VALUE_VERTICAL_INSET;
	labelSize.w+=2*_horizontalLabelInset;

	ownSize.w = MAX(ownSize.w,MAX(valueSize.w,labelSize.w));
	ownSize.h = MAX(ownSize.h,valueSize.h);
	// if(( (float)ownSize.w/(float)ownSize.h)>1.8){
	// 	ownSize.h = ownSize.w/1.6180;
	// }
	Serial << "resultSize: "<<ownSize<<endl;

	// relayout the Value<
	GCRect p;
	p.size = ownSize;
	//p.size.w-=2*VALUE_HORIZONTAL_INSET;
	p.size.h=(p.size.h - (2*_frameInset))-(2*_verticalValueInset);
	p.size.w=p.size.w - (2*_horizontalValueInset);
	p.origin.x = _horizontalValueInset;
	p.origin.y = 2*_frameInset+_verticalValueInset -baslineCorrection;
	Serial << "layoutRect: "<<p<<endl;
	#if DEBUG_LAYOUT_VALUESIZE
	this->debugRect =p;
	this->drawDebugRect = true;
	#endif
	_valueView.allignInRect(HALLIGN_CENTER | VALLIGN_CENTER,p);

	setSize(ownSize);
}

/** metaList **/
metaList::~metaList()
{

}
void metaList::addSubview(metaView* aView){
	metaView::addSubview(aView);
	//aView->setVisualizeState(true);

	aView->sizeToFit();
	GCSize l = aView->getSize();
	Serial << " View: "<<l<<endl;
	_maxElementSize.w = MAX(aView->getSize().w,_maxElementSize.w);
	_maxElementSize.h = MAX(aView->getSize().h,_maxElementSize.h);
}
void metaList::sizeToFit()
{
	GCSize resultSize ;
	if(!_subViews.size()){
		return;
	}
	int p = _subViews.size();
	GCSize tS = _maxElementSize;
	tS.h=_maxElementSize.h*p;
	if(p>1){
		tS.h+=_cellInset.h*(p-1);
	}
	tS+=_borderInset*2;

	resultSize += tS;
	Serial << "Size: "<< resultSize<<endl;
	setSize(tS);
}
void metaList::layoutList()
{
	GCPoint currentLine;
	currentLine.x = _borderInset.w;
	currentLine.y = _borderInset.h;
	vector<metaView*>::iterator subIter = _subViews.begin();
	//Serial << "Layout: "<<_maxElementSize<<endl;
	while(subIter != _subViews.end()){
		(*subIter)->setSize(_maxElementSize);
		(*subIter)->setOrigin(currentLine);
		//(*subIter)->setVisualizeState(true);
		if(subIter != _subViews.begin()){
			(*subIter)->setDrawsOutline(false);
			(*subIter)->setState(metaView::State::Off);
		}else{
			(*subIter)->setState(metaView::State::Off);
			(*subIter)->setDrawsOutline(true);
			_lastSelectedView = NULL;
		}
		currentLine.y+=_maxElementSize.h+_cellInset.h;
		subIter++;
	}
}

metaView* metaList::selectedSubview(){
	vector<metaView*>::iterator subIter = _subViews.begin();
	while(subIter!=_subViews.end()){
		if((*subIter)->getDrawsOutline()){
			return *subIter;
		}
		subIter++;
	}
	return NULL;
}
void metaList::drawConnectionFor(metaView* v, uint16_t lineColor)
{
	if(v){
		GCPoint vo = v->getOrigin();
//		vo+=_frame.origin;
		GCSize  vs = v->getSize();
		// Serial << "DrawConnection: "<<vo<<", "<<vs<<endl;
		vo.x+=vs.w;
		vo.y+=vs.h/2-1;
		// Serial << "Start: "<<vo<<endl;
		// Serial << "Base: "<<_gc._base<<endl;
		GraphicsContext::setStrokeColor(lineColor);
		GraphicsContext::drawLine(vo,GCPoint(_frame.size.w-1,vo.y));
		vo.y++;
		GraphicsContext::setStrokeColor(_backgroundColor);
		GraphicsContext::drawLine(vo,GCPoint(_frame.size.w-1,vo.y));
		vo.y++;
		GraphicsContext::setStrokeColor(lineColor);
		GraphicsContext::drawLine(vo,GCPoint(_frame.size.w-1,vo.y));
	}
}

void metaList::redraw(){
	//Serial << endl<<">>>>>metaList"<<endl;
	metaView::redraw();
	metaView *sv = selectedSubview();
	if(sv != _lastSelectedView){
		drawConnectionFor(_lastSelectedView,_backgroundColor);
		if(sv){
			drawConnectionFor(sv,sv->getOutlineColor());
		}
		_lastSelectedView = sv;
	}
	resetFlags();
	//Serial << "<<<<<metaList"<<endl;
}

int8_t metaList::selectedIndex(){
	vector<metaView*>::iterator subIter = _subViews.begin();
	int8_t result = -1;
	int8_t c = 0;
	while(subIter != _subViews.end()){
		if((*subIter)->getDrawsOutline()){
			result = c;
			break;
		}
		subIter ++;
		c++;
	}
	return result;
}
vector<metaView*>::iterator metaList::onIterator(){
	vector<metaView*>::iterator result = _subViews.begin();
	while(result != _subViews.end()){
		if((*result)->getState()==metaView::State::On){
			break;
		}
		result ++;
	}
	return result;
}
vector<metaView*>::iterator metaList::selectedIterator(){
	vector<metaView*>::iterator result = _subViews.begin();
	while(result != _subViews.end()){
		if((*result)->getDrawsOutline()){
			break;
		}
		result ++;
	}
	return result;
}

void metaList::selectIndex(int8_t idx){
	vector<metaView*>::iterator subIter = _subViews.begin();
	if(selectedIndex() != idx){
		if(_lastSelectedView){
			_lastSelectedView->setDrawsOutline(false);
		}
		subIter += idx;
		if(subIter != _subViews.end()){
			(*subIter)->setDrawsOutline(true);
		}
	}
}
void metaList::initResponder(UserEventQueue* queue){
	metaResponder::initResponder(queue);
}

int16_t metaList::processEvent(UserEvent* k){
	if(k->getType() == UserEvent::EventType::EventTypeKey){
		UserEvent::ButtonData bData = k->getData().buttonData;
		// Serial << k<<endl;
		int8_t step = 0;
		switch(bData.id){
			case UserEvent::ButtonID::DownButton: if(bData.state == UserEvent::ButtonState::ButtonDown){step = 1;} break;
			case UserEvent::ButtonID::UpButton: if(bData.state == UserEvent::ButtonState::ButtonDown){step = -1;} break;
			case UserEvent::ButtonID::CenterButton: if(bData.state == UserEvent::ButtonState::ButtonDown){
				vector<metaView*>::iterator oldIter = onIterator();
				vector<metaView*>::iterator newIter = selectedIterator();
				if (oldIter != _subViews.end() ){
					(*oldIter)->setState(metaView::State::Off);
				}
				if (newIter != _subViews.end()) {
					(*newIter)->setState(metaView::State::On);
				}
			} break;
			default: break;
		}
		if(step){
			vector<metaView*>::iterator subIter = selectedIterator();
			vector<metaView*>::iterator k = subIter+step;
			if( (k>=_subViews.begin()) && (k<_subViews.end()) ) {
				//(*subIter)->setState(metaView::State::Off);
				(*subIter)->setDrawsOutline(false);
				//(*k)->setState(metaView::State::On);
				(*k)->setDrawsOutline(true);
				//setNeedsRedraw();
			}
		}
	}else if(k->getType() == UserEvent::EventType::EventTypeEncoder){
		// Serial <<"processEvent "<<k<<endl;
		vector<metaView*>::iterator subIter = selectedIterator();
		// const String *label = ((metaLabel*) *subIter)->getLabel();
		// Serial <<"SelectedIter: "<<*label<<endl;
		UserEvent::EncoderData eData =k->getData().encoderData;
		int8_t offSet = eData.absSteps;
		// Serial <<"Offset: "<<offSet<<endl;
		vector<metaView*>::iterator k = subIter+offSet;
		if( (k>=_subViews.begin()) && (k<_subViews.end()) ) {
			// label = ((metaLabel*) *k)->getLabel();
			// Serial << "NextSelect: "<<*label<<endl;
			//(*subIter)->setState(metaView::State::Off);
			(*subIter)->setDrawsOutline(false);
			//(*k)->setState(metaView::State::On);
			(*k)->setDrawsOutline(true);
			//setNeedsRedraw();
		}
	}
	return selectedIndex();
}

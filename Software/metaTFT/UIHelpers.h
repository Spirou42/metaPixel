/**
* a here the ui elements go
*/

#ifndef __UIHELPERS_H__
#define __UIHELPERS_H__
#include "Arduino.h"
#include <vector>
#include "metaTFTDisplay.h"
#include "font_Arial.h"

#define DEBUG_LAYOUT                  (1)
#define DEBUG_LAYOUT_VALUE            (0)
#define DEBUG_LAYOUT_VALUEBACKGROUND  (0 && DEBUG_LAYOUT_VALUE)
#define DEBUG_LAYOUT_VALUESIZE        (0 /*&& DEBUG_LAYOUT_VALUE*/)


/** @todo: rework */
/** AllignmentMasks terms*/
#define HALLIGN_LEFT    (1<<1)
#define HALLIGN_CENTER  (1<<2)
#define HALLIGN_RIGHT   (1<<3)
#define VALLIGN_TOP     (1<<4)
#define VALLIGN_CENTER  (1<<5)
#define VALLIGN_BOTTOM  (1<<6)

using namespace std;
/** baseclass for EventResponding */
class UserEventQueue;
class UserEvent;
class metaResponder
{
public:
  metaResponder(){}
  void initResponder(UserEventQueue* queue){
    _eventQueue = queue;}

  void setRespondsToEvents(uint16_t m){
    _respondsToEvents = m;}
  uint16_t respondsToEvents(){
    return _respondsToEvents;}
  virtual int16_t processEvent(UserEvent *k)=0;
protected:
    UserEventQueue * _eventQueue;
    uint16_t _respondsToEvents;
};

/** baseclass for UI elements */

class metaView : public GraphicsContext
{
public:
  typedef enum _states{
    Off,
    On,
    Mixed
  }State;
  metaView(void):_frame(),_outlineColor(0),_backgroundColor(0),_cornerRadius(0),_opaque(true),
  _drawsOutline(true),_needsRedraw(false),_needsLayout(false),_superView(NULL),_visualizeState(false),_state(Off) {}
  metaView(GCRect frame):_frame(frame),_outlineColor(0),_backgroundColor(0),_opaque(true),
  _drawsOutline(false),_needsRedraw(true),_needsLayout(true),_superView(NULL),_visualizeState(false),_state(Off){}

  virtual void initView(metaTFT* tft, GCRect frame);
  virtual void initView(metaTFT* tft, GCPoint origin, GCSize size);
  virtual void initView(metaTFT* tft, int16_t x, int16_t y, int16_t w, int16_t h);
  virtual GCPoint getBase();
  GCRect getBounds(){
    GCRect k;
    k.size = getSize();
    return k;
  }
  void setOutlineColor(uint16_t c){
    _outlineColor = c;_needsRedraw = true;}

  uint16_t getOutlineColor(){
    return _outlineColor;}

  void setDrawsOutline(bool flag){
    _drawsOutline = flag;_needsRedraw = true;}

  bool getDrawsOutline(){
    return _drawsOutline;}

  void setBackgroundColor(uint16_t c){
    _backgroundColor = c;_needsRedraw=true;}

  uint16_t getBackgroundColor(){
    return _backgroundColor;}

  void setOpaque(bool f){
    _opaque = f;_needsRedraw=true;}

  bool getOpaque(){
    return _opaque;}

  void setCornerRadius(uint8_t r){
    _cornerRadius = r;_needsRedraw=true;};

  uint8_t getCornerRadius(){
    return _cornerRadius;};

  void setSize(GCSize s){
    _frame.size = s;_needsLayout = true;};

  void setSize(int16_t w, int16_t h){
    setSize(GCSize(w,h));};

  GCSize getSize(){
    return _frame.size;};

  void setOrigin(GCPoint p);
  void setOrigin(int16_t x, int16_t y){
    setOrigin(GCPoint(x,y));};
  GCPoint getOrigin(){
    return _frame.origin;};

  void setVisualizeState(bool f){
    _visualizeState = f; setNeedsLayout();}

  void setState(State s){
    _state = s;setNeedsRedraw();}

  State getState(){
    return _state;}

  virtual void addSubview(metaView* ptr);
  void removeSubview(metaView* subView);
  void removeFromSuperview();
  metaView * getSuperview(){
    return _superView;}
  void setNeedsRedraw(){
    _needsRedraw = true;};
  void setNeedsLayout(){
    _needsLayout = true;};
  virtual GCSize intrinsicSize(){
    return GCSize();};
  virtual void redraw();
  void allignInSuperView(uint8_t allignmentMask);
  void allignInRect(uint8_t allignmentMask,GCRect r);


  #if DEBUG_LAYOUT
  bool drawDebugRect = false;
  GCRect debugRect;
  void drawDebug();
  #endif

protected:
  const vector<metaView*>::iterator findSubview(metaView* subView);
  bool childNeedsLayout();
  void redrawChildren(bool forceRedraw = false);
  void resetFlags(){
    _needsLayout = false;
    _needsRedraw = false;
  };
//  GraphicsContext _gc;
  GCRect _frame;
	uint16_t _outlineColor, _backgroundColor;  // colorization
  uint8_t _cornerRadius;                     // cornerradius
  bool _opaque;                           // draws background color
  bool _drawsOutline;
  bool _needsRedraw;                       // triggers a redraw of the View
  bool _needsLayout;                       // triggers a redraw of the superView;
  metaView* _superView;
  bool _visualizeState;                     // used by subclass to
  State _state;                           //
  vector<metaView*> _subViews;
};

/** class for displaying a simple String*/
class metaLabel : public metaView
{
public:
  metaLabel(void):metaView(),_textColor(ILI9341_GREEN),_allignmentMask(),_textPosition(),
  _insets(2,2),_textSize(3),_font(NULL),_label(NULL){
    _drawsOutline=false; _opaque=false;};

  metaLabel(const String* label,uint16_t textColor=ILI9341_GREEN):metaView(),_textColor(textColor),
  _allignmentMask(),_textPosition(),_insets(2,2),_textSize(3),_font(NULL),_label(label){
    _drawsOutline=false;_opaque=false;}

  void setFont(const ILI9341_t3_font_t *f){
    _font = f;_needsRedraw=true;GraphicsContext::setFont(_font);}

  const ILI9341_t3_font_t *getFont(){
    return _font;}

  void setTextColor(uint16_t tc){
    _textColor=tc;_needsRedraw=true;}

  uint16_t getTextColor(){
    return _textColor;};

  void setTextSize(uint8_t s){
    _textSize = s;_needsRedraw=true;GraphicsContext::setTextSize(s);};

  uint8_t getTextSize(){
    return _textSize;};

  GCPoint getTextPosition(){
    return _textPosition;};

  void setTextPosition(GCPoint p){
    _textPosition = p;};
  void setTextPosition(int16_t x, int16_t y){
    setTextPosition(GCPoint(x,y));};

  void setInsets(GCSize in){
    _insets=in;setNeedsLayout();}
  GCSize getInsets(){
    return _insets;}
  void setLabel(String* label){
    _label = label;_needsRedraw=true;};
  const String * getLabel(){return _label;}

  void setAllignmentMask(uint8_t mask){
    _allignmentMask = mask;};

  virtual void sizeToFit(){
    GCSize is = intrinsicSize();is.w+=2*_insets.w; is.h+=2*_insets.h;
    setSize(is); _textPosition.x = _insets.w; _textPosition.y = _insets.h;setNeedsLayout();}

  virtual void redraw();

  virtual GCSize intrinsicSize();

protected:
  uint16_t _textColor;        // textColor
  uint8_t  _allignmentMask;   // mask for TextAlignment;
  GCPoint  _textPosition;     // used if allignmentMask == 0
  GCSize   _insets;
  uint8_t _textSize;          // used if font == NULL
  const ILI9341_t3_font_t *_font;   //
  const String *_label;
};
/** compleate rectangular display unit with label and value
*/
class metaValue : public metaView
{
public:
  typedef struct _valueLayout{
  public:
    const ILI9341_t3_font_t *labelFont;
    const ILI9341_t3_font_t *valueFont;
    uint16_t labelColor;
    uint16_t valueColor;
    bool labelDrawOutline;
    uint8_t labelOutlineInset;
    uint8_t labelOutlineCornerRadius;
    uint8_t cornerRadius;
    uint8_t horizontalLabelInset;
    uint8_t horizontalValueInset;
    uint8_t verticalValueInset;
    _valueLayout():labelFont(&Arial_10),valueFont(&Arial_16),labelColor(ILI9341_ORANGE),
    valueColor(ILI9341_PURPLE),labelDrawOutline(false),labelOutlineInset(3),
    labelOutlineCornerRadius(10),cornerRadius(8),horizontalLabelInset(18),horizontalValueInset(18),verticalValueInset(20){};
    _valueLayout(const ILI9341_t3_font_t* lf, const ILI9341_t3_font_t* vf, uint16_t lc, uint16_t vc,
    bool ldo, uint8_t loi, uint8_t locr, uint8_t cr,uint8_t hli,uint8_t hvi,uint8_t vvi):labelFont(lf),valueFont(vf),labelColor(lc),
    valueColor(vc),labelDrawOutline(ldo),labelOutlineInset(loi),labelOutlineCornerRadius(locr),
    cornerRadius(cr),horizontalLabelInset(hli),horizontalValueInset(hvi),verticalValueInset(vvi){};

  }LayoutDefinition;

  metaValue():metaView(),_label(),_value(),_frameInset(),_labelColor(ILI9341_YELLOW),_valueColor(ILI9341_GREEN),
  _labelDrawOutline(false),_labelOutlineInset(3),_labelOutlineCornerRadius(6){};
  metaValue(String* label, String* value):metaView(),_label(label),_value(value),_frameInset(),_labelColor(ILI9341_YELLOW),
  _valueColor(ILI9341_GREEN),_labelDrawOutline(false),_labelOutlineInset(6),_labelOutlineCornerRadius(6){};

  void initValue(metaTFT* tft, GCRect frame, String* label, String* value);

  void initValue(metaTFT* tft, GCRect frame);

  void setLayout(LayoutDefinition definition){
    setLabelFont(definition.labelFont);
    setValueFont(definition.valueFont);
    setLabelColor(definition.labelColor);
    setValueColor(definition.valueColor);
    setDrawLabelOutline(definition.labelDrawOutline);
    setLabelOutlineInset(definition.labelOutlineInset);
    setLabelOulineCornerRadius(definition.labelOutlineCornerRadius);
    setCornerRadius(definition.cornerRadius);
    setHorizontalLabelInset(definition.horizontalLabelInset);
    setHorizontalValueInset(definition.horizontalValueInset);
    setVerticalValueInset(definition.verticalValueInset);
  }

  void setLabel(String* label){
    _label = label; setNeedsRedraw();};

  void setValue(String* value){
    _value = value; setNeedsRedraw();};

  void setLabelFont(const ILI9341_t3_font_t* f){
    _labelFont = f;setNeedsLayout();}
  void setValueFont(const ILI9341_t3_font_t* f){
    _valueFont = f;setNeedsLayout();}

  void setDrawLabelOutline(bool flag){
    _labelDrawOutline = flag; setNeedsRedraw();};

  bool getDrawLabelOutline(){
    return _labelDrawOutline;}

  void setLabelOutlineInset(uint8_t offset){
    _labelOutlineInset=offset;setNeedsLayout();}

  uint8_t getLabelOutlineInset(){
    return _labelOutlineInset;}

  void setLabelOulineCornerRadius(uint8_t r){
    _labelOutlineCornerRadius=r;setNeedsLayout();}
  uint8_t getLabelOutlineCornerRadius(){
    return _labelOutlineCornerRadius;}

  void setLabelColor(uint16_t c){
    _labelColor = c;_outlineColor=c; _labelView.setTextColor(c);setNeedsRedraw();}

  void setValueColor(uint16_t c){
    _valueColor = c;_valueView.setTextColor(c); setNeedsRedraw();};

  void setVerticalValueInset(uint8_t vvi){
    _verticalValueInset = vvi;}
  void setHorizontalLabelInset(uint8_t hli){
    _horizontalLabelInset = hli;}
  void setHorizontalValueInset(uint8_t hvi){
    _horizontalValueInset = hvi;}
  void valueUpdate(){
    _valueView.setNeedsRedraw();};

  virtual void redraw();
  virtual void sizeToFit();
protected:
  String * _label;
  String * _value;
  metaLabel _labelView;
  metaLabel _valueView;
  int16_t  _frameInset;

  const ILI9341_t3_font_t *_labelFont;
  const ILI9341_t3_font_t *_valueFont;
  uint16_t _labelColor;
  uint16_t _valueColor;

  bool _labelDrawOutline;
  uint8_t _labelOutlineInset;
  uint8_t _labelOutlineCornerRadius;
  uint8_t _horizontalLabelInset;
  uint8_t _horizontalValueInset;
  uint8_t _verticalValueInset;
};

class metaList : public metaView, public metaResponder{
public:
  metaList():metaView(),_cellInset(2,2),_borderInset(10,10),_lastSelectedView(NULL),_maxElementSize(){};

  void initResponder(UserEventQueue* q);
  void setBorderInset(GCSize i){_borderInset = i;setNeedsLayout();}
  GCSize getBorderInset(){return _borderInset;}


  void layoutList();
  virtual void addSubview(metaView* view);
  virtual void redraw();
  int8_t selectedIndex();
  void selectIndex(int8_t);
  virtual int16_t processEvent(UserEvent* k);
  void forgetSelection(){_lastSelectedView = NULL;}
  metaResponder* addEntry(const String *k);
protected:
  void drawConnectionFor(metaView* view, uint16_t lineColor);
  metaView* selectedSubview();
  vector<metaView*>::iterator selectedIterator();
  GCSize _cellInset;
  GCSize _borderInset;
  metaView* _lastSelectedView;
  GCSize _maxElementSize;
};
#endif

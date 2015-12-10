/**
* a here the ui elements go
*/

#ifndef __UIHELPERS_H__
#define __UIHELPERS_H__
#include "Arduino.h"
#include <vector>
#include <stack>
#include "metaTFTDisplay.h"
#include "font_Arial.h"

#define DEBUG_LAYOUT                  (0)
#define DEBUG_LAYOUT_VALUE            (0)
#define DEBUG_LAYOUT_VALUEBACKGROUND  (0 && DEBUG_LAYOUT_VALUE)
#define DEBUG_LAYOUT_VALUESIZE        (1 && DEBUG_LAYOUT_VALUE)

#define DEBUG_EVENTS       0
#define DEBUG_VALUE_EVENTS (0 && DEBUG_EVENTS)
#define DEBUG_LIST_EVENTS  (0 && DEBUG_EVENTS)

#define DEBUG_VALUE_VALUE 0
#define DEBUG_VALUE_REDRAW 0
#define DEBUG_LIST_VALUE 0
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

class UserEvent;
class metaView;
class metaValue;
typedef std::stack<metaView*,std::vector<metaView*>> ResponderStack;
/**
the Responder is the base class for all event processing related stuff
*/
typedef enum _responderResult{
  ChangedValue   = -1,
  ChangedVisual   = -1,                          ///< this result from a processEvent call will remove this responder from the responder stack.
  ChangedState    = -2,
  ChangedNothing  = -3,
  ResponderExit   = -4,
}ResponderResult;

/** simple wrapper for values */
class ValueWrapper{
 public:
  ValueWrapper(int16_t* val, int16_t min, int16_t max, String name):_value(val),_min(min),_max(max),_name(name){}

  virtual int16_t getValue(){return *_value;}
  virtual void setValue(int16_t val){
    if(val < _min){
      val = _min;
    }else if(val>_max){
      val = _max;
    }
    *_value = val;
  }

  virtual int16_t getMinValue(){return _min;}
  virtual int16_t getMaxValue(){return _max;}
  virtual String& getName(){return _name;}
  int16_t mapInto(GCSize source, GCSize target, int16_t input){
    float inp = (100.0/(source.h-source.w)) * (input -source.w);
    int16_t out = (target.h-target.w)*inp/100.0 + target.w;
    return out;
  }
  friend Print& operator<<(Print& obj,ValueWrapper* v){
    obj << "["<<v->_name<<"]{"<<*(v->_value)<<", "<<v->_min<<", "<<v->_max<<"}";
    return obj;
  }

 protected:
   int16_t* _value;
   int16_t _min;
   int16_t _max;
   String _name;
};

/** wraper for simple actions */
class metaAction{
 public:
   metaAction(metaView* mask,ValueWrapper *value);
   void operator()(void)const;
   void operator()(int16_t value)const;
   friend Print& operator<<(Print& obj, metaAction* a){
     Serial << "[Action]"<<(a->_value)<<_HEX((unsigned long)a->_mask);
     return obj;
   }
   metaView* getView(){return _mask;}
   ValueWrapper* getValue(){return _value;}
  protected:
   metaView* _mask;                         /// this is the next responder
   ValueWrapper *_value;                          /// pointer to a concrete int16_t
};


class metaResponder{
 public:

  metaResponder():_responderStack(NULL),_respondsToEvents(),_action(NULL){}
  void initResponder(ResponderStack* rS){
    _responderStack = rS;}

  virtual void setRespondsToEvents(uint16_t m){
    _respondsToEvents = m;}

  virtual uint16_t respondsToEvents(){
    return _respondsToEvents;}

  void setAction(metaAction* action){_action = action;}
  metaAction* getAction(){return _action;}

  virtual int16_t selectedIndex(){return -1;}                             /// returns the index of the selected (outlined) entry
  virtual void selectIndex(int16_t){}

  virtual int16_t activeIndex(){return -1;}
  virtual void activateIndex(int16_t){}
  virtual metaView* activeElement(){return NULL;};

  virtual int16_t processEvent(UserEvent *k){return ChangedNothing;};

  virtual void setValueWrapper(ValueWrapper* val){
    _ValueWrapper = val;
  }
  ValueWrapper* getValueWrapper(){return _ValueWrapper;}

  virtual int16_t getNumericValue(){
    if(_ValueWrapper){
      return _ValueWrapper->getValue();
    }
    return 0;
  }

  virtual void setNumericValue(int16_t k){
    if(_ValueWrapper){
      _ValueWrapper->setValue(k);
    }
  }
 protected:
    ResponderStack * _responderStack;
    uint16_t _respondsToEvents;
    metaAction* _action;
    ValueWrapper* _ValueWrapper;
};

/** baseclass for UI elements */

class metaView : public GraphicsContext, public metaResponder{
 public:

  typedef enum _states{
    Off,
    On,
    Mixed
  }State;

  typedef struct _viewLayout {
   public:
    uint16_t outlineColor;
    uint16_t backgroundColor;
    uint8_t cornerRadius;
    bool opaque;
    bool drawsOutline;
    bool visualizeState;
    State state;
    _viewLayout():outlineColor(0),backgroundColor(0),cornerRadius(0),opaque(true),drawsOutline(true),
    visualizeState(false),state(Off){};

    _viewLayout(uint16_t olc,uint16_t bc,uint8_t cr, bool op, bool dol, bool vs, State s):
    outlineColor(olc),backgroundColor(bc),cornerRadius(cr),opaque(op),drawsOutline(dol),
    visualizeState(vs),state(s){};
  }ViewLayout;

  metaView(void):_frame(),_outlineColor(0),_backgroundColor(0),_cornerRadius(0),_opaque(true),
  _drawsOutline(true),_needsRedraw(false),_needsLayout(false),_superView(NULL),_visualizeState(false),_state(Off) {}

  metaView(GCRect frame):_frame(frame),_outlineColor(0),_backgroundColor(0),_opaque(true),
  _drawsOutline(false),_needsRedraw(true),_needsLayout(true),_superView(NULL),_visualizeState(false),_state(Off){}

  virtual void initView(metaTFT* tft, GCRect frame);
  virtual void initView(metaTFT* tft, GCPoint origin, GCSize size);
  virtual void initView(metaTFT* tft, int16_t x, int16_t y, int16_t w, int16_t h);

  virtual GCPoint getScreenOrigin();

  void setLayout(ViewLayout vl);

  GCRect getBounds(){ GCRect k; k.size = getSize(); return k;}

  void setOutlineColor(uint16_t c){
    if(c!=_outlineColor){_outlineColor = c;_needsRedraw = true;}}
  uint16_t getOutlineColor(){return _outlineColor;}

  void setDrawsOutline(bool flag){
    if(flag != _drawsOutline){_drawsOutline = flag;_needsRedraw = true;}}
  bool getDrawsOutline(){return _drawsOutline;}

  void setBackgroundColor(uint16_t c){
    if(c != _backgroundColor){_backgroundColor = c;_needsRedraw=true;}}
  uint16_t getBackgroundColor(){return _backgroundColor;}

  void setOpaque(bool f){
    if(f != _opaque){_opaque = f;_needsRedraw=true;}}
  bool getOpaque(){return _opaque;}

  void setCornerRadius(uint8_t r){
    if(r != _cornerRadius){_cornerRadius = r;_needsRedraw=true;}}
  uint8_t getCornerRadius(){return _cornerRadius;};

  void setSize(GCSize s){_frame.size = s;_needsLayout = true;};
  void setSize(int16_t w, int16_t h){setSize(GCSize(w,h));};
  GCSize getSize(){return _frame.size;};

  void setOrigin(GCPoint p){
  	if(_frame.origin != p){_frame.origin = p;_needsLayout=true;}}
  void setOrigin(int16_t x, int16_t y){setOrigin(GCPoint(x,y));};
  GCPoint getOrigin(){return _frame.origin;};

  void setVisualizeState(bool f){
    if(f != _visualizeState){_visualizeState = f; setNeedsLayout();}}

  void setState(State s){
    if(s != _state){_state = s;setNeedsRedraw();}}
  State getState(){return _state;}

  virtual void addSubview(metaView* ptr);
  void removeSubview(metaView* subView);

  void removeFromSuperview();

  metaView* getSuperview(){return _superView;}

  void setNeedsRedraw(){_needsRedraw = true;}
  void setNeedsLayout(){_needsLayout = true;};

  virtual GCSize intrinsicSize(){return GCSize();};

  virtual void redraw();
  virtual void removeFromScreen();
  virtual void prepareForDisplay();

  void allignInSuperView(uint8_t allignmentMask);

  void allignInRect(uint8_t allignmentMask,GCRect r);

  virtual void sizeToFit();


  #if DEBUG_LAYOUT
  bool drawDebugRect = false;
  GCRect debugRect;
  void drawDebug();
  #endif

 public:
  const vector<metaView*>::iterator findSubview(metaView* subView);
  bool childNeedsLayout();
  void redrawChildren(bool forceRedraw = false);
  void resetFlags(){_needsLayout = false;_needsRedraw = false;};

  GCRect _frame;
	uint16_t _outlineColor, _backgroundColor;  // colorization
  uint8_t _cornerRadius;                     // cornerradius
  bool _opaque;                              // draws background color
  bool _drawsOutline;
  bool _needsRedraw;                        // triggers a redraw of the View
  bool _needsLayout;                        // triggers a redraw of the superView;
  metaView* _superView;
  bool _visualizeState;                     // used by subclass to
  State _state;                             //
  vector<metaView*> _subViews;
};

/** class for displaying a simple String*/
class metaLabel : public metaView{
  friend metaValue;
  public:
  typedef struct _labelLayout{
   public:
    ViewLayout *viewLayout;
    const ILI9341_t3_font_t *font;
    uint8_t textSize;
    uint16_t textColor;
    uint8_t  allignmentMask;   // mask for TextAlignment;
    GCPoint  textPosition;     // used if allignmentMask == 0
    GCSize   insets;
    uint8_t  indicatorSpace;
    GCSize   indicatorSize;

    _labelLayout():viewLayout(NULL),font(NULL),textSize(1),textColor(ILI9341_GREEN),
    allignmentMask(),textPosition(0,0),insets(0,0),indicatorSpace(0),indicatorSize(){};

    _labelLayout(ViewLayout *vl, ILI9341_t3_font_t* f, uint8_t ts, uint16_t tc, uint8_t am, GCPoint tp, GCSize i, uint8_t is,GCSize inSi):
    viewLayout(vl),font(f),textSize(ts),textColor(tc),allignmentMask(am),textPosition(tp),insets(i),indicatorSpace(is),indicatorSize(inSi){};

  }LabelLayout;

  metaLabel(void):metaView(),_textColor(ILI9341_GREEN),_allignmentMask(),_textPosition(),
  _insets(2,2),_indicatorSize(8,8),_indicatorSpace(16),_textSize(3),_font(NULL){
    _drawsOutline=false; _opaque=false;};

  metaLabel(const String& label,uint16_t textColor=ILI9341_GREEN):metaView(),_textColor(textColor),
  _allignmentMask(),_textPosition(),_insets(2,2),_indicatorSize(8,8),_indicatorSpace(16),_textSize(3),_font(NULL),_label(label){
    _drawsOutline=false;_opaque=false;}

  void setLayout(LabelLayout ll);

  void setFont(const ILI9341_t3_font_t *f){
    if(f != _font){_font = f;_needsRedraw=true;GraphicsContext::setFont(_font);}}

  const ILI9341_t3_font_t *getFont(){return _font;}

  void setTextColor(uint16_t tc){
    if(tc != _textColor){_textColor=tc;_needsRedraw=true;}}

  uint16_t getTextColor(){return _textColor;}

  void setTextSize(uint8_t s){
    if(s != _textSize){_textSize = s;_needsRedraw=true;GraphicsContext::setTextSize(s);}}

  uint8_t getTextSize()                     {return _textSize;}

  void setTextPosition(GCPoint p)           {_textPosition = p;};
  void setTextPosition(int16_t x, int16_t y){setTextPosition(GCPoint(x,y));};

  GCPoint getTextPosition()                 {return _textPosition;}

  void setInsets(GCSize in)                 {_insets=in;setNeedsLayout();}
  GCSize getInsets()                        {return _insets;}

  void setIndicatorSize(GCSize f)           {if(f!=_indicatorSize){_indicatorSize=f;setNeedsRedraw();}}

  GCSize getIndicatorSize()                 {return _indicatorSize;}

  void setIndicatorSpace(uint8_t s)         {if(s != _indicatorSpace){_indicatorSpace = s;setNeedsLayout();}}

  uint8_t getIndicatorSpace()               {return _indicatorSpace;}

  void setLabel(const String &label);
  const String&  getLabel()                 {return _label;}

  void setAllignmentMask(uint8_t mask)      {_allignmentMask = mask;};

  virtual void sizeToFit();

  virtual void redraw();

  virtual GCSize intrinsicSize();

  protected:
  uint16_t _textColor;        // textColor
  uint8_t  _allignmentMask;   // mask for TextAlignment;
  GCPoint  _textPosition;     // used if allignmentMask == 0
  GCSize   _insets;
  GCSize  _indicatorSize;    // frame of the Indicator rectangle
  uint8_t _indicatorSpace;    // horizontal space for the indicator
  uint8_t  _textSize;          // used if font == NULL
  const ILI9341_t3_font_t *_font;   //
  String _label = String();
};


/** compleate rectangular display unit with label and value */
class metaValue : public metaView{
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

  }ValueLayout;

  metaValue():metaView(),_frameInset(),
  _labelOutlineInset(3){};
  metaValue(String label, String value):metaView(),_frameInset(),
  _labelOutlineInset(6){};

  void initValue(metaTFT* tft, GCRect frame, String label, String value);

  void initValue(metaTFT* tft, GCRect frame);
  void setLayout(ValueLayout definition);

  GCSize resizeValue( bool dontResize=false);
  GCSize resizeLabel();
  void setLabel(String label){
    Serial<<"Value: setLabel "<<label<<endl;
    _labelView.setLabel(label);
    setNeedsRedraw();
  };

  void setValue(String label);

  void setLabelFont(const ILI9341_t3_font_t* f){
    _labelView.setFont(f);setNeedsLayout();}

  void setValueFont(const ILI9341_t3_font_t* f){
    _valueView.setFont(f);setNeedsLayout();}

  void setDrawLabelOutline(bool flag){
    _labelView.setDrawsOutline(flag); setNeedsRedraw();
  };

  bool getDrawLabelOutline(){
    return _labelView.getDrawsOutline();}

  void setLabelOutlineInset(uint8_t offset){
    _labelOutlineInset=offset;setNeedsLayout();}

  uint8_t getLabelOutlineInset(){
    return _labelOutlineInset;}

  void setLabelOulineCornerRadius(uint8_t r){
    _labelView.setCornerRadius(r);setNeedsLayout();}
  uint8_t getLabelOutlineCornerRadius(){
    return _labelView.getCornerRadius();}

  void setLabelColor(uint16_t c){
    _outlineColor=c; _labelView.setTextColor(c);_labelView.setOutlineColor(c);setNeedsRedraw();}

  void setValueColor(uint16_t c){
    _valueView.setTextColor(c); setNeedsRedraw();};

  void setVerticalValueInset(uint8_t vvi){
    _verticalValueInset = vvi;}
  void setHorizontalLabelInset(uint8_t hli){
    _horizontalLabelInset = hli;}
  void setHorizontalValueInset(uint8_t hvi){
    _horizontalValueInset = hvi;}
  void valueUpdate(){
    _valueView.setNeedsRedraw();};

  virtual void setNumericValue(int16_t k){
    _valueView._label = k;
    _valueView.setNeedsRedraw();
    metaResponder::setNumericValue(k);
  }

  virtual void setValueWrapper(ValueWrapper* valWrap){
    metaResponder::setValueWrapper(valWrap);
    setLabel(_ValueWrapper->getName());
    setValue(_ValueWrapper->getMaxValue());
    sizeToFit();
    setValue(getNumericValue());
  }

  void setProcessEvents(bool f){_processEvents=f;}
  bool getProcessEvents(){return _processEvents;}
  virtual void setRespondsToEvents(uint16_t m){}
  virtual uint16_t respondsToEvents();

  virtual int16_t processEvent(UserEvent *evnt);

  virtual void redraw();
  virtual void sizeToFit();
  virtual void prepareForDisplay();

  protected:
  GCRect valueLayoutRect();
  GCRect valueLayoutRect(GCSize fs);
  int16_t calcBaselineCorrection();
  metaLabel _labelView;
  metaLabel _valueView;
  int16_t  _frameInset;
  GCRect _lastValueRect;

  uint8_t _labelOutlineInset;
  uint8_t _horizontalLabelInset;
  uint8_t _horizontalValueInset;
  uint8_t _verticalValueInset;
  bool _processEvents;
};

class metaList : public metaView{
 public:
  metaList():metaView(),_cellInset(2,2),_borderInset(10,10),_lastSelectedView(NULL),_maxElementSize(){};
  void setBorderInset(GCSize i){_borderInset = i;setNeedsLayout();}
  GCSize getBorderInset(){return _borderInset;}

  void layoutList();
  virtual void addSubview(metaView* view);
  metaLabel* addEntry(const String);
  virtual void redraw();
  virtual void initView(metaTFT* tft, GCRect frame);
  virtual int16_t processEvent(UserEvent* k);
  virtual int16_t selectedIndex();
  virtual int16_t activeIndex();
  virtual void activateIndex(int16_t idx);
  virtual void selectIndex(int16_t idx);
  virtual metaView* activeElement();
  virtual uint16_t respondsToEvents();
  void forgetSelection(){_lastSelectedView = NULL;}
  metaView* addEntry(const String *k);
  virtual void sizeToFit();
  void setLabelLayout(metaLabel::LabelLayout *l){_ll = l;}
  void setIsSelectList(bool f){_isSelectList = f;}
  bool isSelectList(){return _isSelectList;}
 protected:
  void drawConnectionFor(metaView* view, uint16_t lineColor);
  metaView* selectedSubview();
  vector<metaView*>::iterator selectedIterator();
  vector<metaView*>::iterator onIterator();
  void clearAllStates();
  bool switchSelectedOn();
  GCSize _cellInset;
  GCSize _borderInset;
  metaView* _lastSelectedView;
  GCSize _maxElementSize;
  metaLabel::LabelLayout *_ll ;
  bool _isSelectList;
};
#endif

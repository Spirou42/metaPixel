/**
* simple container for userEvents
*/

#ifndef __USEREVENT_H__
#define __USEREVENT_H__
#include "Arduino.h"
#include "Streaming.h"
#define DEBUG_USEREVENT 0
class UserEventQueue;

class UserEvent
{
  friend UserEventQueue;
public:
  /** at the moment we have Only Key and Encoder Events */
  typedef enum _EventType{
    EventTypeKey,
    EventTypeEncoder
  } EventType;
  /** and we got 5 Buttons */
  typedef enum _ButtonID{
    Button0 = 0,
    LeftButton    = Button0,
    Button1,
    DownButton    = Button1,
    Button2,
    RightButton   = Button2,
    Button3,
    UpButton      = Button3,
    Button4,
    CenterButton  = Button4,
    ButtonMax
  }ButtonID;

  typedef enum _ButtonState{
    ButtonDown,
    ButtonUp,
    ButtonClick,
    ButtonLongClick,
    ButtonDoubleClick
  }ButtonState;

  typedef enum _EncoderID{
    Encoder0 = 0,
    EncoderMax
  }EncoderID;

  typedef enum _EncoderDirection{
    EncoderRight,
    EncoderUp     = EncoderRight,
    EncoderLeft,
    EncoerDown    = EncoderLeft,
  }EncoderDirection;

  typedef struct _ButtonData{
    ButtonID          id;
    ButtonState       state;
  }ButtonData;

  typedef struct _EncoderData{
    EncoderID         id;
    EncoderDirection  direction;
    uint8_t           steps;
    int8_t            absSteps;
    int32_t           position;
    float             speed;
  }EncoderData;

  typedef union _EventData{
    ButtonData buttonData;
    EncoderData encoderData;
  }EventData;

  typedef enum _EventMask{
    ButtonEvents            = (1 << 0),
    EncoderEvents           = (1 << 1),
    ButtonEvent_Left        = (1 << 2),
    ButtonEvent_Down        = (1 << 3),
    ButtonEvent_Right       = (1 << 4),
    ButtonEvent_Up          = (1 << 5),
    ButtonEvent_Center      = (1 << 6),
    ButtonState_Down        = (1 << 7),
    ButtonState_Up          = (1 << 8),
    ButtonState_Click       = (1 << 9),
    ButtonState_LongClick   = (1 << 10),
    ButtonState_DoubleClick = (1 << 11),
    ButtonState_All         = (ButtonState_Down | ButtonState_Up | ButtonState_Click | ButtonState_LongClick | ButtonState_DoubleClick),
    ButtonState_AllClick    = (ButtonState_Click | ButtonState_DoubleClick | ButtonState_LongClick),

  }EventMask;

  UserEvent(EventType type):_type(type){};

  EventType getType(){
    return _type;};

  void setType(EventType t){
    _type = t;}

  EventData getData(){
    return _data;}

  void setData(EventData d){
    _data = d;}

  friend Print& operator<<(Print& out,UserEvent* event){
    out << "Event{"<<event->_type<<", ";
    switch(event->_type){
      case EventTypeKey:      out<<event->_data.buttonData;break;
      case EventTypeEncoder:  out<<event->_data.encoderData;break;
      default: out<<"??"; Serial << "Type: "<<(int)event->_type<< endl;break;
    }
    out << "}";
    return out;}

  friend Print& operator<<(Print& out,_EventType type){
    switch(type){
      case EventTypeKey: out << "Key";break;
      case EventTypeEncoder: out << "Enc";break;
    }
    return out;}

  friend Print& operator<<(Print& out, _ButtonData data){
    out << data.id<<", "<<data.state;
    return out;}

  friend Print& operator<<(Print& out, _ButtonID id){
    switch(id){
      case LeftButton:    out << "[L]";break;
      case UpButton:      out << "[U]";break;
      case DownButton:    out << "[D]";break;
      case RightButton:   out << "[R]";break;
      case CenterButton:  out << "[C]";break;
      default: break;
    }
    return out;}

  friend Print& operator<<(Print& out, _ButtonState state){
    switch(state){
      case ButtonDown:        out<<"Down";  break;
      case ButtonUp:          out<<"Up";    break;
      case ButtonClick:       out<<"Click"; break;
      case ButtonLongClick:   out<<"Long";  break;
      case ButtonDoubleClick: out<<"Double";break;
    }
    return out;}

  friend Print& operator<<(Print& out, _EncoderData data){
    out << data.id <<", "<<data.direction<<", "<<data.steps<<", "<<data.position<<", "<<data.speed;
    return out;}

  friend Print& operator<<(Print& out, _EncoderID id){
    switch(id){
      case Encoder0:  out << "[0]";break;
      default: break;
    }
    return out;}

  friend Print& operator<<(Print& out, _EncoderDirection dir){
      switch(dir){
        case EncoderLeft:   out <<"Left";   break;
        case EncoderRight:  out <<"Right";  break;
      }
    return out;}
  uint16_t eventMask();
protected:
  UserEvent *nextEvent;
  EventType _type;
  EventData _data;
  unsigned long timeStamp;
};



class UserEventQueue
{
public:
  UserEventQueue():_queueStart(NULL),_queueEnd(NULL),_queueLength(0){};
  void addEvent(UserEvent* event);
  UserEvent* popEvent();

  volatile size_t length(){
    return _queueLength;}

  UserEvent*  getStart(){
    return _queueStart;}

  UserEvent*  getEnd(){
    return _queueEnd;}

protected:
  UserEvent*        _queueStart;
  UserEvent*        _queueEnd;
  volatile size_t   _queueLength;
  bool     consolidateEvent(UserEvent* evnt);
};
#endif

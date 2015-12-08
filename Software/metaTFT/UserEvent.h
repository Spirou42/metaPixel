/**
* simple container for userEvents
*/

#ifndef __USEREVENT_H__
#define __USEREVENT_H__
#include "Arduino.h"
#include "Streaming.h"
#define DEBUG_USEREVENT 0
class UserEventQueue;

/** at the moment we have Only Key and Encoder Events */
typedef enum _EventType{
  EventTypeButton,
  EventTypeEncoder

} EventType;

Print& operator<<(Print& out,EventType type);

/** and we got 5 Buttons */
typedef enum _ButtonID{
  NoButton = -1,
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
Print& operator<<(Print& out, ButtonID id);

typedef enum _ButtonState{
  NoState = -1,
  ButtonDown,
  ButtonUp,
  ButtonClick,
  ButtonLongClick,
  ButtonDoubleClick
}ButtonState;
Print& operator<<(Print& out, ButtonState state);

typedef enum _EncoderID{
  NoEncoder = -1,
  Encoder0 = 0,
  EncoderMax
}EncoderID;
Print& operator<<(Print& out, EncoderID id);

typedef enum _EncoderDirection{
  NoDirection = -1,
  EncoderRight,
  EncoderUp     = EncoderRight,
  EncoderLeft,
  EncoerDown    = EncoderLeft,
}EncoderDirection;
Print& operator<<(Print& out, EncoderDirection dir);

typedef struct _ButtonData{
  ButtonID          id;
  ButtonState       state;
}ButtonData;
Print& operator<<(Print& out, ButtonData data);

typedef struct _EncoderData{
  EncoderID         id;
  EncoderDirection  direction;
  uint8_t           steps;
  int8_t            absSteps;
  int32_t           position;
  float             speed;
}EncoderData;
Print& operator<<(Print& out, EncoderData data);


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


class UserEvent
{
  friend UserEventQueue;
public:


  UserEvent(EventType type):_type(type){};

  EventType getType(){
    return _type;};

  void setType(EventType t){
    _type = t;}

  EventData getData(){
    return _data;}

  void setData(EventData d){
    _data = d;}


  uint16_t eventMask();
  bool matchesMask(uint16_t mask);
  ButtonID getButtonID();
  ButtonState getButtonState();
  int16_t getAbsEncoderSteps(){
    return _data.encoderData.absSteps;
  }

  float getEncoderSpeed(){
    return _data.encoderData.speed;
  }

  friend Print& operator<<(Print& out,UserEvent* event){
    out << "Event{"<<event->_type<<", ";
    switch(event->_type){
      case EventTypeButton:      out<<event->_data.buttonData;break;
      case EventTypeEncoder:  out<<event->_data.encoderData;break;
      default: out<<"??"; Serial << "Type: "<<(int)event->_type<< endl;break;
    }
    out << "}";
    return out;}

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

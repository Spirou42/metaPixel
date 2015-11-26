/**
* Generic Switch Class, maniging states and sending events
* Will be triggered by interrupt
*/


#ifndef __Switch_H__
#define __Switch_H__
#include "Arduino.h"
#include "UserEvent.h"

#define DOUBLE_CLICK_TIME 620
#define LONG_CLICK_TIME   320
#define CLICK_TIME        60

class Switch
{
public:
  Switch(UserEvent::ButtonID button,UserEventQueue* eventC,uint8_t pin,size_t idx):
  _id(button),_eventQueue(eventC),_pin(pin),_idx(idx),_downTimer(0),_upTimer(0),_sendUpEvent(true),_sendDownEvent(true),_sendClickEvent(true),_sendDoubleClickEvent(true),_sendLongClickEvent(true),_isDouble(false){};
  void setSendUpEvent(boolean f){_sendUpEvent=f;}
  void setSendDownEvent(boolean f){_sendDownEvent=f;}
  void setSendClickEvent(boolean f){_sendClickEvent=f;}
  void setSendDoubleClickEvent(boolean f){_sendDoubleClickEvent=f;}
  void setSendLongClickEvent(boolean f){_sendLongClickEvent=f;}
  boolean getSendUpEvent(){return _sendUpEvent;}
  boolean getSendDownEvent(){return _sendDownEvent;}
  boolean getSendClickEvent(){return _sendClickEvent;}
  boolean getSendDoubleClickEvent(){return _sendDoubleClickEvent;}
  boolean getSendLongClickEvent(){return _sendLongClickEvent;}

  uint8_t getPin(){return _pin;}

  void enable();
  void disable();

  void doSwitchDown();
  void doSwitchUp();
protected:
  UserEvent::ButtonID _id;
  UserEventQueue *_eventQueue;
  uint8_t         _pin;
  size_t          _idx;
  elapsedMillis _downTimer;
  elapsedMillis _upTimer;
  // send event flags;
  boolean _sendUpEvent            : 1;
  boolean _sendDownEvent          : 1;
  boolean _sendClickEvent         : 1;
  boolean _sendDoubleClickEvent   : 1;
  boolean _sendLongClickEvent     : 1;
  boolean _isDouble               : 1;
  UserEvent::EventData buttonData(UserEvent::ButtonState state);
};
#endif

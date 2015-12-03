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
  void setSendUpEvent(bool f){_sendUpEvent=f;}
  void setSendDownEvent(bool f){_sendDownEvent=f;}
  void setSendClickEvent(bool f){_sendClickEvent=f;}
  void setSendDoubleClickEvent(bool f){_sendDoubleClickEvent=f;}
  void setSendLongClickEvent(bool f){_sendLongClickEvent=f;}
  bool getSendUpEvent(){return _sendUpEvent;}
  bool getSendDownEvent(){return _sendDownEvent;}
  bool getSendClickEvent(){return _sendClickEvent;}
  bool getSendDoubleClickEvent(){return _sendDoubleClickEvent;}
  bool getSendLongClickEvent(){return _sendLongClickEvent;}

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
  bool _sendUpEvent            : 1;
  bool _sendDownEvent          : 1;
  bool _sendClickEvent         : 1;
  bool _sendDoubleClickEvent   : 1;
  bool _sendLongClickEvent     : 1;
  bool _isDouble               : 1;
  UserEvent::EventData buttonData(UserEvent::ButtonState state);
};
#endif

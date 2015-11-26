/**
* Implementation of Switch
*/

#include "Switch.h"
#include "UserEvent.h"
#include "Switches.h"

void Switch::doSwitchDown()
{
  _downTimer = 0;
  if(_sendDownEvent && _eventQueue){
    // create an event
    UserEvent *downEvent = new UserEvent(UserEvent::EventType::EventTypeKey);
    UserEvent::EventData data = buttonData(UserEvent::ButtonState::ButtonDown);
    downEvent->setData(data);
    _eventQueue->addEvent(downEvent);
  }
  if(_upTimer<=DOUBLE_CLICK_TIME){
    _isDouble = true;
  }
}

void Switch::doSwitchUp()
{
  _upTimer = 0;
  if(_eventQueue){
    if(_sendUpEvent){
        UserEvent *upEvent = new UserEvent(UserEvent::EventType::EventTypeKey);
        UserEvent::EventData data = buttonData(UserEvent::ButtonState::ButtonUp);
        upEvent->setData(data);
        _eventQueue->addEvent(upEvent);
    }
    if(_isDouble && _sendDoubleClickEvent){
      _isDouble = false;
      UserEvent * clickEvent = new UserEvent(UserEvent::EventType::EventTypeKey);
      UserEvent::EventData data = buttonData(UserEvent::ButtonState::ButtonDoubleClick);
      clickEvent->setData(data);
      _eventQueue->addEvent(clickEvent);
    }else if( (_downTimer>LONG_CLICK_TIME) && _sendLongClickEvent ){
      UserEvent * clickEvent = new UserEvent(UserEvent::EventType::EventTypeKey);
      UserEvent::EventData data = buttonData(UserEvent::ButtonState::ButtonLongClick);
      clickEvent->setData(data);
      _eventQueue->addEvent(clickEvent);
    }else if( (_downTimer<LONG_CLICK_TIME) && _sendLongClickEvent){
      UserEvent * clickEvent = new UserEvent(UserEvent::EventType::EventTypeKey);
      UserEvent::EventData data = buttonData(UserEvent::ButtonState::ButtonClick);
      clickEvent->setData(data);
      _eventQueue->addEvent(clickEvent);
    }
  }
}

UserEvent::EventData Switch::buttonData(UserEvent::ButtonState state)
{
  UserEvent::EventData result;
  UserEvent::ButtonData data;
  data.id = this->_id;
  data.state = state;
  result.buttonData = data;
//  Serial << data<<endl;
  return result;
}

void Switch::enable()
{
  pinMode(_pin,INPUT_PULLUP);
  attachInterrupt(_pin,switchHandlers[_idx],CHANGE);

}

void Switch::disable()
{
  detachInterrupt(_pin);
}

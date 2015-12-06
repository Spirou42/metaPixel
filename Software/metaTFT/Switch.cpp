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
    UserEvent *downEvent = new UserEvent(EventType::EventTypeButton);
    EventData data = buttonData(ButtonState::ButtonDown);
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
        UserEvent *upEvent = new UserEvent(EventType::EventTypeButton);
        EventData data = buttonData(ButtonState::ButtonUp);
        upEvent->setData(data);
        _eventQueue->addEvent(upEvent);
    }
    if(_isDouble && _sendDoubleClickEvent){
      _isDouble = false;
      UserEvent * clickEvent = new UserEvent(EventType::EventTypeButton);
      EventData data = buttonData(ButtonState::ButtonDoubleClick);
      clickEvent->setData(data);
      _eventQueue->addEvent(clickEvent);
    }else if( (_downTimer>LONG_CLICK_TIME) && _sendLongClickEvent ){
      UserEvent * clickEvent = new UserEvent(EventType::EventTypeButton);
      EventData data = buttonData(ButtonState::ButtonLongClick);
      clickEvent->setData(data);
      _eventQueue->addEvent(clickEvent);
    }else if( (_downTimer<LONG_CLICK_TIME) && _sendLongClickEvent){
      UserEvent * clickEvent = new UserEvent(EventType::EventTypeButton);
      EventData data = buttonData(ButtonState::ButtonClick);
      clickEvent->setData(data);
      _eventQueue->addEvent(clickEvent);
    }
  }
}

EventData Switch::buttonData(ButtonState state)
{
  EventData result;
  ButtonData data;
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

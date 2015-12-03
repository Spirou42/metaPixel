/**
* implmentation of UserEvents*/

#include "UserEvent.h"

uint16_t UserEvent::eventMask()
{
  uint16_t result = 0;
  switch(_type){
    case EventType::EventTypeEncoder: result |=EventMask::EncoderEvents; break;
    case EventType::EventTypeKey: {
      result |= EventMask::ButtonEvents;
      ButtonData bData = getData().buttonData;
      switch(bData.id){
        case ButtonID::LeftButton: result |= EventMask::ButtonEvent_Left; break;
        case ButtonID::RightButton: result |= EventMask::ButtonEvent_Right; break;
        case ButtonID::ButtonMax:
        case ButtonID::CenterButton: result |= EventMask::ButtonEvent_Center; break;
        case ButtonID::UpButton: result |= EventMask::ButtonEvent_Up; break;
        case ButtonID::DownButton: result |= EventMask::ButtonEvent_Down; break;
      }
      switch(bData.state){
        case ButtonState::ButtonDown: result |= EventMask::ButtonState_Down; break;
        case ButtonState::ButtonUp: result |= EventMask::ButtonState_Up; break;
        case ButtonState::ButtonClick: result |= EventMask::ButtonState_Click; break;
        case ButtonState::ButtonLongClick: result |= EventMask::ButtonState_LongClick; break;
        case ButtonState::ButtonDoubleClick: result |= EventMask::ButtonState_DoubleClick; break;
      }
    } break;
  }
  return result;
}


void UserEventQueue::addEvent(UserEvent* evnt)
{
  #if DEBUG_USEREVENT
  Serial <<"addUserEvent 0x"<<_HEX((unsigned long)evnt)<<endl;
  #endif
  __disable_irq();
  #if DEBUG_USEREVENT
  bool consolidated = false;
  #endif
  if(!_queueStart){
    evnt->nextEvent=NULL;
    _queueStart = evnt;
    _queueEnd = evnt;
    _queueLength = 1;
  }else{
    //
    if(consolidateEvent(evnt)){
      #if DEBUG_USEREVENT
      consolidated = true;
      #endif
      delete evnt;
    }else{
      _queueEnd->nextEvent=evnt;
      _queueEnd = evnt;
      evnt->nextEvent = NULL;
      _queueLength++;
    }
  }
  #if DEBUG_USEREVENT
  if(consolidated){
    Serial << "Event consolideted"<<endl;
  }else{
    Serial <<"addedUserEvent "<<evnt<<" L: "<<_queueLength<<endl;
  }
  #endif
  __enable_irq();
}

bool UserEventQueue::consolidateEvent(UserEvent* evnt)
{
  if( (evnt->getType() == _queueEnd->getType()) &&
      (evnt->getType() == UserEvent::EventType::EventTypeEncoder)){
    UserEvent::EventData dOld = _queueEnd->getData();
    UserEvent::EventData dNew = evnt->getData();
    if( (dOld.encoderData.direction == dNew.encoderData.direction) &&
        (dOld.encoderData.id == dNew.encoderData.id)){
      dOld.encoderData.steps ++;
      _queueEnd->setData(dOld);
      return true;
    }
  }else if( (evnt->getType() == UserEvent::EventType::EventTypeKey) &&
            (_queueEnd->getType() == UserEvent::EventType::EventTypeKey)){
    UserEvent::EventData dOld = _queueEnd->getData();
    UserEvent::EventData dNew = evnt->getData();
    if( (dOld.buttonData.id == dNew.buttonData.id) &&
        (dOld.buttonData.state == UserEvent::ButtonState::ButtonClick) &&
        (dNew.buttonData.state == UserEvent::ButtonState::ButtonDoubleClick)){
      dOld.buttonData.state = UserEvent::ButtonState::ButtonDoubleClick;
      _queueEnd->setData(dOld);
      return true;

    }

  }

  return false;
}

UserEvent *UserEventQueue::popEvent()
{
  __disable_irq();
  UserEvent* evnt = _queueStart;
  if (evnt) {

    _queueStart = evnt->nextEvent;
    if (_queueStart == NULL) {
      _queueEnd = NULL;
      _queueLength = 0;
    }
  }
  __enable_irq();
  #if DEBUG_USEREVENT
  if(evnt){
    Serial << "pop evnt 0x"<<_HEX((unsigned long)evnt)<<endl;
  }
  #endif
  return evnt;
}

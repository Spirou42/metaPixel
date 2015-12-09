/**
* implmentation of UserEvents*/

#include "UserEvent.h"

Print& operator<<(Print& out,EventType type){
  switch(type){
    case EventTypeButton: out << "Key";break;
    case EventTypeEncoder: out << "Enc";break;
  }
  return out;
}
Print& operator<<(Print& out, ButtonID id){
  switch(id){
    case LeftButton:    out << "[L]";break;
    case UpButton:      out << "[U]";break;
    case DownButton:    out << "[D]";break;
    case RightButton:   out << "[R]";break;
    case CenterButton:  out << "[C]";break;
    default: break;
  }
  return out;
}
Print& operator<<(Print& out, ButtonState state){
  switch(state){
    case NoState:           out<<"None";  break;
    case ButtonDown:        out<<"Down";  break;
    case ButtonUp:          out<<"Up";    break;
    case ButtonClick:       out<<"Click"; break;
    case ButtonLongClick:   out<<"Long";  break;
    case ButtonDoubleClick: out<<"Double";break;
  }
  return out;
}
Print& operator<<(Print& out, EncoderID id){
  switch(id){
    case Encoder0:  out << "[0]";break;
    default: break;
  }
  return out;
}

Print& operator<<(Print& out, EncoderDirection dir){
    switch(dir){
      case NoDirection:   out<<"Undefined"; break;
      case EncoderLeft:   out <<"Left";   break;
      case EncoderRight:  out <<"Right";  break;
    }
  return out;
}
Print& operator<<(Print& out, ButtonData data){
  out << data.id<<", "<<data.state;
  return out;
}
Print& operator<<(Print& out, EncoderData data){
  out << data.id <<", "<<data.direction<<", "<<data.steps<<", "<<data.position<<", "<<data.speed;
  return out;
}


uint16_t UserEvent::eventMask()
{
  uint16_t result = 0;
  switch(_type){
    case EventType::EventTypeEncoder:
      result |= EventMask::EncoderEvents;
      break;
    case EventType::EventTypeButton: {
      result |= EventMask::ButtonEvents;
      ButtonData bData = getData().buttonData;
      switch(bData.id){
        case ButtonID::LeftButton: result |= EventMask::ButtonEvent_Left; break;
        case ButtonID::RightButton: result |= EventMask::ButtonEvent_Right; break;
        case ButtonID::ButtonMax:
        case ButtonID::CenterButton: result |= EventMask::ButtonEvent_Center; break;
        case ButtonID::UpButton: result |= EventMask::ButtonEvent_Up; break;
        case ButtonID::DownButton: result |= EventMask::ButtonEvent_Down; break;
        default: break;
      }
      switch(bData.state){
        case ButtonState::ButtonDown: result |= EventMask::ButtonState_Down; break;
        case ButtonState::ButtonUp: result |= EventMask::ButtonState_Up; break;
        case ButtonState::ButtonClick: result |= EventMask::ButtonState_Click; break;
        case ButtonState::ButtonLongClick: result |= EventMask::ButtonState_LongClick; break;
        case ButtonState::ButtonDoubleClick: result |= EventMask::ButtonState_DoubleClick; break;
        default: break;
      }
    }
    break;
  }

  return result;
}

bool UserEvent::matchesMask(uint16_t mask){
  bool result = false;
  uint16_t myMask = eventMask();
  bool typeMatch=false,buttonMatch=false,stateMatch=false;
  typeMatch = ((myMask & 0x3)&(mask & 0x3))!=0;

  if(typeMatch && ((myMask & EventMask::EncoderEvents)!=0) ){
    return true;
  }
  mask = mask >> 2;
  myMask = myMask >> 2;

  buttonMatch = ((myMask & 0x1F)&(mask &0x1f))!= 0;

  mask = mask >> 5;
  myMask = myMask >> 5;
  stateMatch = ((myMask & 0x1F)&(mask &0x1f))!= 0;

  result = typeMatch && buttonMatch && stateMatch;

  return result;
}

ButtonID UserEvent::getButtonID(){
  if(_type != EventTypeButton){
    return NoButton;
  }
  return _data.buttonData.id;
}

ButtonState UserEvent::getButtonState(){
  if(_type !=EventTypeButton){
    return NoState;
  }
  return _data.buttonData.state;
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
      (evnt->getType() == EventType::EventTypeEncoder)){
    EventData dOld = _queueEnd->getData();
    EventData dNew = evnt->getData();
    if( (dOld.encoderData.direction == dNew.encoderData.direction) &&
        (dOld.encoderData.id == dNew.encoderData.id)){
      dOld.encoderData.steps ++;
      dOld.encoderData.absSteps +=dNew.encoderData.absSteps;
      _queueEnd->setData(dOld);
      return true;
    }
  }else if( (evnt->getType() == EventType::EventTypeButton) &&
            (_queueEnd->getType() == EventType::EventTypeButton)){
    EventData dOld = _queueEnd->getData();
    EventData dNew = evnt->getData();
    if( (dOld.buttonData.id == dNew.buttonData.id) &&
        (dOld.buttonData.state == ButtonState::ButtonClick) &&
        (dNew.buttonData.state == ButtonState::ButtonDoubleClick)){
      dOld.buttonData.state = ButtonState::ButtonDoubleClick;
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

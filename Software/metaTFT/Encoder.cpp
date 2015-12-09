/**
* encoder Implementation
*/

#include "Encoder.h"
#include "Streaming.h"
#include "Encoders.h"
void Encoder::enable()
{
  initialState();
  attachInterrupt(_aPin,encoderHandlers[_id],CHANGE);
  attachInterrupt(_bPin,encoderHandlers[_id],CHANGE);
}

void Encoder::disable()
{
  detachInterrupt(_aPin);
  detachInterrupt(_bPin);
}

EventData Encoder::encoderData(bool state,int8_t steps,float vel)
{
  EventData result;
  EncoderData data;
  data.id = this->_id;
  data.direction = state?EncoderDirection::EncoderRight:EncoderDirection::EncoderLeft;
  data.steps = abs(steps);
  data.absSteps = steps;
  data.position = _position;
  data.speed = vel;
  result.encoderData = data;
  //Serial << data<<endl;
  return result;
}


void Encoder::doEncoderStep()
{
  uint32_t wallClock = millis();
  uint8_t p1val = digitalReadFast(_aPin);
  uint8_t p2val = digitalReadFast(_bPin);
  uint8_t s = _state & 3;
  if (p1val) s |= 4;
  if (p2val) s |= 8;
  _state = (s >> 2);
  int8_t step = 0;
  bool shouldCountPhase = true;
  float velocity = 0;
  if(_divider){
    shouldCountPhase = false;
    --_idiv ;
    if(_idiv == 0){
      _idiv =_divider;
      shouldCountPhase = true;
    }
    velocity = (60.0/30.0) / ((wallClock - _lastTick)/1000.0);
  }
  if(shouldCountPhase){
    switch (s) {
      case 1: case 7: case 8: case 14:
      step = +1;
      break;
      case 2: case 4: case 11: case 13:
      step = -1;
      break;
      case 3: case 12:
      step = +2;
      break;
      case 6: case 9:
      step = -2;
      break;
    }
    if(step != 0){
      _position += step;
      //Serial << "Encoder: "<<_id<< "got a Step in "<<(step>0)<<endl;
      UserEvent *evnt = new UserEvent(EventType::EventTypeEncoder);
      EventData data = encoderData(step>0,step,velocity);
      _lastTick = wallClock;
      evnt->setData(data);
      _eventQueue->addEvent(evnt);
    }
  }
}

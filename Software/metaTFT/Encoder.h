/**
* a simple encoder class sending step diretion events
*/

#ifndef __ENCODER_H__
#define __ENCODER_H__
#include "UserEvent.h"


class Encoder
{
public:
  Encoder(UserEvent::EncoderID encoder, UserEventQueue* eventC,uint8_t aPin, uint8_t bPin, size_t idx, uint8_t div=0):
  _id(encoder),_eventQueue(eventC),_aPin(aPin),_bPin(bPin),_divider(div),_idx(idx)
  {
    pinMode(aPin, INPUT_PULLUP);
    pinMode(bPin, INPUT_PULLUP);
    _position = 0;
    _idiv = _divider ;
    uint8_t s = 0;
    if(digitalReadFast(_aPin))
      s |= 1;
    if(digitalReadFast(_bPin))
      s |= 2;
    _state = s;
    _lastTick = millis();
  }

  int32_t getPosition(){return _position;}
  void setPosition(int32_t p){_position = p;}
  void enable();
  void disable();

  void doEncoderStep();

protected:
  UserEvent::EncoderID  _id;                  ///< id of the encoder
  UserEventQueue        *_eventQueue;         ///< ref to the eventQueue to use
  uint8_t               _aPin;                ///< A output of the encoder
  uint8_t               _bPin;                ///< B output of the encoder
  uint8_t               _state;               ///< internal state of the encoder
  //	new	     new	     old       old
  //	pin2     pin1	     pin2	     pin1	    Result
  //	----     ----      ----      ----     ------
  //	 0	      0       	0        	0       no movement
  //	 0	      0	        0	        1	      +1
  //	 0	      0	        1	        0	      -1
  //	 0	      0	        1	        1	      +2  (assume pin1 edges only)
  //	 0	      1	        0	        0	      -1
  //	 0	      1	        0	        1	      no movement
  //	 0	      1	        1	        0	      -2  (assume pin1 edges only)
  //	 0	      1	        1	        1	      +1
  //	 1	      0	        0	        0	      +1
  //	 1	      0	        0	        1	      -2  (assume pin1 edges only)
  //	 1	      0	        1	        0	      no movement
  //	 1	      0	        1	        1	      -1
  //	 1	      1	        0	        0	      +2  (assume pin1 edges only)
  //	 1	      1	        0	        1	      -1
  //	 1	      1	        1	        0	      +1
  //	 1	      1	        1	        1	      no movement
  int32_t               _position;            ///< internal position
  uint8_t               _divider;             ///< internal phase division useful to map the encoder phases to actual nodges
  uint8_t               _idiv;
  size_t                _idx;
  unsigned long         _lastTick;            ///< the wallclock at the last registered tick
  UserEvent::EventData encoderData(bool direction,int8_t step,float vel);
  void initialState(){
    uint8_t s = 0;
    if(digitalReadFast(_aPin))
      s |= 1;
    if(digitalReadFast(_bPin))
      s |= 2;
    _state = s;
  }
};
#endif

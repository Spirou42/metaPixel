/**
	@description: simple class of int16_ts for synchronizing interrupt based value changes
*/
#ifndef __PARAMETER_H__
#define __PARAMETER_H__
#include <Streaming.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "VT100Stream.h"
//#include "Arduino.h"

class AnimationValue {
public:
	int16_t _value;
	volatile int16_t _tempValue;
	bool _shouldAnimate;
	bool _bounce;
	bool _stopNext;
	bool _lastDirection;
	int16_t _targetValue;
	int16_t _startValue;
	elapsedMillis _sinceLast;
	unsigned long _tmilli;
	unsigned long _smilli;
public:
	AnimationValue() :_value(0),_tempValue(0),_shouldAnimate(false),_bounce(false),_stopNext(false){}
	AnimationValue(int16_t a):_value(a),_tempValue(a),_shouldAnimate(false),_bounce(false),_stopNext(false){}

	void SetValue(int16_t newValue)
	{
		_tempValue = newValue;
	}

	int16_t GetValue(){
		return _value;
	}
	bool hasChanged(){ return (this->_value != this->_tempValue);}

	void setStopNext(bool flag){this->_stopNext = flag;}

	bool getStopNext(){return this->_stopNext;}
	bool isAnimating(){return this->_shouldAnimate;}
	bool syncValue()
	{
		if(this->_value != this->_tempValue){
			_value = _tempValue;
			return true;
		}else{
			return false;
		}
	}
	void initTo(int16_t a){
		this->_value = a;
		this->_tempValue = a;
		_shouldAnimate = false;
	}

	void animateTo(int16_t target, unsigned long milli)
	{
	  _targetValue = target;
		_startValue = _tempValue;
	  _shouldAnimate = true;
		_bounce = false;
	  _smilli = _tmilli = milli;
	  _sinceLast = 0;
		_lastDirection = this->_targetValue<this->_value;
	}

	void bounce(int16_t target, unsigned long milli)
	{
		_startValue = _tempValue;
		_smilli = milli;
		animateTo(target,milli);
		_bounce = true;
	}
	void animateParameter()
	{
		//static uint8_t lineOffset = 0;
	  unsigned long k = _sinceLast;
	  unsigned long frac = (k*65536/_tmilli);
		int16_t pValue=0;
		//char idx = ' ';
		if((frac<65535)){
			//idx = 'B';
		}else{
			//idx = 'G';
			frac = 1;
		}
		uint16_t distanceToGo=0;
		bool currentDirection = false;
		if(this->_targetValue>this->_value){
			distanceToGo = this->_targetValue - this->_value;
		}else{
			distanceToGo = this->_value - this->_targetValue;
			currentDirection = true;
		}
		pValue = scale16(distanceToGo,frac);
		if(currentDirection){
			this->_tempValue = _value - pValue;
		}else{
			this->_tempValue = _value + pValue;
		}

		if(currentDirection != this->_lastDirection){
			if(_stopNext){
				this->_shouldAnimate = false;
			}
			_lastDirection = currentDirection;
		}


		//Serial <<ScreenPos(10+lineOffset,0)<<clearLineRight<<"k: "<<k<<" Frac "<<frac<<"  StepVal: "<<pValue<<" cValue:"<<this->_value<< " tValue:"<<this->_tempValue<<"  Lmillis:"<<this->_tmilli<<endl;

		//lineOffset = (lineOffset+1)%10;

		if(pValue != 0){
			if(this->_tmilli > this->_sinceLast){
				this->_tmilli -= this->_sinceLast;
			}else{
				this->_tempValue = this->_targetValue;
				this->_shouldAnimate = false;
			}
			this->_sinceLast = 0;
		}else if(this->_tmilli < this->_sinceLast){
			this->_tempValue = this->_targetValue;
			this->_shouldAnimate = false;
		}
		if((this->_tempValue == this->_targetValue) || (this->_tmilli==0)){
			this->_shouldAnimate = false;
		}
		if ((this->_shouldAnimate == false) && this->_bounce && !this->_stopNext) {
			bounce(_startValue,_smilli);
		}
		if(!this->_shouldAnimate){
			this->_stopNext = false;
		}
	}

	int16_t currentValue(){return _value;}
	int16_t nextValue(){return _tempValue;}
	bool operator==(const AnimationValue a)
	{
		return (this->_value == a._value);
	}
	AnimationValue* operator=(const AnimationValue a){
		this->_tempValue = a._value;
		return this;
	}
	int16_t operator=(const int16_t a){
		this->_tempValue = a;
		return a;
	}
	friend Print &operator<<(Print &output, AnimationValue d){
		output<<""<<d.currentValue()<<"";
		return output;
	}
};

// typedef struct {
// 	char code;					// char, naming the parameter
// 	int16_t min;				// min value of the parameter
// 	int16_t max;				// max value of the parameter
// 	int8_t  step;				// value change for
// 	int16_t *value;
// 	volatile int16_t *tempValue;
// } parameter_t;



class Parameter16_t{
public:
	char code;					// char, naming the parameter
	int16_t minValue;							// min value of the parameter
	int16_t maxValue;							// max value of the parameter
	AnimationValue *value;
	Parameter16_t():code(0x00),minValue(int16_t()),maxValue(int16_t()),value(NULL){};
	Parameter16_t(char c, int16_t minVal, int16_t maxVal,AnimationValue* val): code(c),minValue(minVal),maxValue(maxVal),value(val){};
	int16_t clampValue(int16_t v){
		if(v<minValue){
			v = minValue;
		}else if(v>maxValue){
			v = maxValue;
		}
		return v;
	}
	void setValue(int16_t v){
		value->SetValue(clampValue(v));
	};
	String * getCommandString(){
		String *result =NULL;
		if(value->_shouldAnimate){
			if(value->_bounce){
				result = new String("!");
			}else{
				result = new String("#");
			}
			(*result) += code;
			(*result) += value->_startValue;
			(*result) += " ";
			(*result) += value->_targetValue;
			(*result) += " ";
			(*result) += value->_smilli/1000;
		}else
		{
			result = new String(code);
			*result+=value->currentValue();
		}
		return result;
	}
} ;
extern Parameter16_t parameterArray[];
extern int16_t parameterArraySize;


inline Print& operator<<(Print& obj, Parameter16_t k){
	char p = k.value->_shouldAnimate?'*':' ';
	obj<<light<<'['<<k.code<<"]"<<p<<k.value->currentValue()<<" ("<<k.maxValue<<')'<<normal;
	return obj;
}

class CommandString
{
public:
	Parameter16_t *parameter;
	CommandString():parameter(NULL){};
	CommandString(Parameter16_t *k):parameter(k){};
	friend Print &operator<<(Print &obj,CommandString p){
		String *k = p.parameter->getCommandString();
		obj << (char*)k;
		return obj;
	};
};

//extern parameter_t pixelParameters[];
//extern uint8_t numetaModuleParams;

#endif

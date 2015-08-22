/**
	@description: simple class of int16_ts for synchronizing interrupt based value changes
*/
#ifndef __PARAMETER_H__
#define __PARAMETER_H__
#include <Streaming.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
//template <class T>

class AnimationValue {
public:
	int16_t _value;
	volatile int16_t _tempValue;
	bool _shouldAnimate;
	int16_t _targetValue;
	elapsedMillis _sinceLast;
	unsigned long _tmilli;
public:
	AnimationValue() :_value(0),_tempValue(0){}
	AnimationValue(int16_t a):_value(a),_tempValue(a){}

	void SetValue(int16_t newValue)
	{
		_tempValue = newValue;
	}

	int16_t GetValue(){
		return _value;
	}
	bool hasChanged(){ return (this->_value != this->_tempValue);}
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
	  _shouldAnimate = true;
	  _tmilli = milli;
	  _sinceLast = 0;
	}
	void animateParameter()
	{
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
		bool down = false;
		if(this->_targetValue>this->_value){
			distanceToGo = this->_targetValue - this->_value;
		}else{
			distanceToGo = this->_value - this->_targetValue;
			down = true;
		}
		pValue = scale16(distanceToGo,frac);
		if(down){
			this->_tempValue = _value - pValue;
		}else{
			this->_tempValue = _value+pValue;
		}
		if(this->_tmilli > this->_sinceLast){
			this->_tmilli -= _sinceLast;
		}else{
			this->_tempValue = _targetValue;
			this->_shouldAnimate=false;
		}
		if(pValue != 0){
			this->_sinceLast = 0;
		}
		if((this->_tempValue == this->_targetValue) || (this->_tmilli==0)){
			this->_shouldAnimate=false;
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
} ;
extern Parameter16_t parameterArray[];
extern int16_t parameterArraySize;


inline Print& operator<<(Print& obj, Parameter16_t k){
	obj<<light<<'['<<k.code<<"] "<<k.value->currentValue()<<" ("<<k.maxValue<<')'<<normal;
	return obj;
}
//extern parameter_t pixelParameters[];
//extern uint8_t numetaModuleParams;

#endif

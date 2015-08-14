/**
	@description: simple class of int16_ts for synchronizing interrupt based value changes
*/
#ifndef __PARAMETER_H__
#define __PARAMETER_H__
#include <Streaming.h>
template <class T>

class Parameter {
public:
	T _value;
	volatile T _tempValue;
public:
	Parameter() :_value(T()),_tempValue(T()){}
	Parameter(T a):_value(a),_tempValue(a){}

	void SetValue(T newValue)
	{
		_tempValue = newValue._value;
	}

	T GetValue(){
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
	void initTo(T a){
		this->_value = a;
		this->_tempValue = a;
	}

	T currentValue(){return _value;}
	T nextValue(){return _tempValue;}

	bool operator==(const Parameter<T> a)
	{
		return (this->_value == a._value);
	}
	Parameter<T> operator=(const Parameter<T> a){
		this->_tempValue = a.value;
		return this;
	}
	T operator=(const T a){
		this->_tempValue = a;
		return a;
	}
	friend Print &operator<<(Print &output, Parameter<T> d){
		output<<""<<d.currentValue()<<"";
		return output;
	}
};

typedef struct {
	char code;					// char, naming the parameter
	int16_t min;				// min value of the parameter
	int16_t max;				// max value of the parameter
	int8_t  step;				// value change for
	int16_t *value;
	volatile int16_t *tempValue;
} parameter_t;


template <class t>
class newParameter{
public:
	char code;					// char, naming the parameter
	t minValue;							// min value of the parameter
	t maxValue;							// max value of the parameter
	Parameter<t> &value;
	newParameter():code(0x00),minValue(t()),maxValue(t()),value(NULL){};
	newParameter(char c, t minVal, t maxVal,Parameter<t>& val): code(c),minValue(minVal),maxValue(maxVal),value(val){};
} ;
typedef struct newParameter<int16_t> newParameter_t;
extern newParameter_t parameterArray[];
extern int16_t parameterArraySize;

//extern parameter_t pixelParameters[];
//extern uint8_t numPixelParams;

#endif

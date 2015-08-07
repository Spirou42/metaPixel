
//#include "Encoder.h"

typedef struct {
	char code;					// char, naming the parameter
	int16_t min;				// min value of the parameter
	int16_t max;				// max value of the parameter
	int8_t  step;				// value change for 
	int16_t *value;
	volatile int16_t *tempValue;
} parameter_t;

typedef enum {
	stateNone,
	stateMenu,
	stateValueChange,
	stateCancel
}ballState_t;

extern parameter_t pixelParameters[];
extern uint8_t numPixelParams;
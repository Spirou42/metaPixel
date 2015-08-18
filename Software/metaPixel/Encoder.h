/* header file for the encoder class
not realy a class yet but it will be one :) */
#ifndef __ENCODER_H__
#define __ENCODER_H__
#if USE_LEGACY_MENU
#include <LedControl.h>

typedef void (*encoderCallback)(unsigned long);

typedef void(*encoderValueChangeFpt)(int16_t );
typedef void(*encoderClickFpt)(uint8_t ,boolean );

extern int16_t  encoderMaxValue;
extern int16_t  encoderMinValue;
extern int8_t  encoderStep;
extern volatile  int16_t *encoderPos;

extern encoderValueChangeFpt encoderValueChangeCallback;
extern encoderClickFpt       encoderClickCallback;
extern LedControl lc;
void printNumber(int v);
#endif
#endif

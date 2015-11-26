/**
* definition of a predefined switches
*/

#ifndef __SWITCHES_H__
#define __SWITCHES_H__
#include "Switch.h"

typedef void(*SwitchIRQHandler)();

extern Switch switches[];
extern size_t numberOfSwitches;

extern UserEventQueue eventQueue;

extern SwitchIRQHandler switchHandlers[];

void enableSwitches();
void disableSwitches();
#endif

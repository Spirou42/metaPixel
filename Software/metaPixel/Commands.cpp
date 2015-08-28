/**
* Command.cpp
* Simple command processor
*/

#include "Commands.h"
#include <Arduino.h>
#include "Streaming.h"
#include "VT100Stream.h"
#include "metaPixel.h"
#define USE_OLD_COMMAND 0
/****************************
Serial Interface
****************************/
const char *allowedCommands="#!*";
char serial_buffer[SERIAL_BUFFER_LENGTH];
uint8_t currentCharB=0;

int serialReader(unsigned long now, void* userData)
{
	bool endLine=false;
	while(Serial.available()){
		char c = Serial.read();
		if(c==0x0d){
			c = 0x00;
			endLine = true;
		}
		serial_buffer[currentCharB++]=c;
		if(currentCharB ==(SERIAL_BUFFER_LENGTH-1)){
			serial_buffer[currentCharB]=0x00;
			commandProcessor(serial_buffer);
			currentCharB =0;
		}
		if(endLine){
			#if DEBUG_COMMAND
			Serial << "Line:"<<serial_buffer <<endl;
			#endif
			commandProcessor(serial_buffer);
			#if DEBUG_COMMAND
			Serial <<clearLineRight<<"cleanBuffer"<< endl;
			#endif
			memset(serial_buffer,0x00,SERIAL_BUFFER_LENGTH);
			currentCharB = 0;
		}
	}
	return 0;
}

int getParameterIdxFor(char p)
{
	if(p>0x61){
		p-=0x20;
	}
	for(int16_t l = 0 ; l<parameterArraySize;l++){
		if(parameterArray[l].code == p){
			return l;
		}
	}
	return -1;
}

Parameter16_t* getParameterFor(char p)
{
	int idx = getParameterIdxFor(p);
	if (idx != -1) {
		return &parameterArray[idx];
	}else{
		return NULL;
	}
}

char getCommand(char** currentChar)
{
	while( (**currentChar != 0x00) && (getParameterIdxFor(**currentChar)==-1) && (index(allowedCommands,(int)**currentChar)==NULL) ){
		*currentChar =  (*currentChar+1);
	}
	char command = **currentChar;
	if(command >=0x61){
		command -=0x20;
	}
	return command;
}

long getValue(char** currentChar){
	long myValue = 0;
	while( (**currentChar != 0x00) && (**currentChar <'0' || **currentChar >'9')  ){
		*currentChar = (*currentChar+1) ;
	}
	//read number
	while( (**currentChar != 0x00)&&(**currentChar>='0') && (**currentChar<='9') ){
		myValue = 10*myValue +(**currentChar-'0');
		*currentChar =  (*currentChar+1);
	}
	return myValue;
}
#if USE_OLD_COMMAND
void commandProcessor(char* line_buffer){
	char *currentChar = line_buffer;
	Serial << XOFF;
	do{
		bool processParameter = true;
		// read till first know parameter mnomic char;
		// read till first number;
		bool animate = false;
		bool bounce = false;
		#if DEBUG_COMMAND
		Serial <<clearLineRight<< "Current "<<currentChar<<endl;
		#endif
		char command = getCommand(&currentChar);
		if(command == '#')		// Animate
		{
			#if DEBUG_COMMAND
			Serial << clearLineRight<<"Animate"<<endl;
			#endif
			currentChar++;
			animate = true;
			command = getCommand(&currentChar);
		}else if(command == '!')	// bounce
		{
			#if DEBUG_COMMAND
			Serial << clearLineRight<<"Bounce"<<endl;
			#endif
			currentChar ++;
			animate=true;
			bounce = true;
			command = getCommand(&currentChar);
		}else if(command == '*') // reset animation
		{
			#if DEBUG_COMMAND
			Serial << clearLineRight<<"Reset command"<<endl;
			#endif
			currentChar ++;
			command = getCommand(&currentChar);
			int parameterSlot = getParameterIdxFor(command);
			if(parameterSlot != -1){
				#if DEBUG_COMMAND
				Serial << clearLineRight<<" RSlot "<<parameterSlot<<" "<<parameterArray[parameterSlot]<<endl;
				#endif
				parameterArray[parameterSlot].value->_shouldAnimate=false;
			}
			processParameter = false;
			currentChar ++;
		}

		if(processParameter){
			long myValue = getValue(&currentChar);
			#if DEBUG_COMMAND
			Serial << clearLineRight<<command << ":"<<myValue<<endl;
			#endif
			int parameterSlot = getParameterIdxFor(command);
			#if DEBUG_COMMAND
			Serial << clearLineRight<<"Slot: "<<parameterSlot<<endl;
			#endif
			if(parameterSlot==-1){
				Serial << XON;
				return;
			}

			if(myValue <parameterArray[parameterSlot].minValue){
				myValue = parameterArray[parameterSlot].minValue;
				#if DEBUG_COMMAND
				Serial << clearLineRight<<"Clamp to "<<myValue<<endl;
				#endif
			}
			if(myValue>parameterArray[parameterSlot].maxValue){
				myValue = parameterArray[parameterSlot].maxValue;
				#if DEBUG_COMMAND
				Serial << clearLineRight<<"Clamp to "<<myValue<<endl;
				#endif
			}
			*(parameterArray[parameterSlot].value) = myValue;
			if(animate){
				int16_t to = getValue(&currentChar);
				unsigned long tim = getValue(&currentChar)*100;
				if(bounce){
					parameterArray[parameterSlot].value -> bounce(to,tim);
				}else{
					parameterArray[parameterSlot].value -> animateTo(to,tim);
				}
			}
		}
	}while(*currentChar != 0);
	Serial << XON;
}
#else
void commandProcessor(char* line_buffer){
	char *currentChar = line_buffer;
	Serial << XOFF;
	do{
		#if DEBUG_COMMAND
		Serial <<clearLineRight<< "Current "<<currentChar<<endl;
		#endif
		char command = getCommand(&currentChar);
		metaPixelCommand *currentCommandObj = NULL;
		#if DEBUG_COMMAND
		Serial <<clearLineRight<<"Command Object "<<_HEX((unsigned long)currentCommandObj)<<endl;
		#endif
		if( (command == '#') || (command == '!') || (command == '*'))		// Animate and bounce
		{
			currentChar++;
			//goto cleanUp;
			bool isBounce = (command == '!');
			bool isReset = (command == '*');
			#if DEBUG_COMMAND
			Serial << clearLineRight<<"Animate B"<<isBounce<<" R"<<isReset<<endl;
			#endif
			currentCommandObj = new metaPixelCommand(commandAnimation);
			command = getCommand(&currentChar);		// get ParameterName

			Parameter16_t* param = getParameterFor(command);
			if(param != NULL){
				CommandParameterAnimation_t * dd = &currentCommandObj->data.parameterAnimationData;
				dd->parameter = param;
				if(!isReset){
					#if DEBUG_COMMAND
					Serial << clearLineRight<<"animate"<<endl;
					#endif
					// read in from, to, time
					dd->fromValue = getValue(&currentChar);
					dd->toValue = getValue(&currentChar);
					dd->timeToGo = getValue(&currentChar)*10;
					dd->bounce = isBounce;
				}else{
					#if DEBUG_COMMAND
					Serial << clearLineRight<<"Reset"<<endl;
					#endif
					currentChar ++;
					dd->fromValue = 0;
					dd->toValue = 0;
					dd->timeToGo = 0;
					dd->bounce = false;
				}
			}else{
				#if DEBUG_COMMAND
				Serial <<clearLineRight<< "Invalid Parameter delete command"<<endl;
				#endif
				delete currentCommandObj;
				currentCommandObj = NULL;
				//goto cleanUp;
			}
		}else{					// this is a parameterSet
			currentChar++;
			currentCommandObj = new metaPixelCommand(commandParameter);
			//goto cleanUp;
			Parameter16_t* param = getParameterFor(command);
			#if DEBUG_COMMAND
			Serial << clearLineRight<< "SetParameter: "<<*param<<endl;
			#endif
			if(param != NULL){
				CommandParameterSet_t * dd = &currentCommandObj->data.parameterSetData;
				dd->parameter = param;
				dd->value = getValue(&currentChar);
			}else{
				#if DEBUG_COMMAND
				Serial << clearLineRight<<"Invalid Parameter in Set, delete command"<<endl;
				#endif
				delete currentCommandObj;
				currentCommandObj = NULL;
				//goto cleanUp;
			}
		}
//cleanUp:
		#if DEBUG_COMMAND
		Serial <<clearLineRight<<"continue "<<currentCommandObj->type<<" ("<<_HEX((unsigned long)currentCommandObj)<<')'<<endl;
		#endif
		if(currentCommandObj != NULL){
			#if DEBUG_COMMAND
			Serial << clearLineRight<< "Command Object Ok"<<endl;
			#endif
			bool result = currentCommandObj->processCommand();
			if(!result){
				#if DEBUG_COMMAND
				Serial << clearLineRight<<"Command Failed"<<endl;
				#endif
			}
			delete currentCommandObj;
			currentCommandObj = NULL;
			#if DEBUG_COMMAND
			Serial <<clearLineRight<<"command "<<currentCommandObj->type<<" ("<<_HEX((unsigned long)currentCommandObj)<<')'<<endl;
			#endif

		}else{
			#if DEBUG_COMMAND
			Serial <<clearLineRight<< "Command was deleted"<<endl;
			#endif
		}
	}while(*currentChar != 0);
	Serial << XON;
	#if DEBUG_COMMAND
	Serial <<clearLineRight<< "Command processor end"<<endl;
	#endif
}
#endif

bool metaPixelCommand::processCommand()
{
	#if DEBUG_COMMAND
	Serial << clearLineRight<<"processCommand:"<<endl;
	#endif
	bool result = false;
	switch(this->type){
		case commandParameter:
			{
				#if DEBUG_COMMAND
				Serial << clearLineRight<< "Set Parameter ";
				#endif
				CommandParameterSet_t dd = data.parameterSetData;
				if (dd.parameter) {
					#if DEBUG_COMMAND
					Serial << *dd.parameter<<" to "<<dd.value<<endl;
					#endif
					dd.parameter->setValue(dd.value);
					result = true;
				}else{
					#if DEBUG_COMMAND
					Serial << clearLineRight<<"(invalid)"<<endl;
					#endif
				}
			}
		break;
		case commandAnimation:
			{
				#if DEBUG_COMMAND
				Serial << clearLineRight<< "Animate"<<endl;
				#endif
				CommandParameterAnimation_t dd = data.parameterAnimationData;
				if (dd.parameter) {
					#if DEBUG_COMMAND
					Serial << clearLineRight<<" Parameter :"<<*dd.parameter<<endl;
					#endif
					int16_t start = dd.parameter->clampValue(dd.fromValue);
					int16_t stop = dd.parameter->clampValue(dd.toValue);
					#if DEBUG_COMMAND
					Serial << clearLineRight<<" Start: "<<start<<" End:"<<stop<<endl;
					#endif
					if((start == stop) || (dd.timeToGo == 0)){
						#if DEBUG_COMMAND
						Serial << clearLineRight<<"Stop Animation"<<endl;
						#endif
						dd.parameter->value->_shouldAnimate = false;
					}else{
						dd.parameter->setValue(start);
						if (dd.bounce) {
							dd.parameter->value->bounce(stop,dd.timeToGo);
						}else{
							dd.parameter->value->animateTo(stop,dd.timeToGo);
						}
					}
					result = true;
				}
			}
		break;
		case commandWait: break;
	}
	return result;
}

/**
* Command.cpp
* Simple command processor
*/

#include "Commands.h"
#include <Arduino.h>
#include "Streaming.h"
#include "VT100Stream.h"
#include "metaPixel.h"


/****************************
Serial Interface
****************************/
const char *allowedCommands="#!*";
char serial_buffer[SERIAL_BUFFER_LENGTH];
uint8_t currentChar=0;

int serialReader(unsigned long now, void* userData)
{
	bool endLine=false;
	while(Serial.available()){
		char c = Serial.read();
		if(c==0x0d){
			c = 0x00;
			endLine = true;
		}
		serial_buffer[currentChar++]=c;
		if(currentChar ==(SERIAL_BUFFER_LENGTH-1)){
			serial_buffer[currentChar]=0x00;
			commandProcessor(serial_buffer);
			currentChar =0;
		}
		if(endLine){
			#if DEBUG_COMMAND
			Serial << "Line:"<<serial_buffer <<endl;
			#endif
			commandProcessor(serial_buffer);
			#if DEBUG_COMMAND
			Serial << endl;
			#endif
			memset(serial_buffer,0x00,SERIAL_BUFFER_LENGTH);
			currentChar = 0;
		}

	}
	return 0;
}

int getParameterFor(char p)
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

char getCommand(char** currentChar)
{
	while( (**currentChar != 0x00) && (getParameterFor(**currentChar)==-1) && (index(allowedCommands,(int)**currentChar)==NULL) ){
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
		if(command == '#')
		{
			#if DEBUG_COMMAND
			Serial << clearLineRight<<"Animate"<<endl;
			#endif
			currentChar++;
			animate = true;
			command = getCommand(&currentChar);
		}else if(command == '!')
		{
			#if DEBUG_COMMAND
			Serial << clearLineRight<<"Bounce"<<endl;
			#endif
			currentChar ++;
			animate=true;
			bounce = true;
			command = getCommand(&currentChar);
		}else if(command == '*')
		{
			#if DEBUG_COMMAND
			Serial << clearLineRight<<"Reset command"<<endl;
			#endif
			currentChar ++;
			command = getCommand(&currentChar);
			int parameterSlot = getParameterFor(command);
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
			int parameterSlot = getParameterFor(command);
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

/**
* Command.cpp
* Simple command processor
* Commands are simple single char mnomics. We have two different types of commands
* simple assignments
*		here a variable name is followed by the number i.e. A17
*
* And commands:
* commands do a bit more of work
* we have the following commands
*	Bounce 		[!] ![varname][startValue] [endValue] [sec.]		- bounces the parameters value between start and end value in [sec.] time
* Animate 	[#]	#[varName][startValue] [endValue] [sec.]		- single animation of parameters value from start to end value ind [sec.] time
* Stop   		[*] *[varName]																	- interrup the animation on [varName] parameter
* WaitTime	[&] &[sec.]																			- dissables the command processing for [sec] seconds
* WaitAnim	[%] %[varName]																	- wait for the animation to finnish -
*/

#include "Commands.h"
#include <Arduino.h>
#include "Streaming.h"
#include "VT100Stream.h"
#include "metaPixel.h"

/****************************
Serial Interface
****************************/
const char *allowedCommands="#!*&%?";
char serial_buffer[SERIAL_BUFFER_LENGTH];
uint8_t currentCharB=0;
CommandQueue commandQueue = CommandQueue();

int serialReader(unsigned long now, void* userData)
{
	bool endLine=false;
	while(Serial.available()){
		char c = Serial.read();
		#if DEBUG_SERIAL
		Serial<<clearLineRight<<"Got Char: 0x"<<_HEX(c)<<"|"<<c<<"| ("<<currentCharB<<")"<<endl;
		Serial.flush();
		#endif
		if(c==0x0d){
			c = 0x00;
			endLine = true;
		}
		serial_buffer[currentCharB]=c;
		currentCharB++;
		// if(currentCharB ==(SERIAL_BUFFER_LENGTH-1)){
		// 	serial_buffer[currentCharB]=0x00;
		// 	commandProcessor(serial_buffer);
		// 	currentCharB =0;
		// }
		if(endLine){
			#if DEBUG_SERIAL
			Serial << clearLineRight<<"Line:"<<serial_buffer <<endl;
			#endif
			commandProcessor(serial_buffer);
			#if DEBUG_SERIAL
			Serial <<clearLineRight<<"cleanBuffer"<<endl;
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
	#if DEBUG_PARSER
	//Serial << "getCommand:" << endl;
	#endif
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

void commandProcessor(char* line_buffer, bool executeImediately)
{
	char *currentChar = line_buffer;
	Serial << XOFF;
	#if DEBUG_PARSER
	Serial << ScreenPos(8,0);
	#endif
	do{
		#if DEBUG_PARSER
		Serial <<clearLineRight<< "Current Line: |"<<currentChar<<'|'<<endl;
		#endif
		char command = getCommand(&currentChar);
		metaPixelCommand *currentCommandObj = NULL;
		if(command == '?'){
			currentChar ++;
			currentCommandObj = new metaPixelCommand(commandDump);
		}else
		if( (command == '#') || (command == '!') || (command == '*'))		// Animate and bounce and stop-animation
		{
			currentChar++;

			bool isBounce = (command == '!');
			bool isReset = (command == '*');
			#if DEBUG_PARSER
			// Serial << clearLineRight<<"Animate B"<<isBounce<<" R"<<isReset<<endl;
			#endif

			command = getCommand(&currentChar);		// get ParameterName
			Parameter16_t* param = getParameterFor(command);
			if(param != NULL){
				currentCommandObj = new metaPixelCommand(commandAnimation);

				#if DEBUG_PARSER
				if(currentCommandObj== NULL){
					Serial << clearLineRight<<bold<<"Command NULL"<<normal<<endl;
				}
				#endif

				CommandParameterAnimation_t * dd = &currentCommandObj->data.parameterAnimationData;
				dd->parameter = param;
				if(!isReset){
					#if DEBUG_PARSER
					// Serial << clearLineRight<<"animate"<<endl;
					#endif
					// read in from, to, time
					dd->fromValue = getValue(&currentChar);
					dd->toValue = getValue(&currentChar);
					dd->timeToGo = getValue(&currentChar)*1000;
					dd->bounce = isBounce;
				}else{
					#if DEBUG_PARSER
					// Serial << clearLineRight<<"Reset"<<endl;
					#endif
					currentChar ++;
					dd->fromValue = 0;
					dd->toValue = 0;
					dd->timeToGo = 0;
					dd->bounce = false;
				}
			}else{
				#if DEBUG_PARSER
				Serial <<clearLineRight<< "Invalid Parameter delete command"<<endl;
				#endif
				// delete currentCommandObj;
				// currentCommandObj = NULL;

			}
		}else if(command == '&'){	// and WaitForTime
			currentChar++;
			int16_t value = getValue(&currentChar);
			currentCommandObj = new metaPixelCommand(commandWait);
			CommandWait_t *dd = &currentCommandObj->data.commandWaitData;
			dd->waitForAnimationStop = false;
			dd->parameter = NULL;
			dd->time = value * 1000;
			if(dd->time == 0){
				dd->time=200;
			}
		}else if(command == '%'){	// and waitForAnmination
			currentChar++;
			command = getCommand(&currentChar);
			currentChar++;
			Parameter16_t* param = getParameterFor(command);
			if(param != NULL){
				currentCommandObj = new metaPixelCommand(commandWait);
				CommandWait_t *dd = &currentCommandObj->data.commandWaitData;
				dd->waitForAnimationStop = true;
				dd->parameter = param;
				dd->time = 0;
			}else{
				Serial << "FUCK"<<endl;
			}
		}else{					// this is a parameterSet
			currentChar++;
			Parameter16_t* param = getParameterFor(command);
			#if DEBUG_PARSER
			// Serial << clearLineRight<< "SetParameter: "<<*param<<endl;
			#endif
			if(param != NULL){
				currentCommandObj = new metaPixelCommand(commandParameter);
				CommandParameterSet_t * dd = &currentCommandObj->data.parameterSetData;
				dd->parameter = param;
				dd->value = getValue(&currentChar);
			}else{
				#if DEBUG_PARSER
				Serial << clearLineRight<<"Invalid Parameter in Set, delete command"<<endl;
				#endif
				// delete currentCommandObj;
				// currentCommandObj = NULL;

			}
		}
		//cleanUp:

		#if DEBUG_PARSER
		Serial <<clearLineRight<<"continue "<<currentCommandObj->type<<" ("<<_HEX((unsigned long)currentCommandObj)<<')'<<endl;
		#endif
		if(currentCommandObj != NULL){
			#if DEBUG_PARSER
			Serial << clearLineRight<< "Command Object Ok"<<endl;
			#endif
			if(executeImediately){
				if(currentCommandObj->type !=commandWait ){
					// we only execute non wait commands
					currentCommandObj->processCommand();
				}
				delete currentCommandObj;
			}else{
				commandQueue.addCommand(currentCommandObj);
			}
			currentCommandObj = NULL;
			/*
			bool result = currentCommandObj->processCommand();
			if(!result){
			#if DEBUG_PARSER
			Serial << clearLineRight<<"Command Failed"<<endl;
			#endif
		}
		delete currentCommandObj;
		currentCommandObj = NULL;
		#if DEBUG_PARSER
		Serial <<clearLineRight<<"command "<<currentCommandObj->type<<" ("<<_HEX((unsigned long)currentCommandObj)<<')'<<endl;
		#endif
		*/
	}else{
		#if DEBUG_PARSER
		Serial <<clearLineRight<< "Command was deleted"<<endl;
		#endif
	}
}while(*currentChar != 0);
Serial << XON;
#if DEBUG_PARSER
Serial <<clearLineRight<< "Command processor end"<<endl;
#endif
}


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
				parametersInvalid = true;
			}
		}
		break;
		case commandDump:
		{
			uint16_t t = EffectProgram.currentValue()%(newMaxPrograms);
			Effect *effect = effectProgramsN[t].program;
			String * paramString = effect->parameterString();
			Serial << ScreenPos(20,1)<<clearLineRight<<*paramString;
		}
		break;
		case commandWait: break;
	}
	return result;
}
void CommandQueue::addCommand(metaPixelCommand* cmd)
{
	#if DEBUG_COMMAND
	Serial << clearLineRight<<"addCommand 0x"<<_HEX((unsigned long)cmd)<<endl;
	#endif
	if(!queueStart){
		cmd->nextCommand=NULL;
		queueStart = cmd;
		queueEnd = cmd;
		queueLength = 1;
	}else{
		queueEnd->nextCommand=cmd;
		queueEnd = cmd;
		cmd->nextCommand = NULL;
		queueLength++;
	}
}
metaPixelCommand *CommandQueue::popCommand()
{
	metaPixelCommand* cmd = queueStart;
	if (cmd) {
		queueStart = cmd->nextCommand;
		if (queueStart == NULL) {
			queueEnd = NULL;
			queueLength = 0;
		}
	}
	#if DEBUG_COMMAND
	if(cmd)
	Serial <<clearLineRight<< "pop command 0x"<<_HEX((unsigned long)cmd)<<endl;
	#endif
	return cmd;
}

void CommandQueue::processQueue()
{
	metaPixelCommand *p=NULL;
	if(waiting){
		if(waitParameter == NULL){
			if (waitTimer >= waitTill) {
				waiting = false;
				waitTill = 0;
				waitTimer = 0;
				#if DEBUG_COMMAND
				Serial << ScreenPos(38,0)<<clearLineRight;
				#endif
			}else{
				#if DEBUG_COMMAND
				Serial <<ScreenPos(38,0)<<clearLineRight<< "Waiting to "<<waitTill<<" ("<<waitTimer<<")"<<endl;
				#endif
			}
		}else{
			if(!waitParameter->value->isAnimating()){
				waiting = false;
				waitTill = 0;
				waitTimer = 0;
				waitParameter = NULL;
			}else{
				#if DEBUG_COMMAND
				Serial <<ScreenPos(38,0)<<clearLineRight<< "Waiting FOR "<<*waitParameter;
				#endif

			}
		}
	}
	while( !waiting && ((p=popCommand())!= NULL)  ){
		#if DEBUG_COMMAND
		Serial <<clearLineRight<<bold<<"Process command at "<<_HEX((unsigned long)p)<<normal<<endl;
		#endif
		if(p->type == commandWait){
			CommandWait_t *dd = &p->data.commandWaitData;
			waiting = true;
			waitTimer = 0;
			waitTill = dd->time;
			waitParameter = dd->parameter;
			if(waitParameter){
				waitParameter->value->setStopNext(true);
			}
			break;
		}else{
			p->processCommand();
		}
		delete p;
	}

}

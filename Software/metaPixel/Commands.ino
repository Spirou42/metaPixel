//static const char *validParameters="PpDdBbCcUu";

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

void commandProcessor(char* line_buffer){
	char *currentChar = line_buffer;
	int16_t myValue = 0;
	// read till first char;
	while( (*currentChar != 0x00) && (getParameterFor(*currentChar)==-1)  ){
		currentChar ++;
	}
	char command = *currentChar;
	if(command >=0x61){
		command -=0x20;
	}
	// read till first number;
	while( (*currentChar != 0x00) && (*currentChar <'0' || *currentChar >'9')  ){
		currentChar ++;
	}
	//read number
	while( (*currentChar>='0') && (*currentChar<='9')){
		myValue = 10*myValue +(*currentChar-'0');
		currentChar ++;
	}
#if DEBUG_COMMAND
	Serial << command << ":"<<myValue<<endl;
#endif
	int parameterSlot = getParameterFor(command);
#if DEBUG_COMMAND
	Serial << "Slot: "<<parameterSlot<<endl;
#endif
	if(parameterSlot==-1)
		return;


	if(myValue <parameterArray[parameterSlot].minValue){
		myValue = parameterArray[parameterSlot].minValue;
#if DEBUG_COMMAND
		Serial << "Clamp to "<<myValue<<endl;
#endif
	} 
	if(myValue>parameterArray[parameterSlot].maxValue){
		myValue = parameterArray[parameterSlot].maxValue;
#if DEBUG_COMMAND
		Serial << "Clamp to "<<myValue<<endl;
#endif
	}
	parameterArray[parameterSlot].value = myValue;	
}
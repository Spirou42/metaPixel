//static const char *validParameters="PpDdBbCcUu";

int getParameterFor(char p)
{
	if(p>0x61){
		p-=0x20;
	}
	for(unsigned int l = 0 ; l<numPixelParams;l++){
		if(pixelParameters[l].code == p){
			return l;
		}
	}
	return -1;
}

void commandProcessor(char* line_buffer){
	char *currentChar = line_buffer;
	int16_t value = 0;
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
		value = 10*value +(*currentChar-'0');
		currentChar ++;
	}
	Serial << command << ":"<<value<<endl;
	int parameterSlot = getParameterFor(command);
	Serial << "Slot: "<<parameterSlot<<endl;
	if(parameterSlot==-1)
		return;

	volatile int16_t *parameterP;
	parameterP =pixelParameters[parameterSlot].tempValue;
	Serial << "parameter:"<<_HEX((unsigned int)parameterP)<<endl;
	if(value <pixelParameters[parameterSlot].min){
		Serial << "smaller than min"<<endl;
		value = pixelParameters[parameterSlot].min;
	} 
	if(value>pixelParameters[parameterSlot].max){
		Serial << "bigger than max"<<endl;
		value = pixelParameters[parameterSlot].max;
	}
	Serial << "Value: " << value<<endl;
	*parameterP = value;
	
}
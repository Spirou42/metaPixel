//static const char *validParameters="PpDdBbCcUu";

/****************************
Serial Interface
****************************/
#define SERIAL_BUFFER_LENGTH 20
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
		if(currentChar >(SERIAL_BUFFER_LENGTH-1)){
			currentChar =0;
		}
		if(endLine){
#if DEBUG_COMMAND
			Serial << "Command:"<<serial_buffer <<endl;
#endif
			commandProcessor(serial_buffer);
			Serial << endl;
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

void commandProcessor(char* line_buffer){
	char *currentChar = line_buffer;

	do{
		// read till first know parameter mnomic char;
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
		int16_t myValue = 0;
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
		*(parameterArray[parameterSlot].value) = myValue;
	}while(*currentChar != 0);
}

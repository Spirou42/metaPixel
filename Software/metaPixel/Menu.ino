/*
* this is a simple menu using the current LC / Encoder code 
*/
int16_t currentSelectedMenu=0;
volatile int16_t nextSelectedMenu = currentSelectedMenu;
char valuePrefix = ' ';

parameter_t pixelParameters[] = {
	{'P',0,(int16_t)(maxPrograms-1),1,&currentProgram,&nextProgram},
	{'D',1,800,1,&currentDelay,&nextDelay},
	{'C',0,(int16_t)(numberOfPalettes-1),1,&currentPalette,&nextPalette},
	{'B',0,255,1,&currentBrightness,&nextBrightness},
	{'U',0,2,1,&currentResolution,&nextResolution}
};
uint8_t numPixelParams = sizeof(pixelParameters)/sizeof(parameter_t);

void genericChangeHandler(int16_t newValue)
{
	#if DEBUG_MENU
	Serial << "GCH "<< valuePrefix << "("<<newValue<<")"<<endl;
	Serial << (long)encoderPos << endl;
	#endif
	printNumber(newValue);
	lc.setChar(0,3,valuePrefix,false);
}

void menuChangeHandler(int16_t newValue)
{
	#if DEBUG_MENU
	Serial << "Menu "<<newValue<<' '<<currentSelectedMenu;
	#endif
	currentSelectedMenu = newValue;
	#if DEBUG_MENU
	Serial << " "<<currentSelectedMenu<<endl;
	#endif
	displayMenu();
	#if DEBUG_MENU
	Serial << "endMenu"<<endl;
	#endif
}


void displayMenu()
{
	lc.clearDisplay(0);
	int lcdPos = 3;
	int menuStart = currentSelectedMenu-3;
	if (menuStart<0){
		menuStart = 0;
	}
	
	for(uint8_t menuIndex=menuStart;menuIndex<numPixelParams;menuIndex++){
		lc.setChar(0,lcdPos--,pixelParameters[menuIndex].code, currentSelectedMenu == menuIndex);
	}
}


void selectMenuState()
{
	#if DEBUG_MENU
	Serial <<"selectMenuState"<<endl;
	#endif
	currentState = stateMenu;
	nextSelectedMenu=currentSelectedMenu = 0;
	encoderMinValue = 0;
	encoderMaxValue = numPixelParams -1;
	encoderPos = &nextSelectedMenu;
	*encoderPos = currentSelectedMenu;
	encoderStep = 1;
	displayMenu();
	encoderValueChangeCallback = menuChangeHandler;
}
/** central menu input handling state machine */
void clickHandler(uint8_t clicks, boolean wasLongClick)
{
	#if DEBUG_MENU
	Serial << "Got a click "<<clicks<<" "<<wasLongClick<<endl;
	#endif
	if(wasLongClick){                              // cancel all value changes
		currentState = stateCancel;
	}
	switch(currentState){
		case stateNone:
		if(clicks == 1){
			if(!wasLongClick){ // we activate the menu
					#if DEBUG_MENU
				Serial << "None -> Menu"<<endl;
					#endif
				selectMenuState();
			}
		}
		break;
		case stateMenu:
		if(clicks == 1){
			if( (currentSelectedMenu >=0) && (currentSelectedMenu<numPixelParams)){
				#if DEBUG_MENU
				Serial << "Menu entry "<<currentSelectedMenu<<" "<<pixelParameters[currentSelectedMenu].code<<" selected";
				#endif
				// we got a selected menu entry
				currentState = stateValueChange;
				encoderMaxValue = pixelParameters[currentSelectedMenu].max;
				encoderMinValue = pixelParameters[currentSelectedMenu].min;
				encoderStep = pixelParameters[currentSelectedMenu].step;
				encoderPos = pixelParameters[currentSelectedMenu].tempValue;
				*encoderPos = *pixelParameters[currentSelectedMenu].value;
				encoderValueChangeCallback = genericChangeHandler;
				valuePrefix = pixelParameters[currentSelectedMenu].code;
				genericChangeHandler(*pixelParameters[currentSelectedMenu].value);
			}else{
				#if DEBUG_MENU
				Serial << "Illegal menu index: "<<currentSelectedMenu<<endl;
				#endif
			}
		}
		break;
		case stateValueChange:
		{
			selectMenuState();
		}
		break;
		case stateCancel:
		default:
		{
			lc.clearDisplay(0);
			encoderValueChangeCallback = NULL;
			currentSelectedMenu = -1;
			currentState = stateNone;
			encoderPos = NULL;
		}
		break;
	}
}



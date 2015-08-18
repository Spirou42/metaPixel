/*
* this is a simple menu using the current LC / Encoder code
*/

#if USE_LEGACY_MENU

int16_t currentSelectedMenu=0;
volatile int16_t nextSelectedMenu = currentSelectedMenu;
char valuePrefix = ' ';

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

	for(uint8_t menuIndex=menuStart;menuIndex<parameterArraySize;menuIndex++){
		lc.setChar(0,lcdPos--,parameterArray[menuIndex].code, currentSelectedMenu == menuIndex);
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
	encoderMaxValue = parameterArraySize -1;
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
			if( (currentSelectedMenu >=0) && (currentSelectedMenu<parameterArraySize)){
				#if DEBUG_MENU
				Serial << "Menu entry "<<currentSelectedMenu<<" "<<parameterArray[currentSelectedMenu].code<<" selected";
				#endif
				// we got a selected menu entry
				currentState = stateValueChange;
				encoderMaxValue = parameterArray[currentSelectedMenu].maxValue;
				encoderMinValue = parameterArray[currentSelectedMenu].minValue;
				encoderStep = 1;//parameterArray[currentSelectedMenu].step;
				encoderPos = &parameterArray[currentSelectedMenu].value._tempValue;
				*encoderPos = parameterArray[currentSelectedMenu].value._value;
				encoderValueChangeCallback = genericChangeHandler;
				valuePrefix = parameterArray[currentSelectedMenu].code;
				genericChangeHandler(parameterArray[currentSelectedMenu].value._value);
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
#endif

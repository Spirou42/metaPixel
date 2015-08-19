/**
* metaModule
*/

#include "metaModule.h"

uint16_t metaModule::XY( uint8_t x, uint8_t y)
	{
		uint16_t i;
		if(this->flipped){
			int tx = this->w-1-x;
			int ty = this->h-1-y;
			x = tx;
			y=ty;
		}
		{
			if (x & 0x01){
				//odd collumns run Backwards
				uint8_t reverseY = (h - 1) - y;
				i = (x * h)+reverseY;
			}else{
				i = (x * h)+y;
			}
		}
		return i;
	}

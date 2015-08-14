#include "EffectFire.h"

void EffectFire::runEffect(unsigned long now)
{
 Serial << "EffectFire ";
 int16_t val = getValueFor(bottomHeating,200);
 Serial << "bottomHeat ("<<getCodeFor(bottomHeating)<<"): "<<val<<"\ttopCool ("<<getCodeFor(topCooling)<<"): "<<getValueFor(topCooling,0)<<endl;
}
int16_t EffectFire::heatAt(int16_t x,int16_t y)
{
	if( (x<0) || (x > DISPLAY_WIDTH-1)){
		return -20;
	}
	if(y<0)
		return genericSpeed2.currentValue();
	if( (y<0) || (y > DISPLAY_HEIGHT-1)){
		return -genericParam1.currentValue();
	}
	return heatCells[display.XY(x,y)];
}

int16_t EffectFire::neightbours(int16_t x, int16_t y)
{
	int16_t result = 0;
	int16_t div=0;
	result += heatAt(x,y-1)*3.0/2.0;div++;
	result += heatAt(x-1,y-1)/3.0;div++;
	result += heatAt(x+1,y-1)/3.0;div++;
	//	result += heatAt(x-1,y+1)/6;div++;
	//	result += heatAt(x+1,y+1)/6;div++;
	//	result += heatAt(x,y)/10;div++;
	if(random8(100)<10){
		//		result += heatAt(x+1,y)/2;div++;
	}
	if(random8(100)>90){
		//		result += heatAt(x-1,y)/2;div++;
	}
	if (result <0){
		result =0;
	}
	return result / div;
}

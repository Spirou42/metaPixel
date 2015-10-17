/*程序说明
STC12LE5A60S2 3.3V1T单片机 硬件SPI直接驱动天马TM022HDH26液晶
单片机主频11.0592MHZ，实测该主频下约1秒刷完一次全屏
STC12C5A60S2 5V1T单片机结构一致,请根据PDF驱动3.3V器件
本程序已经测试可用。
淘宝名:pei_gang 2013/2/5
*/

#include <stc12.h> //STC12单片机头文件

//颜色定义
#define Blue 		0x001f
#define Yellow		0xffe0
#define Green 	 	0x07e0
#define Black		0x0000
#define White		0xffff
#define Red			0xf800

//引脚定义
sbit	R_ESET =	P1^2; //液晶RESET引脚,接单片机IO引脚
sbit	C_D	=	P1^3;//液晶D/CX引脚，数据/命令控制，接单片机IO引脚
sbit	C_S	=	P1^4;//液晶片选CS,P1.4为单片机SPI总线的SS引脚,通过MSTR位配置为IO模式

//以下3个引脚的连接。因接单片机默认硬件SPI引脚,只需要在SPCTL中使能即可,不需程序中定义
//sbit	SDI	=	P1^5; 液晶SDI引脚,主机写到液晶,接单片机MOSI引脚P1.5
//sbit	SDO	=	P1^6; 液晶SDO引脚,从液晶读数据,接单片机MISO引脚P1.6
//sbit	SCL =   P1^7; 液晶SCL时钟引脚,接单片机SCLK时钟引脚P1.7

//LED阳极(10脚)接电源3.3V,11-14为4个LED阴极引脚,单片机IO引脚,置0即点亮
//3.3V时每LED通过电流约20毫安,不需要限流电阻
sbit	LED1 =  P2^4;//背光LED控制。
sbit	LED2 =  P2^5;//背光LED控制
sbit	LED3 =  P2^6;//背光LED控制
sbit	LED4 =  P2^7;//背光LED控制

/*SPCTL参数  11010000=0xd0 该参数下单片机硬件SPI为主模式,P1.4为IO模式,SPI时钟频率为MCU时钟频率/4,为2.7648M
Bit7 SSIG  SS引脚忽略控制位 1
Bit6 SPEN  SPI使能位		1 
Bit5 DORD  数据顺序			0 最高位优先发送
Bit4 MSTR  主/从模式选择位  1
Bit3 SPOL  时钟极性			0
Bit2 SPHA  时钟相位			0
Bit1 SPR1  时钟速率选择		0 
Bit0 SPR0  时钟速率选择		0
*/

//函数
void Delay_us(int value)////延时函数_us
	{
	while (value)
	value--;
	}

void Delay_ms(int value)////延时函数_ms
	{
	while (value){
		Delay_us(999);
		value--;
		}
	}

void HD_reset(){
	//SPI初始化
	SPDAT = 0;
	SPCTL = 0xd0;	//设置1101,0000
	SPSTAT = 0xc0;	//标志位写1清零
	//LCD Reset
	R_ESET=1;
	Delay_ms(1);
	R_ESET=0;
	Delay_ms(10);
	R_ESET=1;
	Delay_ms(120);
}

void LCD_Writ_Bus(char a){   //数据写入函数 8位
      C_S = 0;
      SPDAT = a;//开始传输
	  while(!(SPSTAT&0x80));  // 等待SPIF = 1,传输完成  
	  SPSTAT = 0xc0;    
      C_S = 1;
}	

void LCD_WRITE_CMD(char cmd){//8 bit
      C_D = 0;
      LCD_Writ_Bus(cmd);
}

void LCD_WRITE_COM_DATA(char com_data){//8 bit
      C_D = 1;
      LCD_Writ_Bus(com_data);
}

void LCD_WRITE_DATA(int a){//16位数据,分两次，每次送8位
      C_D = 1;
      LCD_Writ_Bus(a>>8);
	  LCD_Writ_Bus(a);
}

//设置绘点窗口，x=0~239,y=0~319
void Address_set(unsigned int x1,unsigned int x2,unsigned int y1,unsigned int y2)
{
     	LCD_WRITE_CMD(0x2a); 	//x轴   
     	LCD_WRITE_DATA(x1);
     	LCD_WRITE_DATA(x2);
	    LCD_WRITE_CMD(0x2b); 	//y轴   
	    LCD_WRITE_DATA(y1);
     	LCD_WRITE_DATA(y2);
     	LCD_WRITE_CMD(0x2c);
}

void LCD_TEST_SingleColor(int Discolor){
     	int i,j;
		Address_set(0,239,0,319);

		for (i=0;i<320;i++){
			for (j=0;j<240;j++){
				LCD_WRITE_DATA(Discolor);
				}
		}
}

//画一条横线
void LCD_draw_Line(unsigned int y,unsigned int color)	
{		int i;	
		Address_set(0,239,y,y);
		for (i=0;i<240;i++){
			LCD_WRITE_DATA(color);
			}
}

//液晶主控初始化
void Initial_ILI9340C(void)
{
        LCD_WRITE_CMD(0xCB);  
        LCD_WRITE_COM_DATA(0x39); 
        LCD_WRITE_COM_DATA(0x2C); 
        LCD_WRITE_COM_DATA(0x00); 
        LCD_WRITE_COM_DATA(0x34); 
        LCD_WRITE_COM_DATA(0x02); 

        LCD_WRITE_CMD(0xCF);  
        LCD_WRITE_COM_DATA(0x00); 
        LCD_WRITE_COM_DATA(0XC1); 
        LCD_WRITE_COM_DATA(0X30); 
 
        LCD_WRITE_CMD(0xE8);  
        LCD_WRITE_COM_DATA(0x85); 
        LCD_WRITE_COM_DATA(0x00); 
        LCD_WRITE_COM_DATA(0x78); 
 
        LCD_WRITE_CMD(0xEA);  
        LCD_WRITE_COM_DATA(0x00); 
        LCD_WRITE_COM_DATA(0x00); 
 
        LCD_WRITE_CMD(0xED);  
        LCD_WRITE_COM_DATA(0x64); 
        LCD_WRITE_COM_DATA(0x03); 
        LCD_WRITE_COM_DATA(0X12); 
        LCD_WRITE_COM_DATA(0X81); 

        LCD_WRITE_CMD(0xF7);  
        LCD_WRITE_COM_DATA(0x20); 
  
        LCD_WRITE_CMD(0xC0);    //Power control 
        LCD_WRITE_COM_DATA(0x23);   //VRH[5:0] 
 
        LCD_WRITE_CMD(0xC1);    //Power control 
        LCD_WRITE_COM_DATA(0x10);   //SAP[2:0];BT[3:0] 
 
        LCD_WRITE_CMD(0xC5);    //VCM control 
        LCD_WRITE_COM_DATA(0x3e); //对比度调节
        LCD_WRITE_COM_DATA(0x28); 
 
        LCD_WRITE_CMD(0xC7);    //VCM control2 
        LCD_WRITE_COM_DATA(0x86);  //--
 
        LCD_WRITE_CMD(0x36);    // Memory Access Control 
        LCD_WRITE_COM_DATA(0xC8); 

        LCD_WRITE_CMD(0x3A);    
        LCD_WRITE_COM_DATA(0x55); 

        LCD_WRITE_CMD(0xB1);    
        LCD_WRITE_COM_DATA(0x00);  
        LCD_WRITE_COM_DATA(0x18); 
 
        LCD_WRITE_CMD(0xB6);    // Display Function Control 
        LCD_WRITE_COM_DATA(0x08); 
        LCD_WRITE_COM_DATA(0x82);
        LCD_WRITE_COM_DATA(0x27);  
 
        LCD_WRITE_CMD(0xF2);    // 3Gamma Function Disable 
        LCD_WRITE_COM_DATA(0x00); 
 
        LCD_WRITE_CMD(0x26);    //Gamma curve selected 
        LCD_WRITE_COM_DATA(0x01); 
 
        LCD_WRITE_CMD(0xE0);    //Set Gamma 
        LCD_WRITE_COM_DATA(0x0F); 
        LCD_WRITE_COM_DATA(0x31); 
        LCD_WRITE_COM_DATA(0x2B); 
        LCD_WRITE_COM_DATA(0x0C); 
        LCD_WRITE_COM_DATA(0x0E); 
        LCD_WRITE_COM_DATA(0x08); 
        LCD_WRITE_COM_DATA(0x4E); 
        LCD_WRITE_COM_DATA(0xF1); 
        LCD_WRITE_COM_DATA(0x37); 
        LCD_WRITE_COM_DATA(0x07); 
        LCD_WRITE_COM_DATA(0x10); 
        LCD_WRITE_COM_DATA(0x03); 
        LCD_WRITE_COM_DATA(0x0E); 
        LCD_WRITE_COM_DATA(0x09); 
        LCD_WRITE_COM_DATA(0x00); 

        LCD_WRITE_CMD(0XE1);    //Set Gamma 
        LCD_WRITE_COM_DATA(0x00); 
        LCD_WRITE_COM_DATA(0x0E); 
        LCD_WRITE_COM_DATA(0x14); 
        LCD_WRITE_COM_DATA(0x03); 
        LCD_WRITE_COM_DATA(0x11); 
        LCD_WRITE_COM_DATA(0x07); 
        LCD_WRITE_COM_DATA(0x31); 
        LCD_WRITE_COM_DATA(0xC1); 
        LCD_WRITE_COM_DATA(0x48); 
        LCD_WRITE_COM_DATA(0x08); 
        LCD_WRITE_COM_DATA(0x0F); 
        LCD_WRITE_COM_DATA(0x0C); 
        LCD_WRITE_COM_DATA(0x31); 
        LCD_WRITE_COM_DATA(0x36); 
        LCD_WRITE_COM_DATA(0x0F); 
 
        LCD_WRITE_CMD(0x11);    //Exit Sleep 
        Delay_ms(120); 
				
        LCD_WRITE_CMD(0x29);    //Display on 
        LCD_WRITE_CMD(0x2c); 

}

//LCD进入休眠状态
void LCD_Sleep_ILI9340C(void)
{
  LCD_WRITE_CMD(0x28);//Display off
  Delay_ms(20);
  LCD_WRITE_CMD(0x10);//Enter Sleep mode
}
//LCD退出休眠状态
void LCD_ExitSleep_ILI9340C(void)
{
  LCD_WRITE_CMD(0x11);//Exit Sleep
  Delay_ms(120);
  LCD_WRITE_CMD(0x29);//Display on
  LCD_WRITE_CMD(0x2c); 
}

void main()
{
		//打开背光
		LED1 = 0;
		LED2 = 0;
		LED3 = 0;
		LED4 = 0;
		//硬件初始化
		HD_reset();
        Delay_ms(50);//延时50ms
		Initial_ILI9340C();
		Delay_ms(300);
		//测试进入/退出休眠
		LCD_Sleep_ILI9340C();
		Delay_ms(300);
		LCD_ExitSleep_ILI9340C();
		//显示黑屏
		LCD_TEST_SingleColor(Black); 
		Delay_ms(500);
		while(1){
		//循环6种颜色满屏
		LCD_TEST_SingleColor(Red);
		LCD_TEST_SingleColor(Green);
		LCD_TEST_SingleColor(Blue);
		LCD_TEST_SingleColor(Yellow);
		LCD_TEST_SingleColor(White);
		LCD_TEST_SingleColor(Black);
		//y轴高度100，画一条红色横线
		LCD_draw_Line(100,Red);
		Delay_ms(1000);
		}
}


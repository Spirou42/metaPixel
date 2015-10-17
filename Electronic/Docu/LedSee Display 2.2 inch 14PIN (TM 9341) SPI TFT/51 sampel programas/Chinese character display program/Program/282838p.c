
//     
/***************************************************/



#include <reg52.h>
#include <math.h>
#include <intrins.h>

#include "8X16.h"
#include "GB1616.h"	//16*16汉字字模
#include "GB3232.h"	//32*32汉字字模
#define uchar unsigned char
#define uint unsigned int

void LCD_PutChar8x16(unsigned short x, unsigned short y, char c, unsigned int fColor, unsigned int bColor);
void LCD_PutChar(unsigned short x, unsigned short y, char c, unsigned int fColor, unsigned int bColor);
void LCD_PutString(unsigned short x, unsigned short y, char *s, unsigned int fColor, unsigned int bColor);

void LCD_PutChar8x8(unsigned short x, unsigned short y, char c, unsigned int fColor, unsigned int bColor);
void PutGB1616(unsigned short x, unsigned short  y, unsigned char c[2], unsigned int fColor,unsigned int bColor);
void PutGB3232(unsigned short x, unsigned short  y, unsigned char c[2], unsigned int fColor,unsigned int bColor);


#define White          0xFFFF
#define Black          0x0000
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

sbit cs        =P3^5;
sbit rs        =P2^3;
sbit sda       =P2^2;
sbit scl       =P2^1;
sbit reset     =P3^4;


sbit ConAdd    =P3^1;
sbit ConSub    =P3^0;
sbit PAUSE     =P3^3;

sbit Type      =P3^5; //0-成品检测,1-半成品检测
sbit Mtp       =P3^2;  //MTP Burn


uchar code map1[];
uchar code map2[];

uchar bdata bitdata;
sbit bit7=bitdata^7;
sbit bit6=bitdata^6;
sbit bit5=bitdata^5;
sbit bit4=bitdata^4;
sbit bit3=bitdata^3;
sbit bit2=bitdata^2;
sbit bit1=bitdata^1;
sbit bit0=bitdata^0;

void  OTP_Program();
void  delay(uint t);
void  Contrast_Adjust();
uchar code Zk_ASCII8X16[];

void Display_Desc();
void Output_Pixel(uint x,uint y);
void Display_ASCII8X16(uint x0,uint y0,uchar *s);
void Test_FinshGoods();
void Test_HalfFinshGoods();
void EEPROM_ERASE();
void EEPROM_Writing();

void delay(uint time)
{
 uint i,j;
  for(i=0;i<time;i++)
   for(j=0;j<500;j++);
}

void Pause() interrupt 2
{
    delay(100);
    while(!PAUSE);
    delay(100);
    while(PAUSE);
    delay(100);
    while(!PAUSE);
    delay(100);
}



void LCD_CtrlWrite_IC(uchar c)
{
cs=0;


rs=0;
bitdata=c;


sda=bit7;scl=0;scl=1;
sda=bit6;scl=0;scl=1;
sda=bit5;scl=0;scl=1;
sda=bit4;scl=0;scl=1;
sda=bit3;scl=0;scl=1;
sda=bit2;scl=0;scl=1;
sda=bit1;scl=0;scl=1;
sda=bit0;scl=0;scl=1;
cs=1;


}
void  LCD_DataWrite_IC(uchar d)  
{
cs=0;
rs=1;
bitdata=d;

sda=bit7;scl=0;scl=1;
sda=bit6;scl=0;scl=1;
sda=bit5;scl=0;scl=1;
sda=bit4;scl=0;scl=1;
sda=bit3;scl=0;scl=1;
sda=bit2;scl=0;scl=1;
sda=bit1;scl=0;scl=1;
sda=bit0;scl=0;scl=1;
cs=1;
}

void LCD_DataWrite(uchar LCD_DataH,uchar LCD_DataL)
{
LCD_DataWrite_IC(LCD_DataH);
LCD_DataWrite_IC(LCD_DataL);
}
void wr_dat(unsigned int dat)
{
LCD_DataWrite_IC(dat>>8);

LCD_DataWrite_IC(dat);
}

void  write_command(uchar c)
{
cs=0;


rs=0;
bitdata=c;

sda=bit7;scl=0;scl=1;
sda=bit6;scl=0;scl=1;
sda=bit5;scl=0;scl=1;
sda=bit4;scl=0;scl=1;
sda=bit3;scl=0;scl=1;
sda=bit2;scl=0;scl=1;
sda=bit1;scl=0;scl=1;
sda=bit0;scl=0;scl=1;
cs=1;      
}

void  write_data(uchar d)
{
cs=0;
rs=1;
bitdata=d;
sda=bit7;scl=0;scl=1;
sda=bit6;scl=0;scl=1;
sda=bit5;scl=0;scl=1;
sda=bit4;scl=0;scl=1;
sda=bit3;scl=0;scl=1;
sda=bit2;scl=0;scl=1;
sda=bit1;scl=0;scl=1;
sda=bit0;scl=0;scl=1;
cs=1;
}


void Reset()
{
    reset=0;
    delay(100);
    reset=1;
    delay(100);
}
//////////////////////////////////////////////////////////////////////////////////////////////

void lcd_initial()
{

   reset=0;
   delay(100);
   reset=1;
   delay(100);

//------------------------------------------------------------------//  
//-------------------Software Reset-------------------------------//


  write_command(0xCB);  
        write_data(0x39); 
        write_data(0x2C); 
        write_data(0x00); 
        write_data(0x34); 
        write_data(0x02); 

        write_command(0xCF);  
        write_data(0x00); 
        write_data(0XC1); 
        write_data(0X30); 
 
        write_command(0xE8);  
        write_data(0x85); 
        write_data(0x00); 
        write_data(0x78); 
 
        write_command(0xEA);  
        write_data(0x00); 
        write_data(0x00); 
 
        write_command(0xED);  
        write_data(0x64); 
        write_data(0x03); 
        write_data(0X12); 
        write_data(0X81); 

        write_command(0xF7);  
        write_data(0x20); 
  
        write_command(0xC0);    //Power control 
        write_data(0x23);   //VRH[5:0] 
 
        write_command(0xC1);    //Power control 
        write_data(0x10);   //SAP[2:0];BT[3:0] 
 
        write_command(0xC5);    //VCM control 
        write_data(0x3e); //对比度调节
        write_data(0x28); 
 
        write_command(0xC7);    //VCM control2 
        write_data(0x86);  //--
 
        write_command(0x36);    // Memory Access Control 
        write_data(0x48); //C8	   //48 68竖屏//28 E8 横屏

        write_command(0x3A);    
        write_data(0x55); 

        write_command(0xB1);    
        write_data(0x00);  
        write_data(0x18); 
 
        write_command(0xB6);    // Display Function Control 
        write_data(0x08); 
        write_data(0x82);
        write_data(0x27);  
 
        write_command(0xF2);    // 3Gamma Function Disable 
        write_data(0x00); 
 
        write_command(0x26);    //Gamma curve selected 
        write_data(0x01); 
 
        write_command(0xE0);    //Set Gamma 
        write_data(0x0F); 
        write_data(0x31); 
        write_data(0x2B); 
        write_data(0x0C); 
        write_data(0x0E); 
        write_data(0x08); 
        write_data(0x4E); 
        write_data(0xF1); 
        write_data(0x37); 
        write_data(0x07); 
        write_data(0x10); 
        write_data(0x03); 
        write_data(0x0E); 
        write_data(0x09); 
        write_data(0x00); 

        write_command(0XE1);    //Set Gamma 
        write_data(0x00); 
        write_data(0x0E); 
        write_data(0x14); 
        write_data(0x03); 
        write_data(0x11); 
        write_data(0x07); 
        write_data(0x31); 
        write_data(0xC1); 
        write_data(0x48); 
        write_data(0x08); 
        write_data(0x0F); 
        write_data(0x0C); 
        write_data(0x31); 
        write_data(0x36); 
        write_data(0x0F); 
 
        write_command(0x11);    //Exit Sleep 
        delay(120); 
				
        write_command(0x29);    //Display on 
        write_command(0x2c); 
}



void  RamAdressSet()
{
   write_command(0x2A);
   write_data(0x00);
   write_data(0x00);
   write_data(0x00);
   write_data(0xaf);
  
   write_command(0x2B);
   write_data(0x00);
   write_data(0x00);
   write_data(0x00);
   write_data(0xdb);		  
}

static void LCD_SetPos(unsigned int x0,unsigned int x1,unsigned int y0,unsigned int y1)
{
       write_command(0x2A);
   write_data(x0>>8);
   write_data(x0);
   write_data(x1>>8);
   write_data(x1);
  
   write_command(0x2B);
   write_data(y0>>8);
   write_data(y0);
   write_data(y1>>8);
   write_data(y1);
	write_command(0x2c); 
}

void LCD_PutChar(unsigned short x, unsigned short y, char c, unsigned int fColor, unsigned int bColor) {


		LCD_PutChar8x16( x, y, c, fColor, bColor );
	}
 void LCD_PutString(unsigned short x, unsigned short y, unsigned char *s, unsigned int fColor, unsigned int bColor) {
	 unsigned char l=0;
	while(*s) {
		if( *s < 0x80) 
		    {
			LCD_PutChar(x+l*8,y,*s,fColor,bColor);
			s++;l++;
			}
		else
		    {
			PutGB1616(x+l*8,y,(unsigned char*)s,fColor,bColor);
			s+=2;l+=2;
			}
		}
	}

void LCD_PutChar8x16(unsigned short x, unsigned short y, char c, unsigned int fColor, unsigned int bColor)
{
 unsigned int i,j;
 LCD_SetPos(x,x+8-1,y,y+16-1);
 for(i=0; i<16;i++) {
		unsigned char m=Font8x16[c*16+i];
		for(j=0;j<8;j++) {
			if((m&0x80)==0x80) {
				wr_dat(fColor);
				}
			else {
				wr_dat(bColor);
				}
			m<<=1;
			}
		}
}
void PutGB1616(unsigned short x, unsigned short  y, unsigned char c[2], unsigned int fColor,unsigned int bColor){

	unsigned int i,j,k;

	LCD_SetPos(x,  x+16-1,y, y+16-1);

	for (k=0;k<64;k++) { //64标示自建汉字库中的个数，循环查询内码
	  if ((codeGB_16[k].Index[0]==c[0])&&(codeGB_16[k].Index[1]==c[1])){ 
    	for(i=0;i<32;i++) {
		  unsigned short m=codeGB_16[k].Msk[i];
		  for(j=0;j<8;j++) {
			if((m&0x80)==0x80) {
				wr_dat(fColor);
				}
			else {
				wr_dat(bColor);
				}
			m<<=1;
			} 
		  }
		}  
	  }	
	}

void PutGB3232(unsigned short x, unsigned short  y, unsigned char c[2], unsigned int fColor,unsigned int bColor){
	unsigned int i,j,k;

	LCD_SetPos(x,  x+32-1,y, y+32-1);

	for (k=0;k<15;k++) { //15标示自建汉字库中的个数，循环查询内码
	  if ((codeGB_32[k].Index[0]==c[0])&&(codeGB_32[k].Index[1]==c[1])){ 
    	for(i=0;i<128;i++) {
		  unsigned short m=codeGB_32[k].Msk[i];
		  for(j=0;j<8;j++) {
			if((m&0x80)==0x80) {
				wr_dat(fColor);
				}
			else {
				wr_dat(bColor);
				}
			m<<=1;
			} 
		  }
		}  
	  }	
	}


void dsp_single_colour(DH,DL)
{
unsigned int i,j;
 //RamAdressSet();
 for (i=0;i<320;i++)
    for (j=0;j<240;j++)
        LCD_DataWrite(DH,DL);
}



main()
{
 lcd_initial();

     while(1)
  {
 write_command(0x2C);


    dsp_single_colour(0xf8,0x00);
  LCD_PutString(2,20,"connan40.taobao.com/",Red,Blue);                  ///前景颜色和背景颜色，可以直接预定义，也可以直接填入16进制数字
LCD_PutString(2,40,"I LOVE MY JOB ",Blue,Blue2);
LCD_PutString(2,60,"闪耀电子科技有限公司",Magenta,Green);
LCD_PutString(2,80,"专业技术支持论坛",0x07e0,0xf800);
LCD_PutString(2,100,"专业开发板工控板",0xF800,Blue2);
LCD_PutString(2,120,"1234567890",0xF800,Blue);
LCD_PutString(2,140,"abcdefghijklmnopqistuvwxyz",0xF800,Magenta);
LCD_PutString(2,160,"`,./<>';:[]{}\|?)(-=+*&^%$",0xF800,Blue2);
PutGB3232(10,180,"闪",Blue,0X00);	
PutGB3232(50,180,"耀",Blue,0X00);	
PutGB3232(90,180,"电",Blue,0X00);	
PutGB3232(130,180,"子",Blue,0X00);	

  delay(1000);	
  PutGB3232(10,180,"闪",0xF800,0xF800);	
PutGB3232(50,180,"耀",0xF800,0xF800);	
PutGB3232(90,180,"电",0xF800,0xF800);	
PutGB3232(130,180,"子",0xF800,0xF800);
  delay(50);		
    }
 }



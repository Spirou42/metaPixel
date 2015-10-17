
//     
/***************************************************/

#include<reg51.h>
#include<absacc.h>
#include<intrins.h>
#include<string.h>
#define uchar unsigned char
#define uint unsigned int

sbit cs        =P3^5;
sbit rs        =P2^3;
sbit sda       =P2^2;
sbit scl       =P2^1;
sbit reset     =P3^4;

uchar bdata bitdata;
sbit bit7=bitdata^7;
sbit bit6=bitdata^6;
sbit bit5=bitdata^5;
sbit bit4=bitdata^4;
sbit bit3=bitdata^3;
sbit bit2=bitdata^2;
sbit bit1=bitdata^1;
sbit bit0=bitdata^0;


void  delay(uint t);


void delay(uint time)
{
 uint i,j;
  for(i=0;i<time;i++)
   for(j=0;j<2500;j++);
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

void wr_com16(unsigned char i,unsigned char j)    
{         
  cs=0;

rs=1;
bitdata=i;

sda=bit7;scl=0;scl=1;
sda=bit6;scl=0;scl=1;
sda=bit5;scl=0;scl=1;
sda=bit4;scl=0;scl=1;
sda=bit3;scl=0;scl=1;
sda=bit2;scl=0;scl=1;
sda=bit1;scl=0;scl=1;
sda=bit0;scl=0;scl=1;
cs=1;  
  cs=0;


rs=1;
bitdata=j;

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
//////////////////////////////////////////////////////////////////////////////////////////////

void lcd_initial()
{

   reset=0;
   delay(100);
   reset=1;
   delay(100);
 

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
 void addset(unsigned int x,unsigned int y)
{
  	write_command(0x2a);
	wr_com16(x>>8,x);
	write_command(0x2b);
	wr_com16(y>>8,y);
	write_command(0x2c);
}

void dsp_single_colour(DH,DL)
{
 unsigned int i,j;
addset(0,0);
 for (i=0;i<320;i++)
    for (j=0;j<240;j++)
        wr_com16(DH,DL);    

}



main()
{
 lcd_initial();

      while(1)
  {

  //  Disp_gradscal(); //灰阶
   // delay(500); 
      
//    dsp_single_colour(0x84,0x10);//灰色
//    delay(500);	

    //dsp_single_colour(0xff,0xff);//白色
   // delay(500);	
    	
    //dsp_single_colour(0x00,0x00);//黑色
   // delay(500);	
 
    	
 //   dsp_single_colour(0xf8,0x00);//红色
  //  delay(500);	
    

    dsp_single_colour(0x07,0xe0);//绿色


    dsp_single_colour(0x00,0x1f);//蓝色
///    delay(500);	


    }

 }




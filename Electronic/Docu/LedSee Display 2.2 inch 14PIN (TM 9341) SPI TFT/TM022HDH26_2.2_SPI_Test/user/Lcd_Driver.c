#include "stm32f10x.h"
#include "Lcd_Driver.h"
#include "LCD_Config.h"
#include "delay.h"

/******************** (C) COPYRIGHT 2011 闪耀电子嵌入式开发工作室 ********************
//深圳TFT液晶模组批发
//专注液晶批发
//全程技术支持
//Tel:15989313508
//QQ：573355510
//淘宝网店地址：mytft.taobao.com
//热诚欢迎您的光临~！
/***************************************************************/
//液晶屏数据引脚说明：
//   液晶屏引脚       对应功能
//  Pin2		CS
//	Pin3		SCL
//	Pin4		SDI
//	Pin5		RS
//	Pin6		RESET	
/***************************************************************/

/***************************************************************/
//程序接线说明：
//#define LCD_CTRL   		GPIOB		//定义TFT数据端口
//#define LCD_CS        	GPIO_Pin_11 //MCU_PB11 对应--->>TFT --PIN_CS
//#define LCD_SCL        	GPIO_Pin_12	//MCU_PB12 对应--->>TFT --SCL
//#define LCD_SDA        	GPIO_Pin_13	//MCU_PB13 对应--->>TFT --SDA 
//#define LCD_RS         	GPIO_Pin_14	//MCU_PB14 对应--->>TFT --RS
//#define LCD_RST     		GPIO_Pin_15	//MCU_PB15 对应--->>TFT --RST	
/***************************************************************/

/***************************************************************/
//本测试程序功能说明：
//1.全屏填充测试例程
//2.英文显示测试例程
//3.中文显示测试例程
//4.2D按钮显示测试例程
//5.数码管字体数字显示测试例程
//备注：由于本人时间很忙，未加入触摸测试和图片显示示例，有需要的客户可以联系我

//给予发送其他液晶模块上面所用的触摸例程和图片显示示例作为参考，谢谢理解。
/***************************************************************/

//------------------------io模拟spi部分---------------------------
// PB6-MOSI
#define SPIv_SetData(d) { if(d & 0x80) GPIO_SetBits(GPIOB,GPIO_Pin_15); else GPIO_ResetBits(GPIOB,GPIO_Pin_15);}
//PB7-MISO
#define SPIv_ReadData() GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)
// PE0-SCK
#define SPIv_SetClk()	GPIO_SetBits(GPIOB,GPIO_Pin_13)
#define SPIv_ClrClk()	GPIO_ResetBits(GPIOB,GPIO_Pin_13)

u8 SPIv_WriteByte(u8 Byte)
{
	u8 i,Read;
	
	for(i=8; i; i--)
	{	
		SPIv_ClrClk();
		SPIv_SetData(Byte);	
		Byte<<=1;
		SPIv_SetClk();
		//Read <<= 1;
		//Read |= SPIv_ReadData();
	}
	//SPIv_ClrClk();
	return Read;
}

//用io口模拟的spi初始化
void SPIv_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;  //PB7-MISO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;  //PB6-MOSI
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12| GPIO_Pin_10 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}




//spi 写一个字节
u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte)
{
	while((SPIx->SR&SPI_I2S_FLAG_TXE)==RESET);		//等待发送区空	  
	SPIx->DR=Byte;	 	//发送一个byte   
	while((SPIx->SR&SPI_I2S_FLAG_RXNE)==RESET);//等待接收完一个byte  
	return SPIx->DR;          	     //返回收到的数据			
} 

//设置SPI的速度
//SpeedSet:1,高速;0,低速;
void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet)
{
	SPIx->CR1&=0XFFC7;
	if(SpeedSet==1)//高速
	{
		SPIx->CR1|=SPI_BaudRatePrescaler_8;//Fsck=Fpclk/2	
	}
	else//低速
	{
		SPIx->CR1|=SPI_BaudRatePrescaler_32; //Fsck=Fpclk/32
	}
	SPIx->CR1|=1<<6; //SPI设备使能
} 

void SPI2_Init(void)	
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	 
	//配置SPI2管脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//SPI2配置选项
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 ,ENABLE);
	   
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);

	//使能SPI2
	SPI_Cmd(SPI2, ENABLE);   
}

/****************************************************************************
* 名    称：void ili9220B_WriteIndex(u16 idx)
* 功    能：写 ili9220B 控制器寄存器地址
* 入口参数：idx   寄存器地址
* 出口参数：无
* 说    明：调用前需先选中控制器，内部函数
****************************************************************************/
void Lcd_WriteIndex(u8 Index)
{
   u8 i=0;
   //SPI 写命令时序开始
   //LCD_CS_CLR;
   LCD_RS_CLR;
   //SPIv_WriteByte(Index);
   SPI_WriteByte(SPI2,Index);
   
   //LCD_CS_SET;
}

/****************************************************************************
* 名    称：void ili9220B_WriteData(u16 dat)
* 功    能：写 ili9220B 寄存器数据
* 入口参数：dat     寄存器数据
* 出口参数：无
* 说    明：向控制器指定地址写入数据，调用前需先写寄存器地址，内部函数
****************************************************************************/
void Lcd_WriteData(u8 Data)
{
   u8 i=0;
   //LCD_CS_CLR;
   LCD_RS_SET;
   //SPIv_WriteByte(Data);
   SPI_WriteByte(SPI2,Data);

   //LCD_CS_SET; 
}

void Lcd_WriteData16Bit(u8 DataH,u8 DataL)
{
	Lcd_WriteData(DataH);
	Lcd_WriteData(DataL);
}

void Lcd_WriteIndex16Bit(u8 DataH,u8 DataL)
{
	Lcd_WriteIndex(DataH);
	Lcd_WriteIndex(DataL);
}



void Lcd_Reset(void)
{
	LCD_RST_CLR;
	delay_ms(100);
	LCD_RST_SET;
	delay_ms(50);
}


void Lcd_Init(void)
{
	//SPIv_Init();
	SPI2_Init();
	Lcd_Reset();
	
	Lcd_WriteIndex(0xCB);  
        Lcd_WriteData(0x39); 
        Lcd_WriteData(0x2C); 
        Lcd_WriteData(0x00); 
        Lcd_WriteData(0x34); 
        Lcd_WriteData(0x02); 

        Lcd_WriteIndex(0xCF);  
        Lcd_WriteData(0x00); 
        Lcd_WriteData(0XC1); 
        Lcd_WriteData(0X30); 
 
        Lcd_WriteIndex(0xE8);  
        Lcd_WriteData(0x85); 
        Lcd_WriteData(0x00); 
        Lcd_WriteData(0x78); 
 
        Lcd_WriteIndex(0xEA);  
        Lcd_WriteData(0x00); 
        Lcd_WriteData(0x00); 
 
        Lcd_WriteIndex(0xED);  
        Lcd_WriteData(0x64); 
        Lcd_WriteData(0x03); 
        Lcd_WriteData(0X12); 
        Lcd_WriteData(0X81); 

        Lcd_WriteIndex(0xF7);  
        Lcd_WriteData(0x20); 
  
        Lcd_WriteIndex(0xC0);    //Power control 
        Lcd_WriteData(0x23);   //VRH[5:0] 
 
        Lcd_WriteIndex(0xC1);    //Power control 
        Lcd_WriteData(0x10);   //SAP[2:0];BT[3:0] 
 
        Lcd_WriteIndex(0xC5);    //VCM control 
        Lcd_WriteData(0x3e); //对比度调节
        Lcd_WriteData(0x28); 
 
        Lcd_WriteIndex(0xC7);    //VCM control2 
        Lcd_WriteData(0x86);  //--
 
        Lcd_WriteIndex(0x36);    // Memory Access Control 
        Lcd_WriteData(0x48); //C8	   //48 68竖屏//28 E8 横屏

        Lcd_WriteIndex(0x3A);    
        Lcd_WriteData(0x55); 

        Lcd_WriteIndex(0xB1);    
        Lcd_WriteData(0x00);  
        Lcd_WriteData(0x18); 
 
        Lcd_WriteIndex(0xB6);    // Display Function Control 
        Lcd_WriteData(0x08); 
        Lcd_WriteData(0x82);
        Lcd_WriteData(0x27);  
 
        Lcd_WriteIndex(0xF2);    // 3Gamma Function Disable 
        Lcd_WriteData(0x00); 
 
        Lcd_WriteIndex(0x26);    //Gamma curve selected 
        Lcd_WriteData(0x01); 
 
        Lcd_WriteIndex(0xE0);    //Set Gamma 
        Lcd_WriteData(0x0F); 
        Lcd_WriteData(0x31); 
        Lcd_WriteData(0x2B); 
        Lcd_WriteData(0x0C); 
        Lcd_WriteData(0x0E); 
        Lcd_WriteData(0x08); 
        Lcd_WriteData(0x4E); 
        Lcd_WriteData(0xF1); 
        Lcd_WriteData(0x37); 
        Lcd_WriteData(0x07); 
        Lcd_WriteData(0x10); 
        Lcd_WriteData(0x03); 
        Lcd_WriteData(0x0E); 
        Lcd_WriteData(0x09); 
        Lcd_WriteData(0x00); 

        Lcd_WriteIndex(0XE1);    //Set Gamma 
        Lcd_WriteData(0x00); 
        Lcd_WriteData(0x0E); 
        Lcd_WriteData(0x14); 
        Lcd_WriteData(0x03); 
        Lcd_WriteData(0x11); 
        Lcd_WriteData(0x07); 
        Lcd_WriteData(0x31); 
        Lcd_WriteData(0xC1); 
        Lcd_WriteData(0x48); 
        Lcd_WriteData(0x08); 
        Lcd_WriteData(0x0F); 
        Lcd_WriteData(0x0C); 
        Lcd_WriteData(0x31); 
        Lcd_WriteData(0x36); 
        Lcd_WriteData(0x0F); 
 
        Lcd_WriteIndex(0x11);    //Exit Sleep 
        delay_ms(120); 
				
        Lcd_WriteIndex(0x29);    //Display on 
        Lcd_WriteIndex(0x2c); 
#if 0
	//************* Start Initial Sequence **********//
	
	Lcd_WriteIndex16Bit(0x00,0x01);
	Lcd_WriteData16Bit(0x01,0x1C); // set SS and NL bit
	Lcd_WriteIndex16Bit(0x00,0x02);
	Lcd_WriteData16Bit(0x01,0x00); // set 1 line inversion
	Lcd_WriteIndex16Bit(0x00,0x03);
	Lcd_WriteData16Bit(0x10,0x30); // set GRAM write direction and BGR=1.//1030
	Lcd_WriteIndex16Bit(0x00,0x08);
	Lcd_WriteData16Bit(0x08,0x08); // set BP and FP
	Lcd_WriteIndex16Bit(0x00,0x0C);
	Lcd_WriteData16Bit(0x00,0x00); // RGB interface setting R0Ch=0x0110 for RGB 18Bit and R0Ch=0111for RGB16Bit
	Lcd_WriteIndex16Bit(0x00,0x0F);
	Lcd_WriteData16Bit(0x0b,0x01); // Set frame rate//0b01
	Lcd_WriteIndex16Bit(0x00,0x20);
	Lcd_WriteData16Bit(0x00,0x00); // Set GRAM Address
	Lcd_WriteIndex16Bit(0x00,0x21);
	Lcd_WriteData16Bit(0x00,0x00); // Set GRAM Address
	//*************Power On sequence ****************//
	delay_ms(50);                         // Delay 50ms
	Lcd_WriteIndex16Bit(0x00,0x10);
	Lcd_WriteData16Bit(0x0a,0x00); // Set SAP,DSTB,STB//0800
	Lcd_WriteIndex16Bit(0x00,0x11);
	Lcd_WriteData16Bit(0x10,0x38); // Set APON,PON,AON,VCI1EN,VC
	delay_ms(50);                  // Delay 50ms
	Lcd_WriteIndex16Bit(0x00,0x12);
	Lcd_WriteData16Bit(0x11,0x21); // Internal reference voltage= Vci;
	Lcd_WriteIndex16Bit(0x00,0x13);
	Lcd_WriteData16Bit(0x00,0x63); // Set GVDD
	Lcd_WriteIndex16Bit(0x00,0x14);
	Lcd_WriteData16Bit(0x4b,0x44); // Set VCOMH/VCOML voltage//3944
	//------------- Set GRAM area ------------------//
	Lcd_WriteIndex16Bit(0x00,0x30);
	Lcd_WriteData16Bit(0x00,0x00);
	Lcd_WriteIndex16Bit(0x00,0x31);
	Lcd_WriteData16Bit(0x00,0xDB);
	Lcd_WriteIndex16Bit(0x00,0x32);
	Lcd_WriteData16Bit(0x00,0x00);
	Lcd_WriteIndex16Bit(0x00,0x33);
	Lcd_WriteData16Bit(0x00,0x00);
	Lcd_WriteIndex16Bit(0x00,0x34);
	Lcd_WriteData16Bit(0x00,0xDB);
	Lcd_WriteIndex16Bit(0x00,0x35);
	Lcd_WriteData16Bit(0x00,0x00);
	Lcd_WriteIndex16Bit(0x00,0x36);
	Lcd_WriteData16Bit(0x00,0xAF);
	Lcd_WriteIndex16Bit(0x00,0x37);
	Lcd_WriteData16Bit(0x00,0x00);
	Lcd_WriteIndex16Bit(0x00,0x38);
	Lcd_WriteData16Bit(0x00,0xDB);
	Lcd_WriteIndex16Bit(0x00,0x39);
	Lcd_WriteData16Bit(0x00,0x00);
	// ----------- Adjust the Gamma Curve ----------//
	Lcd_WriteIndex16Bit(0x00,0x50);
	Lcd_WriteData16Bit(0x00,0x03);
	Lcd_WriteIndex16Bit(0x00,0x51);
	Lcd_WriteData16Bit(0x09,0x00);
	Lcd_WriteIndex16Bit(0x00,0x52);
	Lcd_WriteData16Bit(0x0d,0x05);
	Lcd_WriteIndex16Bit(0x00,0x53);
	Lcd_WriteData16Bit(0x09,0x00);
	Lcd_WriteIndex16Bit(0x00,0x54);
	Lcd_WriteData16Bit(0x04,0x07);
	Lcd_WriteIndex16Bit(0x00,0x55);
	Lcd_WriteData16Bit(0x05,0x02);
	Lcd_WriteIndex16Bit(0x00,0x56);
	Lcd_WriteData16Bit(0x00,0x00);
	Lcd_WriteIndex16Bit(0x00,0x57);
	Lcd_WriteData16Bit(0x00,0x05);
	Lcd_WriteIndex16Bit(0x00,0x58);
	Lcd_WriteData16Bit(0x17,0x00);
	Lcd_WriteIndex16Bit(0x00,0x59);
	Lcd_WriteData16Bit(0x00,0x1F);
	delay_ms(50);                    // Delay 50ms
	Lcd_WriteIndex16Bit(0x00,0x07);
	Lcd_WriteData16Bit(0x10,0x17);
	Lcd_WriteIndex16Bit(0x00,0x22);		
	delay_ms(200);
#endif

}


/*************************************************
函数名：LCD_Set_Region
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：xy起点和终点,Y_IncMode表示先自增y再自增x
返回值：无
*************************************************/
void Lcd_SetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end)
{	
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData16Bit(x_start>>8,x_start);
	Lcd_WriteIndex(0x2b);
	Lcd_WriteData16Bit(y_start>>8,y_start);
	Lcd_WriteIndex(0x2c);
	/*	
	Lcd_WriteIndex(0x50);
	Lcd_WriteData16Bit(x_start>>8,x_start);
	Lcd_WriteIndex(0x51);
	Lcd_WriteData16Bit(x_end>>8,x_end);

	Lcd_WriteIndex(0x52);
	Lcd_WriteData16Bit(y_start>>8,y_start);
	Lcd_WriteIndex(0x53);
	Lcd_WriteData16Bit(y_end>>8,y_end);


	Lcd_WriteIndex(0x22); */

}

/*************************************************
函数名：LCD_Set_XY
功能：设置lcd显示起始点
入口参数：xy坐标
返回值：无
*************************************************/
void Lcd_SetXY(u16 x,u16 y)
{
  	Lcd_WriteIndex(0x2a);
	Lcd_WriteData16Bit(x>>8,x);
	Lcd_WriteIndex(0x2b);
	Lcd_WriteData16Bit(y>>8,y);

	Lcd_WriteIndex(0x2c);
}

	
/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：无
返回值：无
*************************************************/
void Gui_DrawPoint(u16 x,u16 y,u16 Data)
{
	Lcd_SetRegion(x,y,x,y);
	Lcd_WriteData(Data>>8);
	Lcd_WriteData(Data);

}    

/*****************************************
 函数功能：读TFT某一点的颜色                          
 出口参数：color  点颜色值                                 
******************************************/
unsigned int Lcd_ReadPoint(u16 x,u16 y)
{
  unsigned int Data;
  Lcd_SetXY(x,y);

  //Lcd_ReadData();//丢掉无用字节
  //Data=Lcd_ReadData();
  Lcd_WriteData(Data);
  return Data;
}
/*************************************************
函数名：Lcd_Clear
功能：全屏清屏函数
入口参数：填充颜色COLOR
返回值：无
*************************************************/
void Lcd_Clear(u16 Color)               
{	
   unsigned int i,m;
   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
   //LCD_CS_CLR;
   //LCD_RS_SET;
   LCD_RS_SET;
   //SPIv_WriteByte(Data);

   
   for(i=0;i<Y_MAX_PIXEL;i++)
   {
    for(m=0;m<X_MAX_PIXEL;m++)
      {	 
	  	//SPIv_WriteByte(Color>>8);  
		//SPIv_WriteByte(Color);
		SPI_WriteByte(SPI2,Color>>8);
		SPI_WriteByte(SPI2,Color);
	  	//Lcd_WriteData16Bit(Color>>8,Color);
		//Lcd_WriteData(Color>>8);
		//Lcd_WriteData(Color);
      }   
	}
	 // LCD_CS_SET;  
}



    	  
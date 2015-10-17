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

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "delay.h"
GPIO_InitTypeDef GPIO_InitStructure;


void RCC_Configuration(void);

void Delayms(__IO uint32_t nCount);

unsigned char Num[10]={0,1,2,3,4,5,6,7,8,9};



void Redraw_Mainmenu(void)
{

	Lcd_Clear(GRAY0);
	
	Gui_DrawFont_GBK16(16,0,BLUE,GRAY0,"闪耀电子科技");
	Gui_DrawFont_GBK16(16,20,RED,GRAY0,"液晶测试程序");

	DisplayButtonUp(15,38,113,58); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(16,40,YELLOW,GRAY0,"颜色填充测试");

	DisplayButtonUp(15,68,113,88); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(16,70,BLUE,GRAY0,"文字显示测试");

	DisplayButtonUp(15,98,113,118); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(16,100,RED,GRAY0,"图片显示测试");


	Gui_DrawFont_GBK16(16,120,BLUE,GRAY0,"S1:Move.    ");
	Gui_DrawFont_GBK16(16,140,RED,GRAY0, "S2:Sellect  ");
	
	Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[5]);
	delay_ms(1000);
	Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[4]);
	delay_ms(1000);
	Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[3]);
	delay_ms(1000);
	Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[2]);
	delay_ms(1000);
	Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[1]);
	delay_ms(1000);
	Gui_DrawFont_Num32(100,125,RED,GRAY0,Num[0]);	
}

void Num_Test(void)
{
	u8 i=0;
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,20,RED,GRAY0,"Num Test");
	delay_ms(1000);
	Lcd_Clear(GRAY0);

	for(i=0;i<10;i++)
	{
	Gui_DrawFont_Num32((i%3)*40,32*(i/3)+30,RED,GRAY0,Num[i+1]);
	delay_ms(100);
	}
	
}

void Font_Test(void)
{
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,10,BLUE,GRAY0,"文字显示测试");

	delay_ms(1000);
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,30,YELLOW,GRAY0,"闪耀电子科技");


	Gui_DrawFont_GBK16(16,50,BLUE,GRAY0,"专注液晶批发");


	Gui_DrawFont_GBK16(16,70,RED,GRAY0, "全程技术支持");


	Gui_DrawFont_GBK16(0,100,BLUE,GRAY0,"Tel:15989313508");
	Gui_DrawFont_GBK16(0,130,RED,GRAY0, "mytft.taobao.com");
	
	delay_ms(3000);	
}

void Color_Test(void)
{
	u8 i=2;
	Lcd_Clear(GRAY0);
	
	Gui_DrawFont_GBK16(20,10,BLUE,GRAY0,"Color Test");
	delay_ms(1000);

	while(i--)
	{
	Lcd_Clear(WHITE);
	Lcd_Clear(BLACK);
	Lcd_Clear(RED);
  	Lcd_Clear(GREEN);
  	Lcd_Clear(BLUE);
	}		
}


u16 ID=0;
int main(void)
{
  //uint16_t a;
  /* System Clocks Configuration **********************************************/
  u8 i=2;
  SystemInit();
  delay_init(72);//延时初始化  
  Lcd_Init();


  Redraw_Mainmenu();
  Color_Test();
  Num_Test();
  Font_Test();
  Lcd_Clear(GRAY0);
  Gui_DrawFont_GBK16(10,50,BLUE,GRAY0,"测试成功.");
  delay_ms(1000);
  Redraw_Mainmenu();
  while(1);
  while (1)
  {    
	//刷屏1000次40秒  @72MHZ
	//全屏填充时间：40000ms/1000=40ms
	while(i--)
	{
	Lcd_Clear(WHITE);
	Lcd_Clear(BLACK);
	Lcd_Clear(RED);
  	Lcd_Clear(GREEN);
  	Lcd_Clear(BLUE);
	}
	Redraw_Mainmenu();
	//while(1);
  	

  }

}


void RCC_Configuration(void)
{   
  /* Setup the microcontroller system. Initialize the Embedded Flash Interface,  
     initialize the PLL and update the SystemFrequency variable. */
  SystemInit();
}


void Delayms(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */



void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif



/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

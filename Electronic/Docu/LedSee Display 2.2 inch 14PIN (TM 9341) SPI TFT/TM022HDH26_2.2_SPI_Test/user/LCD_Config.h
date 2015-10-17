#define LCD_8BIT_MODE   0	//定义液晶驱动为8位模式   1为使能 数据口使用高8位DP_H
#define LCD_16BIT_MODE  1	//定义液晶驱动为16位模式  1为使能

#define LCD_DRIVER_ILI9320  0
#define LCD_DRIVER_ILI9325  0
#define LCD_DRIVER_ILI9328  0
#define LCD_DRIVER_ILI9331  0
#define LCD_DRIVER_R61509V  0
#define LCD_DRIVER_HX8352   1

#define X_MAX_PIXEL	        240
#define Y_MAX_PIXEL	        320

#if ((LCD_DRIVER_ILI9320)||(LCD_DRIVER_ILI9325)||(LCD_DRIVER_ILI9328)||(LCD_DRIVER_ILI9331))
//9320功能寄存器地址
#define WINDOW_XADDR_START		0x0050 // 水平的开始地址组
#define WINDOW_XADDR_END		0x0051 // 水平的结束地址组
#define WINDOW_YADDR_START		0x0052 // 垂直的开始地址组
#define WINDOW_YADDR_END		0x0053 // 垂直的结束地址组
#define GRAM_XADDR		    	0x0020 // GRAM 水平的地址组
#define GRAM_YADDR		    	0x0021 // GRAM 垂直的地址组
#define GRAMWR 			    	0x0022 // GRAM
#endif

#if (LCD_DRIVER_R61509V)
//9325功能寄存器地址
#define WINDOW_XADDR_START		0x0210 // 水平的开始地址组
#define WINDOW_XADDR_END		0x0211 // 水平的结束地址组
#define WINDOW_YADDR_START		0x0212 // 垂直的开始地址组
#define WINDOW_YADDR_END		0x0213 // 垂直的结束地址组
#define GRAM_XADDR		    	0x0200 // GRAM 水平的地址组
#define GRAM_YADDR		    	0x0201 // GRAM 垂直的地址组
#define GRAMWR 			    	0x0202 // GRAM
#endif

#if (LCD_DRIVER_HX8352)
#define WINDOW_XADDR_START_H	0x0002 // 水平的开始地址组
#define WINDOW_XADDR_START_L	0x0003 // 水平的开始地址组
#define WINDOW_XADDR_END_H		0x0004 // 水平的结束地址组
#define WINDOW_XADDR_END_L		0x0005 // 水平的结束地址组

#define WINDOW_YADDR_START_H	0x0006 // 垂直的开始地址组
#define WINDOW_YADDR_START_L	0x0007 // 垂直的开始地址组
#define WINDOW_YADDR_END_H		0x0008 // 垂直的结束地址组
#define WINDOW_YADDR_END_L		0x0009 // 垂直的结束地址组

#define GRAM_XADDR_H		    0x0002 // GRAM 水平的地址组
#define GRAM_XADDR_L		    0x0003 // GRAM 水平的地址组
#define GRAM_YADDR_H		    0x0004 // GRAM 垂直的地址组
#define GRAM_YADDR_L		    0x0005 // GRAM 垂直的地址组
#define GRAMWR 			    	0x0022 // GRAM
#endif


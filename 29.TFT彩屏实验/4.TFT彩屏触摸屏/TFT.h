#ifndef __TFT_H
#define __TFT_H
#include <reg52.h>
#include <intrins.h>
#include "ch_enlib.h"

//#define FOSC 11059200L //晶振设置，默认使用11.0592M Hz
//#define FOSC 12000000L //晶振设置，使用12M Hz
#define FOSC 24000000L //晶振设置，使用24M Hz

//IO接口定义
#define  LCD_DataPortH P0     //高8位数据口,8位模式下只使用高8位 
sbit LCD_RS = P3^5;  		 //数据/命令切换
sbit LCD_WR = P3^6;		  //写控制
sbit LCD_RD =P3^4;		     //读控制
sbit LCD_CS=P3^7;		//片选	
sbit LCD_REST = P3^3;	      //复位 

void CLeanFullScreen(char VH,char VL);
char ShowPic(unsigned int x,unsigned int y,struct Picture pic);
void LcdInit(void);
char CleanRect(unsigned int x,unsigned int y,unsigned int x_lenght,unsigned int y_lenght);
void ShowString(unsigned int x,unsigned int y,unsigned char *str,unsigned int dcolor,unsigned int bgcolor);
void AddressSet(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2);
void LcdWriteData(char VH,char VL);
void ShowPoint(unsigned int x,unsigned int y,unsigned int color);

//画笔颜色
//#define WHITE         	 0xFFFF
//#define BLACK         	 0x0000	  
//#define BLUE         	 0x001F  
//#define BRED             0XF81F
//#define GRED 			 0XFFE0
//#define GBLUE			 0X07FF
//#define RED           	 0xF800
//#define MAGENTA       	 0xF81F
//#define GREEN         	 0x07E0
//#define CYAN          	 0x7FFF
//#define YELLOW        	 0xFFE0
//#define BROWN 			 0XBC40 //棕色
//#define BRRED 			 0XFC07 //棕红色
//#define GRAY  			 0X8430 //灰色
////GUI颜色

//#define DARKBLUE      	 0X01CF	//深蓝色
//#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
//#define GRAYBLUE       	 0X5458 //灰蓝色
////以上三色为PANEL的颜色 
// 
//#define LIGHTGREEN     	 0X841F //浅绿色
//#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

//#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
//#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

#endif

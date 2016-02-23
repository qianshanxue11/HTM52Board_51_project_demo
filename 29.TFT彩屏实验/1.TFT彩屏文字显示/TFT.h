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
#endif

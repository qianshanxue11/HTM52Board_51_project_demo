#ifndef __TFT_H
#define __TFT_H
#include <reg52.h>
#include <intrins.h>
#include "ch_enlib.h"

//#define FOSC 11059200L //�������ã�Ĭ��ʹ��11.0592M Hz
//#define FOSC 12000000L //�������ã�ʹ��12M Hz
#define FOSC 24000000L //�������ã�ʹ��24M Hz

//IO�ӿڶ���
#define  LCD_DataPortH P0     //��8λ���ݿ�,8λģʽ��ֻʹ�ø�8λ 
sbit LCD_RS = P3^5;  		 //����/�����л�
sbit LCD_WR = P3^6;		  //д����
sbit LCD_RD =P3^4;		     //������
sbit LCD_CS=P3^7;		//Ƭѡ	
sbit LCD_REST = P3^3;	      //��λ 

void CLeanFullScreen(char VH,char VL);
char ShowPic(unsigned int x,unsigned int y,struct Picture pic);
void LcdInit(void);
char CleanRect(unsigned int x,unsigned int y,unsigned int x_lenght,unsigned int y_lenght);

#endif

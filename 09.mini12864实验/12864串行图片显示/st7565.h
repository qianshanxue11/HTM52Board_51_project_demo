#ifndef __ST7565_H
#define __ST7565_H

#include<reg51.h>
#include<intrins.h>

//---包含字库头文件
#define CHAR_CODE


//---重定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

//--定时使用的IO口--//
#define DATA_PORT P0
sbit lcd_CS  = P3^5; //片选
sbit lcd_RST = P3^6; //复位
sbit lcd_RS  = P3^4; //命令与数据选择
sbit lcd_SCLK= P0^0; //串行钟
sbit lcd_SID = P0^1; //串行输入

extern unsigned char code pic_QQ[];
extern unsigned char code pic_monkey[];
extern unsigned char code pic_hantech[];
//--定义全局函数--//
void LcdWriteCmd(cmd);
void LcdWriteData(dat);
void Lcd12864_Init();
void LcdClearScreen(void);
uchar LcdWriteString(uchar x, uchar y, uchar *cn);
void LcdFullPic(const unsigned char *pic);

#endif

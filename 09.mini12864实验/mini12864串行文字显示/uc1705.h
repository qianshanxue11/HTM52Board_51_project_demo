#ifndef __ST7565_H
#define __ST7565_H

#include<reg51.h>
#include<intrins.h>

//---�����ֿ�ͷ�ļ�
#define CHAR_CODE


//---�ض���ؼ���---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

//--��ʱʹ�õ�IO��--//
#define DATA_PORT P0
sbit lcd_CS  = P3^5; //Ƭѡ
sbit lcd_RST = P3^6; //��λ
sbit lcd_RS  = P3^4; //����������ѡ��
sbit lcd_SCLK= P0^0; //������
sbit lcd_SID = P0^1; //��������

//--����ȫ�ֺ���--//
void LcdWriteCmd(cmd);
void LcdWriteData(dat);
void Lcd12864_Init();
void LcdClearScreen(void);
uchar LcdWriteString(uchar x, uchar y, uchar *cn);

#endif

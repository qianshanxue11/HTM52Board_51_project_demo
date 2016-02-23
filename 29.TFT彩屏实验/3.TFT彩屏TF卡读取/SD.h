#ifndef __SD_H
#define __SD_H
#include<reg52.h>

//IO接口定义
sbit SD_CS  = P1^0;
sbit SD_MOSI  = P1^1;
sbit SD_CLK = P1^2;
sbit SD_MISO  = P1^3;

extern unsigned char xdata DATA[512];

unsigned char SdReadBlock(unsigned char *Block, unsigned long address,int len);
unsigned char SdInit(void);

#endif

/********************************* 深圳市航太电子有限公司 *******************************
* 实 验 名 ：TFT彩屏文字显示
* 实验说明 ：TFT彩屏显示中文和英文两行文字
* 实验平台 ：航太51单片机开发板 V1.2
* 连接方式 ：TFT彩屏朝外连接
* 注    意 ：本程序使用8bit模式，也就是使用了整个P0口
* 作    者 ：航太电子产品研发部    QQ ：1909197536
* 店    铺 ：http://shop120013844.taobao.com/
****************************************************************************************/
#include<reg52.h>
#include <intrins.h>
#include <stdio.h>
#include "TFT.h"


//#define FOSC 11059200L //晶振设置，默认使用11.0592M Hz
//#define FOSC 12000000L //晶振设置，使用12M Hz
#define FOSC 24000000L //晶振设置，使用24M Hz

sbit DULA = P2^6;
sbit WELA = P2^7;

/*******************************************************************************
* 函 数 名 ：Delayms
* 函数功能 ：实现 ms级的延时
* 输    入 ：ms
* 输    出 ：无
*******************************************************************************/
void Delayms(unsigned int ms)
{
	unsigned int i,j;
	for(i=0;i<ms;i++)
	#if FOSC == 11059200L
		for(j=0;j<114;j++);
	#elif FOSC == 12000000L
	  for(j=0;j<123;j++);
	#elif FOSC == 24000000L
		for(j=0;j<249;j++);
	#else
		for(j=0;j<114;j++);
	#endif
}

/*******************************************************************************
* 函 数 名: main
* 函数功能: 主函数
* 输    入: 无
* 输    出: 无
*******************************************************************************/
void main()
{
	DULA = 1;
	P0 = 0xff;
	DULA = 0;
	WELA = 1;
	P0 = 0xff;
	WELA = 0;
	
	LcdInit();
	CLeanFullScreen(0xff,0xff);//清屏
	
	while (1)
	{
		ShowString(20,100,"欢迎使用航太电子单片机",BLUE,WHITE);
		ShowString(20,150,"Welcome To 51MCU World!",RED,WHITE);
//		ShowString(20,100,"!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ ",BLACK,WHITE);//显示所有字符
//		ShowString(20,200,"kdfgdsklbgd航太电子skdncs航太电子",BLACK,WHITE);
		while(1);
	}
}


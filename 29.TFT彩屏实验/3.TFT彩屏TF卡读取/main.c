/********************************* 深圳市航太电子有限公司 *******************************
* 实 验 名 ：TFT彩屏SD卡读取
* 实验说明 ：读取SD卡文本信息和图片到彩屏
* 实验平台 ：航太51单片机开发板 V1.2
* 连接方式 ：TFT彩屏朝外连接，将SD卡插入到彩屏后面，
*            使用杜邦线连接，SD_CS接P1.0，SD_DIN接P1.1，SD_SCK接P1.2，SD_OUT接P1.3
* 注    意 ：当前程序只支持小于4G的SD卡，SD卡中的图片和文本地址需要先在winhex软件中查看
* 作    者 ：航太电子产品研发部    QQ ：1909197536
* 店    铺 ：http://shop120013844.taobao.com/
****************************************************************************************/
#include<reg52.h>
#include <intrins.h>
#include <stdio.h>
#include "TFT.h"
#include "SD.h"


//#define FOSC 11059200L //晶振设置，默认使用11.0592M Hz
//#define FOSC 12000000L //晶振设置，使用12M Hz
#define FOSC 24000000L //晶振设置，使用24M Hz

//通过winhex可查看着文件的起始地址
#define PIC1ADD 0x428000
#define PIC2ADD 0x402000
#define TXTADD  0x401000

sbit DULA = P2^6;
sbit WELA = P2^7;
sbit LED = P1^5;

//char buf[512];
char printbuf[30];

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
* 函 数 名 ：UARTInit
* 函数功能 ：串口初始化 波特率9600
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void UARTInit()
{
	SCON=0X50;			
	TMOD=0X20;			
	TH1=0XFd;		 
	TL1=0XFd;
	ES=1;								
	TR1=1;				
}

/*******************************************************************************
* 函 数 名 ：PutNChar
* 函数功能 ：发送N个字符数据
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void PutNChar(char *buf,int length)
{
	int i;
  for(i=0;i<length;i++)
	{
	ES=0;
	TI=0;
	SBUF=buf[i];
	while(!TI);
	TI=0;
    ES=1;
	}
}

/*******************************************************************************
* 函 数 名 ：ShowFullSDPic
* 函数功能 ：显示SD卡的bin文件形式的图片
* 输    入 ：add 图片起始地址，通过winhex查看
* 输    出 ：无
*******************************************************************************/
void ShowFullSDPic(unsigned long add)
{
		unsigned long i=0,j=0,k=0;
		AddressSet(0,0,239,319);
		while(i < 76800)//240*320
		{
			LED = ~LED;
			SdReadBlock(DATA,add+k*512,512);
			for(j=0;j<256;j++)
			{
				LcdWriteData(DATA[j*2+1],DATA[j*2]);	//写一个像素点
				i++;
				if(i >= 76800) break;
			}
			k++;
		}
}

/*******************************************************************************
* 函 数 名: main
* 函数功能: 主函数
* 输    入: 无
* 输    出: 无
*******************************************************************************/
void main()
{
	//彩屏实验时最好关闭数码管的锁存器，避免数码管乱闪
	DULA = 1;
	P0 = 0xff;
	DULA = 0;
	WELA = 1;
	P0 = 0xff;
	WELA = 0;
	
	LcdInit();
	
	CLeanFullScreen(0xff,0xff);//清屏
	
	ShowString(0,0,"LCD init OK",RED,WHITE);
	
	if(!SdInit())
	{
		ShowString(0,20,"SD Init Fail",RED,WHITE);
		while(1);
	}
	
	if(!SdReadBlock(DATA,TXTADD,512))
	{
		ShowString(0,20,"SD Read Fail",RED,WHITE);
		while(1);
	}
	
	ShowString(0,20,DATA,RED,WHITE);//显示TXT文本内容
	Delayms(1000);
	while (1)
	{
		ShowFullSDPic(PIC1ADD);
		Delayms(2000);
		ShowFullSDPic(PIC2ADD);
		Delayms(2000);
		

	}
}


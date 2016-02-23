/********************************* �����к�̫�������޹�˾ *******************************
* ʵ �� �� ��TFT����������ʵ��
* ʵ��˵�� ��ʵ������Ļ����д��ͼ
* ʵ��ƽ̨ ����̫51��Ƭ�������� V1.2
* ���ӷ�ʽ ��TFT�����������ӣ���SD�����뵽�������棬
*            ʹ�öŰ������ӣ�SD_CS��P1.0��SD_DIN��P1.1��SD_SCK��P1.2��SD_OUT��P1.3
*            T_IRQ��P3.2��T_OUT��P1.4��T_DIN��P1.5��T_CS��P1.6��T_CLK��P1.7
* ע    �� ��1.ʵ��ʱ�п��ܳ��ִ���λ�ú���ʾλ�ò�ͬ���������ܱ�������ʾ����������ں��� ReadToutchAxis()
*              ���ڲ���
*            2.ʹ�ô��������˻�����ʱ����������������λ����Ļ�м䣬�м������������ܱ�Ҫ��
* ��    �� ����̫���Ӳ�Ʒ�з���    QQ ��1909197536
* ��    �� ��http://shop120013844.taobao.com/
****************************************************************************************/
#include<reg52.h>
#include <intrins.h>
#include <stdio.h>
#include "TFT.h"
#include "SD.h"
#include "toutch.h"


//#define FOSC 11059200L //�������ã�Ĭ��ʹ��11.0592M Hz
//#define FOSC 12000000L //�������ã�ʹ��12M Hz
//#define FOSC 24000000L //�������ã�ʹ��24M Hz

//ͨ��winhex�ɲ鿴���ļ�����ʼ��ַ
#define PIC1ADD 0x428000
#define PIC2ADD 0x402000
#define TXTADD  0x401000

sbit DULA = P2^6;
sbit WELA = P2^7;


struct pix_ touch_point;

/*******************************************************************************
* �� �� �� ��Delayms
* �������� ��ʵ�� ms������ʱ
* ��    �� ��ms
* ��    �� ����
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
* �� �� �� ��UARTInit
* �������� �����ڳ�ʼ�� ������9600
* ��    �� ����
* ��    �� ����
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
* �� �� �� ��PutNChar
* �������� ������N���ַ�����
* ��    �� ����
* ��    �� ����
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
* �� �� �� ��ShowFullSDPic
* �������� ����ʾSD����bin�ļ���ʽ��ͼƬ
* ��    �� ��add ͼƬ��ʼ��ַ��ͨ��winhex�鿴
* ��    �� ����
*******************************************************************************/
void ShowFullSDPic(unsigned long add)
{
		unsigned long i=0,j=0,k=0;
		AddressSet(0,0,239,319);
		while(i < 76800)//240*320
		{
			SdReadBlock(DATA,add+k*512,512);
			for(j=0;j<256;j++)
			{
				LcdWriteData(DATA[j*2+1],DATA[j*2]);	//дһ�����ص�
				i++;
				if(i >= 76800) break;
			}
			k++;
		}
}

/*******************************************************************************
* �� �� ��: main
* ��������: ������
* ��    ��: ��
* ��    ��: ��
*******************************************************************************/
void main()
{
	//����ʵ��ʱ��ùر�����ܵ����������������������
	DULA = 1;
	P0 = 0xff;
	DULA = 0;
	WELA = 1;
	P0 = 0xff;
	WELA = 0;
	
	touch_point.x = 0;
	touch_point.y = 0;
	
	LcdInit();
	
	CLeanFullScreen(0xff,0xff);//����
	
//	ShowString(0,0,"LCD init OK",RED,WHITE);
//	
//	if(!SdInit())
//	{
//		ShowString(0,20,"SD Init Fail",RED,WHITE);
//	}
//	
//	//��ȡtxt����
//	if(!SdReadBlock(DATA,TXTADD,512))
//	{
//		ShowString(0,20,"SD Read Fail",RED,WHITE);
//	}
//	else
//	{
//		ShowString(0,20,DATA,RED,WHITE);
//		Delayms(2000);	
//		
//		//��ʾͼƬ
//		ShowFullSDPic(PIC1ADD);
//		Delayms(2000);		
//	}	
	
	
	ShowString(0,40,"Please Touch Me!",RED,WHITE);
	while (1)
	{		
		touch_point = ReadToutchAxis();//��ȡ�����崥������
		if(touch_point.x != 0xffff)
		{
			ShowPoint(touch_point.x,touch_point.y,GREEN);//��ʾ���������
		}
	}
}


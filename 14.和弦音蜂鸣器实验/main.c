/********************************* 深圳市航太电子有限公司 *******************************
* 实 验 名 ：和弦音蜂鸣器实验
* 实验说明 ：在main函数中，对函数BeepOn()赋予不同的参数，蜂鸣器会唱出不同的歌曲
* 实验平台 ：航太51单片机开发板 V1.2
* 连接方式 ：跳线帽CN3 引脚2,4相连；跳线帽CN4 引脚1,3相连
* 注    意 ：1.本程序使用了指针，结构体等较高级的用法，请仔细看源程序
*            2.一个音由3部分组成，频率，BEEP导通时间，BEEP2振动时间
*            3.BEEP导通时给电容充电，关闭后电容放电继续时蜂鸣器响，并响度会逐渐降低从而产生拖音效果，BEEP2控制频率和一个音的时间
* 作    者 ：航太电子产品研发部    QQ ：1909197536
* 店    铺 ：http://shop120013844.taobao.com/
****************************************************************************************/
#include <reg52.h>
#include <intrins.h>

#define FOSC 11059200L //晶振设置，默认使用11.0592M Hz
//#define FOSC 12000000L //晶振设置，使用12M Hz
//#define FOSC 24000000L //晶振设置，使用24M Hz

#define TIME_MS 100 //设定定时时间 ms 


//IO接口定义
sbit BEEP2 = P1^4;
sbit beer = P1^7;

//全局变量定义
bit Beep_EN;
unsigned char *pSoundSel;//选曲指针 (*pSoundSel)[10]
unsigned char time_h;
unsigned char time_l;

/*=======================以下是音调定义======================*/
#define	 TONE_L1	1
#define	 TONE_L2	2
#define	 TONE_L3	3
#define	 TONE_L4	4
#define	 TONE_L5	5
#define	 TONE_L6	6
#define	 TONE_L7	7
#define	 TONE_M1	8
#define	 TONE_M2	9
#define	 TONE_M3	10
#define	 TONE_M4	11
#define	 TONE_M5	12
#define	 TONE_M6	13
#define	 TONE_M7	14
#define	 TONE_H1	15
#define	 TONE_H2	16
#define	 TONE_H3	17
#define	 TONE_H4	18
#define	 TONE_H5	19
#define	 TONE_H6	20
#define	 TONE_H7	21

#define END 0//音频结束位 

#define BUZZER_IO	BEEP2		//BEEP IO

#define PWR_CTRL	beer		//蜂鸣器供电控制脚

#define S_SHUTDOWN 1//关机音
#define S_POWERON  2//开机音
#define S_SINGLE   3//单音
#define S_MUSIC    4//两只老虎
#define S_ALL      5//所有音符

/*=========================音调频率表=======================*/
unsigned int code Tune_Tbl[21]=
{
	523	,//	低音1	,TONE_L1
	587	,//	低音2	,TONE_L2
	659	,//	低音3	,TONE_L3
	698	,//	低音4	,TONE_L4
	784	,//	低音5	,TONE_L5
	880	,//	低音6	,TONE_L6
	988	,//	低音7	,TONE_L7
	1046	,//	中音1	,TONE_M1
	1175	,//	中音2	,TONE_M2
	1318	,//	中音3	,TONE_M3
	1397	,//	中音4	,TONE_M4
	1568	,//	中音5	,TONE_M5
	1760	,//	中音6	,TONE_M6
	1976	,//	中音7	,TONE_M7
	2093	,//	高音1	,TONE_H1
	2394	,//	高音2	,TONE_H2
	2637	,//	高音3	,TONE_H3
	2794	,//	高音4	,TONE_H4
	3136	,//	高音5	,TONE_H5
	3520	,//	高音6	,TONE_H6
	3951	,//	高音7	,TONE_H7
};

/*======================关机和弦1,2,3...====================*/
unsigned char code Sound_ShutDown[10]=		//频率，节拍，音频时间
{
	 TONE_H1,2,14,TONE_H2,2,6,TONE_H3,6,60,END
};
/*======================开机和弦3,2,1...====================*/

unsigned char code Sound_PowerOn[10]=		
{
	TONE_H3,2,6,TONE_H2,2,14, TONE_H1,6,60,END
};
/*======================单音和弦2...=======================*/
unsigned char code Sound_Single[10]=		
{
	TONE_H2,12,60,END,END,END,END,END,END,END,
};

unsigned char code TONE_DefTone4[]=//两只老虎,两只老虎,跑得快，跑得快
{
	TONE_H1,2,50,//1
	TONE_H2,2,50,//2
	TONE_H3,2,50,//3
	TONE_H1,2,70,//1
	TONE_H1,2,50,//1
	TONE_H2,2,50,//2
	TONE_H3,2,50,//3
	TONE_H1,2,70,//1
	TONE_H2,2,50,//2
	TONE_H3,2,50,//3
	TONE_H5,2,80,//4
	TONE_H2,2,50,//2
	TONE_H3,2,50,//3
	TONE_H5,2,100,//4
	END,0,0//
};

//演奏所有的音
unsigned char code Sound_all[]=		
{
	1	,	2,30,
	2	,	2,30,
	3	,	2,30,
	4	,	2,30,
	5	,	2,30,
	6	,	2,30,
	7	,	2,30,
	8	,	2,30,
	9	,	2,30,
	10 ,	2,30,
	11	,	2,30,
	12	,	2,30,
	13	,	2,30,
	14	,	2,30,
	15	,	2,30,
	16	,	2,30,
	17	,	2,30,
	18	,	2,30,
	19	,	2,30,
	20	,	2,30,
	21	,	2,30,
	END,
};

#define S_SHUTDOWN 1//关机音
#define S_POWERON  2//开机音
#define S_SINGLE   3//单音
#define S_MUSIC    4//两只老虎
#define S_ALL      5//所有音符

struct BE
{
	unsigned int	CountFR;	//定时器计数值，通过改变计数值改变音频频率
	unsigned char PWR_time; 	//开启时间,单位为10mS
	unsigned char FREQ_time; 	//音频供给时间,单位为10mS
	unsigned char index;	 	//音符计数，指示当前需要演奏的音符
}Beep;

/*******************************************************************************
* 函 数 名 ：BeepOn
* 函数功能 ：选择声音种类，蜂鸣器唱出相应的歌曲
* 输    入 ：SoundSel 
* 输    出 ：无
*******************************************************************************/
void BeepOn(unsigned char SoundSel)
{
	Beep_EN=1;
	Beep.index=0;
	
	switch(SoundSel)
	{
	case S_SHUTDOWN:
		pSoundSel=&Sound_ShutDown;
		break;
	
	case S_POWERON:		
		pSoundSel=&Sound_PowerOn;	
		break;
	
	case S_SINGLE:		
		pSoundSel=&Sound_Single;		
		break;
	
		case S_MUSIC:	
		pSoundSel=&TONE_DefTone4;
		break;
		
	case S_ALL:
		pSoundSel=&Sound_all;
	break;
	
	default:
		break;
	}	
}

/*******************************************************************************
* 函 数 名 ：BeepSet
* 函数功能 ：蜂鸣器参数设置
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void BeepSet()
{	 
	
	if (pSoundSel[Beep.index]!=END)
	{
		Beep.CountFR = 65535-1000000/(Tune_Tbl[pSoundSel[Beep.index]-1] * 2);//需要对音调频率*2
		Beep.PWR_time=pSoundSel[Beep.index+1];
		
		Beep.FREQ_time=pSoundSel[Beep.index+2];
		
		Beep.index=Beep.index+3;
		
	}else {Beep.index=0; PWR_CTRL=1;Beep_EN=0;}
	return;
}

/*******************************************************************************
* 函 数 名 ：BeepControl
* 函数功能 ：蜂鸣器控制 , 每10ms调用一次
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void BeepControl()//每10mS调用一次
{
	
	if (Beep_EN)
	{
		
		if (Beep.PWR_time) 
		{
			PWR_CTRL=0;//供电	
			Beep.PWR_time--;
		}
		else 
		{
			PWR_CTRL=1;//掉电,依靠电容放电实现和弦拖音 	
			
		}
		
		if (Beep.FREQ_time)
		{
			Beep.FREQ_time--; 
			TR0 = 1;//启动定时器发声
		}
		else 
		{
			TR0 = 0;//停止定时器发声
			BeepSet();
			PWR_CTRL=0;//发声完毕，打开电源给电容充电	
		}
	}
	else
	{
		BUZZER_IO=0;
		TR0 = 0;//停止定时器发声
		
	}
	return;
}

/*******************************************************************************
* 函 数 名 ：InitTime
* 函数功能 ：定时器初始化
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void InitTime()
{ 
	TMOD|=0x11;	//定时器0和定时器1采用相同配置 
	TH0=0xff;
	TL0=0xef;	 
	TH1=(65536-10000)/256;
	TL1=(65536-10000)%256;	//10ms定时
	ET0 = 1; 
	ET1 = 1; 
	TR0=0;
	TR1 = 1;
	EA = 1;
}

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
* 函 数 名 ：main
* 函数功能 ：主函数
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void main()
{
	InitTime();
	while(1)
	{
		if(Beep_EN == 0)//实现循环播放
		{
			BeepOn(S_SHUTDOWN);//关机音
			Delayms(2000);
			BeepOn(S_POWERON);//开机音
			Delayms(2000);
			BeepOn(S_SINGLE);//单音
			Delayms(2000);
			BeepOn(S_MUSIC);//两只老虎
			Delayms(8000);
			BeepOn(S_ALL);//所有音符
			Delayms(2000);
		}	
	}
}

/*******************************************************************************
* 函 数 名 ：Timer0Int
* 函数功能 ：定时器0中断服务函数，控制频率
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void Timer0Int()	interrupt 1 using 1	//采用中断0 控制频率	
{	
	TH0=Beep.CountFR/256;	 
	TL0=Beep.CountFR%256;	
	BUZZER_IO = ~BUZZER_IO;	 
}	 

/*******************************************************************************
* 函 数 名 ：Timer1Int
* 函数功能 ：定时器1中断服务函数， 控制节拍时长 10ms
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void Timer1Int()	interrupt 3	using 3 //采用中断0 控制频率	
{	
	TH1=(65536-10000)/256;
	TL1=(65536-10000)%256;	//10ms定时
	BeepControl(); 
}	 

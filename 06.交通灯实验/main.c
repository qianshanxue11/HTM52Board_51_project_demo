/********************************* 深圳市航太电子有限公司 *******************************
* 实 验 名 ：交通灯实验
* 实验说明 ：依据交通规则点亮一个十字路口的交通灯
* 实验平台 ：航太51单片机开发板 V1.2
* 连接方式 ：短路帽CN1 引脚4,6短接，引脚3,5短接；短路帽CN2 引脚4,6短接，引脚3,5短接；
*            短路帽CN3 引脚4,6短接，引脚3,5短接；短路帽CN4 引脚4,6短接，引脚3,5短接；
*            短路帽CN6 引脚1,2短接；
* 注    意 ：本实验主要是为了熟悉定时器和数码管动态扫描，交通灯实际运行比本程序要复杂得多，请不要太较真
* 作    者 ：航太电子产品研发部    QQ ：1909197536
* 店    铺 ：http://shop120013844.taobao.com/
****************************************************************************************/

#include <reg52.h>
#include <intrins.h>

#define FOSC 11059200L //晶振设置，默认使用11.0592M Hz
//#define FOSC 12000000L //晶振设置，使用12M Hz
//#define FOSC 24000000L //晶振设置，使用24M Hz

#define TIME_MS 50 //设定定时时间 ms ,在11.0592M晶振下，不易超过60ms

//IO接口定义
sbit led_hepin_red = P1^0;      //和平大道车辆红灯
sbit led_hepin_green = P1^1;    //和平大道车辆绿灯
sbit led_hepin_yellow = P1^2;   //和平大道车辆黄灯

sbit led_jianjun_red = P1^3;     //建军路车辆红灯
sbit led_jianjun_green = P1^4;   //建军路车辆绿灯
sbit led_jianjun_yellow = P1^5;  //建军路车辆黄灯

sbit led_hepin_p_red = P1^6;     //和平大道行人红灯
sbit led_hepin_p_green = P1^7;   //和平大道行人绿灯

sbit led_jianjun_p_red = P3^6;   //建军路行人红灯
sbit led_jianjun_p_green = P3^7; //建军路行人绿灯

#define LED_PORT P0
sbit dula=P2^6;
sbit wela=P2^7;

//全局变量定义
unsigned char Second=1,count=0,disp_data=0;
//LED显示字模 0-F 共阳模式
unsigned code table[]= {0Xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};

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
* 函 数 名 ：Timer0Init
* 函数功能 ：定时器0初始化
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void Timer0Init()
{
	TMOD=0x01; //设置定时器0工作方式为1
	TH0=(65536-FOSC/12/1000*TIME_MS)/256;
	TL0=(65536-FOSC/12/1000*TIME_MS)%256;
	ET0=1; //开启定时器0中断
	TR0=1;	//开启定时器	
	EA=1;  //打开总中断
}

/*******************************************************************************
* 函 数 名 ：LEDdisplay
* 函数功能 ：循环显示各个位上的数据
* 输    入 ：num 需要显示的数据
* 输    出 ：无
*******************************************************************************/
void LEDdisplay(unsigned int num)
{
	unsigned char shi,ge;
	shi=num/10;
	ge=num%10;
	
	wela=1;
	LED_PORT=0x02;
	wela=0;
	dula=1;			   //显示十位
	LED_PORT=table[shi];	 
	dula=0;
	Delayms(1);
	
	wela=1;
	LED_PORT=0x04;
	wela=0;
	dula=1;			 // 显示个位
	LED_PORT=table[ge];
	dula=0;
	Delayms(1);
}

/*******************************************************************************
* 函 数 名 ：OffAllLED
* 函数功能 ：熄灭所有灯
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void OffAllLED(void)
{
	P1 = 0xff;
	P3 |= 0xc0;
}

/*******************************************************************************
* 函 数 名 ：main
* 函数功能 ：主函数，实现LED灯从上到下循环移动
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void main()
{
	Timer0Init();
	while(1)
	{
		if(Second == 70)
		{
			Second = 1;
		}

		/**和平大道车辆通行，建军路人行道通行 30秒**/
		if(Second < 31)
		{
			disp_data = 30 - Second;//数码管显示倒数时间
			OffAllLED();

			led_hepin_green = 0;    //和平大道车辆绿灯亮			
			led_hepin_p_red = 0;      //和平大道行人红灯亮

			led_jianjun_red = 0;      //建军路车辆红灯亮
			led_jianjun_p_green	= 0;    //建军路人行道绿灯亮	
		}

		/***黄灯等待切换状态，5秒***/
		else if(Second < 36) 
		{
			/**和平大道车辆黄灯阶段**/
			disp_data = 35 - Second;//数码管显示倒数时间
			OffAllLED();

			led_hepin_yellow = 0;    //和平大道车辆黄灯亮			
			led_hepin_p_red = 0;      //和平大道行人红灯亮

			led_jianjun_red = 0;      //建军路车辆红灯亮
			led_jianjun_p_red	= 0;    //建军路人行道红灯亮
		}

		/**建军路车辆通行，和平大道人行道通行 30秒**/
		else if(Second < 66) 
		{
			disp_data = 65 - Second;//数码管显示倒数时间
			OffAllLED();

			led_hepin_red = 0;    //和平大道车辆红灯亮
			led_hepin_p_green= 0;      //和平大道行人绿灯亮
			
			led_jianjun_green = 0;      //建军路车辆绿灯亮
			led_jianjun_p_red	= 0;    //建军路人行道红灯亮
		}

		/***黄灯等待切换状态，5秒***/
		else 
		{
			disp_data = 70 - Second;//数码管显示倒数时间
			OffAllLED();

			led_hepin_red = 0;    //和平大道车辆红灯亮
			led_hepin_p_red= 0;      //和平大道行人绿灯亮
			
			led_jianjun_yellow = 0;      //建军路车辆绿灯亮
			led_jianjun_p_red = 0;    //建军路人行道红灯亮
		}
		LEDdisplay(disp_data);
	}	
}

/*******************************************************************************
* 函 数 名 ：Timer0Int
* 函数功能 ：定时器0中断函数 ， 每隔TIME_MS ms进入
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void Timer0Int() interrupt 1
{
	TH0=(65536-FOSC/12/1000*TIME_MS)/256;
	TL0=(65536-FOSC/12/1000*TIME_MS)%256;
	count++;
	if(count == 20)//1s
	{
		Second ++;
		count = 0;
	}
}

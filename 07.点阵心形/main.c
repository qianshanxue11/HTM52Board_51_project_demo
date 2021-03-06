/********************************* 深圳市航太电子有限公司 *******************************
* 实 验 名 ：8x8点阵动态扫描,显示心形
* 实验说明 ：烧录成功后显示一个上下闪烁的红心
* 实验平台 ：航太51单片机开发板 V1.2
* 连接方式 ：跳线帽CN4 引脚4,6短接，引脚3,5短接；
*            跳线帽CN3 引脚4,6短接，引脚3,5短接；
*			 跳线帽CN1 引脚4,6短接，引脚3,5短接；
*            跳线帽CN2 引脚4,6短接，引脚3,5短接
* 注    意 ：点阵实验必须关闭数码管的位选WELA，否则会有漏电流影响实验效果
*            插入点阵时注意带字的那一边朝下
*            运行程序时如果出现亮点较乱，或某一条LED无显示或全显示，可能是因为没插好，请拔下来对好重新插入
* 作    者 ：航太电子产品研发部    QQ ：1909197536
* 店    铺 ：http://shop120013844.taobao.com/
****************************************************************************************/

#include <reg52.h>
#include <intrins.h> 

#define FOSC 11059200L //晶振设置，默认使用11.0592M Hz
//#define FOSC 12000000L //晶振设置，使用12M Hz
//#define FOSC 24000000L //晶振设置，使用24M Hz

//IO接口定义
#define LED_SEG P1
#define LED_DATA P0
sbit WELA = P2^7;


//全局变量定义
unsigned char code seg[]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};//分别对应相应的段亮
unsigned char code dofly[]={0x0c,0x12,0x22,0x44,0x22,0x12,0x0c,0x00};// 心的形状

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
	unsigned char i=0;
	unsigned char j=0;
	WELA = 1; //关闭数码管相关的位选
	LED_DATA = 0;
	WELA = 0;

	while(1)
	{	
		j=0;
		while(j<30)
		{
			i = 0;
			while(i<8)
			{
				LED_SEG=seg[i];  //取段码
				LED_DATA=dofly[i];//取显示数据	

				Delayms(2); //扫描间隙延时
				i++;
			}
			j++;
	  }
		
		Delayms(10); //扫描间隙延时
			
		j=0;
		while(j<30)
		{
			i = 0;
			while(i<8)
			{
				LED_SEG=seg[i];  //取段码
				LED_DATA=(dofly[i]<<1 & 0xfe);//取显示数据	

				Delayms(2); //扫描间隙延时
				i++;
			}
			j++;
	  }
	}	
}

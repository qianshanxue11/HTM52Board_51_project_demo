/********************************* 深圳市航太电子有限公司 *******************************
* 实 验 名 ：DS1302数字时钟实验
* 实验说明 ：显示时分秒的值到数码管上，注释函数InitDs1302() 再次烧录后，时间仍在继续运行
* 实验平台 ：航太51单片机开发板 V1.2
* 连接方式 ：短路帽CN1，引脚1,3连接；短路帽CN2，引脚2,4连接，引脚1,3连接
* 注    意 ：数码管采用供阳模式，位选和段选时要输出高电平
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
#define LED_PORT P0
sbit dula=P2^6;
sbit wela=P2^7;

sbit DS1302_CLK = P1^1;
sbit DS1302_IO  = P1^2;
sbit DS1302_RST = P1^3;

//全局变量定义
unsigned char count,temp;
unsigned char second,minute,hour,week,day,month,year;		//秒、分、时、星期、日、月、年
unsigned char time[]={0x06,0x03,0x14,0x03,0x10,0x59,0x55};	//初始时间数组

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
* 函 数 名 ：LEDdisplay
* 函数功能 ：循环显示各个位上的数据
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void LEDdisplay()
{
		unsigned char shi,ge;
		shi=hour/10%10;
		ge=hour%10;

		wela=1;
		P0=0x01;
		wela=0;
		P0=0;
		dula=1;
		P0=table[shi];
		dula=0;
		Delayms(1);

		wela=1;
		P0=0x02;
		wela=0;
		P0=0;
		dula=1;
		P0=table[ge];
		dula=0;
		Delayms(1);

		shi=minute/10%10;
		ge=minute%10;
		wela=1;
		P0=0x04;
		wela=0;
		P0=0;
		dula=1;
		P0=table[shi];
		dula=0;
		Delayms(1);

		wela=1;
		P0=0x08;
		wela=0;
		P0=0;
		dula=1;
		P0=table[ge];
		dula=0;
		Delayms(1);

		shi=second/10%10;
		ge=second%10;
		wela=1;
		P0=0x10;
		wela=0;
		P0=0;
		dula=1;
		P0=table[shi];
		dula=0;
		Delayms(1);

		wela=1;
		P0=0x20;
		wela=0;
		P0=0;
		dula=1;
		P0=table[ge];
		dula=0;
//		Delayms(1);  
}

/*****************************************************************************
* 函 数 名 ：InputByte
* 函数功能：向DS1302送一字节数据子程序
* 输    入：byte1
* 输    出：
*****************************************************************************/
void InputByte(unsigned char byte1)
{
	char i;
	for(i=8;i>0;i--)
	{
		DS1302_IO=(bit)(byte1&0x01);
		DS1302_CLK=1;
		_nop_();_nop_();
		DS1302_CLK=0;
		byte1>>=1;
	}
	return;
}

/*****************************************************************************
* 函 数 名：outputbyte
* 函数功能：读DS1302一个字节子程序
* 输    入：
* 输    出：读取的数值
*****************************************************************************/
unsigned char OutPutByte(void)  
{
	unsigned char i;
	unsigned ucdat=0;
	for(i=8;i>0;i--)
	{
		DS1302_IO=1;
		ucdat>>=1;
		if(DS1302_IO)ucdat|=0x80;
		DS1302_CLK=1;
		_nop_();_nop_();
		DS1302_CLK=0;
	 }
	 return(ucdat);
}

/*****************************************************************************
* 函 数 名：write_ds1302
* 函数功能：向DS1302某地址写一字节数据子程序
* 输    入：addr,TDat
* 输    出：
*****************************************************************************/
void WriteDs1302(unsigned char addr,unsigned char TDat)
{
	DS1302_RST=0;
	_nop_();_nop_();
	DS1302_CLK=0;
	_nop_();_nop_();
	DS1302_RST=1;
 	InputByte(addr);//传送8字节地址命令
	_nop_();_nop_();
	InputByte(TDat);//写入数据命令
	DS1302_CLK=1;
	_nop_();_nop_();
	DS1302_RST=0;
}

/*****************************************************************************
* 函 数 名：read_ds1302
* 函数功能：读DS1302地址子程序
* 输    入：add
* 输    出：timedata
*****************************************************************************/
unsigned char ReadDs1302(unsigned char addr)
{
	unsigned char timedata,aa;
	DS1302_RST=0;
	_nop_();_nop_();
	DS1302_CLK=0;
	_nop_();_nop_();
	DS1302_RST=1;
	InputByte(addr);//传送8字节地址命令
	timedata=OutPutByte();//读取字节命令
	DS1302_CLK=1;
	_nop_();_nop_();
	DS1302_RST=0;
	aa=timedata%16;
	timedata=timedata/16;
	timedata=timedata*10+aa;
	return(timedata);
}

/*****************************************************************************
* 函 数 名：initial_ds1302
* 函数功能：初始化DS1302子程序
* 输    入：time[](全局变量)
* 输    出：
*****************************************************************************/
void InitDs1302()
{
	WriteDs1302(0x8e,0x00);		//写保护寄存器，在对时钟或RAM写前WP一定要为0
	Delayms(5);
	WriteDs1302(0x8c,time[0]);		//年
	Delayms(5);
	WriteDs1302(0x88,time[1]);		//月
	Delayms(5);
	WriteDs1302(0x86,time[2]);		//日
	Delayms(5);
	WriteDs1302(0x8A,time[3]);		//星期
	Delayms(5);
	WriteDs1302(0x84,time[4]);		//时
	Delayms(5);
	WriteDs1302(0x82,time[5]);		//分
	Delayms(5);
	WriteDs1302(0x80,time[6]);		//秒
	Delayms(5);
	WriteDs1302(0x8e,0x80);		//写保护寄存器
	Delayms(5);
}

/*****************************************************************************
* 函 数 名：read_time
* 函数功能：读DS1302时间子程序
* 输    入：
* 输    出：全局变量(second,minute,hour,week,day,month,year)
*****************************************************************************/
void ReadTime()
{
	second=ReadDs1302(0x81);		//秒寄存器

	minute=ReadDs1302(0x83);		//分
//	Delayms(5);
	hour=ReadDs1302(0x85);			//时
//	Delayms(5);
	week=ReadDs1302(0x8B);			//星期
//	Delayms(5);
	day=ReadDs1302(0x87);			//日
//	Delayms(5);
	month=ReadDs1302(0x89);		//月
//	Delayms(5);
	year=ReadDs1302(0x8d);
//	Delayms(5);			//年
}

/*******************************************************************************
* 函 数 名 ：main
* 函数功能 ：主函数
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void main()
{
	Delayms(100);
	InitDs1302();  //初始化DS1302，第一次需要该函数写入初始时间到DS1302，后面可注释掉该函数
	count = 0;
	temp = 0;
	while(1)
	{ 
		ReadTime();				//读取时间
		LEDdisplay();					//显示时间
	}
}


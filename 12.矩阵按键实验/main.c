/********************************* 深圳市航太电子有限公司 *******************************
* 实 验 名 ：矩阵按键实验
* 实验说明 ：按下矩阵按键S1-S16，数码管显示对应的键值
* 实验平台 ：航太51单片机开发板 V1.2
* 连接方式 ：
* 注    意 ：平方按键对应的是P3口
*            请务必确认短路帽CN26处于断开状态
* 作    者 ：航太电子产品研发部    QQ ：1909197536
* 店    铺 ：http://shop120013844.taobao.com/
****************************************************************************************/

#include <reg52.h>
#include <intrins.h>

#define FOSC 11059200L //晶振设置，默认使用11.0592M Hz
//#define FOSC 12000000L //晶振设置，使用12M Hz
//#define FOSC 24000000L //晶振设置，使用24M Hz

//IO接口定义
#define LED_PORT P0
#define KEY_PORT P3
sbit dula=P2^6;
sbit wela=P2^7;

//全局变量定义
unsigned char key_value,key_value_disp;

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
* 输    入 ：bai 百位  shi 十位  个 位
* 输    出 ：无
*******************************************************************************/
void LEDdisplay(unsigned int num)
{
	unsigned char bai,shi,ge;
	bai=num/100;
	shi=num%100/10;
	ge=num%10;
	
	wela=1;
	LED_PORT=0x01;
	wela=0;	
	dula=1;			   //显示百位
	LED_PORT=table[bai];
	dula=0;
	Delayms(1);
	
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
* 函 数 名 ：KeyScan
* 函数功能 ：4*4键盘扫描
* 输    入 ：无
* 输    出 ：num 获取的键值，若没有键值则返回 0xff
*******************************************************************************/
unsigned char KeyScan()
{
	unsigned char temp,num;
	num = 0xff;
	KEY_PORT=0xfe;
	temp=KEY_PORT;
	temp=temp&0xf0;
	while(temp!=0xf0)
	{
		Delayms(5); //延时消抖
		temp=KEY_PORT;
		temp=temp&0xf0;
		while(temp!=0xf0)
		{
			temp=KEY_PORT;
		switch(temp)
			{
				case 0xee:num=1;
					break;
				case 0xde:num=2;
					break;
				case 0xbe:num=3;
					break;
				case 0x7e:num=4;
					break;
			}
		while(temp!=0xf0)
			{
				temp=KEY_PORT;
				temp=temp&0xf0;
			}
		}
	}

	KEY_PORT=0xfd;
	temp=KEY_PORT;
	temp=temp&0xf0;
	while(temp!=0xf0)
	{
		Delayms(5); //延时消抖
		temp=KEY_PORT;
		temp=temp&0xf0;
		while(temp!=0xf0)
		{
			temp=KEY_PORT;
		switch(temp)
			{
				case 0xed:num=5;
					break;
				case 0xdd:num=6;
					break;
				case 0xbd:num=7;
					break;
				case 0x7d:num=8;
					break;
			}
		while(temp!=0xf0)
			{
				temp=KEY_PORT;
				temp=temp&0xf0;
			}
		}
	}

	KEY_PORT=0xfb;
	temp=KEY_PORT;
	temp=temp&0xf0;
	while(temp!=0xf0)
	{
		Delayms(5); //延时消抖
		temp=KEY_PORT;
		temp=temp&0xf0;
		while(temp!=0xf0)
		{
			temp=KEY_PORT;
		switch(temp)
			{
				case 0xeb:num=9;
					break;
				case 0xdb:num=10;
					break;
				case 0xbb:num=11;
					break;
				case 0x7b:num=12;
					break;
			}
		while(temp!=0xf0)
			{
				temp=KEY_PORT;
				temp=temp&0xf0;
			}
		}
	}

	KEY_PORT=0xf7;
	temp=KEY_PORT;
	temp=temp&0xf0;
	while(temp!=0xf0)
	{
		Delayms(5); //延时消抖
		temp=KEY_PORT;
		temp=temp&0xf0;
		while(temp!=0xf0)
		{
			temp=KEY_PORT;
		switch(temp)
			{
				case 0xe7:num=13;
					break;
				case 0xd7:num=14;
					break;
				case 0xb7:num=15;
					break;
				case 0x77:num=16;
					break;
			}
		while(temp!=0xf0)
			{
				temp=KEY_PORT;
				temp=temp&0xf0;
			}
		}
	}
return num;
}

/*******************************************************************************
* 函 数 名 ：main
* 函数功能 ：主函数
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void main()
{
	key_value_disp = 0;
	key_value = 0;
	while(1)
	{ 
	   key_value=KeyScan();
	   if(key_value != 0xff)
	   {
		   key_value_disp = key_value;
	   }
	   LEDdisplay(key_value_disp);
	 }
}

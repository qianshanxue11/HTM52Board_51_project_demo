/********************************* 深圳市航太电子有限公司 *******************************
* 实 验 名 ：红外遥控
* 实验说明 ：烧录成功后，将单片机的红外发射口对着另外一个运行 遥控键值解码LCD1602 程序的单片机，
*            按下相应的矩阵键盘按键即可实现键值发送
* 实验平台 ：航太51单片机开发板 V1.2
* 连接方式 ：跳线帽CN4 引脚2,4相接
* 注    意 ：程序通过关断和打开TR1来实现发送低电平和高电平，打开TR1时，发送38k高电平载波，接收端经过反相后接收到低电平，
*            关断TR1时，发送端不发送载波，接收端经反相后接收到高电平
* 作    者 ：航太电子产品研发部    QQ ：1909197536
* 店    铺 ：http://shop120013844.taobao.com/
****************************************************************************************/
#include <reg52.h> 

#define FOSC 11059200L //晶振设置，默认使用11.0592M Hz
//#define FOSC 12000000L //晶振设置，使用12M Hz
//#define FOSC 24000000L //晶振设置，使用24M Hz

//IO接口定义
sbit IR_OUT=P1^6 ;
sbit dula=P2^6;
sbit wela=P2^7;

#define LED_PORT P0
#define KEY_PORT P3

//LED显示字模 0-F 共阳模式
unsigned code table[]= {0Xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};

unsigned char KeyScan();
void LEDdisplay(unsigned int num);

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
* 函 数 名 ：Init_Timer
* 函数功能 ：定时器1初始化 13us产生一次中断 用于产生38K载波 
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void Init_Timer(void) 
{ 
	TMOD=0x21;	 //T0 mode 1      T1 mode 2  	
	TH1=256-(1000*11.002/38.0/12)/2+0.5;						
	//特殊
	TL1=TH1; 
	ET1=1; 
	EA=1; 
} 

/*******************************************************************************
* 函 数 名 ：Send_Start_Bit
* 函数功能 ：发送引导码  发送方:9ms高电平 4.5ms低电平 
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void Send_Start_Bit(void)    //TR1的值=发送的电平 
{ 
	//9ms 1 
	TH0=(65536-8295)/256;  
	TL0=(65536-8295)%256; 
	TR0=1; 
	TR1=1; 
	
	while(!TF0); 
	
	TR1=0; 
	TF0=0;
	TR0=0; 
	
	IR_OUT=1; 
	//4.5ms 0 
	TH0=(65536-4146)/256;  
	TL0=(65536-4146)%256; 
	TR0=1; 
	TR1=0; 
	
	while(!TF0); 
	
	TR1=0; 
	TF0=0;
	TR0=0; 
	
	IR_OUT=1; 
} 

/*******************************************************************************
* 函 数 名 ：Send_Bit_0
* 函数功能 ：发送bit0
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void Send_Bit_0(void) 
{ 
	//0.565ms 1 
	TH0=(65536-521)/256;  
	TL0=(65536-521)%256; 
	TR0=1; 
	TR1=1; 
	
	while(!TF0); 
	
	TR1=0; 
	TF0=0;
	TR0=0; 
	
	IR_OUT=1; 
	//0.565ms 0 
	TH0=(65536-521)/256;  
	TL0=(65536-521)%256; 
	TR0=1; 
	TR1=0; 
	
	while(!TF0); 
	
	TR1=0; 
	TF0=0;
	TR0=0; 
	
	IR_OUT=1; 
} 

/*******************************************************************************
* 函 数 名 ：Send_Bit_1
* 函数功能 ：发送bit1
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void Send_Bit_1(void) 
{ 
//0.565ms 1 
	TH0=(65536-521)/256;  
	TL0=(65536-521)%256; 
	TR0=1; 
	TR1=1; 
	
	while(!TF0); 
	
	TR1=0; 
	TF0=0;
	TR0=0; 
	
	IR_OUT=1;
	//1.685ms 0 
	TH0=(65536-1563)/256;  
	TL0=(65536-1563)%256; 
	TR0=1; 
	TR1=0; 
	
	while(!TF0); 
	
	TR1=0; 
	TF0=0;
	TR0=0; 
	
	IR_OUT=1;  
} 

/*******************************************************************************
* 函 数 名 ：Send_over
* 函数功能 ：发送一个结束码，因为最后一个位只有遇到下降沿才能读取（发射端的上升沿）
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void Send_over(void) 	   //
{ 
//0.500ms 1 			   //小于0.5ms 接收端很难识别到
	TH0=(65536-500)/256;  
	TL0=(65536-500)%256; 
	TR0=1; 
	TR1=1; 
	
	while(!TF0); 
	
	TR1=0; 
	TF0=0;
	TR0=0; 
	
	IR_OUT=1;
	//0.500ms 0 
	TH0=(65536-500)/256;  
	TL0=(65536-500)%256; 
	TR0=1; 
	TR1=0; 
	
	while(!TF0); 
	
	TR1=0; 
	TF0=0;
	TR0=0; 
	
	IR_OUT=1;  
} 

/*******************************************************************************
* 函 数 名 ：Send_Char
* 函数功能 ：发送一个8位的字节，注意反码也要发送
* 输    入 ：add 地址  date 数据
* 输    出 ：无
*******************************************************************************/
void Send_Char(unsigned char add,unsigned char date) 
{ 
	unsigned char i,j1,j2,j3,j4; 
	j1=add;
 	j2=~add;
	j3=date;
	j4=~date;
	Send_Start_Bit(); 								

  //发射引导码
	for(i=0;i<8;i++) 
	{ 
		if(j1&0x01) 
			Send_Bit_1(); 
		else 
			Send_Bit_0(); 
		j1=j1>>1; 								

		//先发射低位
	} 

	for(i=0;i<8;i++) 
	{ 
		if(j2&0x01) 
			Send_Bit_1(); 
		else 
			Send_Bit_0(); 
		j2=j2>>1; 								

		//先发射低位
	} 

	for(i=0;i<8;i++) 
	{ 
		if(j3&0x01) 
			Send_Bit_1(); 
		else 
			Send_Bit_0(); 
		j3=j3>>1; 								

		//先发射低位
	} 
	
	for(i=0;i<8;i++) 
	{ 
		if(j4&0x01) 
			Send_Bit_1(); 
		else 
			Send_Bit_0(); 
		j4=j4>>1; 								

		//先发射低位
	} 
	
	Send_over();									

	//结束符
} 

/*******************************************************************************
* 函 数 名 ：T1_ISR
* 函数功能 ：定时器1中断服务函数，用于产生38k载频
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void T1_ISR(void) interrupt 3 
{ 
	IR_OUT=!IR_OUT; 
} 

/*******************************************************************************
* 函 数 名 ：main
* 函数功能 ：主函数
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void main(void) 
{ 
    unsigned char date = 0;
	unsigned int disp_data = 0;
	IR_OUT=1;
	Init_Timer(); 
	while(1) 
	{ 
//	Send_Start_Bit(); 

//	Send_Bit_1(); 
	// Send_Char(date+2,date);
	// date++;	
	// Delayms(1000); 
	// if(date == 100) date = 0;
	
	date = KeyScan();
	if(date != 0xff)
	{
		disp_data = (int)date;
		Send_Char(0x55,date);
	}
	LEDdisplay(disp_data);
	} 
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
				case 0x77:num=0;
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
* 函 数 名 ：LEDdisplay
* 函数功能 ：循环显示各个位上的数据
* 输    入 ：bai 百位  shi 十位  个 位
* 输    出 ：无
*******************************************************************************/
void LEDdisplay(unsigned int num)
{
	unsigned char bai,shi,ge;
	// bai=num/100;
	// shi=num%100/10;
	// ge=num%10;
	bai = num & 0xf0;
	shi = num & 0x0f;
	
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
	dula=1;			 // 显示个位table[ge]
	LED_PORT=0x89;//H 
	dula=0;
	Delayms(1);
}

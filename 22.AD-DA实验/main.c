/********************************* 深圳市航太电子有限公司 *******************************
* 实 验 名 ：AD/DA实验
* 实验说明 ：调节电位器RP3，可在数码管上看到采样得到的AD值，同时LED灯的亮度也在变化
* 实验平台 ：航太51单片机开发板 V1.2
* 连接方式 ：
* 注    意 ：该程序同样也适用于光敏和热敏值的读取，只是读取地址不一样
* 作    者 ：航太电子产品研发部    QQ ：1909197536
* 店    铺 ：http://shop120013844.taobao.com/
****************************************************************************************/

#include <reg52.h>
#include <intrins.h>

#define FOSC 11059200L //晶振设置，默认使用11.0592M Hz
//#define FOSC 12000000L //晶振设置，使用12M Hz
//#define FOSC 24000000L //晶振设置，使用24M Hz
#define  PCF8591 0x90    //PCF8591 地址

//IO接口定义
#define LED_PORT P0
sbit dula=P2^6;
sbit wela=P2^7;

sbit PCF_SCL=P2^3;         //串行时钟输入端
sbit PCF_SDA=P2^4;   //串行数据输入端

//全局变量定义

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
* 函 数 名 ：PcfStart
* 函数功能 ：I2C开始信号
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void PcfStart()
{
	PCF_SDA=1;
	_nop_();
	PCF_SCL=1;
	_nop_();
	PCF_SDA=0;
	_nop_();
}

/*******************************************************************************
* 函 数 名 ：PcfStop
* 函数功能 ：I2C停止信号
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void PcfStop()      
{
	PCF_SDA=0;
	_nop_();
	PCF_SCL=1;
	_nop_();
	PCF_SDA=1;
	_nop_();  
}

/*******************************************************************************
* 函 数 名 ：PcfRespons
* 函数功能 ：I2C应答信号
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void PcfRespons()
{
	unsigned char i;
	PCF_SCL=1;
	_nop_();
	while((PCF_SDA==1)&&(i<250))
	i++;
	PCF_SCL=0;
	_nop_();
}

/*******************************************************************************
* 函 数 名 ：PcfInit
* 函数功能 ：I2C初始化
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void PcfInit()     
{
	PCF_SDA=1;
	_nop_();
	PCF_SCL=1;
	_nop_();    
}

/*******************************************************************************
* 函 数 名 ：PcfReadByte
* 函数功能 ：读取一个字节
* 输    入 ：无
* 输    出 ：k 读取到的字节
*******************************************************************************/
unsigned char PcfReadByte()
{
	unsigned char i,k;
	PCF_SCL=0;
	_nop_();
	PCF_SDA=1;
	_nop_();
	for(i=0;i<8;i++)
	{
		PCF_SCL=1;
		_nop_();
		k=(k<<1)|PCF_SDA;//先左移一位，再在最低位接受当前位
		PCF_SCL=0;
		_nop_();
	}
	return k;

}

/*******************************************************************************
* 函 数 名 ：PcfWriteByte
* 函数功能 ：写入一个字节
* 输    入 ：data 要写如的字节
* 输    出 ：无
*******************************************************************************/
void PcfWriteByte(unsigned char date)       //写一字节数据
{
	unsigned char i,temp;
	temp=date;
	for(i=0;i<8;i++)
	{
		temp=temp<<1; //左移一位 移出的一位在CY中
		PCF_SCL=0;                      //只有在scl=0时sda能变化值
		_nop_();
		PCF_SDA=CY;
		_nop_();
		PCF_SCL=1;
		_nop_();            
	}   
	PCF_SCL=0;
	_nop_();
	PCF_SDA=1;
	_nop_();
}


/*******************************************************************************
* 函 数 名 ：PcfWriteAdd
* 函数功能 ：在某个地址写入数据
* 输    入 ：control 控制字，地址  date 要写入的数据
* 输    出 ：无
*******************************************************************************/
void PcfWriteAdd(unsigned char control,unsigned char date)
{
	PcfStart();
	PcfWriteByte(PCF8591);        //10010000  前四位固定 接下来三位全部被接地了 所以都是0 最后一位是写 所以为低电平
	PcfRespons();
	PcfWriteByte(control);
	PcfRespons();
	PcfWriteByte(date);
	PcfRespons();
	PcfStop();

}

/*******************************************************************************
* 函 数 名 ：PcfReadAdd
* 函数功能 ：在某个地址读取数据
* 输    入 ：control 控制字，地址 
* 输    出 ：date 读到的数据
*******************************************************************************/
unsigned char PcfReadAdd(unsigned char control)
{
	unsigned char date;
	PcfStart();
	PcfWriteByte(PCF8591);
	PcfRespons();
	PcfWriteByte(control);
	PcfRespons();
	PcfStart();
	PcfWriteByte(PCF8591+1);       //把最后一位变成1，读
	PcfRespons();
	date=PcfReadByte();
	PcfStop();
	return date;
}

/*******************************************************************************
* 函 数 名 ：main
* 函数功能 ：主函数
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void main()
{
	int num,count=0;
	PcfInit();
	while(1)
	{ 
		if(count == 10)//隔一段时间对PCF8591操作，避免过于频繁
		{
			count = 0;
			num=PcfReadAdd(0x40);  //电位器AD采样，读取地址0x40
			Delayms(1);		 //两次AD操作之间需要间隔一定的时间才能正常读取
			//	 num=PcfReadAdd(0x41);	//热敏，读取地址0x41
			 //  Delayms(1);
				// num=PcfReadAdd(0x42);	 //光敏，读取地址0x42
			  // Delayms(1);
			//	 num=PcfReadAdd(0x43);	 //外部，读取地址0x43	
			//   Delayms(1);
			PcfWriteAdd(0x40,num);  //写DA值，写入地址0x40
			Delayms(1);				
		}
 		 count++;
		LEDdisplay(num);
	}
}


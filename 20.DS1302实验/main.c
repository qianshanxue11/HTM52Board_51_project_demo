/********************************* �����к�̫�������޹�˾ *******************************
* ʵ �� �� ��DS1302����ʱ��ʵ��
* ʵ��˵�� ����ʾʱ�����ֵ��������ϣ�ע�ͺ���InitDs1302() �ٴ���¼��ʱ�����ڼ�������
* ʵ��ƽ̨ ����̫51��Ƭ�������� V1.2
* ���ӷ�ʽ ����·ñCN1������1,3���ӣ���·ñCN2������2,4���ӣ�����1,3����
* ע    �� ������ܲ��ù���ģʽ��λѡ�Ͷ�ѡʱҪ����ߵ�ƽ
* ��    �� ����̫���Ӳ�Ʒ�з���    QQ ��1909197536
* ��    �� ��http://shop120013844.taobao.com/
****************************************************************************************/

#include <reg52.h>
#include <intrins.h>

#define FOSC 11059200L //�������ã�Ĭ��ʹ��11.0592M Hz
//#define FOSC 12000000L //�������ã�ʹ��12M Hz
//#define FOSC 24000000L //�������ã�ʹ��24M Hz

#define TIME_MS 50 //�趨��ʱʱ�� ms ,��11.0592M�����£����׳���60ms

//IO�ӿڶ���
#define LED_PORT P0
sbit dula=P2^6;
sbit wela=P2^7;

sbit DS1302_CLK = P1^1;
sbit DS1302_IO  = P1^2;
sbit DS1302_RST = P1^3;

//ȫ�ֱ�������
unsigned char count,temp;
unsigned char second,minute,hour,week,day,month,year;		//�롢�֡�ʱ�����ڡ��ա��¡���
unsigned char time[]={0x06,0x03,0x14,0x03,0x10,0x59,0x55};	//��ʼʱ������

//LED��ʾ��ģ 0-F ����ģʽ
unsigned code table[]= {0Xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};

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
* �� �� �� ��LEDdisplay
* �������� ��ѭ����ʾ����λ�ϵ�����
* ��    �� ����
* ��    �� ����
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
* �� �� �� ��InputByte
* �������ܣ���DS1302��һ�ֽ������ӳ���
* ��    �룺byte1
* ��    ����
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
* �� �� ����outputbyte
* �������ܣ���DS1302һ���ֽ��ӳ���
* ��    �룺
* ��    ������ȡ����ֵ
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
* �� �� ����write_ds1302
* �������ܣ���DS1302ĳ��ַдһ�ֽ������ӳ���
* ��    �룺addr,TDat
* ��    ����
*****************************************************************************/
void WriteDs1302(unsigned char addr,unsigned char TDat)
{
	DS1302_RST=0;
	_nop_();_nop_();
	DS1302_CLK=0;
	_nop_();_nop_();
	DS1302_RST=1;
 	InputByte(addr);//����8�ֽڵ�ַ����
	_nop_();_nop_();
	InputByte(TDat);//д����������
	DS1302_CLK=1;
	_nop_();_nop_();
	DS1302_RST=0;
}

/*****************************************************************************
* �� �� ����read_ds1302
* �������ܣ���DS1302��ַ�ӳ���
* ��    �룺add
* ��    ����timedata
*****************************************************************************/
unsigned char ReadDs1302(unsigned char addr)
{
	unsigned char timedata,aa;
	DS1302_RST=0;
	_nop_();_nop_();
	DS1302_CLK=0;
	_nop_();_nop_();
	DS1302_RST=1;
	InputByte(addr);//����8�ֽڵ�ַ����
	timedata=OutPutByte();//��ȡ�ֽ�����
	DS1302_CLK=1;
	_nop_();_nop_();
	DS1302_RST=0;
	aa=timedata%16;
	timedata=timedata/16;
	timedata=timedata*10+aa;
	return(timedata);
}

/*****************************************************************************
* �� �� ����initial_ds1302
* �������ܣ���ʼ��DS1302�ӳ���
* ��    �룺time[](ȫ�ֱ���)
* ��    ����
*****************************************************************************/
void InitDs1302()
{
	WriteDs1302(0x8e,0x00);		//д�����Ĵ������ڶ�ʱ�ӻ�RAMдǰWPһ��ҪΪ0
	Delayms(5);
	WriteDs1302(0x8c,time[0]);		//��
	Delayms(5);
	WriteDs1302(0x88,time[1]);		//��
	Delayms(5);
	WriteDs1302(0x86,time[2]);		//��
	Delayms(5);
	WriteDs1302(0x8A,time[3]);		//����
	Delayms(5);
	WriteDs1302(0x84,time[4]);		//ʱ
	Delayms(5);
	WriteDs1302(0x82,time[5]);		//��
	Delayms(5);
	WriteDs1302(0x80,time[6]);		//��
	Delayms(5);
	WriteDs1302(0x8e,0x80);		//д�����Ĵ���
	Delayms(5);
}

/*****************************************************************************
* �� �� ����read_time
* �������ܣ���DS1302ʱ���ӳ���
* ��    �룺
* ��    ����ȫ�ֱ���(second,minute,hour,week,day,month,year)
*****************************************************************************/
void ReadTime()
{
	second=ReadDs1302(0x81);		//��Ĵ���

	minute=ReadDs1302(0x83);		//��
//	Delayms(5);
	hour=ReadDs1302(0x85);			//ʱ
//	Delayms(5);
	week=ReadDs1302(0x8B);			//����
//	Delayms(5);
	day=ReadDs1302(0x87);			//��
//	Delayms(5);
	month=ReadDs1302(0x89);		//��
//	Delayms(5);
	year=ReadDs1302(0x8d);
//	Delayms(5);			//��
}

/*******************************************************************************
* �� �� �� ��main
* �������� ��������
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void main()
{
	Delayms(100);
	InitDs1302();  //��ʼ��DS1302����һ����Ҫ�ú���д���ʼʱ�䵽DS1302�������ע�͵��ú���
	count = 0;
	temp = 0;
	while(1)
	{ 
		ReadTime();				//��ȡʱ��
		LEDdisplay();					//��ʾʱ��
	}
}


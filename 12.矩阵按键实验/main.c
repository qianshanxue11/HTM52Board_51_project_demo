/********************************* �����к�̫�������޹�˾ *******************************
* ʵ �� �� �����󰴼�ʵ��
* ʵ��˵�� �����¾��󰴼�S1-S16���������ʾ��Ӧ�ļ�ֵ
* ʵ��ƽ̨ ����̫51��Ƭ�������� V1.2
* ���ӷ�ʽ ��
* ע    �� ��ƽ��������Ӧ����P3��
*            �����ȷ�϶�·ñCN26���ڶϿ�״̬
* ��    �� ����̫���Ӳ�Ʒ�з���    QQ ��1909197536
* ��    �� ��http://shop120013844.taobao.com/
****************************************************************************************/

#include <reg52.h>
#include <intrins.h>

#define FOSC 11059200L //�������ã�Ĭ��ʹ��11.0592M Hz
//#define FOSC 12000000L //�������ã�ʹ��12M Hz
//#define FOSC 24000000L //�������ã�ʹ��24M Hz

//IO�ӿڶ���
#define LED_PORT P0
#define KEY_PORT P3
sbit dula=P2^6;
sbit wela=P2^7;

//ȫ�ֱ�������
unsigned char key_value,key_value_disp;

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
* ��    �� ��bai ��λ  shi ʮλ  �� λ
* ��    �� ����
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
	dula=1;			   //��ʾ��λ
	LED_PORT=table[bai];
	dula=0;
	Delayms(1);
	
	wela=1;
	LED_PORT=0x02;
	wela=0;
	dula=1;			   //��ʾʮλ
	LED_PORT=table[shi];	 
	dula=0;
	Delayms(1);
	
	wela=1;
	LED_PORT=0x04;
	wela=0;
	dula=1;			 // ��ʾ��λ
	LED_PORT=table[ge];
	dula=0;
	Delayms(1);
}

/*******************************************************************************
* �� �� �� ��KeyScan
* �������� ��4*4����ɨ��
* ��    �� ����
* ��    �� ��num ��ȡ�ļ�ֵ����û�м�ֵ�򷵻� 0xff
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
		Delayms(5); //��ʱ����
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
		Delayms(5); //��ʱ����
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
		Delayms(5); //��ʱ����
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
		Delayms(5); //��ʱ����
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
* �� �� �� ��main
* �������� ��������
* ��    �� ����
* ��    �� ����
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

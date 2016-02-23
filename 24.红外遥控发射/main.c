/********************************* �����к�̫�������޹�˾ *******************************
* ʵ �� �� ������ң��
* ʵ��˵�� ����¼�ɹ��󣬽���Ƭ���ĺ��ⷢ��ڶ�������һ������ ң�ؼ�ֵ����LCD1602 ����ĵ�Ƭ����
*            ������Ӧ�ľ�����̰�������ʵ�ּ�ֵ����
* ʵ��ƽ̨ ����̫51��Ƭ�������� V1.2
* ���ӷ�ʽ ������ñCN4 ����2,4���
* ע    �� ������ͨ���ضϺʹ�TR1��ʵ�ַ��͵͵�ƽ�͸ߵ�ƽ����TR1ʱ������38k�ߵ�ƽ�ز������ն˾����������յ��͵�ƽ��
*            �ض�TR1ʱ�����Ͷ˲������ز������ն˾��������յ��ߵ�ƽ
* ��    �� ����̫���Ӳ�Ʒ�з���    QQ ��1909197536
* ��    �� ��http://shop120013844.taobao.com/
****************************************************************************************/
#include <reg52.h> 

#define FOSC 11059200L //�������ã�Ĭ��ʹ��11.0592M Hz
//#define FOSC 12000000L //�������ã�ʹ��12M Hz
//#define FOSC 24000000L //�������ã�ʹ��24M Hz

//IO�ӿڶ���
sbit IR_OUT=P1^6 ;
sbit dula=P2^6;
sbit wela=P2^7;

#define LED_PORT P0
#define KEY_PORT P3

//LED��ʾ��ģ 0-F ����ģʽ
unsigned code table[]= {0Xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};

unsigned char KeyScan();
void LEDdisplay(unsigned int num);

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
* �� �� �� ��Init_Timer
* �������� ����ʱ��1��ʼ�� 13us����һ���ж� ���ڲ���38K�ز� 
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void Init_Timer(void) 
{ 
	TMOD=0x21;	 //T0 mode 1      T1 mode 2  	
	TH1=256-(1000*11.002/38.0/12)/2+0.5;						
	//����
	TL1=TH1; 
	ET1=1; 
	EA=1; 
} 

/*******************************************************************************
* �� �� �� ��Send_Start_Bit
* �������� ������������  ���ͷ�:9ms�ߵ�ƽ 4.5ms�͵�ƽ 
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void Send_Start_Bit(void)    //TR1��ֵ=���͵ĵ�ƽ 
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
* �� �� �� ��Send_Bit_0
* �������� ������bit0
* ��    �� ����
* ��    �� ����
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
* �� �� �� ��Send_Bit_1
* �������� ������bit1
* ��    �� ����
* ��    �� ����
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
* �� �� �� ��Send_over
* �������� ������һ�������룬��Ϊ���һ��λֻ�������½��ز��ܶ�ȡ������˵������أ�
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void Send_over(void) 	   //
{ 
//0.500ms 1 			   //С��0.5ms ���ն˺���ʶ��
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
* �� �� �� ��Send_Char
* �������� ������һ��8λ���ֽڣ�ע�ⷴ��ҲҪ����
* ��    �� ��add ��ַ  date ����
* ��    �� ����
*******************************************************************************/
void Send_Char(unsigned char add,unsigned char date) 
{ 
	unsigned char i,j1,j2,j3,j4; 
	j1=add;
 	j2=~add;
	j3=date;
	j4=~date;
	Send_Start_Bit(); 								

  //����������
	for(i=0;i<8;i++) 
	{ 
		if(j1&0x01) 
			Send_Bit_1(); 
		else 
			Send_Bit_0(); 
		j1=j1>>1; 								

		//�ȷ����λ
	} 

	for(i=0;i<8;i++) 
	{ 
		if(j2&0x01) 
			Send_Bit_1(); 
		else 
			Send_Bit_0(); 
		j2=j2>>1; 								

		//�ȷ����λ
	} 

	for(i=0;i<8;i++) 
	{ 
		if(j3&0x01) 
			Send_Bit_1(); 
		else 
			Send_Bit_0(); 
		j3=j3>>1; 								

		//�ȷ����λ
	} 
	
	for(i=0;i<8;i++) 
	{ 
		if(j4&0x01) 
			Send_Bit_1(); 
		else 
			Send_Bit_0(); 
		j4=j4>>1; 								

		//�ȷ����λ
	} 
	
	Send_over();									

	//������
} 

/*******************************************************************************
* �� �� �� ��T1_ISR
* �������� ����ʱ��1�жϷ����������ڲ���38k��Ƶ
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void T1_ISR(void) interrupt 3 
{ 
	IR_OUT=!IR_OUT; 
} 

/*******************************************************************************
* �� �� �� ��main
* �������� ��������
* ��    �� ����
* ��    �� ����
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
* �� �� �� ��LEDdisplay
* �������� ��ѭ����ʾ����λ�ϵ�����
* ��    �� ��bai ��λ  shi ʮλ  �� λ
* ��    �� ����
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
	dula=1;			 // ��ʾ��λtable[ge]
	LED_PORT=0x89;//H 
	dula=0;
	Delayms(1);
}

/********************************* �����к�̫�������޹�˾ *******************************
* ʵ �� �� ��DS18B20 LCD1602��ʾ
* ʵ��˵�� ��������ȡ18B20���кţ�֮����ʾ��ǰ�¶ȣ����¶ȸ��ڻ�����趨ֵʱ����
* ʵ��ƽ̨ ����̫51��Ƭ�������� V1.2
* ���ӷ�ʽ ����DS18B20�嵽�����¶ȼ��ģ���U5��,��·ñCN3 ����1,3����
* ע    �� ������ضϿ�����ñCN26
* ��    �� ����̫���Ӳ�Ʒ�з���    QQ ��1909197536
* ��    �� ��http://shop120013844.taobao.com/
****************************************************************************************/
#include <reg52.h>
#include <intrins.h>

#define uchar unsigned char
#define uint  unsigned int

sbit DQ = P2^2;  //����DS18B20�˿�DQ  
sbit BEEP=P1^5 ; //������������

bit presence,flag;
bit compare_th,compare_tl,alarm_on_off=0,temp_th,temp_tl;

sbit LCD_RS = P3^5;             
sbit LCD_RW = P3^6;
sbit LCD_EN = P3^4;

uchar code  cdis1[ ] = {"  READ_ROMCORD  "};
uchar code  cdis2[ ] = {"                "};
uchar code  cdis3[ ] = {" DS18B20  ERR0R "};
uchar code  cdis4[ ] = {"  PLEASE CHECK  "};
uchar code  cdis5[ ] = {" TEMP:          "};
uchar code  cdis6[ ] = {"TH:     TL:     "};

unsigned char data  temp_data[2] = {0x00,0x00};
unsigned char data  temp_alarm[2] = {0x22,0x10};
unsigned char data  display[5] =   {0x00,0x00,0x00,0x00,0x00}; //�¶�ֵ��ʾ
unsigned char data  display1[3] =  {0x00,0x00,0x00}; //�¶ȱ���ֵ��ʾ

unsigned char data  RomCode[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
 
unsigned char code  mytab1[8] = {0x0C,0x12,0x12,0x0C,0x00,0x00,0x00,0x00};
unsigned char code  mytab2[8] = {0x01,0x1b,0x1d,0x19,0x1d,0x1b,0x01,0x00};//С����
#define delayNOP(); {_nop_();_nop_();_nop_();_nop_();};

unsigned char Temp,temp_comp,timecount,count;
unsigned char  crc;
void Disp_Temp_alarm(uchar addr,uchar num);
void  spk(uchar addr);
void set_temp_alarm();
void temp_compare();
void beep();
/*******************************************************************/
void delay1(int ms)
{
   unsigned char y;
   while(ms--)
   {
     for(y = 0; y<250; y++)
    {
      _nop_();
      _nop_();
      _nop_();
      _nop_();
    }
   }
}

/******************************************************************/
/*                                                                */
/*���LCDæ״̬                                                   */
/*lcd_busyΪ1ʱ��æ���ȴ���lcd-busyΪ0ʱ,�У���дָ�������ݡ�     */
/*                                                                */
/******************************************************************/ 

bit lcd_busy()
 {                          
    bit result;
    LCD_RS = 0;
    LCD_RW = 1;
    LCD_EN = 1;
    delayNOP();
    result = (bit)(P0&0x80);
    LCD_EN = 0;
    return(result); 
 }

/*******************************************************************/
/*                                                                 */
/*дָ�����ݵ�LCD                                                  */
/*RS=L��RW=L��E=�����壬D0-D7=ָ���롣                             */
/*                                                                 */
/*******************************************************************/
void lcd_wcmd(uchar cmd)
{                          
   while(lcd_busy());
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_EN = 0;
    _nop_();
    _nop_(); 
    P0 = cmd;
    delayNOP();
    LCD_EN = 1;
    delayNOP();
    LCD_EN = 0;  
}

/*******************************************************************/
/*                                                                 */
/*д��ʾ���ݵ�LCD                                                  */
/*RS=H��RW=L��E=�����壬D0-D7=���ݡ�                               */
/*                                                                 */
/*******************************************************************/
void lcd_wdat(uchar dat)
{                          
   while(lcd_busy());
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_EN = 0;
    P0 = dat;
    delayNOP();
    LCD_EN = 1;
    delayNOP();
    LCD_EN = 0; 
}

/*******************************************************************/
/*                                                                 */
/*�Զ����ַ�д��CGRAM                                              */
/*                                                                 */
/*******************************************************************/
void  writetab()  
{  
    unsigned char i;
    lcd_wcmd(0x40);            //дCGRAM
    for (i = 0; i< 8; i++)       
    lcd_wdat(mytab1[i]);
    for (i = 0; i< 8; i++)       
    lcd_wdat(mytab2[i]);    
}

/*******************************************************************/
/*                                                                 */
/*  LCD��ʼ���趨                                                  */
/*                                                                 */
/*******************************************************************/
void lcd_init()
{ 
    delay1(15);   
    lcd_wcmd(0x01);      //���LCD����ʾ����            
    lcd_wcmd(0x38);      //16*2��ʾ��5*7����8λ����
    delay1(5);
    lcd_wcmd(0x38);         
    delay1(5);
    lcd_wcmd(0x38);         
    delay1(5);

    lcd_wcmd(0x0c);      //��ʾ�����ع��
    delay1(5);
    lcd_wcmd(0x06);      //�ƶ����
    delay1(5);
    lcd_wcmd(0x01);      //���LCD����ʾ����
    delay1(5);

	writetab();               //�Զ����ַ�д��CGRAM
}

/*******************************************************************/
/*                                                                 */
/*  �趨��ʾλ��                                                   */
/*                                                                 */
/*******************************************************************/

void lcd_pos(uchar pos)
{                          
  lcd_wcmd(pos | 0x80);  //����ָ��=80+��ַ����
}

/*******************************************************************/
/*                                                                 */
/*us����ʱ����                                                     */
/*                                                                 */
/*******************************************************************/

void Delay(unsigned int num)
{
  while( --num );
}

/*******************************************************************/
/*                                                                 */
/*��ʼ��ds1820                                                     */
/*                                                                 */
/*******************************************************************/
bit Init_DS18B20(void)
{  
     DQ = 1;      //DQ��λ
     Delay(8);    //������ʱ

     DQ = 0;      //��Ƭ����DQ����
     Delay(90);   //��ȷ��ʱ ���� 480us

     DQ = 1;       //��������
     Delay(8);

     presence = DQ;    //���=0���ʼ���ɹ� =1���ʼ��ʧ��
     Delay(100);
     DQ = 1; 
     
     return(presence); //�����źţ�0=presence,1= no presence
}

/*******************************************************************/
/*                                                                 */
/* ��һ���ֽ�                                                      */
/*                                                                 */
/*******************************************************************/
 ReadOneChar(void)
{
   unsigned char i = 0;
   unsigned char dat = 0;

   for (i = 8; i > 0; i--)
   {
     DQ = 0; // �������ź�
     dat >>= 1;
     DQ = 1; // �������ź�

     if(DQ)
     dat |= 0x80;
     Delay(4);
   }
    return (dat);
}

/*******************************************************************/
/*                                                                 */
/* дһ���ֽ�                                                      */
/*                                                                 */
/*******************************************************************/
void  WriteOneChar(unsigned char dat)
{
  unsigned char i = 0;
  for (i = 8; i > 0; i--)
  {
    DQ = 0;
    DQ = dat&0x01;
    Delay(5);

    DQ = 1;
    dat>>=1;
  }
}

/*******************************************************************/
/*                                                                 */
/* �¶ȱ���ֵд��DS18B20                                           */
/*                                                                 */
/*******************************************************************/
void Write_Temperature_alarm(void)
{
    Init_DS18B20();
  
    WriteOneChar(0xCC);           //����������кŵĲ��� 
    WriteOneChar(0x4e);           //���趨���¶ȱ���ֵд�� DS18B20
	WriteOneChar(temp_alarm[0]);  //дTH
	WriteOneChar(temp_alarm[1]);  //дTL
    WriteOneChar(0x7f);           //12λ��ȷ��
 
	Init_DS18B20();
    WriteOneChar(0xCC);           //����������кŵĲ��� 
    WriteOneChar(0x48);           //���ݴ�������¶ȱ���ֵ������EEROM
}

/*******************************************************************/
/*                                                                 */
/* ��ȡ64λ������                                                  */
/*                                                                 */
/*******************************************************************/
void Read_RomCord(void)
{
   unsigned char j;
   Init_DS18B20();
  
   WriteOneChar(0x33);          // ��������Ĳ���
   for (j = 0; j < 8; j++)
   {
     RomCode[j] = ReadOneChar() ; 
   }
}

/*******************************************************************/
/*                                                                 */
/*DS18B20��CRC8У�����                                            */
/*                                                                 */
/*******************************************************************/
uchar CRC8() 
{ 
   uchar i,x; uchar crcbuff;
   
   crc=0;
   for(x = 0; x <8; x++)
   {
    crcbuff=RomCode[x];
    for(i = 0; i < 8; i++) 
     { 
      if(((crc ^ crcbuff)&0x01)==0) 
      crc >>= 1; 
       else { 
              crc ^= 0x18;   //CRC=X8+X5+X4+1
              crc >>= 1; 
              crc |= 0x80; 
            }         
      crcbuff >>= 1;       
	 }
   }
     return crc;	
}
/*******************************************************************/
/*                                                                 */
/* ����ת������ʾ                                                  */
/*                                                                 */
/*******************************************************************/

void Disp_RomCode(uchar H_num)
{
   uchar j;
   
   if(H_num==1)
   lcd_pos(0x00); 
   if(H_num==2)
   lcd_pos(0x40);
     
   for(j=0;j<8;j++)
   {
    Temp = RomCode[j];

    display[0]=((Temp&0xf0)>>4);
    if(display[0]>9)
     { display[0]=display[0]+0x37;}
    else{display[0]=display[0]+0x30;}          
    lcd_wdat(display[0]);        //��λ����ʾ 

    display[1]=(Temp&0x0f);
    if(display[1]>9)
     {display[1]=display[1]+0x37;}
    else {display[1]=display[1]+0x30;}           
    lcd_wdat(display[1]);        //��λ����ʾ 
   }
}	 

/*******************************************************************/
/*                                                                 */
/* ��ȡ�¶�                                                        */
/*                                                                 */
/*******************************************************************/
void Read_Temperature(void)
{
     uchar  i;
     TR0=0;                     //���ж�,��ֹ��������
     Init_DS18B20();
  
     WriteOneChar(0xCC);        //����������кŵĲ���
     WriteOneChar(0x44);        //�����¶�ת��

     Init_DS18B20();

     WriteOneChar(0x55);         //ƥ��ROM����
     for(i=0;i<8;i++)
     WriteOneChar(RomCode[i]);

     WriteOneChar(0xBE);         //��ȡ�¶ȼĴ���

     temp_data[0] = ReadOneChar();   //�¶ȵ�8λ
     temp_data[1] = ReadOneChar();   //�¶ȸ�8λ
     temp_alarm[0] = ReadOneChar();  //�¶ȱ���TH
	 temp_alarm[1] = ReadOneChar();  //�¶ȱ���TL

	 temp_comp=((temp_data[0]&0xf0)>>4)|((temp_data[1]&0x0f)<<4);
                                   //ȡ�¶�����ֵ
     TR0=1;        //���ж�
}
/*******************************************************************/
/*                                                                 */
/* ����ת�����¶���ʾ                                              */
/*                                                                 */
/*******************************************************************/
void Disp_Temperature()
{
   uchar s=0;
   if(temp_data[1]>127)                  //�¶�Ϊ��ֵ
   {                                        
    temp_data[0]=(~temp_data[0])+1;     //ȡ����һ����������ԭ��
    if((~temp_data[0])>=0xff)
    temp_data[1]=(~temp_data[1])+1;
    else temp_data[1]=~temp_data[1];
    s=1;
   }
	
   display[4]=temp_data[0]&0x0f;
   display[0]=(display[4]*10/16)+0x30;
  
   display[4]=((temp_data[0]&0xf0)>>4)|((temp_data[1]&0x0f)<<4);
   display[3]=display[4]/100+0x30;
   display[1]=display[4]%100;
   display[2]=display[1]/10+0x30;
   display[1]=display[1]%10+0x30;

    if(display[3]==0x30)        //��λΪ0������ʾ
   { 
     display[3]=0x20;              
     if(display[2]==0x30)       //�θ�λΪ0������ʾ
     display[2]=0x20;
   }
   if(s)
   display[3]=0x2d;            //��ʾ����
    
    lcd_pos(0x07);             
    lcd_wdat(display[3]);      //��λ����ʾ             
    lcd_wdat(display[2]);      //ʮλ����ʾ              
    lcd_wdat(display[1]);      //��λ����ʾ  
    lcd_wdat('.');            
    lcd_wdat(display[0]);      //С��λ����ʾ
 	lcd_wdat(0x00);            //��ʾ�Զ����ַ�
    lcd_wdat('C');             //��ʾC
}	 

/*******************************************************************/
/*                                                                 */
/* ��������һ��                                                    */
/*                                                                 */
/*******************************************************************/
void beep()
{
    unsigned char y;
    for (y=0;y<100;y++)
    {
      Delay(70);
      BEEP=!BEEP;                //BEEPȡ��
    } 
    BEEP=1;                      //�رշ�����
	Delay(25000);
}

/*******************************************************************/
/*                                                                 */
/* ROMCORD ��ʾ�˵�                                                */
/*                                                                 */
/*******************************************************************/
void  RomCode_Menu ()
{ 
    uchar  m;
    lcd_init();                //��ʼ��LCD
 
    lcd_pos(0);                //������ʾλ��Ϊ��һ��
    for(m=0;m<16;m++)          //��ʾ�ַ�
    lcd_wdat(cdis1[m]);
     
    Read_RomCord();            //��ȡ64λ������
    CRC8();                    //CRCЧ��
    if(crc==0)                 //CRCЧ����ȷ
    {
	  Disp_RomCode(2);	       //��ʾ64λ������
    }
}

/*******************************************************************/
/*                                                                 */
/* DS18B20 ERROR ��ʾ�˵�                                          */
/*                                                                 */
/*******************************************************************/
void  Error_Menu ()
{
     uchar  m;
     lcd_init();                //��ʼ��LCD 
	
  	 lcd_pos(0);                //������ʾλ��Ϊ��һ�еĵ�1���ַ�
     for(m=0;m<16;m++) 
     lcd_wdat(cdis3[m]);        //��ʾ�ַ�
    
     lcd_pos(0x40);             //������ʾλ��Ϊ�ڶ��е�1���ַ�
     for(m=0;m<16;m++) 
     lcd_wdat(cdis4[m]);        //��ʾ�ַ�
}

/*******************************************************************/
/*                                                                 */
/* DS18B20 ������ʾ�˵�                                            */
/*                                                                 */
/*******************************************************************/
void  Natural_Menu ()
{
   uchar  m;
   lcd_init();                //��ʼ��LCD 

   lcd_pos(0);                //������ʾλ��Ϊ��һ�еĵ�1���ַ�
   for(m=0;m<16;m++) 
   lcd_wdat(cdis5[m]);        //��ʾ�ַ�

   lcd_pos(0x40);             //������ʾλ��Ϊ�ڶ��е�1���ַ�
   for(m=0;m<16;m++) 
   lcd_wdat(cdis6[m]);        //��ʾ�ַ�

   Read_Temperature();
   Disp_Temp_alarm(0x43,0);   //��ʾTHֵ
   Disp_Temp_alarm(0x4b,1);   //��ʾTLֵ
   Disp_Temperature();        //��ʾʵʱ�¶�ֵ
}


/*********************************************************/
// ��ʾ�����¶�
/*********************************************************/
void Disp_Temp_alarm(uchar addr,uchar num)
{   //0=TH,1=TL
   display1[2]=temp_alarm[num]/100+0x30;
   display1[0]=temp_alarm[num]%100;
   display1[1]=display1[0]/10+0x30;
   display1[0]=display1[0]%10+0x30;

    if(display1[2]==0x30)        //��λΪ0������ʾ
    { 
      display1[2]=0x20;              
      if(display1[1]==0x30)      //�θ�λΪ0������ʾ
      display1[1]=0x20;
    }

    lcd_pos(addr);
    lcd_wdat(display1[2]);        //��λ����ʾ             
    lcd_wdat(display1[1]);        //ʮλ����ʾ              
    lcd_wdat(display1[0]);        //��λ����ʾ
}    

/**********************************************************/
// Time0�жϺ���
/**********************************************************/
void Time0(void) interrupt 1 using 0
{
   TH0=0x4c;        //50ms��ʱ
   TL0=0x00;
   timecount++;
   if(timecount>9)
   {
    timecount=0;
    flag=~flag;         
   }
}

/*********************************************************/
/*                                                       */
/*С��������                                             */
/*                                                       */
/*********************************************************/
void  spk(uchar addr)
{
   if(flag)
   {
     lcd_pos(addr);
     lcd_wdat(0x01);      //С������ʾ
   }
   else
   {
     lcd_pos(addr);
     lcd_wdat(0x20);      //����
   }
}

/*********************************************************/
// �¶ȱȽϺ���
/*********************************************************/
void temp_compare()
{    
    if(temp_comp >= temp_alarm[0])      //�Ƚ� TH ֵ
     {
       compare_th=1;                    
	 }
	else  compare_th=0;

    if(temp_comp < temp_alarm[1])        //�Ƚ� TL ֵ
	 {
	   compare_tl=1;
	 }
	else  compare_tl=0;
     
	if(compare_th)
	 {
	   spk(0x46);            //С��������
       if(!alarm_on_off)     //����ѡ��
	   beep();
	   count=temp_alarm[0];
	 }
	else 
	 {
       lcd_pos(0x46);
       lcd_wdat(0x20);
	 }
	if(compare_tl)
	 {
	   spk(0x4e);           //С��������
       if(!alarm_on_off)    //����ѡ��
	   beep();
	   count=temp_alarm[1];
	 }
	else 
	 {
       lcd_pos(0x4e);
       lcd_wdat(0x20);
	 }
}   

/*********************************************************/
// �¶ȱ���ֵ����
/*********************************************************/
void  Set_tempalarm_Flash(uchar addr,uchar num)
{
   if(flag)
   {
     temp_alarm[num]=count;
     Disp_Temp_alarm(addr,num);  //��ʾ�¶ȱ���ֵ
   }
   else
   {
     lcd_pos(addr);
     lcd_wdat(0x20);
	 lcd_wdat(0x20);
	 lcd_wdat(0x20);
   }
}


/*******************************************************************/
/*                                                                 */
/* ������                                                          */
/*                                                                 */
/*******************************************************************/
void main()
 {
	uchar  m;
	TMOD=0x01; TH0=0x4c; TL0=0x00; 				//50ms��ʱ
	EA=1; ET0=1; TR0=1;
	P0=0;
	P2&=0x1F;
	Init_DS18B20();
	Write_Temperature_alarm();					//�趨�����������¶�ֵ
	if(presence)
    {
    	Error_Menu ();
		do
		{  
			Init_DS18B20();
			beep();
		}
		while(presence);
    }
    
	RomCode_Menu ();      						//��ʾRomCode
	for(m=0;m<10;m++)
	Delay(50000);
    Natural_Menu ();							//������ʾ����
	Delay(50000);
	while(1)
	{
		Read_Temperature();						//��ȡ��ǰ�¶�
		Delay(50000);
		Disp_Temperature();						//���µ�ǰ�¶�
		temp_compare();							//�¶������ޱȽ�
		if(compare_th == 0 && compare_tl == 0)	//��������Χ������ʾ
		{
		   Disp_Temp_alarm(0x43,0);   //��ʾTHֵ
           Disp_Temp_alarm(0x4b,1);   //��ʾTLֵ
		}
		if(compare_th)
		{
			Set_tempalarm_Flash(0x43,0);		//���ޱ���
		}
		if(compare_tl)
		{
			Set_tempalarm_Flash(0x4b,1);		//���ޱ���
		}
  }//end of while(1)
}  //end of main()

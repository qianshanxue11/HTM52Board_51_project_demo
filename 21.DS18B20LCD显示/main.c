/********************************* 深圳市航太电子有限公司 *******************************
* 实 验 名 ：DS18B20 LCD1602显示
* 实验说明 ：开机读取18B20序列号，之后显示当前温度，当温度高于或低于设定值时报警
* 实验平台 ：航太51单片机开发板 V1.2
* 连接方式 ：将DS18B20插到数字温度检测模块的U5上,短路帽CN3 引脚1,3连接
* 注    意 ：请务必断开跳线帽CN26
* 作    者 ：航太电子产品研发部    QQ ：1909197536
* 店    铺 ：http://shop120013844.taobao.com/
****************************************************************************************/
#include <reg52.h>
#include <intrins.h>

#define uchar unsigned char
#define uint  unsigned int

sbit DQ = P2^2;  //定义DS18B20端口DQ  
sbit BEEP=P1^5 ; //蜂鸣器驱动线

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
unsigned char data  display[5] =   {0x00,0x00,0x00,0x00,0x00}; //温度值显示
unsigned char data  display1[3] =  {0x00,0x00,0x00}; //温度报警值显示

unsigned char data  RomCode[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
 
unsigned char code  mytab1[8] = {0x0C,0x12,0x12,0x0C,0x00,0x00,0x00,0x00};
unsigned char code  mytab2[8] = {0x01,0x1b,0x1d,0x19,0x1d,0x1b,0x01,0x00};//小喇叭
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
/*检查LCD忙状态                                                   */
/*lcd_busy为1时，忙，等待。lcd-busy为0时,闲，可写指令与数据。     */
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
/*写指令数据到LCD                                                  */
/*RS=L，RW=L，E=高脉冲，D0-D7=指令码。                             */
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
/*写显示数据到LCD                                                  */
/*RS=H，RW=L，E=高脉冲，D0-D7=数据。                               */
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
/*自定义字符写入CGRAM                                              */
/*                                                                 */
/*******************************************************************/
void  writetab()  
{  
    unsigned char i;
    lcd_wcmd(0x40);            //写CGRAM
    for (i = 0; i< 8; i++)       
    lcd_wdat(mytab1[i]);
    for (i = 0; i< 8; i++)       
    lcd_wdat(mytab2[i]);    
}

/*******************************************************************/
/*                                                                 */
/*  LCD初始化设定                                                  */
/*                                                                 */
/*******************************************************************/
void lcd_init()
{ 
    delay1(15);   
    lcd_wcmd(0x01);      //清除LCD的显示内容            
    lcd_wcmd(0x38);      //16*2显示，5*7点阵，8位数据
    delay1(5);
    lcd_wcmd(0x38);         
    delay1(5);
    lcd_wcmd(0x38);         
    delay1(5);

    lcd_wcmd(0x0c);      //显示开，关光标
    delay1(5);
    lcd_wcmd(0x06);      //移动光标
    delay1(5);
    lcd_wcmd(0x01);      //清除LCD的显示内容
    delay1(5);

	writetab();               //自定义字符写入CGRAM
}

/*******************************************************************/
/*                                                                 */
/*  设定显示位置                                                   */
/*                                                                 */
/*******************************************************************/

void lcd_pos(uchar pos)
{                          
  lcd_wcmd(pos | 0x80);  //数据指针=80+地址变量
}

/*******************************************************************/
/*                                                                 */
/*us级延时函数                                                     */
/*                                                                 */
/*******************************************************************/

void Delay(unsigned int num)
{
  while( --num );
}

/*******************************************************************/
/*                                                                 */
/*初始化ds1820                                                     */
/*                                                                 */
/*******************************************************************/
bit Init_DS18B20(void)
{  
     DQ = 1;      //DQ复位
     Delay(8);    //稍做延时

     DQ = 0;      //单片机将DQ拉低
     Delay(90);   //精确延时 大于 480us

     DQ = 1;       //拉高总线
     Delay(8);

     presence = DQ;    //如果=0则初始化成功 =1则初始化失败
     Delay(100);
     DQ = 1; 
     
     return(presence); //返回信号，0=presence,1= no presence
}

/*******************************************************************/
/*                                                                 */
/* 读一个字节                                                      */
/*                                                                 */
/*******************************************************************/
 ReadOneChar(void)
{
   unsigned char i = 0;
   unsigned char dat = 0;

   for (i = 8; i > 0; i--)
   {
     DQ = 0; // 给脉冲信号
     dat >>= 1;
     DQ = 1; // 给脉冲信号

     if(DQ)
     dat |= 0x80;
     Delay(4);
   }
    return (dat);
}

/*******************************************************************/
/*                                                                 */
/* 写一个字节                                                      */
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
/* 温度报警值写入DS18B20                                           */
/*                                                                 */
/*******************************************************************/
void Write_Temperature_alarm(void)
{
    Init_DS18B20();
  
    WriteOneChar(0xCC);           //跳过读序号列号的操作 
    WriteOneChar(0x4e);           //将设定的温度报警值写入 DS18B20
	WriteOneChar(temp_alarm[0]);  //写TH
	WriteOneChar(temp_alarm[1]);  //写TL
    WriteOneChar(0x7f);           //12位精确度
 
	Init_DS18B20();
    WriteOneChar(0xCC);           //跳过读序号列号的操作 
    WriteOneChar(0x48);           //把暂存器里的温度报警值拷贝到EEROM
}

/*******************************************************************/
/*                                                                 */
/* 读取64位序列码                                                  */
/*                                                                 */
/*******************************************************************/
void Read_RomCord(void)
{
   unsigned char j;
   Init_DS18B20();
  
   WriteOneChar(0x33);          // 读序列码的操作
   for (j = 0; j < 8; j++)
   {
     RomCode[j] = ReadOneChar() ; 
   }
}

/*******************************************************************/
/*                                                                 */
/*DS18B20的CRC8校验程序                                            */
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
/* 数据转换与显示                                                  */
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
    lcd_wdat(display[0]);        //高位数显示 

    display[1]=(Temp&0x0f);
    if(display[1]>9)
     {display[1]=display[1]+0x37;}
    else {display[1]=display[1]+0x30;}           
    lcd_wdat(display[1]);        //低位数显示 
   }
}	 

/*******************************************************************/
/*                                                                 */
/* 读取温度                                                        */
/*                                                                 */
/*******************************************************************/
void Read_Temperature(void)
{
     uchar  i;
     TR0=0;                     //关中断,防止读数错误
     Init_DS18B20();
  
     WriteOneChar(0xCC);        //跳过读序号列号的操作
     WriteOneChar(0x44);        //启动温度转换

     Init_DS18B20();

     WriteOneChar(0x55);         //匹配ROM命令
     for(i=0;i<8;i++)
     WriteOneChar(RomCode[i]);

     WriteOneChar(0xBE);         //读取温度寄存器

     temp_data[0] = ReadOneChar();   //温度低8位
     temp_data[1] = ReadOneChar();   //温度高8位
     temp_alarm[0] = ReadOneChar();  //温度报警TH
	 temp_alarm[1] = ReadOneChar();  //温度报警TL

	 temp_comp=((temp_data[0]&0xf0)>>4)|((temp_data[1]&0x0f)<<4);
                                   //取温度整数值
     TR0=1;        //开中断
}
/*******************************************************************/
/*                                                                 */
/* 数据转换与温度显示                                              */
/*                                                                 */
/*******************************************************************/
void Disp_Temperature()
{
   uchar s=0;
   if(temp_data[1]>127)                  //温度为负值
   {                                        
    temp_data[0]=(~temp_data[0])+1;     //取反加一，将补码变成原码
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

    if(display[3]==0x30)        //高位为0，不显示
   { 
     display[3]=0x20;              
     if(display[2]==0x30)       //次高位为0，不显示
     display[2]=0x20;
   }
   if(s)
   display[3]=0x2d;            //显示负号
    
    lcd_pos(0x07);             
    lcd_wdat(display[3]);      //百位数显示             
    lcd_wdat(display[2]);      //十位数显示              
    lcd_wdat(display[1]);      //个位数显示  
    lcd_wdat('.');            
    lcd_wdat(display[0]);      //小数位数显示
 	lcd_wdat(0x00);            //显示自定义字符
    lcd_wdat('C');             //显示C
}	 

/*******************************************************************/
/*                                                                 */
/* 蜂鸣器响一声                                                    */
/*                                                                 */
/*******************************************************************/
void beep()
{
    unsigned char y;
    for (y=0;y<100;y++)
    {
      Delay(70);
      BEEP=!BEEP;                //BEEP取反
    } 
    BEEP=1;                      //关闭蜂鸣器
	Delay(25000);
}

/*******************************************************************/
/*                                                                 */
/* ROMCORD 显示菜单                                                */
/*                                                                 */
/*******************************************************************/
void  RomCode_Menu ()
{ 
    uchar  m;
    lcd_init();                //初始化LCD
 
    lcd_pos(0);                //设置显示位置为第一行
    for(m=0;m<16;m++)          //显示字符
    lcd_wdat(cdis1[m]);
     
    Read_RomCord();            //读取64位序列码
    CRC8();                    //CRC效验
    if(crc==0)                 //CRC效验正确
    {
	  Disp_RomCode(2);	       //显示64位序列码
    }
}

/*******************************************************************/
/*                                                                 */
/* DS18B20 ERROR 显示菜单                                          */
/*                                                                 */
/*******************************************************************/
void  Error_Menu ()
{
     uchar  m;
     lcd_init();                //初始化LCD 
	
  	 lcd_pos(0);                //设置显示位置为第一行的第1个字符
     for(m=0;m<16;m++) 
     lcd_wdat(cdis3[m]);        //显示字符
    
     lcd_pos(0x40);             //设置显示位置为第二行第1个字符
     for(m=0;m<16;m++) 
     lcd_wdat(cdis4[m]);        //显示字符
}

/*******************************************************************/
/*                                                                 */
/* DS18B20 正常显示菜单                                            */
/*                                                                 */
/*******************************************************************/
void  Natural_Menu ()
{
   uchar  m;
   lcd_init();                //初始化LCD 

   lcd_pos(0);                //设置显示位置为第一行的第1个字符
   for(m=0;m<16;m++) 
   lcd_wdat(cdis5[m]);        //显示字符

   lcd_pos(0x40);             //设置显示位置为第二行第1个字符
   for(m=0;m<16;m++) 
   lcd_wdat(cdis6[m]);        //显示字符

   Read_Temperature();
   Disp_Temp_alarm(0x43,0);   //显示TH值
   Disp_Temp_alarm(0x4b,1);   //显示TL值
   Disp_Temperature();        //显示实时温度值
}


/*********************************************************/
// 显示报警温度
/*********************************************************/
void Disp_Temp_alarm(uchar addr,uchar num)
{   //0=TH,1=TL
   display1[2]=temp_alarm[num]/100+0x30;
   display1[0]=temp_alarm[num]%100;
   display1[1]=display1[0]/10+0x30;
   display1[0]=display1[0]%10+0x30;

    if(display1[2]==0x30)        //高位为0，不显示
    { 
      display1[2]=0x20;              
      if(display1[1]==0x30)      //次高位为0，不显示
      display1[1]=0x20;
    }

    lcd_pos(addr);
    lcd_wdat(display1[2]);        //百位数显示             
    lcd_wdat(display1[1]);        //十位数显示              
    lcd_wdat(display1[0]);        //个位数显示
}    

/**********************************************************/
// Time0中断函数
/**********************************************************/
void Time0(void) interrupt 1 using 0
{
   TH0=0x4c;        //50ms定时
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
/*小喇叭闪动                                             */
/*                                                       */
/*********************************************************/
void  spk(uchar addr)
{
   if(flag)
   {
     lcd_pos(addr);
     lcd_wdat(0x01);      //小喇叭显示
   }
   else
   {
     lcd_pos(addr);
     lcd_wdat(0x20);      //清屏
   }
}

/*********************************************************/
// 温度比较函数
/*********************************************************/
void temp_compare()
{    
    if(temp_comp >= temp_alarm[0])      //比较 TH 值
     {
       compare_th=1;                    
	 }
	else  compare_th=0;

    if(temp_comp < temp_alarm[1])        //比较 TL 值
	 {
	   compare_tl=1;
	 }
	else  compare_tl=0;
     
	if(compare_th)
	 {
	   spk(0x46);            //小喇叭闪动
       if(!alarm_on_off)     //静音选择
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
	   spk(0x4e);           //小喇叭闪动
       if(!alarm_on_off)    //静音选择
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
// 温度报警值闪动
/*********************************************************/
void  Set_tempalarm_Flash(uchar addr,uchar num)
{
   if(flag)
   {
     temp_alarm[num]=count;
     Disp_Temp_alarm(addr,num);  //显示温度报警值
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
/* 主函数                                                          */
/*                                                                 */
/*******************************************************************/
void main()
 {
	uchar  m;
	TMOD=0x01; TH0=0x4c; TL0=0x00; 				//50ms定时
	EA=1; ET0=1; TR0=1;
	P0=0;
	P2&=0x1F;
	Init_DS18B20();
	Write_Temperature_alarm();					//设定报警上下限温度值
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
    
	RomCode_Menu ();      						//显示RomCode
	for(m=0;m<10;m++)
	Delay(50000);
    Natural_Menu ();							//正常显示界面
	Delay(50000);
	while(1)
	{
		Read_Temperature();						//读取当前温度
		Delay(50000);
		Disp_Temperature();						//更新当前温度
		temp_compare();							//温度上下限比较
		if(compare_th == 0 && compare_tl == 0)	//在正常范围正常显示
		{
		   Disp_Temp_alarm(0x43,0);   //显示TH值
           Disp_Temp_alarm(0x4b,1);   //显示TL值
		}
		if(compare_th)
		{
			Set_tempalarm_Flash(0x43,0);		//上限报警
		}
		if(compare_tl)
		{
			Set_tempalarm_Flash(0x4b,1);		//下限报警
		}
  }//end of while(1)
}  //end of main()

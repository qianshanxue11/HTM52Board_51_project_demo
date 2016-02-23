#include"uc1705.h"

/*******************************************************************************
* 函 数 名 ：LcdWriteCmd
* 函数功能 ：写入一个命令到12864
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void LcdWriteCmd(cmd)
{
 char i; 
 lcd_CS=0;
 lcd_RS=0;
 for(i=0;i<8;i++){ 
   lcd_SCLK=0; 
   if(cmd&0x80) lcd_SID=1; //置数据线
   else       lcd_SID=0; 
   lcd_SCLK=1;  //产生时钟上升沿
   cmd <<= 1; 
 } 
 lcd_CS=1; 
}

/*******************************************************************************
* 函 数 名 ：LcdWriteData
* 函数功能 ：写入一个数据到12864
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void LcdWriteData(dat)
{	
 char i; 
 lcd_CS=0;
 lcd_RS=1;
 for(i=0;i<8;i++){ 
   lcd_SCLK=0; 
   if(dat&0x80) lcd_SID=1; //置数据线
   else       lcd_SID=0; 
   lcd_SCLK=1;  //产生时钟上升沿
   dat <<= 1; 
 } 
 lcd_CS=1; 
}

void lcd_delay(int i){ //LCD专用延时函数
 int j,k; 
 for(j=0;j<i;j++) 
 for(k=0;k<1;k++); 
} 

/*******************************************************************************
* 函 数 名 ：Lcd12864_Init
* 函数功能 ：初始化12864
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
/*LCD12864的命令指令可以查看《03-dts-430 uc1705x_a1.0.pdf》的第12页的位置*/
void Lcd12864_Init()
{
	code unsigned char con_d2 = 0x16; //对比度细调默认值,可设置范围 0x00～0x3f
  lcd_RST=0;     lcd_delay(1000);//低电平复位
  lcd_RST=1;     lcd_delay(20);  //复位完成
	
  LcdWriteCmd(0xe2); lcd_delay(5);   //软复位 

  LcdWriteCmd(0x2c); lcd_delay(5);  //升压步聚 Boost ON
  LcdWriteCmd(0x2e); lcd_delay(5);  //升压步聚 Voltage Regular ON
  LcdWriteCmd(0x2f); lcd_delay(5);  //升压步聚 Voltage Follower ON

  LcdWriteCmd(0x24);  //粗调对比度，可设置范围 0x20～0x27
  LcdWriteCmd(0x81);  //微调对比度,此行及下行需紧跟
  LcdWriteCmd(con_d2);//0x1a,微调对比度的值，可设置范围 0x00～0x3f

  LcdWriteCmd(0xa2);  //1/9 偏压比(bais set)
  LcdWriteCmd(0xc8);  //行扫描顺序：0xc8从上到下,0xc0从下到上
  LcdWriteCmd(0xa0);  //列扫描顺序：0xa0从左到右,0xa1从右到左

  LcdWriteCmd(0x40);  //起始行：第一行开始
  LcdWriteCmd(0xaf);  //开显示	

}

/*******************************************************************************
* 函 数 名 ：LcdClearScreen
* 函数功能 ：清屏12864
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void LcdClearScreen(void)
{
	unsigned char i, j;

	for(i=0; i<8; i++)
	{
		LcdWriteCmd(0xB0+i); //y轴地址

		//--在写入数据之后X坐标的坐标是会自动加1的，我们初始化使用0xA0
    //x轴坐标从0x10,0x00开始
		LcdWriteCmd(0x10); 
		LcdWriteCmd(0x00);							   
		
		//X轴有128位，就一共刷128次，X坐标会自动加1，所以我们不用再设置坐标
		for(j=0; j<128; j++)
		{
			LcdWriteData(0x00);  
		}
	}
}

/*******************************************************************************
* 函 数 名 ：LcdWriteString
* 函数功能 ：在12864上面书写16X16的汉字，该函数可以直接书写字符串
* 输    入 ：x, y, cn
* 输    出 ：无
*******************************************************************************/
#ifdef  CHAR_CODE

#include"charcode.h"

unsigned char LcdWriteString(unsigned char x, unsigned char y, unsigned char *cn)
{
	unsigned char j, x1, x2, wordNum;

	//--Y的坐标只能从0到7，大于则直接返回--//
	if(y > 7)
	{
		return 0;
	}

	//--X的坐标只能从0到128，大于则直接返回--//
	if(x > 128)
	{
		return 0;
	}
	y += 0xB0;	   //求取Y坐标的值
	//--设置Y坐标--//
	LcdWriteCmd(y);
	while ( *cn != '\0')	 //在C语言中字符串结束以‘\0’结尾
	{	
	
		//--设置Y坐标--//
		LcdWriteCmd(y);

		x1 = (x >> 4) & 0x0F;   //由于X坐标要两句命令，分高低4位，所以这里先取出高4位
		x2 = x & 0x0F;          //去低四位
		//--设置X坐标--//
		LcdWriteCmd(0x10 + x1);   //高4位
		LcdWriteCmd(x2);	//低4位

		for (wordNum=0; wordNum<50; wordNum++)
		{
		    //--查询要写的字在字库中的位置--//
			if ((CN16CHAR[wordNum].Index[0] == *cn)
			     &&(CN16CHAR[wordNum].Index[1] == *(cn+1)))
			{
				for (j=0; j<32; j++) //写一个字
				{		
					if (j == 16)	 //由于16X16用到两个Y坐标，当大于等于16时，切换坐标
					{
						//--设置Y坐标--//
			   			LcdWriteCmd(y + 1);
			
						//--设置X坐标--//
						LcdWriteCmd(0x10 + x1);   //高4位
						LcdWriteCmd(x2);	//低4位
					}
					LcdWriteData(CN16CHAR[wordNum].Msk[j]);
				}
				x += 16;
			}//if查到字结束		
		} //for查字结束	
		cn += 2;
	}	//while结束
	return 1;
}

#endif


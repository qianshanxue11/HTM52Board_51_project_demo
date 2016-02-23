#include"uc1705.h"

/*******************************************************************************
* �� �� �� ��LcdWriteCmd
* �������� ��д��һ�����12864
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void LcdWriteCmd(cmd)
{
 char i; 
 lcd_CS=0;
 lcd_RS=0;
 for(i=0;i<8;i++){ 
   lcd_SCLK=0; 
   if(cmd&0x80) lcd_SID=1; //��������
   else       lcd_SID=0; 
   lcd_SCLK=1;  //����ʱ��������
   cmd <<= 1; 
 } 
 lcd_CS=1; 
}

/*******************************************************************************
* �� �� �� ��LcdWriteData
* �������� ��д��һ�����ݵ�12864
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void LcdWriteData(dat)
{	
 char i; 
 lcd_CS=0;
 lcd_RS=1;
 for(i=0;i<8;i++){ 
   lcd_SCLK=0; 
   if(dat&0x80) lcd_SID=1; //��������
   else       lcd_SID=0; 
   lcd_SCLK=1;  //����ʱ��������
   dat <<= 1; 
 } 
 lcd_CS=1; 
}

void lcd_delay(int i){ //LCDר����ʱ����
 int j,k; 
 for(j=0;j<i;j++) 
 for(k=0;k<1;k++); 
} 

/*******************************************************************************
* �� �� �� ��Lcd12864_Init
* �������� ����ʼ��12864
* ��    �� ����
* ��    �� ����
*******************************************************************************/
/*LCD12864������ָ����Բ鿴��03-dts-430 uc1705x_a1.0.pdf���ĵ�12ҳ��λ��*/
void Lcd12864_Init()
{
	code unsigned char con_d2 = 0x16; //�Աȶ�ϸ��Ĭ��ֵ,�����÷�Χ 0x00��0x3f
  lcd_RST=0;     lcd_delay(1000);//�͵�ƽ��λ
  lcd_RST=1;     lcd_delay(20);  //��λ���
	
  LcdWriteCmd(0xe2); lcd_delay(5);   //��λ 

  LcdWriteCmd(0x2c); lcd_delay(5);  //��ѹ���� Boost ON
  LcdWriteCmd(0x2e); lcd_delay(5);  //��ѹ���� Voltage Regular ON
  LcdWriteCmd(0x2f); lcd_delay(5);  //��ѹ���� Voltage Follower ON

  LcdWriteCmd(0x24);  //�ֵ��Աȶȣ������÷�Χ 0x20��0x27
  LcdWriteCmd(0x81);  //΢���Աȶ�,���м����������
  LcdWriteCmd(con_d2);//0x1a,΢���Աȶȵ�ֵ�������÷�Χ 0x00��0x3f

  LcdWriteCmd(0xa2);  //1/9 ƫѹ��(bais set)
  LcdWriteCmd(0xc8);  //��ɨ��˳��0xc8���ϵ���,0xc0���µ���
  LcdWriteCmd(0xa0);  //��ɨ��˳��0xa0������,0xa1���ҵ���

  LcdWriteCmd(0x40);  //��ʼ�У���һ�п�ʼ
  LcdWriteCmd(0xaf);  //����ʾ	

}

/*******************************************************************************
* �� �� �� ��LcdClearScreen
* �������� ������12864
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void LcdClearScreen(void)
{
	unsigned char i, j;

	for(i=0; i<8; i++)
	{
		LcdWriteCmd(0xB0+i); //y���ַ

		//--��д������֮��X����������ǻ��Զ���1�ģ����ǳ�ʼ��ʹ��0xA0
    //x�������0x10,0x00��ʼ
		LcdWriteCmd(0x10); 
		LcdWriteCmd(0x00);							   
		
		//X����128λ����һ��ˢ128�Σ�X������Զ���1���������ǲ�������������
		for(j=0; j<128; j++)
		{
			LcdWriteData(0x00);  
		}
	}
}

/*******************************************************************************
* �� �� �� ��LcdWriteString
* �������� ����12864������д16X16�ĺ��֣��ú�������ֱ����д�ַ���
* ��    �� ��x, y, cn
* ��    �� ����
*******************************************************************************/
#ifdef  CHAR_CODE

#include"charcode.h"

unsigned char LcdWriteString(unsigned char x, unsigned char y, unsigned char *cn)
{
	unsigned char j, x1, x2, wordNum;

	//--Y������ֻ�ܴ�0��7��������ֱ�ӷ���--//
	if(y > 7)
	{
		return 0;
	}

	//--X������ֻ�ܴ�0��128��������ֱ�ӷ���--//
	if(x > 128)
	{
		return 0;
	}
	y += 0xB0;	   //��ȡY�����ֵ
	//--����Y����--//
	LcdWriteCmd(y);
	while ( *cn != '\0')	 //��C�������ַ��������ԡ�\0����β
	{	
	
		//--����Y����--//
		LcdWriteCmd(y);

		x1 = (x >> 4) & 0x0F;   //����X����Ҫ��������ָߵ�4λ������������ȡ����4λ
		x2 = x & 0x0F;          //ȥ����λ
		//--����X����--//
		LcdWriteCmd(0x10 + x1);   //��4λ
		LcdWriteCmd(x2);	//��4λ

		for (wordNum=0; wordNum<50; wordNum++)
		{
		    //--��ѯҪд�������ֿ��е�λ��--//
			if ((CN16CHAR[wordNum].Index[0] == *cn)
			     &&(CN16CHAR[wordNum].Index[1] == *(cn+1)))
			{
				for (j=0; j<32; j++) //дһ����
				{		
					if (j == 16)	 //����16X16�õ�����Y���꣬�����ڵ���16ʱ���л�����
					{
						//--����Y����--//
			   			LcdWriteCmd(y + 1);
			
						//--����X����--//
						LcdWriteCmd(0x10 + x1);   //��4λ
						LcdWriteCmd(x2);	//��4λ
					}
					LcdWriteData(CN16CHAR[wordNum].Msk[j]);
				}
				x += 16;
			}//if�鵽�ֽ���		
		} //for���ֽ���	
		cn += 2;
	}	//while����
	return 1;
}

#endif


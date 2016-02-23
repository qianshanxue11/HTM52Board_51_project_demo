#ifndef __CH_ENLIB_H
#define __CH_ENLIB_H

#define  BLACK  0
#define  GRAY  33808
#define  SILVER  50712
#define  LIGHT_GRAY  54938
#define  BROWN  41285
#define  MAROON  32768
#define  RED  63488
#define  ORANGE_RED  64032
#define  YELLOW  65504
#define  APPLE_GREEN  36640
#define  GREEN  1024
#define  SKY_BLUE  34429
#define  BLUE  31
#define  PURPLE  32784
#define  WHITE  65535


// ------------------  ͼƬ�����ݽṹ���� ------------------------ //
struct Picture{
	unsigned int x_pix;//��������ص�
	unsigned int y_pix;//��������ص�
	unsigned char *pic_buf;//���ͼƬ��buf
};

// ------------------  Ӣ����ģ�����ݽṹ���� ------------------------ //
struct EnCharType                 // Ӣ����ģ���ݽṹ 
{
	unsigned char  Msk[12];             // ����������(12*8��12������) 
	unsigned char  Index;            // ����
};

// ------------------  ������ģ�����ݽṹ���� ------------------------ //
struct CnCharType                 // ������ģ���ݽṹ 
{
	unsigned char  Msk[24];             // ����������(12*8��12������) 
	unsigned char  Index[2];            // ����
};

extern unsigned char code logo_buf[];
extern struct Picture code logo;

extern struct EnCharType code en_char[];
extern struct CnCharType code cn_char[];


#endif


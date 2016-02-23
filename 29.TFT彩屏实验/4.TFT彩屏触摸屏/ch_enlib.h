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


// ------------------  图片的数据结构定义 ------------------------ //
struct Picture{
	unsigned int x_pix;//横向的像素点
	unsigned int y_pix;//纵向的像素点
	unsigned char *pic_buf;//存放图片的buf
};

// ------------------  英文字模的数据结构定义 ------------------------ //
struct EnCharType                 // 英文字模数据结构 
{
	unsigned char  Msk[12];             // 点阵码数据(12*8有12个数据) 
	unsigned char  Index;            // 索引
};

// ------------------  汉字字模的数据结构定义 ------------------------ //
struct CnCharType                 // 汉字字模数据结构 
{
	unsigned char  Msk[24];             // 点阵码数据(12*8有12个数据) 
	unsigned char  Index[2];            // 索引
};

extern unsigned char code logo_buf[];
extern struct Picture code logo;

extern struct EnCharType code en_char[];
extern struct CnCharType code cn_char[];


#endif


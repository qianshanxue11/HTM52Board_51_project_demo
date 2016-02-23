#ifndef __TOUTCH_H
#define __TOUTCH_H
#include<reg52.h>
#include <intrins.h>

/* touch panel interface define */

sbit T_IRQ       =    P3^2;   //检测触摸屏响应信号
sbit T_OUT      =    P1^4;
sbit T_DIN       =    P1^5;	
sbit T_CS        =    P1^6;
sbit T_CLK	   =    P1^7;  

//定义坐标结构体
struct pix_
{
	unsigned int x;
	unsigned int y;
};

struct pix_ ReadToutchAxis();

#endif

/*
*********************************************************************************************************
*   											uC/OS-II
*   									 The Real-Time Kernel
*
*   					  (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*   									   All Rights Reserved
*
*   									STC89C52RC Specific code
*   									   LARGE MEMORY MODEL
*   									   Keil V4.00a
* File  	   : OS_CPU.H
* By		   : Jean J. Labrosse
* Modify	   : Hantech
* Date  	   : 2015.06.11
*********************************************************************************************************
*/

#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT  extern
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*   										   DATA TYPES
*   									  (Compiler Specific)
*********************************************************************************************************
*/

typedef unsigned char BOOLEAN;
typedef unsigned char INT8U;					/* Unsigned  8 bit quantity 						  */
typedef signed   char INT8S;					/* Signed    8 bit quantity 						  */

typedef unsigned int INT16U;				   /* Unsigned 16 bit quantity  						 */
typedef signed   int INT16S;				   /* Signed   16 bit quantity  						 */

typedef unsigned long INT32U;   				/* Unsigned 32 bit quantity 						  */
typedef signed   long INT32S;   				/* Signed   32 bit quantity 						  */

typedef float FP32; 					/* Single precision floating point  				  */

typedef unsigned char OS_STK;   			   /* Each stack entry is 8-bit wide in C51 			 */

typedef INT16U OS_FLAGS;   /* Date type for event flag bits (8, 16 or 32 bits)  		   */

#define BYTE		   INT8S		  //兼容以前版本的数据类型
#define UBYTE   	   INT8U		  //uC/OS-II可以不用这些数据类型
#define WORD		   INT16S
#define UWORD   	   INT16U
#define LONG		   INT32S
#define ULONG   	   INT32U
/* 
*********************************************************************************************************
*   						   处理器相关代码 MCU-51 (大模式)
*   					  针对51单片机，只使用方法1(直接开关中断)
*********************************************************************************************************
*/
#define  OS_CRITICAL_METHOD    1

#define  OS_ENTER_CRITICAL()  EA=0    //关中断

#define  OS_EXIT_CRITICAL()   EA=1    //开中断

#define  OS_STK_GROWTH    0 		  //MCU-51堆栈从下往上增长  1=向下，0=向上

#define  OS_TASK_SW()     OSCtxSw()   //因为MCU-51没有软中断指令，所以用程序调用代替。两者的堆栈格式相同，
//RETI指令复位中断系统，RET则没有。实践表明，对于MCU-51，用子程序调
//用入栈，用中断返回指令RETI出栈是没有问题的，反之中断入栈RET出栈则
//不行。总之，对于入栈，子程序调用与中断调用效果是一样的，可以混用。
//在没有中断发生的情况下复位中断系统也不会影响系统正常运行。
//详见《uC/OS-II》第八章193页第12行

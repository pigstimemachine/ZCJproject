#ifndef __USART_H
#define __USART_H
#include <stm32f10x_map.h>
#include <stm32f10x_nvic.h>
#include "stdio.h"	 

#define USART_MAX_RECV_LEN       100
#define ReadOK                   1
#define ReadERR                  0
#define WriteOK                  1
#define WriteERR                 0

	  	
typedef struct {
	u8   FrameData[USART_MAX_RECV_LEN];
	u16  DLC;
}UARTBufType;

extern u8 USART_RX_STA;         //接收状态标记	

//如果想串口中断接收，请不要注释以下宏定义
//#define EN_USART1_RX //使能串口1接收
void uart_init(u32 pclk2,u32 bound);
void USART1_FrameEND(void);
u8 USART1_ReadFrame(UARTBufType *Framebuf);
u8 USART1_WriteFrame(UARTBufType *Framebuf);
#endif	   

















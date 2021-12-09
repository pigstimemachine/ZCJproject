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

extern u8 USART_RX_STA;         //����״̬���	

//����봮���жϽ��գ��벻Ҫע�����º궨��
//#define EN_USART1_RX //ʹ�ܴ���1����
void uart_init(u32 pclk2,u32 bound);
void USART1_FrameEND(void);
u8 USART1_ReadFrame(UARTBufType *Framebuf);
u8 USART1_WriteFrame(UARTBufType *Framebuf);
#endif	   

















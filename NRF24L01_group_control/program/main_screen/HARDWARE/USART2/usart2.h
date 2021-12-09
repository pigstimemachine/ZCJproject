#ifndef __USART2_H
#define __USART2_H
#include <stm32f10x.h>
#include "stdio.h"
#include "lib.h"	 
//////////////////////////////////////////////////////////////////////////////////	 
typedef struct
{
	u8  StartRev;
	u8  DataLen;
}UART_STATUS;

#define USART2_MAX_RECV_LEN		  1024				               //
#define USART2_MAX_SEND_LEN		  1024				               //
extern u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节
extern u16 USART2_RX_STA;   						//接收数据状态  	
extern u16 USART2_RX_STA;         		//接收状态标记	
void uart2_init(u32 pclk2,u32 bound);
void u2_printf(char* fmt,...);
void u2_send(u8* buf,u16 len);
u8 ProtocolAnalysis(u8 DR);
void UartSendTask(void);
void u2_sendbyte(u8 byte);

#endif

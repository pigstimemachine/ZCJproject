#ifndef __UART4_H
#define __UART4_H
#include <stm32f10x_map.h>
#include <stm32f10x_nvic.h> 
#include "stdio.h"	 
//////////////////////////////////////////////////////////////////////////////////	 

#define UART4_MAX_RECV_LEN		1024				//最大接收缓存字节数
#define UART4_MAX_SEND_LEN		1024				//最大发送缓存字节数
#define UART4_RX_EN 			1					//0,不接收;1,接收.

extern u8  UART4_RX_BUF[UART4_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern u8  UART4_TX_BUF[UART4_MAX_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节
extern u16 UART4_RX_STA;   						//接收数据状态
	  	
extern u16 UART4_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void uart4_init(u32 pclk2,u32 bound);
void u4_printf(char* fmt,...);

#endif

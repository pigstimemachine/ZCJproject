#ifndef __UART4_H
#define __UART4_H
#include <stm32f10x_map.h>
#include <stm32f10x_nvic.h> 
#include "stdio.h"	 
//////////////////////////////////////////////////////////////////////////////////	 

#define UART4_MAX_RECV_LEN		1024				//�����ջ����ֽ���
#define UART4_MAX_SEND_LEN		1024				//����ͻ����ֽ���
#define UART4_RX_EN 			1					//0,������;1,����.

extern u8  UART4_RX_BUF[UART4_MAX_RECV_LEN]; 		//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern u8  UART4_TX_BUF[UART4_MAX_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern u16 UART4_RX_STA;   						//��������״̬
	  	
extern u16 UART4_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart4_init(u32 pclk2,u32 bound);
void u4_printf(char* fmt,...);

#endif

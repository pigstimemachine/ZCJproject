#include "sys.h"
#include "usart.h"
#include "uart4.h"	 
#include "timer.h" 
#include "string.h"	
#include "delay.h"
#include "stdarg.h"	 	 
#include "stdio.h"
#include "includes.h" 
#include "lib.h"

//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 UART4_RX_STA=0;       //����״̬���
u8 UART4_TX_BUF[UART4_MAX_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�   	  
//���ڽ��ջ����� 	
u8 UART4_RX_BUF[UART4_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.	  
  
void UART4_IRQHandler(void)
{
    OSIntEnter();
    if(UART4->SR&(1<<6))//��������    
	 {
	  
	 }
	if(UART4->SR&(1<<5))//���յ�����
	 {	 
		u8 res;	     
		res=UART4->DR; 			 
		if(UART4_RX_STA<UART4_MAX_RECV_LEN)		//�����Խ�������
		{
			TIM4->CNT=0;         					//���������
			if(UART4_RX_STA==0)TIM4_Set(1);	 	//ʹ�ܶ�ʱ��4���ж� 
			UART4_RX_BUF[UART4_RX_STA++]=res;		//��¼���յ���ֵ	 
		}else 
		{
			UART4_RX_STA|=1<<15;					//ǿ�Ʊ�ǽ������
	 	} 											     
	 }
	OSIntExit();
} 									 
//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
//091209
void uart4_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<4;   //ʹ��PORTC��ʱ��  
	RCC->APB1ENR|=1<<19;  //ʹ�ܴ���ʱ�� 
	GPIOC->CRH&=0XFFFF00FF;//IO״̬����
	GPIOC->CRH|=0X00008B00;//IO״̬����
		  
	RCC->APB1RSTR|=1<<19;   //��λ����2
	RCC->APB1RSTR&=~(1<<19);//ֹͣ��λ	   	   
	//����������
 	UART4->BRR=mantissa; // ����������	 
	UART4->CR1|=0X200C;  //1λֹͣ,��У��λ.
	//ʹ�ܽ����ж�
	UART4->CR1|=1<<8;    //PE�ж�ʹ��	
	UART4->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��
//	UART4->CR3=1<<7;   	//ʹ�ܴ���2��DMA����
//	UART4_DMA_Config(DMA1_Channel4,(u32)&USART1->DR,(u32)USART1_TX_BUF);//DMA1ͨ��7,����Ϊ����2,�洢��ΪUSART2_TX_BUF 	    	
	MY_NVIC_Init(3,3,UART4_IRQChannel,2);//��2��������ȼ�
}

void u4_printf(char* fmt,...)  
{  
    u8 i=0;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)UART4_TX_BUF,fmt,ap);
	while ((i<UART4_MAX_SEND_LEN)&&UART4_TX_BUF[i])
      {
          UART4->DR=UART4_TX_BUF[i++];
          while((UART4->SR&0X40)==0); 
      }
	va_end(ap);	   
}

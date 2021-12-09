#include "sys.h"
#include "usart.h"
#include "timer.h"
#include <string.h>
	  
 

//end
//////////////////////////////////////////////////////////////////

//����1�жϷ������ 	
UARTBufType  U1ReceiveBuf,U1ShadowBuf;
//����״̬
//bit7��������ɱ�־
//bit6�����յ�0x0d
//bit5~0�����յ�����Ч�ֽ���Ŀ
  
void USART1_IRQHandler(void)
{
	u8 Tmpdata;	    
	if(USART1->SR&(1<<5))//���յ�����
	{	 	     
		Tmpdata=USART1->DR; 			 
		if(U1ReceiveBuf.DLC<USART_MAX_RECV_LEN)		  //�����Խ�������
		{
			TIM4_Clear(0);         					        //���������
			if(U1ReceiveBuf.DLC==0) TIM4_Enable(1);	 	//ʹ�ܶ�ʱ��4���ж� 
			U1ReceiveBuf.FrameData[U1ReceiveBuf.DLC++]=Tmpdata;		//��¼���յ���ֵ	 
		}else 
		{
			USART1_FrameEND();					//ǿ�Ʊ�ǽ������
		} 				     
	}  											 
} 

void USART1_FrameEND(void)
{
	memcpy(&U1ShadowBuf,&U1ReceiveBuf,sizeof(UARTBufType));
	memset(&U1ReceiveBuf,0,sizeof(U1ReceiveBuf));
}	

void USART1_Timeout(u32 bit,u32 bound)  //Ĭ��3.5�ַ���35λ
{
	 u32 nms;
	 nms=((bit*10000/bound)+5)/10;
	 nms=nms*10-1;
   TIM4_Init(nms,7199);
}										 
//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
//091209
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
  mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<14;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRH&=0XFFFFF00F; 
	GPIOA->CRH|=0X000008B0;//IO״̬����
		  
	RCC->APB2RSTR|=1<<14;   //��λ����1
	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ	   	   
	//����������
 	USART1->BRR=mantissa; // ����������	 
	USART1->CR1|=0X200C;  //1λֹͣ,��У��λ.
	USART1->CR1|=1<<8;    //PE�ж�ʹ��
	USART1->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(1,3,USART1_IRQChannel,2);//��2��������ȼ� 
	
  USART1_Timeout(40,bound);
}

u8 USART1_ReadFrame(UARTBufType *Framebuf)
{
  if(U1ShadowBuf.DLC!=0)
	{
	   memcpy(Framebuf,&U1ShadowBuf,sizeof(UARTBufType));
		 memset(&U1ShadowBuf,0,sizeof(U1ShadowBuf));
		 return ReadOK;
	}	
	return ReadERR;
}

u8 USART1_WriteFrame(UARTBufType *Framebuf)
{
  u16 cnt;
	if(Framebuf->DLC>USART_MAX_RECV_LEN) return WriteERR;
  for(cnt=0;cnt<Framebuf->DLC;cnt++)
	{
	   USART1->DR=Framebuf->FrameData[cnt];
		 while((USART1->SR&0X40)==0); 
	}	
	return WriteOK;
}

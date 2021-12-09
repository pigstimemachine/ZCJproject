#include "timer.h" 
#include "usart.h" 
#include "TaskMgr.h"

void TIM2_Int_Init(u16 arr,u16 psc)
{	
	RCC->APB1ENR|=1<<0;	//TIM2ʱ��ʹ��    
 	TIM2->ARR=arr;  	//�趨�������Զ���װֵ//�պ�1ms    
	TIM2->PSC=psc;  	//Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��		  
	TIM2->DIER|=1<<0;   //��������ж�	  
	TIM2->CR1|=0x01;    //ʹ�ܶ�ʱ��3
  MY_NVIC_Init(3,3,TIM2_IRQChannel,2);//��ռ1�������ȼ�3����2									 
}

void TIM2_IRQHandler(void)
{ 	
	if(TIM2->SR&0X01)//�Ǹ����ж�
	{	 		   
		Timer0_IsrHandler();
		TIM2->SR&=~(1<<0);		//����жϱ�־λ		   
	}	    
}

void TIM4_Clear(u8 cnt)
{
  TIM4->CNT=cnt;
}
//����TIM4�Ŀ���
//sta:0���ر�;1,����;
void TIM4_Enable(u8 sta)
{
	if(sta)
	{
		TIM4->CR1|=1<<0;     //ʹ�ܶ�ʱ��4
    TIM4->CNT=0;         //���������
	}else TIM4->CR1&=~(1<<0);//�رն�ʱ��4	   
}

void TIM4_IRQHandler(void)
{ 	
	if(TIM4->SR&0X01)//�Ǹ����ж�
	{	 		   
		USART1_FrameEND();	//��ǽ������
		TIM4->SR&=~(1<<0);		//����жϱ�־λ		   
		TIM4_Enable(0);			//�ر�TIM4 
	}	    
}

//ͨ�ö�ʱ���жϳ�ʼ��
//����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��		 
void TIM4_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM4ʱ��ʹ��    
 	TIM4->ARR=arr;  	//�趨�������Զ���װֵ   
	TIM4->PSC=psc;  	//Ԥ��Ƶ��
 	TIM4->DIER|=1<<0;   //��������ж�				
 	TIM4->CR1|=0x00;  	//ʹ�ܶ�ʱ��4	  	   
  MY_NVIC_Init(1,3,TIM4_IRQChannel,2);//��ռ2�������ȼ�3����2	��2�����ȼ����								 
}





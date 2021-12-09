#include "timer.h" 
#include "usart.h" 
#include "TaskMgr.h"

void TIM2_Int_Init(u16 arr,u16 psc)
{	
	RCC->APB1ENR|=1<<0;	//TIM2时钟使能    
 	TIM2->ARR=arr;  	//设定计数器自动重装值//刚好1ms    
	TIM2->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟		  
	TIM2->DIER|=1<<0;   //允许更新中断	  
	TIM2->CR1|=0x01;    //使能定时器3
  MY_NVIC_Init(3,3,TIM2_IRQChannel,2);//抢占1，子优先级3，组2									 
}

void TIM2_IRQHandler(void)
{ 	
	if(TIM2->SR&0X01)//是更新中断
	{	 		   
		Timer0_IsrHandler();
		TIM2->SR&=~(1<<0);		//清除中断标志位		   
	}	    
}

void TIM4_Clear(u8 cnt)
{
  TIM4->CNT=cnt;
}
//设置TIM4的开关
//sta:0，关闭;1,开启;
void TIM4_Enable(u8 sta)
{
	if(sta)
	{
		TIM4->CR1|=1<<0;     //使能定时器4
    TIM4->CNT=0;         //计数器清空
	}else TIM4->CR1&=~(1<<0);//关闭定时器4	   
}

void TIM4_IRQHandler(void)
{ 	
	if(TIM4->SR&0X01)//是更新中断
	{	 		   
		USART1_FrameEND();	//标记接收完成
		TIM4->SR&=~(1<<0);		//清除中断标志位		   
		TIM4_Enable(0);			//关闭TIM4 
	}	    
}

//通用定时器中断初始化
//这里始终选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数		 
void TIM4_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM4时钟使能    
 	TIM4->ARR=arr;  	//设定计数器自动重装值   
	TIM4->PSC=psc;  	//预分频器
 	TIM4->DIER|=1<<0;   //允许更新中断				
 	TIM4->CR1|=0x00;  	//使能定时器4	  	   
  MY_NVIC_Init(1,3,TIM4_IRQChannel,2);//抢占2，子优先级3，组2	在2中优先级最低								 
}





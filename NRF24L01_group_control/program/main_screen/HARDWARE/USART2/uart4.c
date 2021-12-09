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

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 UART4_RX_STA=0;       //接收状态标记
u8 UART4_TX_BUF[UART4_MAX_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节   	  
//串口接收缓存区 	
u8 UART4_RX_BUF[UART4_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.	  
  
void UART4_IRQHandler(void)
{
    OSIntEnter();
    if(UART4->SR&(1<<6))//发送数据    
	 {
	  
	 }
	if(UART4->SR&(1<<5))//接收到数据
	 {	 
		u8 res;	     
		res=UART4->DR; 			 
		if(UART4_RX_STA<UART4_MAX_RECV_LEN)		//还可以接收数据
		{
			TIM4->CNT=0;         					//计数器清空
			if(UART4_RX_STA==0)TIM4_Set(1);	 	//使能定时器4的中断 
			UART4_RX_BUF[UART4_RX_STA++]=res;		//记录接收到的值	 
		}else 
		{
			UART4_RX_STA|=1<<15;					//强制标记接收完成
	 	} 											     
	 }
	OSIntExit();
} 									 
//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率
//CHECK OK
//091209
void uart4_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<4;   //使能PORTC口时钟  
	RCC->APB1ENR|=1<<19;  //使能串口时钟 
	GPIOC->CRH&=0XFFFF00FF;//IO状态设置
	GPIOC->CRH|=0X00008B00;//IO状态设置
		  
	RCC->APB1RSTR|=1<<19;   //复位串口2
	RCC->APB1RSTR&=~(1<<19);//停止复位	   	   
	//波特率设置
 	UART4->BRR=mantissa; // 波特率设置	 
	UART4->CR1|=0X200C;  //1位停止,无校验位.
	//使能接收中断
	UART4->CR1|=1<<8;    //PE中断使能	
	UART4->CR1|=1<<5;    //接收缓冲区非空中断使能
//	UART4->CR3=1<<7;   	//使能串口2的DMA发送
//	UART4_DMA_Config(DMA1_Channel4,(u32)&USART1->DR,(u32)USART1_TX_BUF);//DMA1通道7,外设为串口2,存储器为USART2_TX_BUF 	    	
	MY_NVIC_Init(3,3,UART4_IRQChannel,2);//组2，最低优先级
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

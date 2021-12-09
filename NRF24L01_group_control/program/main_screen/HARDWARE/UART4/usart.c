#include "sys.h"
#include "usart.h"
#include "timer.h"
#include <string.h>
	  
 

//end
//////////////////////////////////////////////////////////////////

//串口1中断服务程序 	
UARTBufType  U1ReceiveBuf,U1ShadowBuf;
//接收状态
//bit7，接收完成标志
//bit6，接收到0x0d
//bit5~0，接收到的有效字节数目
  
void UART4_IRQHandler(void)
{
	u8 Tmpdata;	    
	if(UART4->SR&(1<<5))//接收到数据
	{	 	     
		Tmpdata=UART4->DR; 			 
		if(U1ReceiveBuf.DLC<USART_MAX_RECV_LEN)		  //还可以接收数据
		{
			TIM4_Clear(0);         					        //计数器清空
			if(U1ReceiveBuf.DLC==0) TIM4_Enable(1);	 	//使能定时器4的中断 
			U1ReceiveBuf.FrameData[U1ReceiveBuf.DLC++]=Tmpdata;		//记录接收到的值	 
		}else 
		{
			USART1_FrameEND();					//强制标记接收完成
		} 				     
	}  											 
} 

void USART1_FrameEND(void)
{
	memcpy(&U1ShadowBuf,&U1ReceiveBuf,sizeof(UARTBufType));
	memset(&U1ReceiveBuf,0,sizeof(U1ReceiveBuf));
}	

void USART1_Timeout(u32 bit,u32 bound)  //默认3.5字符，35位
{
	 u32 nms;
	 nms=((bit*10000/bound)+5)/10;
	 nms=nms*10-1;
   TIM4_Init(nms,7199);
}										 
//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率
//CHECK OK
//091209
void uart_init(u32 pclk2,u32 bound)
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
	GPIOC->CRH&=0XFFFF00FF; 
	GPIOC->CRH|=0X00008B00;//IO状态设置
		  
	RCC->APB1RSTR|=1<<19;   //复位串口1
	RCC->APB1RSTR&=~(1<<19);//停止复位	   	   
	//波特率设置
 	UART4->BRR=mantissa; // 波特率设置	 
	UART4->CR1|=0X200C;  //1位停止,无校验位.
	UART4->CR1|=1<<8;    //PE中断使能
	UART4->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,3,UART4_IRQChannel,2);//组2，最低优先级 
	
  USART1_Timeout(60,bound);
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
	   UART4->DR=Framebuf->FrameData[cnt];
		 while((UART4->SR&0X40)==0); 
	}	
	return WriteOK;
}

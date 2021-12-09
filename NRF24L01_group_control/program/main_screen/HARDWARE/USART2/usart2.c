 #include "usart2.h"
//////////////////////////////////////////////////////////////////


//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART2_RX_STA=0;       //接收状态标记
u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节   	  
//串口接收缓存区 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.	
PARA_SET     ParaSet;
SWITCH       TrackSwitch;
UART_FORMAT  UartTxFormat;
UART_PARA    UartPara;
UART_STATUS  Uart2Status;
void USART2_IRQHandler(void)
{
	u8 res;
  if(USART2->SR&(1<<6))//发送数据    
	{
	  
	}
	if(USART2->SR&(1<<5))//接收到数据
	{	 
		 res=USART2->DR;
		 ProtocolAnalysis(res);			 													     
	}
} 

u8 ProtocolAnalysis(u8 DR)
{
  if(USART2_RX_STA==0) { Uart2Status.DataLen=0; Uart2Status.StartRev=0; }
	if((USART2_RX_STA&0x8000)==0)//接收未完成
	{	
		USART2_RX_BUF[USART2_RX_STA]=DR;
		if((USART2_RX_STA==0)&&(DR==0x55)) Uart2Status.StartRev=TRUE;
		else if((USART2_RX_STA==1)&&(Uart2Status.StartRev==TRUE)) Uart2Status.DataLen=DR;	
		if(USART2_RX_STA==Uart2Status.DataLen+1) 
		{
			 USART2_RX_STA=NULL;
			 if(USART2_RX_BUF[2]==ItemPara)
				 memcpy(&ParaSet,&USART2_RX_BUF[2],Uart2Status.DataLen);
			 else if(USART2_RX_BUF[2]==ItemSwitch)	
				 memcpy(&TrackSwitch,&USART2_RX_BUF[2],Uart2Status.DataLen);
		}
		else if(Uart2Status.StartRev==TRUE) USART2_RX_STA++;
	}  		 	
}	

//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率
//CHECK OK
//091209
void uart2_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
  mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<5;   //使能PORTD口时钟  
	RCC->APB1ENR|=1<<17;  //使能串口时钟 
	RCC->APB2ENR|=1<<0;   //使能AF时钟
	AFIO->MAPR|=1<<3;     //重映射端口
	
	GPIOD->CRL&=0XF00FFFFF;//IO状态设置
	GPIOD->CRL|=0X08B00000;//IO状态设置
		  
	RCC->APB1RSTR|=1<<17;   //复位串口2
	RCC->APB1RSTR&=~(1<<17);//停止复位	   	   
	//波特率设置
 	USART2->BRR=mantissa; // 波特率设置	 
	USART2->CR1|=0X200C;  //1位停止,无校验位.
	//使能接收中断
	USART2->CR1|=1<<8;    //PE中断使能	
	USART2->CR1|=1<<5;    //接收缓冲区非空中断使能
//	USART2->CR3=1<<7;   	//使能串口2的DMA发送
//	USART2_DMA_Config(DMA1_Channel4,(u32)&USART1->DR,(u32)USART1_TX_BUF);//DMA1通道7,外设为串口2,存储器为USART2_TX_BUF 	    	
	MY_NVIC_Init(1,3,USART2_IRQn,2);//组2，最低优先级
}

void uart2_printf(char* fmt,...)  
{  
  u16 i=0; 
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	while ((i<USART2_MAX_SEND_LEN)&&USART2_TX_BUF[i])
	{
		 USART2->DR=USART2_TX_BUF[i++];
		 while((USART2->SR&0X40)==0); 
	}
	va_end(ap);	   
}
void uart2_send(u8* buf,u16 len)
{
   u16 i;
   for(i=0;i<len;i++)
   {
      USART2->DR=buf[i];
	  while((USART2->SR&0X40)==0); 
   }
}
void uart2_sendbyte(u8 byte)
{
   USART2->DR=byte;
	 while((USART2->SR&0X40)==0);
}	



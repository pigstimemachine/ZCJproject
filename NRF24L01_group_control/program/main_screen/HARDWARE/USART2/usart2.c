 #include "usart2.h"
//////////////////////////////////////////////////////////////////


//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART2_RX_STA=0;       //����״̬���
u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�   	  
//���ڽ��ջ����� 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.	
PARA_SET     ParaSet;
SWITCH       TrackSwitch;
UART_FORMAT  UartTxFormat;
UART_PARA    UartPara;
UART_STATUS  Uart2Status;
void USART2_IRQHandler(void)
{
	u8 res;
  if(USART2->SR&(1<<6))//��������    
	{
	  
	}
	if(USART2->SR&(1<<5))//���յ�����
	{	 
		 res=USART2->DR;
		 ProtocolAnalysis(res);			 													     
	}
} 

u8 ProtocolAnalysis(u8 DR)
{
  if(USART2_RX_STA==0) { Uart2Status.DataLen=0; Uart2Status.StartRev=0; }
	if((USART2_RX_STA&0x8000)==0)//����δ���
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

//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
//091209
void uart2_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
  mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<5;   //ʹ��PORTD��ʱ��  
	RCC->APB1ENR|=1<<17;  //ʹ�ܴ���ʱ�� 
	RCC->APB2ENR|=1<<0;   //ʹ��AFʱ��
	AFIO->MAPR|=1<<3;     //��ӳ��˿�
	
	GPIOD->CRL&=0XF00FFFFF;//IO״̬����
	GPIOD->CRL|=0X08B00000;//IO״̬����
		  
	RCC->APB1RSTR|=1<<17;   //��λ����2
	RCC->APB1RSTR&=~(1<<17);//ֹͣ��λ	   	   
	//����������
 	USART2->BRR=mantissa; // ����������	 
	USART2->CR1|=0X200C;  //1λֹͣ,��У��λ.
	//ʹ�ܽ����ж�
	USART2->CR1|=1<<8;    //PE�ж�ʹ��	
	USART2->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��
//	USART2->CR3=1<<7;   	//ʹ�ܴ���2��DMA����
//	USART2_DMA_Config(DMA1_Channel4,(u32)&USART1->DR,(u32)USART1_TX_BUF);//DMA1ͨ��7,����Ϊ����2,�洢��ΪUSART2_TX_BUF 	    	
	MY_NVIC_Init(1,3,USART2_IRQn,2);//��2��������ȼ�
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



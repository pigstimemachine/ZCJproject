#include "24l01.h"
#include "spi.h"
#include <string.h>
#include "lib.h"		
const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x12,0x00}; //���͵�ַ 303055668
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x12,0x01}; //���͵�ַ
ArrayBufType TxBufType,RxBufType,RetryType;
RF24L01_PARA   Rf24l01Para;
static u8 ArrayPutFrame(ArrayBufType *Arraybuf,FrameBufType *Frame);
static u8 ArrayGetFrame(ArrayBufType *Arraybuf,FrameBufType *Frame);
static void InitArrayBuf(void);
static void _Buff_INIT(void);
 


//��ʼ��24L01��IO��
void NRF24L01_Init(void)
{
  RCC->APB2ENR|=1<<3;     //ʹ��PORTBʱ�� 	    

	GPIOB->CRH&=0XFFF000FF; 
	GPIOB->CRH|=0X00083300;	//PB10 11 12 ���� 	
  GPIOB->ODR|=7<<10;    	  //PB10 11 12 ����		 

	SPI2_Init();    		//��ʼ��SPI	  
	//���NRF���ص��޸�SPI������
 	SPI2->CR1&=~(1<<6); 	//SPI�豸ʧ��
	SPI2->CR1&=~(1<<1); 	//����ģʽ��SCKΪ0 CPOL=0
	SPI2->CR1&=~(1<<0); 	//���ݲ����ӵ�1��ʱ����ؿ�ʼ,CPHA=0  
	SPI2->CR1|=1<<6; 		  //SPI�豸ʹ��
	
	NRF24L01_CE=0; 			//ʹ��24L01
	NRF24L01_CSN=1;			//SPIƬѡȡ��
//  RF_EXTI_Init();	
	NRF24L01_RX_Mode();
	NRF24L01_Check();
	InitArrayBuf();
	NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ���
	NRF24L01_Write_Reg(FLUSH_TX,0xff);//���RX FIFO�Ĵ���
	NRF24L01_Write_Reg(WRITE_REG+STATUS,0xF0);
	_Buff_INIT();
//	memcpy(&Rf24l01Para.TX_ADDRESS,TX_ADDRESS,sizeof(TX_ADDRESS));
//	memcpy(&Rf24l01Para.RX_ADDRESS,RX_ADDRESS,sizeof(RX_ADDRESS));
	Rf24l01Para.Channel=InitChannel;
}
//���24L01�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��	
u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;
	SPI2_SetSpeed(SPI_SPEED_8); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   	 
	NRF24L01_Write_Buf(WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
	for(i=0;i<5;i++) 
	{
		if(buf[i]!=0XA5) 
				break;	
	}	
	if(i!=5)return 1;//���24L01����	
	return 0;		 //��⵽24L01
}	 	 
//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
	u8 status;	
	NRF24L01_CSN=0;                 //ʹ��SPI����
	status =SPI2_ReadWriteByte(reg);//���ͼĴ����� 
	SPI2_ReadWriteByte(value);      //д��Ĵ�����ֵ
	NRF24L01_CSN=1;                 //��ֹSPI����	   
	return(status);       			//����״ֵ̬
}
//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;	    
 	NRF24L01_CSN = 0;          //ʹ��SPI����		
	SPI2_ReadWriteByte(reg);   //���ͼĴ�����
	reg_val=SPI2_ReadWriteByte(0XFF);//��ȡ�Ĵ�������
	NRF24L01_CSN = 1;          //��ֹSPI����		    
	return(reg_val);           //����״ֵ̬
}	
//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ 
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
	u8 status,u8_ctr;	       
	NRF24L01_CSN = 0;           //ʹ��SPI����
	status=SPI2_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);//��������
	NRF24L01_CSN=1;       //�ر�SPI����
	return status;        //���ض�����״ֵ̬
}
//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	u8 status,u8_ctr;	    
 	NRF24L01_CSN = 0;          //ʹ��SPI����
	status = SPI2_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI2_ReadWriteByte(*pBuf++); //д������	 
	NRF24L01_CSN = 1;       //�ر�SPI����
	return status;          //���ض�����״ֵ̬
}				   
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
u8 NRF24L01_TxPacket(u8 *txbuf,long Tout)
{
	u8 sta,status=0xff;
	NRF24L01_TX_Mode(); 
	NRF24L01_CE=0;
  NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,FRAME_WIDTH);//д���ݵ�TX BUF  32���ֽ�
 	NRF24L01_CE=1;//��������	   
  while((NRF24L01_IRQ==1)&&(Tout>=0))
	{		 
		 Tout--;
		 delay_us(1);
	};//�ȴ��������
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_Write_Reg(WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(Tout<0) 
  {		
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
		status= TX_OUT;
	}	
	if(sta&MAX_TX)//�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
		status= MAX_TX; 
	}
	else if(sta&TX_OK)//�������
	{
		status= TX_OK;
	}	
	else 
	{
	  NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
	}	
	NRF24L01_RX_Mode();
	return status;//����ԭ����ʧ��
}
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 sta;		   
  if(Rf24l01Para.Status==STATUS_RX)
	{	 
		sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
		NRF24L01_Write_Reg(WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
		if(sta&RX_OK)//���յ�����
		{
			NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,FRAME_WIDTH);//��ȡ����
			NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ���       
			return 0; 
		}	   
  }		
	return 1;//û�յ��κ�����
}					    
//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
void NRF24L01_RX_Mode(void)
{
	NRF24L01_CE=0;	 
	NRF24L01_Write_Buf(WRITE_REG+RX_ADDR_P0,(u8*)Rf24l01Para.RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
	
	NRF24L01_Write_Reg(WRITE_REG+EN_AA,0x01);    //ʹ��ͨ��0���Զ�Ӧ��    
	NRF24L01_Write_Reg(WRITE_REG+EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ  	 
	NRF24L01_Write_Reg(WRITE_REG+RF_CH,Rf24l01Para.Channel);	     //����RFͨ��Ƶ��		  
	NRF24L01_Write_Reg(WRITE_REG+RX_PW_P0,FRAME_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 	    
	NRF24L01_Write_Reg(WRITE_REG+RF_SETUP,0x27);//����TX�������,0db����,2Mbps,���������濪��   
	NRF24L01_Write_Reg(WRITE_REG+CONFIG, 0x0f);//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
	NRF24L01_CE = 1; //CEΪ��,�������ģʽ 
	Rf24l01Para.Status=STATUS_RX;
}						 
//�ú�����ʼ��NRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
//CEΪ�ߴ���10us,����������.	 
void NRF24L01_TX_Mode(void)
{	    
	NRF24L01_CE=0;	    
	NRF24L01_Write_Buf(WRITE_REG+TX_ADDR,(u8*)Rf24l01Para.TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
	NRF24L01_Write_Buf(WRITE_REG+RX_ADDR_P0,(u8*)Rf24l01Para.TX_ADDRESS,TX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  

	NRF24L01_Write_Reg(WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
	NRF24L01_Write_Reg(WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
	NRF24L01_Write_Reg(WRITE_REG+SETUP_RETR,0x1F);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
	NRF24L01_Write_Reg(WRITE_REG+RF_CH,Rf24l01Para.Channel);       //����RFͨ��Ϊ40
	NRF24L01_Write_Reg(WRITE_REG+RF_SETUP,0x27);//0x07);  //����TX�������,0db����,2Mbps,���������濪��   
	NRF24L01_Write_Reg(WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
    //0x4e ���Խ��� �޷����뷢��  0x5e Ҳ����  0x7e���� 3eҲ���� 
	NRF24L01_CE=1;//CEΪ��,10us����������
	Rf24l01Para.Status=STATUS_TX;
}

//�ⲿ�ж�0�������
void EXTI15_10_IRQHandler(void)
{
	FrameBufType TempRxBuf;
  if(NRF24L01_RxPacket(TempRxBuf.FrameData)==0)	 
	{				 
		ArrayPutFrame(&RxBufType,&TempRxBuf);
	}		 
	EXTI->PR=1<<12;  //���LINE12�ϵ��жϱ�־λ  
}

void RX_Check_TASK(void)
{
	 u8 sta;
	 FrameBufType TempRxBuf;
   if(NRF24L01_RxPacket(TempRxBuf.FrameData)==0)
	    ArrayPutFrame(&RxBufType,&TempRxBuf);
}	

void RF_EXTI_Init(void)
{
	Ex_NVIC_Config(GPIO_B,12,FTIR); 			//�����ش���
	MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,2);	//��ռ2�������ȼ�3����2   
}

static void InitArrayBuf(void)
{
	memset(&TxBufType,0,sizeof(TxBufType));
	memset(&RxBufType,0,sizeof(TxBufType));
	memset(&RetryType,0,sizeof(RetryType));
	TxBufType.ArrayBufInPtr=&TxBufType.ArrayBuf[0];
	TxBufType.ArrayBufOutPtr=&TxBufType.ArrayBuf[0];
	TxBufType.ArrayBufCtr=0;

	RxBufType.ArrayBufInPtr=&RxBufType.ArrayBuf[0];
	RxBufType.ArrayBufOutPtr=&RxBufType.ArrayBuf[0];
	RxBufType.ArrayBufCtr=0;
	
	RetryType.ArrayBufInPtr=&RetryType.ArrayBuf[0];
	RetryType.ArrayBufOutPtr=&RetryType.ArrayBuf[0];
	RetryType.ArrayBufCtr=0;
}

static u16 ArrayGetBufCtr(ArrayBufType *pbuf)
{
	return(pbuf->ArrayBufCtr);       
}

/***********************************************************************************
*                              ����д����л�����
***********************************************************************************/

static u8 ArrayPutFrame(ArrayBufType *Arraybuf,FrameBufType *Frame)
{   
	 if(Arraybuf->ArrayBufCtr<ArrayBufSize)
	 {
			 Arraybuf->ArrayBufCtr++;
			 memcpy(Arraybuf->ArrayBufInPtr->FrameData,Frame->FrameData,FRAME_WIDTH);
			 Arraybuf->ArrayBufInPtr->DLC=Frame->DLC;
			 Arraybuf->ArrayBufInPtr->MSGID=Frame->MSGID;
			 Arraybuf->ArrayBufInPtr++;
			 if(Arraybuf->ArrayBufInPtr==&Arraybuf->ArrayBuf[ArrayBufSize])
					 Arraybuf->ArrayBufInPtr=&Arraybuf->ArrayBuf[0]; 
			 return ABUF_NO_ERR;
			 
	 }
	 return ABUF_TXBUFFULL_ERR;
}
            
    
/***********************************************************************************
*                              ȡ�����л���������
***********************************************************************************/
static u8 ArrayGetFrame(ArrayBufType *Arraybuf,FrameBufType *Frame)
{             
   if(ArrayGetBufCtr(Arraybuf)<1)   
   {
       return ABUF_RXBUFLESS_ERR;                 
   }
	 Arraybuf->ArrayBufCtr--;
	 memcpy(Frame->FrameData,Arraybuf->ArrayBufOutPtr->FrameData,FRAME_WIDTH);
	 Frame->DLC=Arraybuf->ArrayBufOutPtr->DLC;
	 Frame->MSGID=Arraybuf->ArrayBufOutPtr->MSGID;
	 Arraybuf->ArrayBufOutPtr++;
	 if(Arraybuf->ArrayBufOutPtr==&Arraybuf->ArrayBuf[ArrayBufSize])
			Arraybuf->ArrayBufOutPtr=&Arraybuf->ArrayBuf[0];   
   return ABUF_NO_ERR;
}

void TASK_NRF24L01_RESET(u8 Txflag)
{
   if(Txflag==TX_OK)
	 {
	    Rf24l01Para.CrashCnt=0;
	 }	 
	 else
	 {
			Rf24l01Para.CrashCnt++;
		  if(Rf24l01Para.CrashCnt>MaxCrashCnt)
			{
			   	NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ���
					NRF24L01_Write_Reg(FLUSH_TX,0xff);//���RX FIFO�Ĵ���
					NRF24L01_Write_Reg(WRITE_REG+STATUS,0xF0);
			    Rf24l01Para.CrashCnt=0;
			}	
	 }	 
}	

u8 TASK_NRF24L01_AutoSend(void)
{
	 FrameBufType Tmpbuf; u8 sta;
	 sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ	
   if((sta&TX_FIFO_FULL)==0)
   {		 
		 if(ArrayGetFrame(&TxBufType,&Tmpbuf)==ABUF_NO_ERR)
		 {
         Rf24l01Para.TX_ADDRESS[4]=Tmpbuf.MSGID&0xff;
		     Rf24l01Para.Txflag= NRF24L01_TxPacket(Tmpbuf.FrameData,MaxTout);//5500�ﵽ��������ʱ�˳�
		     TASK_NRF24L01_RESET(Rf24l01Para.Txflag);//����Ĵ���
		 }	
   }
	 return 1;
}	

/* txbuf:��������ָ�� TX_NO_ERR:�ɹ� TX_BUFFULL:���ͻ������� */
u8 NRF24L01_TxWithArray(u8 *txbuf,u8 Txddr)
{
	 FrameBufType Tmpbuf;
	 memcpy(Tmpbuf.FrameData,txbuf,FRAME_WIDTH);
	 Tmpbuf.MSGID=Txddr;
	 if(ArrayPutFrame(&TxBufType,&Tmpbuf)==ABUF_NO_ERR)
	 {
			return TX_NO_ERR;
	 }		 
	 else
		  return TX_BUFFULL;
}

/* rxbuf: ��������ָ�� RX_NO_ERR:�ɹ� RX_BUFLESS:���ͻ������� */
u8 NRF24L01_RxWithArray(u8 *rxbuf)
{
	 FrameBufType Tmpbuf;	 
   if(ArrayGetFrame(&RxBufType,&Tmpbuf)==ABUF_NO_ERR)
	 {	 
		 memcpy(rxbuf,Tmpbuf.FrameData,FRAME_WIDTH);
		 
		 return RX_NO_ERR;
	 }	 
	 else 
		 return RX_BUFLESS;
}

static void _Buff_INIT(void)//����
{
   memset(&Rf24l01Para,0,sizeof(Rf24l01Para));
}	

void NRF24L01_Fifo(u8 Flag)//�Ƚ��ȳ��˲���
{
   Rf24l01Para.TxflagFifo[Rf24l01Para.Addnum]=Flag;
	 Rf24l01Para.Addnum++;
	 if(Rf24l01Para.Addnum>=CountPriod)
	 {
	    Rf24l01Para.Addnum=0;
	 }	 
	 switch(Rf24l01Para.TxflagFifo[Rf24l01Para.Addnum])
	 {
	    case TX_OK:
				 Rf24l01Para.TxOkCnt--;
			   break;
		  case TX_OUT:
		  case MAX_TX:
				 Rf24l01Para.TxErrCnt--;
			   break;
	 }	 
}	

void NRF24L01_SignalCount(void)
{
	 u32 TmpCNT=0;
	 switch(Rf24l01Para.Txflag)
	 {
		 case TX_OK:
       Rf24l01Para.TxOkCnt++;
		   NRF24L01_Fifo(Rf24l01Para.Txflag);
		   Rf24l01Para.Txflag=0;
		   TmpCNT=Rf24l01Para.TxOkCnt+Rf24l01Para.TxErrCnt;
		   Rf24l01Para.SuccRatio=(long long)Rf24l01Para.TxOkCnt*100/TmpCNT;
		   break;
		 case TX_OUT://
		 case MAX_TX:
			 Rf24l01Para.TxErrCnt++;
       NRF24L01_Fifo(Rf24l01Para.Txflag);
		   Rf24l01Para.Txflag=0;
			 TmpCNT=Rf24l01Para.TxOkCnt+Rf24l01Para.TxErrCnt;
		   Rf24l01Para.SuccRatio=(long long)Rf24l01Para.TxOkCnt*100/TmpCNT;
		   break;
	 }	 
}	

u16 NRF24L01_ReadSignal(void)
{
   return Rf24l01Para.SuccRatio;
}	
u8 NRF24L01_ReadSourAddr(void)
{
   return Rf24l01Para.RX_ADDRESS[4];
}	
u8 NRF24L01_ReadTarAddr(void)
{
   return Rf24l01Para.TX_ADDRESS[4];
}	

void NRF24L01_ClearSignal(void)
{
   Rf24l01Para.TxOkCnt=0;
	 Rf24l01Para.TxErrCnt=0;
	 Rf24l01Para.Txflag=0;
	 Rf24l01Para.SuccRatio=0;
   Rf24l01Para.ChannelChange=0;
	 Rf24l01Para.Addnum=0;
	 memset(Rf24l01Para.TxflagFifo,0,sizeof(Rf24l01Para.TxflagFifo));
}	
void RfChannelChange(void)//�޸�Ƶ�ʣ�һ��+5  ��Ҫһ��ʱ����ʱ���
{
/*    if(Rf24l01Para.SuccRatio<=60)
    {     
        Rf24l01Para.ChannelChange=Rf24l01Para.ChannelChange+5;
        _SET_CommFreq(Rf24l01Para.ChannelChange,Hostaddr) ;        
    }
    else
    {
        NRF24L01_ClearSignal();
    }
    if(TmpRxData[3]==RFCMD_SetCommFreq)
    {
        NRF24L01_ModifyChannel(Rf24l01Para.ChannelChange+InitChannel);
    }*/
}


void NRF24L01_ModifyChannel(u8 Channel)//����ͨ��Ƶ��
{
//     Rf24l01Para.Channel=Channel;
	 NRF24L01_ClearSignal();
}	

void NRF24L01_ModifyTxAddr(u8 Txaddr)
{
	 Rf24l01Para.TX_ADDRESS[4]=Txaddr;
}
void NRF24L01_ModifyRxAddr(u8 Rxaddr)
{
	 Rf24l01Para.RX_ADDRESS[4]=Rxaddr;
}	

void NRF24L01_ModifyDevID(u8* DevID)//ɨ��ȷ���豸ID
{
	 memcpy(Rf24l01Para.TX_ADDRESS,DevID,(TX_ADR_WIDTH-1));
	 memcpy(Rf24l01Para.RX_ADDRESS,DevID,(RX_ADR_WIDTH-1));
}


void NRF24L01_TASK1MS(void)
{
	 RX_Check_TASK();
//	 NRF24L01_SignalCount();
}	

void NRF24L01_TASK100MS(void)//�к��˲��������е����ݣ�����0
{
     NRF24L01_Fifo(0);
}	



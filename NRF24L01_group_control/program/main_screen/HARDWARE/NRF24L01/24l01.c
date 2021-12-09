#include "24l01.h"
#include "spi.h"
#include <string.h>
#include "lib.h"		
const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x12,0x00}; //发送地址 303055668
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x12,0x01}; //发送地址
ArrayBufType TxBufType,RxBufType,RetryType;
RF24L01_PARA   Rf24l01Para;
static u8 ArrayPutFrame(ArrayBufType *Arraybuf,FrameBufType *Frame);
static u8 ArrayGetFrame(ArrayBufType *Arraybuf,FrameBufType *Frame);
static void InitArrayBuf(void);
static void _Buff_INIT(void);
 


//初始化24L01的IO口
void NRF24L01_Init(void)
{
  RCC->APB2ENR|=1<<3;     //使能PORTB时钟 	    

	GPIOB->CRH&=0XFFF000FF; 
	GPIOB->CRH|=0X00083300;	//PB10 11 12 推挽 	
  GPIOB->ODR|=7<<10;    	  //PB10 11 12 上拉		 

	SPI2_Init();    		//初始化SPI	  
	//针对NRF的特点修改SPI的设置
 	SPI2->CR1&=~(1<<6); 	//SPI设备失能
	SPI2->CR1&=~(1<<1); 	//空闲模式下SCK为0 CPOL=0
	SPI2->CR1&=~(1<<0); 	//数据采样从第1个时间边沿开始,CPHA=0  
	SPI2->CR1|=1<<6; 		  //SPI设备使能
	
	NRF24L01_CE=0; 			//使能24L01
	NRF24L01_CSN=1;			//SPI片选取消
//  RF_EXTI_Init();	
	NRF24L01_RX_Mode();
	NRF24L01_Check();
	InitArrayBuf();
	NRF24L01_Write_Reg(FLUSH_RX,0xff);//清除RX FIFO寄存器
	NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除RX FIFO寄存器
	NRF24L01_Write_Reg(WRITE_REG+STATUS,0xF0);
	_Buff_INIT();
//	memcpy(&Rf24l01Para.TX_ADDRESS,TX_ADDRESS,sizeof(TX_ADDRESS));
//	memcpy(&Rf24l01Para.RX_ADDRESS,RX_ADDRESS,sizeof(RX_ADDRESS));
	Rf24l01Para.Channel=InitChannel;
}
//检测24L01是否存在
//返回值:0，成功;1，失败	
u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;
	SPI2_SetSpeed(SPI_SPEED_8); //spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   	 
	NRF24L01_Write_Buf(WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址  
	for(i=0;i<5;i++) 
	{
		if(buf[i]!=0XA5) 
				break;	
	}	
	if(i!=5)return 1;//检测24L01错误	
	return 0;		 //检测到24L01
}	 	 
//SPI写寄存器
//reg:指定寄存器地址
//value:写入的值
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
	u8 status;	
	NRF24L01_CSN=0;                 //使能SPI传输
	status =SPI2_ReadWriteByte(reg);//发送寄存器号 
	SPI2_ReadWriteByte(value);      //写入寄存器的值
	NRF24L01_CSN=1;                 //禁止SPI传输	   
	return(status);       			//返回状态值
}
//读取SPI寄存器值
//reg:要读的寄存器
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;	    
 	NRF24L01_CSN = 0;          //使能SPI传输		
	SPI2_ReadWriteByte(reg);   //发送寄存器号
	reg_val=SPI2_ReadWriteByte(0XFF);//读取寄存器内容
	NRF24L01_CSN = 1;          //禁止SPI传输		    
	return(reg_val);           //返回状态值
}	
//在指定位置读出指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值 
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
	u8 status,u8_ctr;	       
	NRF24L01_CSN = 0;           //使能SPI传输
	status=SPI2_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);//读出数据
	NRF24L01_CSN=1;       //关闭SPI传输
	return status;        //返回读到的状态值
}
//在指定位置写指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	u8 status,u8_ctr;	    
 	NRF24L01_CSN = 0;          //使能SPI传输
	status = SPI2_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值
	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI2_ReadWriteByte(*pBuf++); //写入数据	 
	NRF24L01_CSN = 1;       //关闭SPI传输
	return status;          //返回读到的状态值
}				   
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:发送完成状况
u8 NRF24L01_TxPacket(u8 *txbuf,long Tout)
{
	u8 sta,status=0xff;
	NRF24L01_TX_Mode(); 
	NRF24L01_CE=0;
  NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,FRAME_WIDTH);//写数据到TX BUF  32个字节
 	NRF24L01_CE=1;//启动发送	   
  while((NRF24L01_IRQ==1)&&(Tout>=0))
	{		 
		 Tout--;
		 delay_us(1);
	};//等待发送完成
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值	   
	NRF24L01_Write_Reg(WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(Tout<0) 
  {		
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
		status= TX_OUT;
	}	
	if(sta&MAX_TX)//达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
		status= MAX_TX; 
	}
	else if(sta&TX_OK)//发送完成
	{
		status= TX_OK;
	}	
	else 
	{
	  NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
	}	
	NRF24L01_RX_Mode();
	return status;//其他原因发送失败
}
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:0，接收完成；其他，错误代码
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 sta;		   
  if(Rf24l01Para.Status==STATUS_RX)
	{	 
		sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值    	 
		NRF24L01_Write_Reg(WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
		if(sta&RX_OK)//接收到数据
		{
			NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,FRAME_WIDTH);//读取数据
			NRF24L01_Write_Reg(FLUSH_RX,0xff);//清除RX FIFO寄存器       
			return 0; 
		}	   
  }		
	return 1;//没收到任何数据
}					    
//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了		   
void NRF24L01_RX_Mode(void)
{
	NRF24L01_CE=0;	 
	NRF24L01_Write_Buf(WRITE_REG+RX_ADDR_P0,(u8*)Rf24l01Para.RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
	
	NRF24L01_Write_Reg(WRITE_REG+EN_AA,0x01);    //使能通道0的自动应答    
	NRF24L01_Write_Reg(WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址  	 
	NRF24L01_Write_Reg(WRITE_REG+RF_CH,Rf24l01Para.Channel);	     //设置RF通信频率		  
	NRF24L01_Write_Reg(WRITE_REG+RX_PW_P0,FRAME_WIDTH);//选择通道0的有效数据宽度 	    
	NRF24L01_Write_Reg(WRITE_REG+RF_SETUP,0x27);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
	NRF24L01_Write_Reg(WRITE_REG+CONFIG, 0x0f);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
	NRF24L01_CE = 1; //CE为高,进入接收模式 
	Rf24l01Para.Status=STATUS_RX;
}						 
//该函数初始化NRF24L01到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
//PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了		   
//CE为高大于10us,则启动发送.	 
void NRF24L01_TX_Mode(void)
{	    
	NRF24L01_CE=0;	    
	NRF24L01_Write_Buf(WRITE_REG+TX_ADDR,(u8*)Rf24l01Para.TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
	NRF24L01_Write_Buf(WRITE_REG+RX_ADDR_P0,(u8*)Rf24l01Para.TX_ADDRESS,TX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  

	NRF24L01_Write_Reg(WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
	NRF24L01_Write_Reg(WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
	NRF24L01_Write_Reg(WRITE_REG+SETUP_RETR,0x1F);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
	NRF24L01_Write_Reg(WRITE_REG+RF_CH,Rf24l01Para.Channel);       //设置RF通道为40
	NRF24L01_Write_Reg(WRITE_REG+RF_SETUP,0x27);//0x07);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
	NRF24L01_Write_Reg(WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
    //0x4e 可以接收 无法进入发送  0x5e 也不行  0x7e不行 3e也不行 
	NRF24L01_CE=1;//CE为高,10us后启动发送
	Rf24l01Para.Status=STATUS_TX;
}

//外部中断0服务程序
void EXTI15_10_IRQHandler(void)
{
	FrameBufType TempRxBuf;
  if(NRF24L01_RxPacket(TempRxBuf.FrameData)==0)	 
	{				 
		ArrayPutFrame(&RxBufType,&TempRxBuf);
	}		 
	EXTI->PR=1<<12;  //清除LINE12上的中断标志位  
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
	Ex_NVIC_Config(GPIO_B,12,FTIR); 			//上升沿触发
	MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,2);	//抢占2，子优先级3，组2   
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
*                              数据写入队列缓冲区
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
*                              取出队列缓冲区数据
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
			   	NRF24L01_Write_Reg(FLUSH_RX,0xff);//清除RX FIFO寄存器
					NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除RX FIFO寄存器
					NRF24L01_Write_Reg(WRITE_REG+STATUS,0xF0);
			    Rf24l01Para.CrashCnt=0;
			}	
	 }	 
}	

u8 TASK_NRF24L01_AutoSend(void)
{
	 FrameBufType Tmpbuf; u8 sta;
	 sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值	
   if((sta&TX_FIFO_FULL)==0)
   {		 
		 if(ArrayGetFrame(&TxBufType,&Tmpbuf)==ABUF_NO_ERR)
		 {
         Rf24l01Para.TX_ADDRESS[4]=Tmpbuf.MSGID&0xff;
		     Rf24l01Para.Txflag= NRF24L01_TxPacket(Tmpbuf.FrameData,MaxTout);//5500达到最大次数计时退出
		     TASK_NRF24L01_RESET(Rf24l01Para.Txflag);//清除寄存器
		 }	
   }
	 return 1;
}	

/* txbuf:发送数据指针 TX_NO_ERR:成功 TX_BUFFULL:发送缓冲区满 */
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

/* rxbuf: 接收数据指针 RX_NO_ERR:成功 RX_BUFLESS:发送缓冲区满 */
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

static void _Buff_INIT(void)//清零
{
   memset(&Rf24l01Para,0,sizeof(Rf24l01Para));
}	

void NRF24L01_Fifo(u8 Flag)//先进先出滤波器
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
void RfChannelChange(void)//修改频率，一次+5  需要一段时间延时检测
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


void NRF24L01_ModifyChannel(u8 Channel)//设置通道频率
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

void NRF24L01_ModifyDevID(u8* DevID)//扫码确定设备ID
{
	 memcpy(Rf24l01Para.TX_ADDRESS,DevID,(TX_ADR_WIDTH-1));
	 memcpy(Rf24l01Para.RX_ADDRESS,DevID,(RX_ADR_WIDTH-1));
}


void NRF24L01_TASK1MS(void)
{
	 RX_Check_TASK();
//	 NRF24L01_SignalCount();
}	

void NRF24L01_TASK100MS(void)//中和滤波器队列中的数据，放入0
{
     NRF24L01_Fifo(0);
}	



#include "lib.h"


u8 TempRx[2] ={0};
short AngleRx[2]={0};
u8 ScanDataLen[3]={0};
u8 ScanDataSum[4]={0};
u8 TmpRxData[MAX_DataWidth];
CONN_CTRL      ConnCtrl;
UPDATA_CTRL    UpdataCtrl;
TRANSMIT_CTRL  CodeTransCtrl,BinTransCtrl;

u8 MEM_Databuf[MAX_PARA_NUM*2];

void _CommWatchDog_Feed(u8 SourAddr);
void _CommWatchDog_TASK(void);
void _Timer_Set(u32* Timer,u32 nms);


void _SYSPARA_INIT(void)
{
   u16 INIT_FLAG=0; u32 INIT_DEVID=123456789;
	 INIT_FLAG=INIT_OK;
	 ConnCtrl.Addr=0x00;
	 ConnCtrl.Freq=100;
	 ConnCtrl.Cidnum=1;
	 memset(MEM_Databuf,0,sizeof(MEM_Databuf));
	 memcpy(MEM_Databuf+ADDR_INIT,&INIT_FLAG,2);
	 memcpy(MEM_Databuf+ADDR_ADDR,&ConnCtrl.Addr,1);
	 memcpy(MEM_Databuf+ADDR_FREQ,&ConnCtrl.Freq,1);
	 memcpy(MEM_Databuf+ADDR_Cidnum,&ConnCtrl.Cidnum,1);
	 memcpy(MEM_Databuf+ADDR_DEVID,&INIT_DEVID,4);
	 STMFLASH_Write(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);
}	

u8 _UAPP_Comm_INIT(void)
{
	 u16 INIT_FLAG=0;
   memset(&ConnCtrl,0,sizeof(ConnCtrl));//从机地址频率
	 memset(&UpdataCtrl,0,sizeof(UpdataCtrl));//接收到更新
	 memset(&CodeTransCtrl,0,sizeof(CodeTransCtrl));
	 memset(&BinTransCtrl,0,sizeof(BinTransCtrl));
	 //FLASH读取各项参数 Cidnum,Addr,Freq,ValidDev,UpdataCtrl.Datalen,UpdataCtrl.Checksum 
   STMFLASH_Read(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);
	 memcpy(&INIT_FLAG,MEM_Databuf+ADDR_INIT,2);
	 memcpy(&ConnCtrl.Freq,MEM_Databuf+ADDR_FREQ,1);
	 memcpy(&ConnCtrl.ValidDev,MEM_Databuf+ADDR_ValidDev,1);
	 memcpy(&ConnCtrl.Cidnum,MEM_Databuf+ADDR_Cidnum,1);
	 ConnCtrl.Addr=0x00;
	 if(INIT_FLAG==INIT_OK)
	 {	 

	 }
	 else
	 {
      _SYSPARA_INIT();
	 }	 	
	 NRF24L01_ModifyRxAddr(ConnCtrl.Addr);
	 NRF24L01_ModifyDevID(MEM_Databuf+ADDR_DEVID);
	 NRF24L01_ModifyChannel(ConnCtrl.Freq);
	 NRF24L01_Init();
   return 0;	
}	

u16 _ChecksumTask(u8* buf,u8 len)
{
	 u8 cnt; u32 Checksum=0x10000;
   for(cnt=0;cnt<len;cnt++)
	 {
	    Checksum-=buf[cnt];
	 }
	 return Checksum;
}

u8 _FrameChecksum(u8 *Frame)
{
	 u8 DLC; u32 Checksum,Countsum;
   DLC=Frame[0];
	 memcpy(&Checksum,&Frame[DLC],2);
	 Countsum=_ChecksumTask(Frame,DLC);
	 if(Checksum==Countsum)
	     return 1;
   else
       return 0;		 
}

void _SEND_ACK(u8 Cmd,u8 Para,u8 Taraddr)
{
   u8 TmpDbuf[32]={0},cnt,len=0x05; u32 Checksum;
	 if(ConnCtrl.OnLineSta[Taraddr]==STA_ON)
	 {	 
		 TmpDbuf[0]=len;
		 TmpDbuf[1]=NRF24L01_ReadSourAddr();
		 TmpDbuf[2]=RFCMD_SendAck;
		 TmpDbuf[3]=Cmd;
		 TmpDbuf[4]=Para;
		 Checksum=_ChecksumTask(TmpDbuf,TmpDbuf[0]);
		 memcpy(&TmpDbuf[len],&Checksum,2);
		 NRF24L01_TxWithArray(TmpDbuf,Taraddr);
	 }	 
}	

static void _SEND_HartBeat(void)
{
   u8 TmpDbuf[32]={0},cnt,len=0x07; u32 Checksum;
	 ConnCtrl.TimerS1++;
	 if(ConnCtrl.DataTransMode==0)
	 {	 
		 for(cnt=0;cnt<MaxDevnum;cnt++)
		 {
			 if(ConnCtrl.OnLineSta[cnt]==STA_ON)
			 {	 
				 TmpDbuf[0]=len;
				 TmpDbuf[1]=NRF24L01_ReadSourAddr();
				 TmpDbuf[2]=RFCMD_Heartbeat;
				 TmpDbuf[3]=ConnCtrl.TimerS1&0xff;
				 Checksum=_ChecksumTask(TmpDbuf,TmpDbuf[0]);
				 memcpy(&TmpDbuf[len],&Checksum,2);
				 NRF24L01_TxWithArray(TmpDbuf,cnt);
			 }	 
		 }
   }
}	

void _SET_NewAddr(u8 NewAddr,u8 Taraddr)
{
	 if(ConnCtrl.DataTransMode==0)
	 {	 
		 u8 TmpDbuf[32]={0},cnt,len=0x04; u32 Checksum;
		 TmpDbuf[0]=len;
		 TmpDbuf[1]=NRF24L01_ReadSourAddr();
		 TmpDbuf[2]=RFCMD_SetSourAddr;
		 TmpDbuf[3]=NewAddr;
		 Checksum=_ChecksumTask(TmpDbuf,TmpDbuf[0]);
		 memcpy(&TmpDbuf[len],&Checksum,2);
		 NRF24L01_TxWithArray(TmpDbuf,Taraddr);	 
   }
}	





u8 _ReqConn_Task(u8 SourAddr)
{
   if(SourAddr>=MaxDevnum)
	 {
	    _SET_NewAddr(ConnCtrl.Cidnum,SourAddr);
	 }	 
	 else 
	 {
		  if(ConnCtrl.DataTransMode==0)
			{	
				if(ConnCtrl.TimerS4==0)
				{	
				  ConnCtrl.OnLineSta[SourAddr]=STA_ON;
          ConnCtrl.OfflineCnt[SourAddr]=0;
					UpdataCtrl.Addr=SourAddr;
					_Timer_Set(&ConnCtrl.TimerS4,100);
					ConnCtrl.DataTransMode=Mode_BinWait;
				}	
				return ACK_SUCC;
		  }
			else return 0;
	 }
   return ACK_ERR;	 
}	

u8 _SetCommFreq_Task(u8 Freq,u16 Delayms)
{
   if(Freq<=MaxFreq)
	 {
		  ConnCtrl.Freq=Freq;
		  ConnCtrl.TimerS2=CpuTimerms+Delayms;
	    return ACK_SUCC;
	 }	
   return ACK_ERR;	 
}

void _Timer_Set(u32* Timer,u32 nms)
{
   *Timer=CpuTimerms+nms;
}

void _Timer_Task(void)
{
   if((ConnCtrl.TimerS2<CpuTimerms)&&(ConnCtrl.TimerS2!=0))
	 {
	     NRF24L01_ModifyChannel(ConnCtrl.Freq);
		                                    //调用存储函数存储频率
       memcpy(MEM_Databuf+ADDR_FREQ,&(ConnCtrl.Freq),1);
       STMFLASH_Write(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);                 
		   ConnCtrl.TimerS2=0;
	 }
   if((ConnCtrl.TimerS3<CpuTimerms)&&(ConnCtrl.TimerS3!=0))
	 {
       ConnCtrl.DataTransMode=0;  
		   ConnCtrl.TimerS3=0;
	 }	
   if((ConnCtrl.TimerS4<CpuTimerms)&&(ConnCtrl.TimerS4!=0))
	 {
       memcpy(&BinTransCtrl.Datalen,MEM_Databuf+ADDR_BinLen2,4);
		   memcpy(&BinTransCtrl.Datasum,MEM_Databuf+ADDR_BinSum2,4);
      // _SEND_BinApply(BinTransCtrl.Datalen,BinTransCtrl.Datasum,UpdataCtrl.Addr,MaxRetryCnt);
		   ConnCtrl.TimerS4=0;
		   _Timer_Set(&ConnCtrl.TimerS3,TransDelayms);
	 }	 	 
}	

void _FrameAnalysis(void)
{
   u8 Addr,CMD,CMDack,AckResult,Ackflag=0;
   if(NRF24L01_RxWithArray(TmpRxData)==RX_NO_ERR)
			{
				 CMD=TmpRxData[2];
				 Addr=TmpRxData[1];
	 {
			if(_FrameChecksum(TmpRxData))
				 switch(CMD)
				 {
						 case RFCMD_SendAck://0x81
							 CMDack=TmpRxData[3];
							 AckResult=TmpRxData[4];
							 switch(CMDack)
							 {
									case RFCMD_SetSourAddr://0x42
										 if(Addr==ConnCtrl.Cidnum)
										 {	 
											  ConnCtrl.Cidnum++;
											  //调用存储函数存储Cidnum
												memcpy(MEM_Databuf+ADDR_Cidnum,&(ConnCtrl.Cidnum),1);
												STMFLASH_Write(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);
										 }	 
									   break;

							 }	 
						   break;
						 case RFCMD_Heartbeat://0x80 
								 _CommWatchDog_Feed(Addr);
								 break;
						 case RFCMD_ReqConn://0x01  接收到从机发送的请求连接指令后，发送回去进行配对
								 Ackflag=_ReqConn_Task(Addr);
								 break;	

						 case RFCMD_SetCommFreq://变更通信频率
								 Ackflag=_SetCommFreq_Task(TmpRxData[3],100);
								 break;
						 case RFCMD_SensorVal://接收到温度角度 0x83

								 //_SensorVal_Analysis(TmpRxData,Addr);
//								 _SensorVal_Analysis(TmpRxData,0x00);
								 break;	

	
				 }  
				 if(Ackflag!=0) 
            _SEND_ACK(CMD,Ackflag,Addr);					 
      }	
	 }	 
}	

short _Read_OnlineSta(void)
{
   return ConnCtrl.OnLinenum;
}	


void _Comm_TASK1000ms(void)  //1HZ
{
     _SEND_HartBeat();
	 _CommWatchDog_TASK();

}	

void _Comm_TASK1ms(void) //1000HZ
{
	 _FrameAnalysis();   	
}	

void _Comm_TASK10ms(void) //100HZ
{
	 _Timer_Task();
}	

void _CommWatchDog_TASK(void)
{
   u8 cnt;
	 ConnCtrl.OnLinenum=MaxDevnum;
	 for(cnt=0;cnt<MaxDevnum;cnt++)
	 {
	    ConnCtrl.OfflineCnt[cnt]++;
		  if(ConnCtrl.OfflineCnt[cnt]>MaxOfflinecnt)
			{
				 ConnCtrl.OnLinenum--;
			   ConnCtrl.OnLineSta[cnt]=STA_OFF;
			}	
	 } 
}	

void _CommWatchDog_Feed(u8 SourAddr)
{
   if(SourAddr<MaxDevnum)
	 {
	    ConnCtrl.OfflineCnt[SourAddr]=0;
	 }	 
}	

void _Comm_TASK5ms(void)
{
   TASK_NRF24L01_AutoSend();
//	 NRF24L01_SignalCount();
}	

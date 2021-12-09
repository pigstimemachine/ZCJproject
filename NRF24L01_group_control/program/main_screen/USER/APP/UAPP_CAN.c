#include "lib.h"

HEARTBEAT_DATA      HeartBeatData;
FEEDBACK_DATA       FeedBackData;
CAN_STATUS          CanStatus;
//ECANID_INTERNAL     ReceiveID;
//DataBufType*        Codedata;
TRANSMIT_CTRL       CAN_CodeTrans,CAN_BinTrans;
static void _CommWatchDog_TASK(void);
static void _CommWatchDog_Feed(void);
#define ADDR_SEL(x) (x==LocalHostAddr1? ADDR_BIN1_OFFSET:ADDR_BIN2_OFFSET)
#define ERR_COMM(x) (x>0? 0:FAIL_COMM)

//u8 TmpFlashbuf[STM_SECTOR_SIZE];





u8 _Rx_ResetDevid(u8* buf)
{
   u32 TmpKey=0;
	 memcpy(&TmpKey,buf+5,3);
	 if(TmpKey==ResetKey)
	 {
	    NRF24L01_ModifyDevID(buf+1);
		  memcpy(MEM_Databuf+ADDR_DEVID,buf+1,(TX_ADR_WIDTH-1));
		  STMFLASH_Write(STM32_FLASH_BASE+ADDR_PARA_OFFSET,(u16*)MEM_Databuf,MAX_PARA_NUM);  
		  return ACK_SUCC;
	 }
	 return ACK_ERR;
}

static void _CommWatchDog_TASK(void)
{
		CanStatus.OffLineCnt++;
		if(CanStatus.OffLineCnt>MaxOfflinecnt)
		{
			 CanStatus.OnLineSta=STA_OFF;
		}	 
}	

static void _CommWatchDog_Feed(void)
{
		CanStatus.OffLineCnt=0;
    CanStatus.OnLineSta=STA_ON;
}	


void CAN_TASK1ms(void)  //1000HZ
{

}

void CAN_TASK1000ms(void)  //1HZ
{


}

void _CAN_TASK10ms(void) //100HZ
{
 
}

void _CAN_TASK250ms(void) //3HZ
{

}

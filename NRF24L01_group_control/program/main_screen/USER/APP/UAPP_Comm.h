#ifndef UAPP_COMM_H
#define UAPP_COMM_H
#include "sys.h"

#define CpuTimerms        CpuTimer0.InterruptCount      

#define MAX_DataWidth  FRAME_WIDTH

#define STA_ON          1

#define STA1_ON         1
#define STA2_ON         2
#define STA3_ON         3

#define STA_OFF        0


#define ACK_SUCC       0xaa
#define ACK_ERR        0xee
#define ACK_END        0x1aa


#define Hostaddr       0x00


#define RFCMD_Heartbeat        0x80
#define RFCMD_SendAck          0x81
#define RFCMD_SendBin          0x40
#define RFCMD_SendBinData      0x41
#define RFCMD_SetSourAddr      0x42

#define RFCMD_ReqConn          0x01
#define RFCMD_SetDevID         0x32
#define RFCMD_SetCommFreq      0x33
#define RFCMD_SendCode         0x34
#define RFCMD_SendCodeData     0x35

#define RFCMD_SensorVal        0x83

#define MaxDatabyte            12

#define MaxOfflinecnt          10

#define MaxFreq                120

#define SEND_OK                0x01
#define SEND_ERR               0x02

#define MAX_BOOT_NUM        10240
#define MAX_APP_NUM         25600
#define MAX_BIN1_NUM        MAX_APP_NUM
#define MAX_BIN2_NUM        MAX_APP_NUM

#define MAX_PARA_NUM        64

#define STM32_FLASH_BASE    0x08000000 	//STM32 FLASH的起始地址
#define ADDR_APP_OFFSET     MAX_BOOT_NUM
#define ADDR_BIN1_OFFSET    MAX_BOOT_NUM+MAX_APP_NUM
#define ADDR_BIN2_OFFSET    MAX_BOOT_NUM+MAX_APP_NUM+MAX_BIN1_NUM
#define ADDR_PARA_OFFSET    MAX_BOOT_NUM+MAX_APP_NUM+MAX_BIN1_NUM+MAX_BIN2_NUM


#define ADDR_INIT           0x00
#define ADDR_DEVID          0x02
#define ADDR_ADDR           0x06
#define ADDR_FREQ           0x07
#define ADDR_ValidDev       0x08
#define ADDR_BinLen1        0x09
#define ADDR_BinSum1        0x0d
#define ADDR_Updata1        0x11
#define ADDR_BinLen2        0x13
#define ADDR_BinSum2        0x17
#define ADDR_Updata2        0x1b
#define ADDR_Cidnum         0x1d



#define MaxDevnum           64

#define INIT_OK             0xaabb
#define UPDATA_OK           0x7788

#define Mode_CodeTrans      0x01
#define Mode_BinTrans       0x02
#define Mode_CodeWait       0x03
#define Mode_BinWait        0x04

#define TransDelayms        300

#define MaxRetryCnt         20

#define MaxTout             18000

typedef struct
{
	u8  Cidnum;
	u8  Addr;
	u8  Freq;	
	u8  ValidDev;
	u8  OnLineSta[MaxDevnum];
	u8  OfflineCnt[MaxDevnum];
	short  OnLinenum;
	u32 TimerS1;
	u32 TimerS2;
	u32 TimerS3;
	u32 TimerS4;
	u8  DataTransMode;
}CONN_CTRL;

typedef struct
{
	u8  Bindata[MaxDatabyte];
	u32 Datalen1;
	u32 Checksum1;
	u32 Datalen2;
	u32 Checksum2;
	u8  Addr;
}UPDATA_CTRL;

#if !TRANSMIT_FLAG  
#define TRANSMIT_FLAG         1

typedef struct
{
	u8   CTSflag;
	long Datasum;
	u32  Framecnt;
	u32  Datalen;
	u32  Datacnt;
	long Tmpsum;
}TRANSMIT_CTRL;

#endif

extern u8 MEM_Databuf[];

short _Read_OnlineSta(void);
u8 _UAPP_Comm_INIT(void);
void _Comm_TASK1000ms(void);  //1HZ
void _Comm_TASK1ms(void); //1000HZ
void _Comm_TASK10ms(void); //100HZ
void _Comm_TASK5ms(void);
#endif






#ifndef UAPP_CAN_H
#define UAPP_CAN_H
#include "sys.h"
typedef struct
{
   u8 count;
   u8 status;
   u8 workmode;
   u8 error;
   u8 stage;
   u8 speedfeedback;	
}HEARTBEAT_DATA;

typedef struct
{
   u16 WeldingCurrent;
   u16 WeldingVoltage;
   u16 PAR1;	
}FEEDBACK_DATA;


typedef struct
{
   u8 CANTxReady;
   u8 CANRxReady;
   u8 CANWaitAck;
   u8 OnLineSta;
	 u8 OffLineCnt;
   u8 MemSyncSign;
}CAN_STATUS;

#if !TRANSMIT_FLAG  
#define TRANSMIT_FLAG         1

typedef struct
{
	u8   CTSflag;
	u32  Datasum;
	u32  Framecnt;
	u32  Datalen;
	u32  Datacnt;
	long long Tmpsum;
}TRANSMIT_CTRL;

#endif

#define MaxCanbyte            6  

#define ResetKey              0xabcdef

#define FAIL_COMM             130

void CAN_TASK1ms(void);  //1000HZ
void CAN_TASK1000ms(void);  //1HZ
void _CAN_TASK10ms(void); //100HZ
void _CAN_TASK250ms(void);
#endif






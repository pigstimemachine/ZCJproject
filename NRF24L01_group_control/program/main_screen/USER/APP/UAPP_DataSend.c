#include "lib.h"

u8 TmpDecodeData[MaxDatalen]={1,2,3,4,5,6,7,8,9,10};
u8 TmpDecodeData1[MaxDatalen];
void DataSendTask(void)
{
   if(1)//_ReadDecodeData(TmpDecodeData)==Readok)
	 {
		   
//	     NRF24L01_TxWithArray(TmpDecodeData);
	 }	 
}	

void DataSendTask2(void)
{
   if(NRF24L01_RxWithArray(TmpDecodeData1)==RX_NO_ERR)
	 {
		   
//	     NRF24L01_TxWithArray(TmpDecodeData);
	 }	 
}	
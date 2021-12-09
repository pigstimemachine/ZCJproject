#include "lib.h"


int main(void)
{			  	
  Stm32_Clock_Init(9);	
	//MY_NVIC_SetVectorTable(NVIC_VectTab_FLASH,ADDR_APP_OFFSET);
	delay_init(72);
  uart_init(72,9600);	
  TIM2_Int_Init(9,7199);
	SysTask_Create();
	//GPIO_Init();
  //Adc_Init();
	//CodeModule_INIT();
	_UAPP_Comm_INIT();
	//CAN_Mode_Init(1,8,7,9,0);
	while(1)
	{
	  Timer1_IsrHandler();
	}	
}





#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

void TIM3_Int_Init(u16 arr,u16 psc);
void TIM2_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr,u16 psc);
void TIM6_Int_Init(u16 arr,u16 psc);
void TIM7_Int_Init(u16 arr,u16 psc);
void TIM4_Clear(u8 cnt);
void TIM4_Enable(u8 sta);
void TIM4_Init(u16 arr,u16 psc);
unsigned char TimerIsOverflowEvent(void);
void TimeCount(void);
#endif
























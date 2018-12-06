#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

/************************************************
 DCM模式下的BUCK变换器数字系统控制
 博客：https://blog.csdn.net/tichimi3375
 作者：唯恋殊雨
************************************************/

#define PWM_VAL1 TIM3->CCR1
#define PWM_VAL2 TIM3->CCR2  

void TIM3_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);
#endif

#ifndef __PWM_H
#define __PWM_H


#include "stm32f10x.h"                  // Device header
#include "sys.h"


void PWM_TIM2_Init(uint16_t u16ARR,uint16_t u16PSC);


#endif


#ifndef __ENCODER_H
#define __ENCODER_H

#include "ti_msp_dl_config.h"
#include "control.h"
#include "quaternion.h"

extern volatile int16_t Encoder_Count_L;   
extern volatile int16_t Encoder_Count_R;  

void Encoder_Init(void);
int16_t Encoder_Get_L(void);
int16_t Encoder_Get_R(void);
void GROUP1_IRQHandler(void);
float rd_Dist(int32_t Total_Pulses);

#endif
#ifndef __TRACK_H
#define __TRACK_H


#include "stm32f10x.h" 
#include "No_Mcu_Ganv_Grayscale_Sensor_Config.h"
#include "My_ADC.h"

extern unsigned short Anolog[8];
extern unsigned short white[8];
extern unsigned short black[8];

void Gray_Init(void);





#endif


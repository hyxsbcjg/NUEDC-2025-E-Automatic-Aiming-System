#ifndef __TRACK_H
#define __TRACK_H

#include "ti_msp_dl_config.h"
#include "No_Mcu_Ganv_Grayscale_Sensor_Config.h"

extern volatile uint8_t Turn_Flag_Old;
extern volatile uint8_t Turn_Flag;
extern volatile uint8_t Weigth_Flag;

extern uint16_t Anolog_black[8];
extern uint16_t Anolog_white[8];
extern uint8_t CAL_Flag; //0校准白色 1校准黑色 2完成校准




// 通道位定义（从左到右）
#define CH1   ((Digtal >> 0) & 0x01)
#define CH2   ((Digtal >> 1) & 0x01)
#define CH3   ((Digtal >> 2) & 0x01)
#define CH4   ((Digtal >> 3) & 0x01)
#define CH5   ((Digtal >> 4) & 0x01)
#define CH6   ((Digtal >> 5) & 0x01)
#define CH7   ((Digtal >> 6) & 0x01)
#define CH8   ((Digtal >> 7) & 0x01)





extern int8_t weight_set[][8];
extern int8_t Totol_Weight;
extern unsigned short Anolog[8];







void Gray_Init(void);
void Gray_Detect(void);
void Gray_CAL(void);


#endif
#ifndef __ENCODER_H
#define __ENCODER_H


#include "stm32f10x.h"                  // Device header
#include "sys.h"
#include "math.h"


void EnCoder_TIM3_Init(void);
void EnCoder_TIM4_Init(void);
int16_t rd_Speed(uint8_t TIMx);

#define ENCODER_LINE	260	//500线
#define REDUCTION_RATIO	20	//减速比30
#define EDGE_CNT		4	//上下边沿都计数，四倍频
#define SAMPLE_RATE		10	//10ms读取一次编码器数据
#define PI 3.141592

float Num_AllEncoder(int32_t encoder_num);
float Ad_Dist(int32_t encoder_num);


#endif


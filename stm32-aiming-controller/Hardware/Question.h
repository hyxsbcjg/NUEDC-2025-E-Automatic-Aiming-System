#ifndef __QUESTION_H
#define __QUESTION_H


#include "stm32f10x.h"                  // Device header
#include "sys.h"
#include <stdbool.h>    // 标准C99 bool类型
#include "control.h"


extern uint16_t Blob1_cx,Blob1_cy;
extern uint16_t Blob2_cx,Blob2_cy;
extern uint8_t Find_Rect_Flag;


//Q3变量
extern int16_t X_Speed;
extern uint8_t X_Speed_Idx;


extern uint16_t dist_center;

extern volatile bool Blob_Track_X_PID_Flag;
extern volatile bool Blob_Track_Y_PID_Flag;

uint16_t PixelDiff_Calc(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);



void Q1_Proc(void);
void Q2_Proc(void);
void Q3_Proc(void);
void Q4_Proc(void);
void Q5_Proc(void);
void Q6_Proc(void);







#endif



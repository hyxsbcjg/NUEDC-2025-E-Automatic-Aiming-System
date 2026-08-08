#ifndef __QUESTION_H
#define __QUESTION_H

#include "ti_msp_dl_config.h"
#include "board.h"

//PID
extern volatile uint8_t Track_PID_Flag;
extern volatile uint8_t Turn_PID_Flag;


//Q1
extern uint8_t Q1_lap_count;  // ÒªÅÜµÄÈ¦Êý

void Q1_Proc(void);
void Q2_Proc(void);
void Q3_Proc(void);
void Q4_Proc(void);
void Q5_Proc(void);










#endif
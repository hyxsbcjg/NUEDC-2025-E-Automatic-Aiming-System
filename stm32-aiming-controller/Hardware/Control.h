#ifndef __CONTROL_H
#define __CONTROL_H

#include "motor.h"







typedef struct{
int32_t Target,Actual,Actual_Old,Actual_Old_Old;
float Kp,Ki,Kd;
int32_t Error0,Error1,Error2;
int32_t ErrorInt,Int_Out_Now,Int_Out_Old,Int_Out_End;
int16_t Base;
int16_t Out_Now,Out_Old,Out_End,Out_Delta;
float DifOut,DifOut_Old;
}PID_int;

typedef struct{
float Target,Actual,Actual_Old,Actual_Old_Old;
float Kp,Ki,Kd;
float Error0,Error1,Error2;
float ErrorInt,Int_Out_Now,Int_Out_Old,Int_Out_End;
int16_t Base;
int16_t Out_Now,Out_Old,Out_End,Out_Delta;
float DifOut,DifOut_Old;
}PID_float;


extern PID_int Blob_Track_X,Blob_Track_Y;
extern uint32_t u32_ms;

void PID_Init(void);
float my_fmod(float a, float b);
float floatAbs(float buf);
void int16_tLimit(int16_t *Actual,int16_t Min,int16_t Max);
void uint16_tLimit(uint16_t *Actual,uint16_t Min,uint16_t Max);
int16_t int16_tAbs(int16_t buf);
void uint16_tLimit(uint16_t *Actual,uint16_t Min,uint16_t Max);

void Blob_Track_X_PID_Control(void);
void Blob_Track_Y_PID_Control(void);

#endif

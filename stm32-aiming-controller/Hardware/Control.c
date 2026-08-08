#include "control.h"
#include "encoder.h"
#include "track.h"
#include "question.h"
#include "myZDT.h"

PID_int Blob_Track_X,Blob_Track_Y;


int16_t int16_tAbs(int16_t buf)
{
	if(buf < 0) return -buf;
	else return buf;
}

int32_t int32_tAbs(int32_t buf)
{
	if(buf < 0) return -buf;
	else return buf;
}

float floatAbs(float buf)
{
	if(buf < 0) return -buf;
	else return buf;
}

void int16_tLimit(int16_t *Actual,int16_t Min,int16_t Max)
{
	if(*Actual > Max)  *Actual = Max;
	if(*Actual < Min)  *Actual = Min;
}

void int32_tLimit(int32_t *Actual,int32_t Min,int32_t Max)
{
	if(*Actual > Max)  *Actual = Max;
	if(*Actual < Min)  *Actual = Min;
}


void uint16_tLimit(uint16_t *Actual,uint16_t Min,uint16_t Max)
{
	if(*Actual > Max)  *Actual = Max;
	if(*Actual < Min)  *Actual = Min;
}

void floatLimit(float *Actual,float Min,float Max)
{
	if(*Actual > Max)  *Actual = Max;
	if(*Actual < Min)  *Actual = Min;
}


// 浮点取模：计算 a mod b，返回值在 [0, b) 或 [0, |b|) 区间
float my_fmod(float a, float b) 
{
    if (b == 0.0f) return 0.0f; // 防止除以0
    int div = (int)(a / b);
    float result = a - (float)div * b;
    
    // 保证结果在 [0, b) 区间
    if (result < 0)
        result += b;
    
    return result;
}


void PID_Init(void)
{
//	Blob_Track_X.Kp = -280e-3;Blob_Track_X.Ki = -230e-5;
//	Blob_Track_Y.Kp = -280e-3;Blob_Track_Y.Ki = -230e-5;
	Blob_Track_X.Kp = -680e-3;Blob_Track_X.Ki = -295e-5;
	Blob_Track_Y.Kp = 650e-3;Blob_Track_Y.Ki = 280e-5;

}


void Blob_Track_X_PID_Control(void)
{
	if(u32_ms % 20 == 0)
	{
		Blob_Track_X.Target = Blob1_cx;
		Blob_Track_X.Actual = Blob2_cx;
		
		Blob_Track_X.Error1 = Blob_Track_X.Error0;
		Blob_Track_X.Error0 = Blob_Track_X.Target - Blob_Track_X.Actual;
		if(int16_tAbs(Blob_Track_X.Error0) < 6)
		Blob_Track_X.ErrorInt += Blob_Track_X.Error0;
		
		int32_tLimit(&Blob_Track_X.ErrorInt,-2000,+2000);
		
		Blob_Track_X.Out_Now = Blob_Track_X.Kp * Blob_Track_X.Error0 + Blob_Track_X.Ki * Blob_Track_X.ErrorInt;
		
//		Step_Motor_Set_Pos(MOTOR_X_ADDR,Blob_Track_X.Out_Now,MOTOR_SPEED_X);
		Step_Motor_Set_Speed(MOTOR_X_ADDR,Blob_Track_X.Out_Now);
		
	}
}

void Blob_Track_Y_PID_Control(void)
{
	if(u32_ms % 20 == 0)
	{
		Blob_Track_Y.Target = Blob1_cy;
		Blob_Track_Y.Actual = Blob2_cy;
		
		Blob_Track_Y.Error1 = Blob_Track_Y.Error0;
		Blob_Track_Y.Error0 = Blob_Track_Y.Target - Blob_Track_Y.Actual;
		if(int16_tAbs(Blob_Track_Y.Error0) < 6)
		Blob_Track_Y.ErrorInt += Blob_Track_Y.Error0;
		
		int32_tLimit(&Blob_Track_Y.ErrorInt,-2000,+2000);
		
		Blob_Track_Y.Out_Now = Blob_Track_Y.Kp * Blob_Track_Y.Error0 + Blob_Track_Y.Ki * Blob_Track_Y.ErrorInt;
		
//		Step_Motor_Set_Pos(MOTOR_Y_ADDR,Blob_Track_Y.Out_Now,MOTOR_SPEED_Y);
		Step_Motor_Set_Speed(MOTOR_Y_ADDR,Blob_Track_Y.Out_Now);
	}
}





#include "control.h"
#include "encoder.h"
#include "track.h"





PID_float Turn_OutAngle; //转向环
PID_int Turn_InGyro;


PID_int Track;









void My_GPIO_WriteVal(GPIO_Regs* gpio, uint32_t pins, uint32_t pinsVal)
{
	if(pinsVal)
		gpio->DOUTSET31_0 = pins;
	else
		gpio->DOUTCLR31_0 = pins;
}

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
	//循迹环
	Track.Target = 0;
	Track.Base = 1000;
	Track.Kp = 110.0f;Track.Ki = 0;Track.Kd = 0;
	
	//转向环
	Turn_OutAngle.Target = 0;
	Turn_OutAngle.Kp = 132.0f;Turn_OutAngle.Ki = 2.7f;  //120 2.2
	Turn_InGyro.Target = 0;
	Turn_InGyro.Kp = 240e-2;Turn_InGyro.Kd = 100e-2;
	Turn_InGyro.Base = 0;
	
}


void Track_PID_Control(void)
{
	if(u32_ms % 20 == 0)
	{
		Track.Actual = Totol_Weight;
			
//		Track.Error1 = Track.Error0;
		Track.Error0 = -Track.Actual;
		
//		Track.DifOut_Old = Track.DifOut;
//		Track.DifOut = Track.Kd * (7 * (Track.Error0 - Track.Error1) + 3*Track.DifOut_Old)*0.1f;
		
		Track.Out_Now = Track.Kp * Track.Error0; //  + Track.Kd * Track.DifOut;
		
		int16_t Pwm_Left = (int16_t)(Track.Base - Track.Out_Now);
		int16_t Pwm_Right = (int16_t)(Track.Base + Track.Out_Now);
		
		int16_tLimit(&Pwm_Left,PWM_MIN,PWM_MAX);
		int16_tLimit(&Pwm_Right,PWM_MIN,PWM_MAX);
		
		Load_Motor_A(Pwm_Left);
		Load_Motor_B(Pwm_Right);
	}
}


void Turn_PID_Control(void)
{
	if(u32_ms % 30 == 0)
	{
//		Turn_OutAngle.Target = 0;
		Turn_OutAngle.Actual = MyYaw_180_180;
		
		Turn_OutAngle.Error1 = Turn_OutAngle.Error0;
		Turn_OutAngle.Error0 = Turn_OutAngle.Target - Turn_OutAngle.Actual;
		if(Turn_OutAngle.Error0 > 180) Turn_OutAngle.Error0 -= 360;
		if(Turn_OutAngle.Error0 < -180) Turn_OutAngle.Error0 += 360;
		
		floatLimit(&Turn_OutAngle.Error0, -16, 16);
		
		if(floatAbs(Turn_OutAngle.Error0) < 8 )
			Turn_OutAngle.ErrorInt += Turn_OutAngle.Error0;
		floatLimit(&Turn_OutAngle.ErrorInt,-1000,1000);
		
		
		
		Turn_InGyro.Target = Turn_OutAngle.Kp*Turn_OutAngle.Error0 + Turn_OutAngle.Ki*Turn_OutAngle.ErrorInt;
	}
	
	if(u32_ms % 10 == 0)
	{
		
		Turn_InGyro.Actual_Old = Turn_InGyro.Actual;
		Turn_InGyro.Actual = (7*MyGyro + 3*Turn_InGyro.Actual_Old)*0.1f;
		

		
		Turn_InGyro.Error1 = Turn_InGyro.Error0;
		Turn_InGyro.Error0 = (Turn_InGyro.Target - Turn_InGyro.Actual);

	
		
		Turn_InGyro.DifOut_Old = Turn_InGyro.DifOut;
		Turn_InGyro.DifOut = (7*Turn_InGyro.Kd * (Turn_InGyro.Actual_Old - Turn_InGyro.Actual) + 3*Turn_InGyro.DifOut_Old)*0.1f;
		Turn_InGyro.Out_Now = (Turn_InGyro.Kp * Turn_InGyro.Error0  + Turn_InGyro.DifOut);
		int32_tLimit(&Turn_InGyro.Out_Now,PWM_MIN,PWM_MAX);
		
		int16_t Pwm_Left = (int16_t)(Turn_InGyro.Base - Turn_InGyro.Out_Now);
		int16_t Pwm_Right = (int16_t)(Turn_InGyro.Base + Turn_InGyro.Out_Now);
//		my_printf(Pwm_Left);
		
		int16_tLimit(&Pwm_Left,PWM_MIN,PWM_MAX);
		int16_tLimit(&Pwm_Right,PWM_MIN,PWM_MAX);
//		my_printf("%d %d %d\r\n",Turn_InGyro.Out_Now,Pwm_Left,Pwm_Right);
		Load_Motor_A(Pwm_Left);
		Load_Motor_B(Pwm_Right);
	}
}






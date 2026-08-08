#include "motor.h"
#include "control.h"

void Motor_Init(void)
{
	//AIN
	My_GPIO_WriteVal(Motor_A_PORT,Motor_A_AIN1_PIN,0);
	My_GPIO_WriteVal(Motor_A_PORT,Motor_A_AIN2_PIN,1);
	
	//Motor_PWMA
	DL_Timer_setCaptureCompareValue(Motor_PWM_INST,0,GPIO_Motor_PWM_C0_IDX);
	
	//BIN
	My_GPIO_WriteVal(GPIOB,Motor_B_BIN1_PIN,0);
	My_GPIO_WriteVal(GPIOB,Motor_B_BIN2_PIN,1);
	
	//Motor_PWMB
	DL_Timer_setCaptureCompareValue(Motor_PWM_INST,0,GPIO_Motor_PWM_C1_IDX);
}

void Load_Motor_A(int16_t MotorA_PWM)
{
	int16_t temp = 0;
	if(MotorA_PWM > 0)
	{My_GPIO_WriteVal(GPIOA,Motor_A_AIN1_PIN,1);
	My_GPIO_WriteVal(GPIOA,Motor_A_AIN2_PIN,0);}
	else
	{My_GPIO_WriteVal(GPIOA,Motor_A_AIN1_PIN,0);
	My_GPIO_WriteVal(GPIOA,Motor_A_AIN2_PIN,1);}
	
	temp = int16_tAbs(MotorA_PWM);
	DL_Timer_setCaptureCompareValue(Motor_PWM_INST,temp,GPIO_Motor_PWM_C0_IDX);
}

void Load_Motor_B(int16_t MotorB_PWM)
{
	int16_t temp = 0;
	if(MotorB_PWM > 0)
	{My_GPIO_WriteVal(GPIOA,Motor_B_BIN1_PIN,1);
	My_GPIO_WriteVal(GPIOB,Motor_B_BIN2_PIN,0);}
	else
	{My_GPIO_WriteVal(GPIOA,Motor_B_BIN1_PIN,0);
	My_GPIO_WriteVal(GPIOB,Motor_B_BIN2_PIN,1);}
	
	temp = int16_tAbs(MotorB_PWM);
	DL_Timer_setCaptureCompareValue(Motor_PWM_INST,temp,GPIO_Motor_PWM_C1_IDX);
}







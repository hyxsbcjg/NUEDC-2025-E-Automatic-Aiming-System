#include "Motor.h"



void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	PWM_TIM2_Init(7199,0);
}


void Load_Motor_A(int16_t MotorA_PWM)
{
	if(MotorA_PWM > 0)	{AIN1 = 1;AIN2 = 0;}
	else	{AIN1 = 0;AIN2 = 1;}
	TIM_SetCompare2(TIM2,int16_tAbs(MotorA_PWM));
}

void Load_Motor_B(int16_t MotorB_PWM)
{
	if(MotorB_PWM > 0)	{AIN1 = 1;AIN2 = 0;}
	else	{AIN1 = 0;AIN2 = 1;}
	TIM_SetCompare2(TIM2,int16_tAbs(MotorB_PWM));
}






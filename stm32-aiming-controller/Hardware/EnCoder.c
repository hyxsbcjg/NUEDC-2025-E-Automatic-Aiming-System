#include "EnCoder.h"

void EnCoder_TIM3_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 65535;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	
	TIM_ICInitTypeDef TIM_ICInitStruct = {0};

	TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStruct.TIM_ICFilter = 0xF;
	TIM_ICInit(TIM3, &TIM_ICInitStruct);
	
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStruct.TIM_ICFilter = 0xF;
	TIM_ICInit(TIM3,&TIM_ICInitStruct);
	
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Falling,TIM_ICPolarity_Rising);
	
	
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM_SetCounter(TIM3,0);
	
	TIM_Cmd(TIM3,ENABLE);
}	


void EnCoder_TIM4_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 65535;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	
	TIM_ICInitTypeDef TIM_ICInitStruct = {0};
	TIM_ICInitStruct .TIM_Channel = TIM_Channel_1|TIM_Channel_2;
	TIM_ICInitStruct.TIM_ICFilter = 0xF;
	
	TIM_ICInit(TIM4,&TIM_ICInitStruct);
	
	TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Falling,TIM_ICPolarity_Falling);
	
	
	TIM_ClearFlag(TIM4,TIM_FLAG_Update);
	TIM_SetCounter(TIM4,0);
	
	TIM_Cmd(TIM4,ENABLE);
}	


int16_t rd_Speed(uint8_t TIMx)
{
	int16_t Speed;
	switch(TIMx)
	{
		case 3:
			Speed = TIM_GetCounter(TIM3);
			TIM_SetCounter(TIM3,0);
		return Speed;
		case 4:
			Speed = TIM_GetCounter(TIM4);
			TIM_SetCounter(TIM4,0);
		return Speed;		
	}
	return 0;
}



float Num_AllEncoder(int32_t encoder_num)
{
	float quanshu;
	quanshu = ((encoder_num) /(13 * 20.409f * 4));
	
	return quanshu;
}


float Ad_Dist(int32_t encoder_num) //µ¥Î»_cm	
{
	float Dist;
	Num_AllEncoder(encoder_num);
	
	Dist = Num_AllEncoder(encoder_num) * ( PI * 48e-1);
	return Dist;
}

#include "Encoder.h"


volatile int16_t Encoder_Count_L;   
volatile int16_t Encoder_Count_R;   

void Encoder_Init(void)
{
	//or Not
    NVIC_EnableIRQ(Encoder_INT_IRQN); //使能编码器模式中断
}

int16_t Encoder_Get_L(void)
{
	int16_t Temp;
	Temp = Encoder_Count_L;
	Encoder_Count_L = 0;
	return Temp;
}

int16_t Encoder_Get_R(void)
{
	int16_t Temp;
	Temp = Encoder_Count_R;
	Encoder_Count_R = 0;
	return Temp;
}




float rd_Dist(int32_t Total_Pulses)
{
	float Total_Rotations = 0;
	float Dist = 0;
	Total_Rotations = (Total_Pulses)/1061.06f;
	
	Dist = Total_Rotations*(PI * 480);
	
	return Dist;
}




void GROUP1_IRQHandler(void)
{
    uint8_t Encodeg_flag,A_flag,B_flag,direction_flag;
	uint8_t Encodeg_flag_R,C_flag,D_flag,direction_flag_R;
	
	switch(DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1)) 
	{
		case Encoder_INT_IIDX:
		if(DL_GPIO_getEnabledInterruptStatus(GPIOB, Encoder_A_PIN))
		{
            Encodeg_flag = 1;//A中断
            if(DL_GPIO_readPins(GPIOB, Encoder_A_PIN)) A_flag = 1;
            else A_flag = 0;
            if(DL_GPIO_readPins(GPIOB, Encoder_B_PIN)) B_flag = 1;
            else B_flag = 0;
            direction_flag = A_flag+B_flag+Encodeg_flag;                   
            if(direction_flag == 0 || direction_flag ==2)Encoder_Count_L--;
            else Encoder_Count_L++;
            
			DL_GPIO_clearInterruptStatus(GPIOB, Encoder_A_PIN);
		}
		
		
		if(DL_GPIO_getEnabledInterruptStatus(GPIOB, Encoder_B_PIN))
		{
            Encodeg_flag = 0;
            if(DL_GPIO_readPins(GPIOB, Encoder_A_PIN)) A_flag = 1;
            else A_flag = 0;
            if(DL_GPIO_readPins(GPIOB, Encoder_B_PIN)) B_flag = 1;
            else B_flag = 0;
            direction_flag = A_flag+B_flag+Encodeg_flag;
            if(direction_flag == 0 || direction_flag ==2)Encoder_Count_L--;
            else Encoder_Count_L++;
            
			DL_GPIO_clearInterruptStatus(GPIOB, Encoder_B_PIN);
		}
 		
		
		if(DL_GPIO_getEnabledInterruptStatus(GPIOB, Encoder_C_PIN))
		{
            Encodeg_flag_R = 1;
            if(DL_GPIO_readPins(GPIOB, Encoder_C_PIN)) C_flag = 1;
            else C_flag = 0;
            if(DL_GPIO_readPins(GPIOB, Encoder_D_PIN)) D_flag = 1;
            else D_flag = 0;
            direction_flag_R = C_flag+D_flag+Encodeg_flag_R;
            if(direction_flag_R == 0 || direction_flag_R ==2)Encoder_Count_R++;
            else Encoder_Count_R--;
            
			DL_GPIO_clearInterruptStatus(GPIOB, Encoder_C_PIN);
		}
		
		if(DL_GPIO_getEnabledInterruptStatus(GPIOB, Encoder_D_PIN))
		{
            Encodeg_flag_R = 0;
            if(DL_GPIO_readPins(GPIOB, Encoder_C_PIN)) C_flag = 1;
            else C_flag = 0;
            if(DL_GPIO_readPins(GPIOB, Encoder_D_PIN)) D_flag = 1;
            else D_flag = 0;
            direction_flag_R = C_flag+D_flag+Encodeg_flag_R;
            if(direction_flag_R == 0 || direction_flag_R ==2)Encoder_Count_R++;
            else Encoder_Count_R--;
            
			DL_GPIO_clearInterruptStatus(GPIOB, Encoder_D_PIN);
		}
 
		break;
	}
}


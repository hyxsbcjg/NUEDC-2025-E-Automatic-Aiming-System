#include "key.h"
#include "delay.h"

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	
	
}



int Key_Read(void)
{
	
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7) == 0)
	{
		
		return 1;
		
	}
	else if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8) == 0)
	{
		
		return 2;
	}
	else if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9) == 0)
	{
		
		return 3;
	}
	else if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) == 0)
	{
		
		return 4;
	}
	return 0;
}


void Relay_Init(void)
{
   
    GPIO_InitTypeDef GPIO_InitStructure;


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;            
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     


    GPIO_Init(GPIOC, &GPIO_InitStructure);


    GPIO_ResetBits(GPIOC, GPIO_Pin_15);
}

void Relay_Light_Set(uint8_t Val)
{
    if (Val == 1)
    {
    
        GPIO_SetBits(GPIOC, GPIO_Pin_15);
    }
    else
    {
        
        GPIO_ResetBits(GPIOC, GPIO_Pin_15);
    }
}

void Send_MN_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIOC和GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB, ENABLE);
    
    // 配置PC10 ?? M
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;            
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOC, GPIO_Pin_10);
    
    // 配置PB14 ?? N
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;            
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}


void Send_MN_Signal(uint8_t M_Signal,uint8_t N_Signal)
{
	if(M_Signal)
		GPIO_SetBits(GPIOC, GPIO_Pin_10);
	else
		GPIO_ResetBits(GPIOC, GPIO_Pin_10);
	if(N_Signal)
		GPIO_SetBits(GPIOB, GPIO_Pin_14);
	else
		GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}



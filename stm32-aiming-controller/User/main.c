#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "serial.h"
#include "track.h"
#include "stdio.h"
#include "myspi.h"
#include "timer.h"
#include "pwm.h"
#include "key.h"
#include "myzdt.h"
#include "myvl53l0x.h"
#include "quaternion.h"
#include "question.h"


uint8_t rx_buff[128] = {0x02,0x9A,0x00,0x00,0x6B};

//Angle
float Angle_Target = 0;
uint8_t Get_Angle_Flag = 0;
float MyYaw_180_180 = 0;
uint8_t absMyYaw_180_180 = 0;
float MyYaw_0_360 = 0;

int16_t MyGyro= 0;

//PID Flag
volatile bool Blob_Track_X_PID_Flag = false;
volatile bool Blob_Track_Y_PID_Flag = false;


//Key
uint8_t Key_Val;
uint8_t Key_Val_Old;

//Question
uint8_t Q_Num = 0;
uint8_t Q_State = 0;

//Temp
uint16_t dist_center = 0;


//Tim
uint32_t u32_ms = 0;
uint32_t u32_UART_Dly = 0;
uint32_t u32_Dist_Dly = 0;
uint32_t u32_OLED_Dly = 0;
uint32_t u32_Key_Dly = 0;


//Founction
void OLED_Proc(void);
void Key_Proc(void);
void USART_Proc(void);
	

int main(void)
{
	delay_init();
	USART3_Init(); //用户串口初始化
	OLED_Init();
	Key_Init();
	Relay_Init();
	USART1_Init();
	USART2_Init();
	UART4_Init();
	UART5_Init();
	Step_Motor_Init();
	delay_ms(200);
	PID_Init();

	Timer_TIM1_Init();

	my_printf("You can start !");
	while (1)
	{
		

//		my_printf("Blob1(%d,%d)  Blob2(%d,%d)\r\n",Blob1_cx,Blob1_cy,Blob2_cx,Blob2_cy);
//		Step_Motor_Set_Speed(MOTOR_X_ADDR,50);
		
//		my_printf("%u\r\n",(uint16_t)Find_Rect_Flag);
//		my_printf("%d,%d,%d,%d",Blob_Track_X.Error0);
//		my_printf("%d\r\n",Blob_Track_X.Out_Now);
		
		dist_center = PixelDiff_Calc(Blob1_cx,Blob1_cy,Blob2_cx,Blob2_cy);
		
		
//		Emm_Read_Pulse(MOTOR_X_ADDR);
		
		
		
		if(Q_Num == 1 && Q_State == 1)
			Q1_Proc();
		else if(Q_Num == 2 && Q_State == 1)
			Q2_Proc();
		else if(Q_Num == 3 && Q_State == 1)
			Q3_Proc();
		else if(Q_Num == 4 && Q_State == 1)
			Q4_Proc();
		else if(Q_Num == 5 && Q_State == 1)
			Q5_Proc();
		
		OLED_Proc();
		Key_Proc();
		USART1_K230_Proc();
	}

}




//void Oled_Proc(void)
//{
//	OLED_Clear();
//	OLED_Printf(0,0,OLED_8X16,"234566464");
//	
//	OLED_Update();
//}





void Key_Proc(void)
{
	if(u32_Key_Dly < 20)
		return;
	u32_Key_Dly = 0;
	
	Key_Val = Key_Read();
	if(Key_Val == Key_Val_Old)
		return;
	switch(Key_Val)
	{
		case 1:
			Q_Num = (Q_Num + 1) % 7;
		break;
		case 2:
			Q_State = (Q_State + 1) % 2;
		break;
		case 3: //调参按键
			if(Q_Num == 3)
			{
				X_Speed += 20;
			}
		break;
		case 4:
			if(Q_Num == 3)
			{
				X_Speed -= 20;
			}
		break;
	}
	
	
	
	Key_Val_Old = Key_Val;
}




void OLED_Proc(void)
{
	if(u32_OLED_Dly < 1000)
		return;
	u32_OLED_Dly = 0;
	
	OLED_Printf(0,0,OLED_8X16,"Num:%1d State:%1d",Q_Num,Q_State);
	
	OLED_Printf(0,16,OLED_8X16,"Rect: %1u\r\n",(uint16_t)Find_Rect_Flag);
	if(Q_Num == 0)
	{
		OLED_Printf(0,32,OLED_8X16,"Blob: (%u,%u)\r\n",(uint16_t)Blob2_cx,(uint16_t)Blob2_cy);
	}
	
	else if(Q_Num == 3)
	{
		OLED_Printf(0,16,OLED_8X16,"Rect: %u\r\n",(uint16_t)Find_Rect_Flag);
		OLED_Printf(0,32,OLED_8X16,"Dist: %u\r\n",(uint16_t)dist_center);
		OLED_Printf(0,48,OLED_8X16,"X_Speed: %3d",(int16_t)X_Speed);
	}
	
	OLED_Update();
}




void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		
		u32_ms++;
		u32_UART_Dly++;
		u32_OLED_Dly++;
		u32_Dist_Dly++;
		u32_Key_Dly++;
//		if(u32_ms % 10 == 0)
//		{
//			quaternion_update();
//			quaternion_to_euler();
//			Data_Proc();
//		}
		
		
		
//		Step_Motor_Set_Pos(MOTOR_X_ADDR,30,MOTOR_SPEED_X);
		if(Blob_Track_X_PID_Flag)
		Blob_Track_X_PID_Control();
		
		if(Blob_Track_Y_PID_Flag)
		Blob_Track_Y_PID_Control();
		
		
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update); 
	}
	
		
}











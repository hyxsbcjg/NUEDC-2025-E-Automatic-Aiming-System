/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "ti_msp_dl_config.h"
#include "UART.h"
#include "stdio.h"
#include "motor.h"
#include "oled.h"
#include "control.h"
#include "board.h"
#include "servo.h"
#include "peripheral.h"
#include "key.h"
#include "track.h"
#include "question.h"
#include "No_Mcu_Ganv_Grayscale_Sensor_Config.h"

int16_t aa = 1000;
int16_t bb = 1000;





//Angle
float Angle_Target = 0;
uint8_t Get_Angle_Flag = 0;
float MyYaw_180_180 = 0;
uint8_t absMyYaw_180_180 = 0;


int16_t MyGyro= 0;

//Dist
float Dist = 0;
int32_t SpeedInt_Left = 0;
int32_t SpeedInt_Right = 0;

//PID
volatile uint8_t Track_PID_Flag = 0;
volatile uint8_t Turn_PID_Flag = 0;

//Key
uint8_t Key_Val;
uint8_t Key_Val_Old;


//Tim
uint32_t u32_ms = 0;
uint32_t u32_UART_Dly = 0;
uint32_t u32_Dist_Dly = 0;
uint32_t u32_OLED_Dly = 0;
uint32_t u32_Key_Dly = 0;



unsigned short Normal[8];
unsigned char rx_buff[256]={0};

//Question
uint8_t Q_Num = 0;
uint8_t Q_State = 0;




//Founction
void UART_User_Proc(void);
void OLED_Proc(void);
void Dist_Proc(void);
void Data_Proc(void);
void Key_Proc(void);

uint16_t t = 10000;

int main(void)
{
    SYSCFG_DL_init();
	
	
	IMU_Init();
	Motor_Init();
	

	
	PID_Init();
	OLED_Init();
	Gray_Init();
	
	delay_ms(100);
//	NVIC_ClearPendingIRQ(UART2_User_INST_INT_IRQN);
	NVIC_ClearPendingIRQ(UART1_K230_INST_INT_IRQN);
	NVIC_ClearPendingIRQ(TIMG_SysTick_INST_INT_IRQN);
	
//	NVIC_EnableIRQ(UART2_User_INST_INT_IRQN);
	NVIC_EnableIRQ(UART1_K230_INST_INT_IRQN);
	NVIC_EnableIRQ(TIMG_SysTick_INST_INT_IRQN);
	
	

	UART2_SendString("You Can Start!");
	
	

    while (1) 
	{
		
//		Load_Servo(SERVO_MID);
//		my_printf("%f,%f,%f\r\n",Turn_OutAngle.Target,Turn_OutAngle.Actual,Turn_OutAngle.Out_Now);
//		my_printf("%d,%f\r\n",MyGyro,MyYaw_180_180);
//		UART2_SendString("You Can Start!");
//		Dist_Proc();
		
//		Load_Motor_A(800);
//		Load_Motor_B(800);
//		delay_ms(200);
//		Load_Motor_A(0);
//		Load_Motor_B(0);
		
//		UART_SendByte(UART1,0x11);
	
		//			//获取传感器数字量结果(只有当有黑白值传入进去了之后才会有这个值！！)
//		Digtal=Get_Digtal_For_User(&sensor);
//		sprintf((char *)rx_buff,"Digtal %d-%d-%d-%d-%d-%d-%d-%d\r\n",(Digtal>>0)&0x01,(Digtal>>1)&0x01,(Digtal>>2)&0x01,(Digtal>>3)&0x01,(Digtal>>4)&0x01,(Digtal>>5)&0x01,(Digtal>>6)&0x01,(Digtal>>7)&0x01);
//		UART2_SendString((char *)rx_buff);
//		memset(rx_buff,0,256);

					//获取传感器模拟量结果(有黑白值初始化后返回1 没有返回 0)
//		if(Get_Anolog_Value(&sensor,Anolog)){
//		sprintf((char *)rx_buff,"Anolog %d-%d-%d-%d-%d-%d-%d-%d\r\n",Anolog[0],Anolog[1],Anolog[2],Anolog[3],Anolog[4],Anolog[5],Anolog[6],Anolog[7]);
//		UART2_SendString((char *)rx_buff);
//		memset(rx_buff,0,256);
//		}
		
		if(Q_Num == 1 && Q_State == 1)
			Q1_Proc();
//		else if(Q_Num == 2 && Q_State == 1)
//			Q2_Proc();
//		else if(Q_Num == 3 && Q_State == 1)
//			Q3_Proc();
//		else if(Q_Num == 4 && Q_State == 1)
//			Q4_Proc();
//		else if(Q_Num == 5 && Q_State == 1)
//			Q5_Proc();
//		Test_Proc();
//		
		
		

		OLED_Proc();
		Key_Proc();
		Gray_Detect();
//		UART_User_Proc();
//		my_printf("%d,%d,%d\r\n",Speed_Left.Target,Speed_Left.Actual,Speed_Left.Error0);
			
//						//获取传感器模拟量结果(有黑白值初始化后返回1 没有返回 0)
//			if(Get_Anolog_Value(&sensor,Anolog)){
//			sprintf((char *)rx_buff,"Anolog %d-%d-%d-%d-%d-%d-%d-%d\r\n",Anolog[0],Anolog[1],Anolog[2],Anolog[3],Anolog[4],Anolog[5],Anolog[6],Anolog[7]);
//			UART2_SendString((char *)rx_buff);
//			memset(rx_buff,0,256);
//			}
		
//		UART_K230_Proc();
//		delay_ms(1);


    }
}

void OLED_Proc(void)
{
	if(u32_OLED_Dly < 1000)
		return;
	u32_OLED_Dly = 0;
	
	OLED_Printf(0,0,OLED_8X16,"Num:%1d State:%1d",Q_Num,Q_State);
	OLED_Printf(0,16,OLED_8X16,"Quan: %1d",Q1_lap_count);
	

//	else if(Q_Num == 1 && Q_State == 1)
//	{
//		OLED_Printf(0,16,OLED_8X16,"Speed:%.3f",speed_mpers);
//		
//	}
//	else if(Q_Num == 2 && Q_State == 1)
//	{
//		OLED_Printf(0,16,OLED_8X16,"Speed:%.3f",speed_mpers);
//	}
//	else if(Q_Num == 3 && Q_State == 1)
//	{
//		OLED_Printf(0,16,OLED_8X16,"Speed:%.3f",speed_mpers);
//	}
//	else if(Q_Num == 4 && Q_State == 1)
//	{
//		OLED_Printf(0,16,OLED_8X16,"Speed:%.3f",speed_mpers);
//	}
//	else if(Q_Num == 5 && Q_State == 1)
//	{
//		OLED_Printf(0,16,OLED_8X16,"Speed:%.3f",speed_mpers);
//	}
	
//	OLED_Printf(0,32,OLED_8X16,"%.2f",speed_mpers);
	OLED_Printf(0,32,OLED_8X16,"%d---%d",Turn_Flag,Turn_Flag);
	OLED_Printf(0,48,OLED_8X16,"%d-%d-%d-%d-%d-%d-%d-%d",(Digtal>>0)&0x01,(Digtal>>1)&0x01,(Digtal>>2)&0x01,(Digtal>>3)&0x01,(Digtal>>4)&0x01,(Digtal>>5)&0x01,(Digtal>>6)&0x01,(Digtal>>7)&0x01);
	
	OLED_Update();
}




void Key_Proc(void)
{
	if(u32_Key_Dly < 20)
		return;
	u32_Key_Dly = 0;
	
	Key_Val = Read_Key();
	if(Key_Val == Key_Val_Old)
		return;
	switch(Key_Val)
	{
		case 1:
			Q_Num = (Q_Num + 1) % 6;
		break;
		case 2:
			Q_State = (Q_State + 1) % 2;
		break;
		case 3:
			if(Q_Num == 1)
				Q1_lap_count = (Q1_lap_count + 1) % 6;
//			Turn_OutAngle.Target += 90;
		break;
		case 4:
//			Q1_lap_count = (Q1_lap_count + 1) % 6;
		break;
	}
	
	
	
	Key_Val_Old = Key_Val;
}


void Data_Proc(void)
{
	

	MyYaw_180_180 = euler.yaw;
	
	MyGyro = Gyro_Actual.Zdata;
	
}



void TIMG_SysTick_INST_IRQHandler(void)
{
	switch(DL_TimerG_getPendingInterrupt(TIMG_SysTick_INST))
	{
		case DL_TIMER_IIDX_ZERO:
			u32_ms++;
			u32_UART_Dly++;
			u32_OLED_Dly++;
			u32_Dist_Dly++;
			u32_Key_Dly++;
			if(u32_ms % 10 == 0)
			{
				quaternion_update();
				quaternion_to_euler();
				Data_Proc();
			
			}
			
			
			if(Turn_PID_Flag == 1)
				Turn_PID_Control();
			if(Track_PID_Flag == 1)
				Track_PID_Control();
			
			
			DL_TimerG_clearInterruptStatus(TIMG_SysTick_INST, DL_TIMER_IIDX_ZERO);
		break;
		default:
			
		break;
	}
}

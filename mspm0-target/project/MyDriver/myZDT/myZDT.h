#ifndef __MYZDT_H
#define __MYZDT_H


#include "ti_msp_dl_config.h"
#include "board.h"
#include "control.h"
#include "Emm_V5.h"



/* 电机控制宏定义 */
#define MOTOR_X_ADDR        0x01          // X轴电机地址
#define MOTOR_Y_ADDR        0x02          // Y轴电机地址
#define MOTOR_X_UART        UART0        // X轴电机串口 xia
#define MOTOR_Y_UART        UART3        // Y轴电机串口 shang
#define MOTOR_MAX_SPEED     30            // 电机最大转速(0.1RPM)
#define MOTOR_ACCEL         0             // 电机加速度(0表示直接启动)
#define MOTOR_SYNC_FLAG     false         // 电机同步标志
#define MOTOR_MAX_ANGLE     50            // 电机最大角度限制(±50°)
#define MOTOR_MAX_ABSOLUTE  false      	  // 电机相对运动标志
	
#define MOTOR_SPEED_X  20      	  // 步进电机X 移动速度 0.1RPM
#define MOTOR_SPEED_Y  20     	  // 步进电机Y 移动速度 0.1RPM







/* 函数声明 */
void Step_Motor_Init(void);                    // 电机初始化
void Step_Motor_Set_Speed(uint8_t addr, int16_t vel);
void Step_Motor_Set_Pos(uint8_t addr,int32_t clk, uint16_t vel);
void Step_Motor_Stop(void);                    // 停止所有电机
void step_motor_proc(void);








#endif


#include "myZDT.h"




void Step_Motor_Init(void)
{
    /* 使能X轴电机 */
    Emm_V5_En_Control(MOTOR_X_UART, MOTOR_X_ADDR, true, MOTOR_SYNC_FLAG);

    /* 使能Y轴电机 */
    Emm_V5_En_Control(MOTOR_Y_UART, MOTOR_Y_ADDR, true, MOTOR_SYNC_FLAG);

    /* 初始停止 */
    Step_Motor_Stop();
}
/**
  * @param    dir ：方向       ，0为CW，其余值为CCW
  * @param    vel ：速度       ，范围0 - 50000 0.1RPM
*/
void Step_Motor_Set_Speed(uint8_t addr, int16_t vel)
{
	/* ------------------ 方向 & 脉冲绝对值 ------------------ */
    uint8_t  dir  = (vel >= 0) ? 0 : 1;         // 0=CW, 1=CCW
    uint32_t step = (vel >= 0) ?  vel : -vel;   // 取绝对值

    /* ----------------------- 分轴发送 --------------------- */
    switch (addr)
    {
        case MOTOR_X_ADDR:   /* X 轴 → UART4 */
            Emm_V5_Vel_Control(MOTOR_X_UART, addr, dir, step, MOTOR_ACCEL, MOTOR_SYNC_FLAG);
            break;

        case MOTOR_Y_ADDR:   /* Y 轴 → UART5 */
            Emm_V5_Vel_Control(MOTOR_Y_UART, addr, dir, step, MOTOR_ACCEL, MOTOR_SYNC_FLAG);
            break;

        default:             /* 非法地址：可加保护 */
            return;
    }
	
	
}



/* ----------------------------------------------------
 * 位置模式：让电机走 ±clk 个脉冲
 * addr   : MOTOR_X_ADDR / MOTOR_Y_ADDR
 * clk    : 有符号脉冲数 (正→CW, 负→CCW)
 * vel    : 运行速度 (0-50000 = 0-5000.0 RPM，单位 0.1 RPM)
 * absolute: true=绝对坐标，false=相对运动
 * ---------------------------------------------------*/
void Step_Motor_Set_Pos(uint8_t addr,int32_t clk, uint16_t vel)
{
    /* ------------------ 方向 & 脉冲绝对值 ------------------ */
    uint8_t  dir  = (clk >= 0) ? 0 : 1;         // 0=CW, 1=CCW
    uint32_t step = (clk >= 0) ?  clk : -clk;   // 取绝对值

    /* ----------------------- 分轴发送 --------------------- */
    switch (addr)
    {
        case MOTOR_X_ADDR:   /* X 轴 → UART4 */
            Emm_V5_Pos_Control(MOTOR_X_UART, addr, dir, vel, MOTOR_ACCEL,
                               step, MOTOR_MAX_ABSOLUTE, MOTOR_SYNC_FLAG);
            break;

        case MOTOR_Y_ADDR:   /* Y 轴 → UART5 */
            Emm_V5_Pos_Control(MOTOR_Y_UART, addr, dir, vel, MOTOR_ACCEL,
                               step, MOTOR_MAX_ABSOLUTE, MOTOR_SYNC_FLAG);
            break;

        default:             /* 非法地址：可加保护 */
            return;
    }
}



/**
 * @brief 停止所有电机
 */
void Step_Motor_Stop(void)
{
    /* 停止X轴电机 */
    Emm_V5_Stop_Now(MOTOR_X_UART, MOTOR_X_ADDR, MOTOR_SYNC_FLAG);

    /* 停止Y轴电机 */
    Emm_V5_Stop_Now(MOTOR_Y_UART, MOTOR_Y_ADDR, MOTOR_SYNC_FLAG);
}

















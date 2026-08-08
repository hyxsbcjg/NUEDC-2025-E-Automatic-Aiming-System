#include "Question.h"
#include "control.h"
#include "encoder.h"
#include "track.h"
#include "peripheral.h"

// Q1变量
uint8_t Q1_lap_count = 0;  // 要跑的圈数（设置为2表示跑2圈）
#define Base_PWM 2600
#define Time_ms 270

void Q1_Proc(void)
{
    static uint8_t state = 0;
    static uint8_t corner_count = 0;
    static uint32_t now_time = 0;
    static float now_yaw = 0;
    static float target_yaw = 0;
    
    switch(state)
    {
        case 0:  // 正常循迹
            if(Turn_Flag == 1)  // 检测到拐角
            {
                if(corner_count < Q1_lap_count * 4)  // 总弯道数 = 圈数 × 4
                {
                    now_yaw = MyYaw_180_180;
                    state = 1;  // 正常转弯
                }
                else  // 已经转够弯了
                {
                    // 停车
                    Track_PID_Flag = 0;
                    Turn_PID_Flag = 0;
                    Load_Motor_A(0);
                    Load_Motor_B(0);
                    state = 6;
                }
            }
            else
            {
                Track.Base = Base_PWM;
                Track_PID_Flag = 1;  // 开循迹环
                Turn_PID_Flag = 0;   // 关转向环
            }
            break;
            
        case 1:  // 检测到拐角，准备转弯
            Turn_OutAngle.Target = now_yaw;
            Track.Base = 0;
            Track_PID_Flag = 0;  // 关循迹环
            Turn_InGyro.Base = Base_PWM;
            Turn_PID_Flag = 1;   // 开转向环 为了直行
            now_time = u32_ms;
            state = 2;
            break;
            
        case 2:  // 直行越过拐角
            if(u32_ms - now_time > Time_ms)  // 直行400ms
            {
                Turn_InGyro.Base = 0;  // 原地转弯
                now_yaw = MyYaw_180_180;
                target_yaw = now_yaw + 90;  // 左转90度
                
                // 角度归一化
                if(target_yaw > 180) target_yaw -= 360;
                if(target_yaw < -180) target_yaw += 360;
                
                state = 3;
            }
            break;
            
        case 3:  // 执行转弯
            Turn_OutAngle.Target = target_yaw;  // 左转90度
            
            // 计算角度误差（考虑环绕）
            float angle_error = target_yaw - MyYaw_180_180;
            if(angle_error > 180) angle_error -= 360;
            if(angle_error < -180) angle_error += 360;
            
            if(floatAbs(angle_error) < 4)
            {
                Turn_PID_Flag = 0;   // 关转向环
                Track_PID_Flag = 1;  // 开循迹环
                Track.Base = Base_PWM;
                now_time = u32_ms;
                corner_count++;      // 拐角计数增加
                state = 4;
            }
            break;
            
        case 4:  // 转弯后继续循迹
            if(u32_ms - now_time > Time_ms)  // 防止重复检测
            {
                state = 0;  // 回到循迹状态
            }
            break;
            
        case 6:  // 结束状态
            // 保持停止
            Track_PID_Flag = 0;
            Turn_PID_Flag = 0;
            Load_Motor_A(0);
            Load_Motor_B(0);
            break;
    }
}
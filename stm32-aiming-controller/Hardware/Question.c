#include "question.h"
#include "key.h"
#include "serial.h"
#include "quaternion.h"
#include "myzdt.h"


//色块变量

//Blob1定义为目标位置 Blob2定义为实际位置
uint16_t Blob1_cx = 0,Blob1_cy = 0;
uint16_t Blob2_cx = 236,Blob2_cy = 126;

//矩形变量

uint8_t Find_Rect_Flag = 0;

//Q3变量
int16_t X_Speed = 0;
uint8_t X_Speed_Idx = 0;

void Find_Blob_Proc(void)
{

}

uint16_t PixelDiff_Calc(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    /* 1. 带符号差值 */
    int16_t dx = (int16_t)x2 - (int16_t)x1;
    int16_t dy = (int16_t)y2 - (int16_t)y1;

    /* 2. 距离平方 (最多 655352+655352 < 2^32) */
    uint32_t d2 = (uint32_t)dx*dx + (uint32_t)dy*dy;

    /* 3. 纯整数平方根（牛顿迭代/位移法，最多 16 次循环） */
    uint32_t res = 0;          /* 结果累积 */
    uint32_t bit = 1UL << 30;  /* 从最高位开始试探 */

    /* 将 bit 降到 <= d2 的最高 2? 位 */
    while (bit > d2) bit >>= 2;

    /* 位移法迭代 */
    while (bit != 0) {
        if (d2 >= res + bit) {
            d2  -= res + bit;
            res += bit << 1;
        }
        res >>= 1;
        bit >>= 2;
    }

    /* 4. 四舍五入：若余数 > 0，距离加 1 */
    if (d2 > res) ++res;

    return (uint16_t)res;  /* <= 65535 */
}



void Q1_Proc(void) 
{

}

void Q2_Proc(void)
{
	static uint8_t state = 0;
	switch(state)
	{
		case 0://启动云台并发送校准信号
			Relay_Light_Set(1);
		break;
	}
}


uint8_t cmd_trigger[128] = {0x02,0x9A,0x00,0x00,0x6B};

//这里实际上(3)和(4)都是问题3，但是特殊处理了


void Q3_Proc(void)  //顺时针
{
	static uint8_t state = 0;

	switch(state)
	{
		case 0:
			
			Trigger_Return(MOTOR_Y_ADDR);
			Step_Motor_Set_Speed(MOTOR_X_ADDR,X_Speed);
			K230_Send_Bytes(0xA1,0xA1);
			state = 1;
		break;
		case 1:
			if(Find_Rect_Flag)
			{
				K230_Send_Bytes(0xA2,0xA2);
				state = 2;
			}
			
		break;
		case 2:
			Blob_Track_Y_PID_Flag = true;
			Blob_Track_X_PID_Flag = true;

			if(dist_center < 4)
			{
				
				Relay_Light_Set(1);
			}
		break;
		
		
		
		
	}
}

void Q4_Proc(void) 
{

}


void Q5_Proc(void) 
{

}

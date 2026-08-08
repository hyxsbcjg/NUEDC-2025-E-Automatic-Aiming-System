#include "track.h"
#include "control.h"
#include "No_Mcu_Ganv_Grayscale_Sensor_Config.h"
#include <string.h>


// --- 修改点 1: 删除了 Weigth_Flag 变量 ---

volatile uint8_t Turn_Flag_Old = 0;
volatile uint8_t Turn_Flag = 0; // 停车标志

int8_t Totol_Weight = 0;
unsigned short Anolog[8] = {0};
unsigned short white[8] = {3053, 3073, 3045, 3025, 3022, 2978, 3081, 3021};
unsigned short black[8] = {806, 1015, 1232, 948, 877, 1313, 1775, 1323};

// --- 修改点 2: 将多维度的 weight_set 修改为单一的、固定的权重数组 ---
// 使用 const 关键字，表示这个数组是只读的常量，是更好的编程习惯。
const int8_t fixed_weight_set[8] = { -3, -2, -2, -1, 0, 1, 2, 3 };


//传感器校准
uint16_t Anolog_black[8];
uint16_t Anolog_white[8];
uint8_t CAL_Flag = 0; //0校准白色 1校准黑色 2完成校准


void Gray_Init(void)
{
    NVIC_ClearPendingIRQ(ADC_INST_INT_IRQN);
    NVIC_EnableIRQ(ADC_INST_INT_IRQN);
    No_MCU_Ganv_Sensor_Init_Frist(&sensor);
    No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);
    Get_Anolog_Value(&sensor, Anolog);
    No_MCU_Ganv_Sensor_Init(&sensor, white, black);
}

static int8_t get_max_continuous_black(const uint8_t ch[])
{
    int max_len = 0, cur_len = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (ch[i] == 0)
            cur_len++;
        else
        {
            if (cur_len > max_len) max_len = cur_len;
            cur_len = 0;
        }
    }
    if (cur_len > max_len) max_len = cur_len;
    return max_len;
}

void Gray_Detect(void)
{
    static int8_t last_weight = 0;
    int8_t sum = 0, count = 0, black_count = 0;
    uint8_t black_segment_count = 0, max_continuous_black = 0;
    uint8_t cur_black = 0, prev = 1;

    No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);
    Digtal = Get_Digtal_For_User(&sensor);

    uint8_t ch[8];
#pragma unroll
    for (int i = 0; i < 8; ++i)
        ch[i] = (Digtal >> i) & 0x01;


    // --- 修改点 3: 直接使用固定的权重数组，不再需要判断 Weigth_Flag ---
    const int8_t *W = fixed_weight_set;

    for (int i = 0; i < 8; ++i)
    {
        if (ch[i] == 0) // 黑线
        {
            sum += W[i];
            count++;
            black_count++;
            cur_black++;

            if (prev == 1) black_segment_count++;  // 新一段黑线开始
            if (cur_black > max_continuous_black)
                max_continuous_black = cur_black;

            prev = 0;
        }
        else
        {
            cur_black = 0;
            prev = 1;
        }
    }

    // 权重输出
    Totol_Weight = (count == 0) ? last_weight : (last_weight = sum);

    // 停车检测（连续黑线 ≥3）
     if(ch[0] == 0 && ch[1] == 0 && ch[2] == 0)
    {
        Turn_Flag = 1;  // 左侧全黑，中间不全黑，说明是拐角
    }
    else
    {
        Turn_Flag = 0;
    }
}




//void Gray_CAL(void)
//{
//	static uint16_t Anolog_temp[8];
//	static uint32_t Anolog_sum[8];
//	static uint32_t Anolog_avg[8];
//	

//	
//	for(uint16_t i = 0;i < 1000;i++)
//	{
//		Get_Anolog_Value(&sensor,Anolog_temp);
//		for(uint16_t j = 0;j < 8;j++)
//		{
//			Anolog_sum[j] += Anolog_temp[j];
//		}
//	}
//	for(uint16_t i = 0;i < 8;i++)
//	{
//		Anolog_avg[i] += Anolog_sum[i] / 1000;
//	}
//	
//	
//	
////	No_MCU_Ganv_Sensor_Init
//	if(CAL_Flag == 1)
//	{
//		for (uint8_t i = 0; i < sizeof(Anolog_temp); i++) 
//		{
//			Anolog_white[i] = Anolog_temp[i];
//		}
//	}
//	else if(CAL_Flag == 2)
//	{
//		for (uint8_t i = 0; i < sizeof(Anolog_temp); i++) 
//		{
//			Anolog_black[i] = Anolog_temp[i];
//		}
//	}
//	else if(CAL_Flag == 3)
//	{
//		No_MCU_Ganv_Sensor_Init(&sensor,(unsigned short *)Anolog_white,(unsigned short *)Anolog_black);
//	}
//}





























//			//获取传感器数字量结果(只有当有黑白值传入进去了之后才会有这个值！！)
//			Digtal=Get_Digtal_For_User(&sensor);
//			sprintf((char *)rx_buff,"Digtal %d-%d-%d-%d-%d-%d-%d-%d\r\n",(Digtal>>0)&0x01,(Digtal>>1)&0x01,(Digtal>>2)&0x01,(Digtal>>3)&0x01,(Digtal>>4)&0x01,(Digtal>>5)&0x01,(Digtal>>6)&0x01,(Digtal>>7)&0x01);
//			uart0_send_string((char *)rx_buff);
//			memset(rx_buff,0,256);
			
			//获取传感器模拟量结果(有黑白值初始化后返回1 没有返回 0)
//			if(Get_Anolog_Value(&sensor,Anolog)){
//			sprintf((char *)rx_buff,"Anolog %d-%d-%d-%d-%d-%d-%d-%d\r\n",Anolog[0],Anolog[1],Anolog[2],Anolog[3],Anolog[4],Anolog[5],Anolog[6],Anolog[7]);
//			uart0_send_string((char *)rx_buff);
//			memset(rx_buff,0,256);
//			}
//			
//			//获取传感器归一化结果(只有当有黑白值传入进去了之后才会有这个值！！有黑白值初始化后返回1 没有返回 0)
//			if(Get_Normalize_For_User(&sensor,Normal)){
//			sprintf((char *)rx_buff,"Normalize %d-%d-%d-%d-%d-%d-%d-%d\r\n",Normal[0],Normal[1],Normal[2],Normal[3],Normal[4],Normal[5],Normal[6],Normal[7]);
//			uart0_send_string((char *)rx_buff);
//			memset(rx_buff,0,256);
//			}
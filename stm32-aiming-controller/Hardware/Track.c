#include "track.h"



unsigned short Anolog[8] = {0};
unsigned short white[8] = {3087, 2699, 2478, 3061, 3132, 2994, 2400, 2789};
unsigned short black[8] = {476, 227, 220, 395, 954, 502, 249, 487};

void Gray_Init(void)
{
	//GPIO_Init
	GPIO_InitTypeDef GPIO_InitStructure={0};
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_WriteBit(GPIOC,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2,Bit_RESET);
	
	
	//ADC_Init
	adc_init();
	
	//Gray_Init
	No_MCU_Ganv_Sensor_Init_Frist(&sensor);
    No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);
    Get_Anolog_Value(&sensor, Anolog);
    No_MCU_Ganv_Sensor_Init(&sensor, white, black);
	
}


			//获取传感器数字量结果(只有当有黑白值传入进去了之后才会有这个值！！)
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




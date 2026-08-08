#include "myvl53l0x.h"

//SCL PA3
//SDA PA2
//XSH PA5
//INT PA4
uint8_t data;
uint8_t len;
uint16_t word;
uint8_t data1;
uint16_t distance_mm;
static char buf[VL53L0X_MAX_STRING_LENGTH];//测试模式字符串字符缓冲区
VL53L0X_Error Status=VL53L0X_ERROR_NONE;//工作状态
u8 mode=0; //0：默认；1：高精度；2：长距离；3：高速度
u32 i;
	
void myvl53l0x_init(void)
{
	
	
	if(vl53l0x_init(&vl53l0x_dev))     //vl53l0x初始化
	 {
//		printf("VL53L0X_Init Error!!!\r\n");
		delay_ms(200);
	 }
	 else
	 {
//		 printf("VL53L0X_Init OK\r\n");
		 
		 VL53L0X_RdByte(&vl53l0x_dev,0xC0,&data);
//		 printf("regc0 = 0x%x\n\r",data);
		
		 VL53L0X_RdByte(&vl53l0x_dev,0xC1,&data1);
//		 printf("regc1 = 0x%x\n\r",data1);
		 
		 VL53L0X_RdByte(&vl53l0x_dev,0xC2,&len);
//		 printf("regc2 = 0x%x\n\r",len); 
		 
		 Status = VL53L0X_WrByte(&vl53l0x_dev,0x51,0x12);
		 VL53L0X_RdByte(&vl53l0x_dev,0x51,&len);  //高四位没法读出来
//		 printf("reg51 = 0x%x\n\r",len);
		 
		 Status = VL53L0X_WrWord(&vl53l0x_dev,0x62,0x9e12);
		 VL53L0X_RdWord(&vl53l0x_dev,0x62,&word);
//		 printf("reg61 = 0x%x\n\r",word);
	 }
	 
	 if(vl53l0x_set_mode(&vl53l0x_dev,mode))   //配置测量模式
		{
//			printf("Mode Set Error!!!\r\n");
		}
//	else
//		printf("Mode Set OK!!!\r\n");
}


void get_distance(void)
{
	
	Status = vl53l0x_start_single_test(&vl53l0x_dev,&vl53l0x_data,buf);//执行一次测量
	if(Status==VL53L0X_ERROR_NONE)
		distance_mm = Distance_data;//打印测量距离
	else
		return;
		
	 //delay_ms(100);
	 
	 
}

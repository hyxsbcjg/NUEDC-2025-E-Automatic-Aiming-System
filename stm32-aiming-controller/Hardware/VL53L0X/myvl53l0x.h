#ifndef __MYVL53L0X_H
#define __MYVL53L0X_H

#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"
#include "vl53l0x_gen.h"
#include "vl53l0x_cali.h"
#include "vl53l0x_it.h"
#include "vl53l0x.h"
#include "sys.h"
#include "delay.h"
#include "serial.h"


extern uint16_t distance_mm;
extern VL53L0X_Dev_t vl53l0x_dev;//设备I2C数据参数

void myvl53l0x_init(void);
void get_distance(void);














#endif


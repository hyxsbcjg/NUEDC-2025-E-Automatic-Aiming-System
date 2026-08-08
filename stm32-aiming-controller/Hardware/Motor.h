#ifndef __MOTOR_H
#define __MOTOR_H



#include "stm32f10x.h"                  // Device header
#include "sys.h"
#include <stdlib.h>
#include "control.h"
#include "PWM.h"


void Load_Motor(int MotorA_PWM,int MotorB_PWM);
void Motor_Init(void);



#define PWM_MAX 7200
#define PWM_MIN -7200



#define AIN1 PAout(5)
#define AIN2 PAout(4)
#define BIN1 PBout(8)
#define BIN2 PAout(12)




#endif


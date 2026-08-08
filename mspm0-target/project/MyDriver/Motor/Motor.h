#ifndef __MOTOR_H
#define __MOTOR_H

#include "ti_msp_dl_config.h"

#define PWM_MAX 7000
#define PWM_MIN -7000


void Motor_Init(void);
void Load_Motor_A(int16_t MotorA_PWM);
void Load_Motor_B(int16_t MotorB_PWM);

#endif

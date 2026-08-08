#ifndef __SERVO_H_
#define __SERVO_H_

#include "board.h"
#include "ti_msp_dl_config.h"



#define SERVO_MAX_PWM       2400                  
#define SERVO_MIN_PWM       600                  
#define SERVO_MID           1500                 



void Load_Servo(int16_t Servo_PWM);



#endif
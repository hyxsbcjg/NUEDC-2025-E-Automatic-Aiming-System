#include "servo.h"
#include "Control.h"


void Load_Servo(int16_t Servo_PWM)
{
	int16_tLimit(&Servo_PWM,SERVO_MIN_PWM,SERVO_MAX_PWM);
	
	DL_Timer_setCaptureCompareValue(SERVO_PWM_INST,Servo_PWM, GPIO_SERVO_PWM_C0_IDX);
}
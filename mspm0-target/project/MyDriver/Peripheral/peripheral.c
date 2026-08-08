#include "peripheral.h"



//1¿ªÆô 0¹Ø±Õ
void Buzzer_Set(uint8_t Val)
{
	if(Val)
		My_GPIO_WriteVal(Peripheral_Buzzer_PORT,Peripheral_Buzzer_PIN,0);
	else
		My_GPIO_WriteVal(Peripheral_Buzzer_PORT,Peripheral_Buzzer_PIN,1);
}
#include "key.h"


uint8_t Read_Key(void)
{
	if(!DL_GPIO_readPins(Key_Key1_PORT,Key_Key1_PIN))
		return 1;
	else if(!DL_GPIO_readPins(Key_Key2_PORT,Key_Key2_PIN))
		return 2;
	else if(!DL_GPIO_readPins(Key_Key3_PORT,Key_Key3_PIN))
		return 3;
	else if(!DL_GPIO_readPins(Key_Key4_PORT,Key_Key4_PIN))
		return 4;
	return 0;
}


void Read_MN_Signal(uint8_t *M_Signal,uint8_t *N_Signal)
{
	
}



//void Read_Signal_MN()
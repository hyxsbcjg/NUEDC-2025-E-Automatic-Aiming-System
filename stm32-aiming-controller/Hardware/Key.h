#ifndef __KEY_H
#define __KEY_H
#include "sys.h" 


void Key_Init(void);
int Key_Read(void);

void Relay_Init(void);
void Relay_Light_Set(uint8_t Val);
void Relay_Gimbal_Set(uint8_t Val);

#endif


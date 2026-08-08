#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>
#include "stm32f10x.h"                  // Device header
#include "sys.h"


void USART1_Init(void);
void USART2_Init(void);
void USART3_Init(void);
void UART4_Init(void);
void UART5_Init(void);



void USART1_SendString(char *String);
void USART2_SendString(char *String);
void USART3_SendString(char *String);
void USART1_K230_Proc(void);
void K230_Send_Bytes(uint8_t d1, uint8_t d2);
int my_printf(const char *format, ...);	

void UART4_Emm_Proc(void);

void USART_SendArray(USART_TypeDef* USARTx, uint8_t *Array, uint16_t Length);




#endif

 /*
 * This file is the serial port configuration file and clock configuration file.
 * forum: https://oshwhub.com/forum
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-26     LCKFB     first version
 */
 
#include "board.h"
#include <stdio.h>

#define RE_0_BUFF_LEN_MAX	128

volatile uint8_t  recv0_buff[RE_0_BUFF_LEN_MAX] = {0};
volatile uint16_t recv0_length = 0;
volatile uint8_t  recv0_flag = 0;

void delay_us(uint32_t __us) { delay_cycles( (CPUCLK_FREQ / 1000 / 1000)*__us); }
void delay_ms(uint32_t __ms) { delay_cycles( (CPUCLK_FREQ / 1000)*__ms); }

void delay_1us(uint32_t __us) { delay_cycles( (CPUCLK_FREQ / 1000 / 1000)*__us); }
void delay_1ms(uint32_t __ms) { delay_cycles( (CPUCLK_FREQ / 1000)*__ms); }


//串口发送单个字符
void uart2_send_char(char ch)
{
	//当串口2忙的时候等待，不忙的时候再发送传进来的字符
	while( DL_UART_isBusy(UART2_User_INST) == true );
	//发送单个字符
	DL_UART_Main_transmitData(UART2_User_INST, ch);

}
//串口发送字符串
void uart2_send_string(char* str)
{
	//当前字符串地址不在结尾 并且 字符串首地址不为空
	while(*str!=0&&str!=0)
	{
		//发送字符串首地址中的字符，并且在发送完成之后首地址自增
		uart2_send_char(*str++);
	}
}


#if !defined(__MICROLIB)
//不使用微库的话就需要添加下面的函数
#if (__ARMCLIB_VERSION <= 6000000)
//如果编译器是AC5  就定义下面这个结构体
struct __FILE
{
	int handle;
};
#endif

FILE __stdout;

//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
	x = x;
}
#endif


//my_printf函数重定义
int fputc(int ch, FILE *stream)
{
	//当串口0忙的时候等待，不忙的时候再发送传进来的字符
	while( DL_UART_isBusy(UART2_User_INST) == true );
	
	DL_UART_Main_transmitData(UART2_User_INST, ch);
	
	return ch;
}

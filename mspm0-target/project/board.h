 /*
 * This file is the serial port configuration file and clock configuration file.
 * forum: https://oshwhub.com/forum
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-26     LCKFB     first version
 */
 
#ifndef	__BOARD_H__
#define __BOARD_H__

#include "ti_msp_dl_config.h"
#include <stdarg.h>
#include <stdio.h>
#include "uart.h" 
#include "quaternion.h"
//关于IMU的个人化
#define SPI2_ReadWriteByte ICM_spi_read_write_byte

//关于LCD的个人化
#define PORTB_PORT GPIOB
#define PORTB_RST_PIN LCD_RES_PIN
#define PORTB_DC_PIN LCD_DC_PIN
#define PORTB_CS_PIN LCD_LCD_CS_PIN
#define SPI_WriteByte LCD_spi_read_write_byte




/* 延时函数 */
void delay_us(uint32_t __us);
void delay_ms(uint32_t __ms);

void delay_1us(uint32_t __us);
void delay_1ms(uint32_t __ms);

extern float MyYaw_180_180;
extern float MyYaw_0_360;




#endif

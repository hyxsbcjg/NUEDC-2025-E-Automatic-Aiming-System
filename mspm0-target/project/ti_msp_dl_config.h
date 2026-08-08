/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)



#define CPUCLK_FREQ                                                     80000000



/* Defines for Motor_PWM */
#define Motor_PWM_INST                                                     TIMA0
#define Motor_PWM_INST_IRQHandler                               TIMA0_IRQHandler
#define Motor_PWM_INST_INT_IRQN                                 (TIMA0_INT_IRQn)
#define Motor_PWM_INST_CLK_FREQ                                         80000000
/* GPIO defines for channel 0 */
#define GPIO_Motor_PWM_C0_PORT                                             GPIOA
#define GPIO_Motor_PWM_C0_PIN                                      DL_GPIO_PIN_8
#define GPIO_Motor_PWM_C0_IOMUX                                  (IOMUX_PINCM19)
#define GPIO_Motor_PWM_C0_IOMUX_FUNC                 IOMUX_PINCM19_PF_TIMA0_CCP0
#define GPIO_Motor_PWM_C0_IDX                                DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_Motor_PWM_C1_PORT                                             GPIOA
#define GPIO_Motor_PWM_C1_PIN                                      DL_GPIO_PIN_7
#define GPIO_Motor_PWM_C1_IOMUX                                  (IOMUX_PINCM14)
#define GPIO_Motor_PWM_C1_IOMUX_FUNC                 IOMUX_PINCM14_PF_TIMA0_CCP1
#define GPIO_Motor_PWM_C1_IDX                                DL_TIMER_CC_1_INDEX

/* Defines for SERVO_PWM */
#define SERVO_PWM_INST                                                     TIMA1
#define SERVO_PWM_INST_IRQHandler                               TIMA1_IRQHandler
#define SERVO_PWM_INST_INT_IRQN                                 (TIMA1_INT_IRQn)
#define SERVO_PWM_INST_CLK_FREQ                                          1000000
/* GPIO defines for channel 0 */
#define GPIO_SERVO_PWM_C0_PORT                                             GPIOB
#define GPIO_SERVO_PWM_C0_PIN                                      DL_GPIO_PIN_2
#define GPIO_SERVO_PWM_C0_IOMUX                                  (IOMUX_PINCM15)
#define GPIO_SERVO_PWM_C0_IOMUX_FUNC                 IOMUX_PINCM15_PF_TIMA1_CCP0
#define GPIO_SERVO_PWM_C0_IDX                                DL_TIMER_CC_0_INDEX



/* Defines for TIMG_SysTick */
#define TIMG_SysTick_INST                                                (TIMG0)
#define TIMG_SysTick_INST_IRQHandler                            TIMG0_IRQHandler
#define TIMG_SysTick_INST_INT_IRQN                              (TIMG0_INT_IRQn)
#define TIMG_SysTick_INST_LOAD_VALUE                                     (1249U)



/* Defines for UART2_User */
#define UART2_User_INST                                                    UART2
#define UART2_User_INST_FREQUENCY                                       40000000
#define UART2_User_INST_IRQHandler                              UART2_IRQHandler
#define UART2_User_INST_INT_IRQN                                  UART2_INT_IRQn
#define GPIO_UART2_User_RX_PORT                                            GPIOA
#define GPIO_UART2_User_TX_PORT                                            GPIOB
#define GPIO_UART2_User_RX_PIN                                    DL_GPIO_PIN_22
#define GPIO_UART2_User_TX_PIN                                    DL_GPIO_PIN_15
#define GPIO_UART2_User_IOMUX_RX                                 (IOMUX_PINCM47)
#define GPIO_UART2_User_IOMUX_TX                                 (IOMUX_PINCM32)
#define GPIO_UART2_User_IOMUX_RX_FUNC                  IOMUX_PINCM47_PF_UART2_RX
#define GPIO_UART2_User_IOMUX_TX_FUNC                  IOMUX_PINCM32_PF_UART2_TX
#define UART2_User_BAUD_RATE                                            (115200)
#define UART2_User_IBRD_40_MHZ_115200_BAUD                                  (21)
#define UART2_User_FBRD_40_MHZ_115200_BAUD                                  (45)
/* Defines for UART0_X */
#define UART0_X_INST                                                       UART0
#define UART0_X_INST_FREQUENCY                                          40000000
#define UART0_X_INST_IRQHandler                                 UART0_IRQHandler
#define UART0_X_INST_INT_IRQN                                     UART0_INT_IRQn
#define GPIO_UART0_X_RX_PORT                                               GPIOB
#define GPIO_UART0_X_TX_PORT                                               GPIOB
#define GPIO_UART0_X_RX_PIN                                        DL_GPIO_PIN_1
#define GPIO_UART0_X_TX_PIN                                        DL_GPIO_PIN_0
#define GPIO_UART0_X_IOMUX_RX                                    (IOMUX_PINCM13)
#define GPIO_UART0_X_IOMUX_TX                                    (IOMUX_PINCM12)
#define GPIO_UART0_X_IOMUX_RX_FUNC                     IOMUX_PINCM13_PF_UART0_RX
#define GPIO_UART0_X_IOMUX_TX_FUNC                     IOMUX_PINCM12_PF_UART0_TX
#define UART0_X_BAUD_RATE                                               (115200)
#define UART0_X_IBRD_40_MHZ_115200_BAUD                                     (21)
#define UART0_X_FBRD_40_MHZ_115200_BAUD                                     (45)
/* Defines for UART1_K230 */
#define UART1_K230_INST                                                    UART1
#define UART1_K230_INST_FREQUENCY                                       40000000
#define UART1_K230_INST_IRQHandler                              UART1_IRQHandler
#define UART1_K230_INST_INT_IRQN                                  UART1_INT_IRQn
#define GPIO_UART1_K230_RX_PORT                                            GPIOA
#define GPIO_UART1_K230_TX_PORT                                            GPIOA
#define GPIO_UART1_K230_RX_PIN                                     DL_GPIO_PIN_9
#define GPIO_UART1_K230_TX_PIN                                    DL_GPIO_PIN_17
#define GPIO_UART1_K230_IOMUX_RX                                 (IOMUX_PINCM20)
#define GPIO_UART1_K230_IOMUX_TX                                 (IOMUX_PINCM39)
#define GPIO_UART1_K230_IOMUX_RX_FUNC                  IOMUX_PINCM20_PF_UART1_RX
#define GPIO_UART1_K230_IOMUX_TX_FUNC                  IOMUX_PINCM39_PF_UART1_TX
#define UART1_K230_BAUD_RATE                                             (38400)
#define UART1_K230_IBRD_40_MHZ_38400_BAUD                                   (65)
#define UART1_K230_FBRD_40_MHZ_38400_BAUD                                    (7)
/* Defines for UART3_Y */
#define UART3_Y_INST                                                       UART3
#define UART3_Y_INST_FREQUENCY                                          80000000
#define UART3_Y_INST_IRQHandler                                 UART3_IRQHandler
#define UART3_Y_INST_INT_IRQN                                     UART3_INT_IRQn
#define GPIO_UART3_Y_RX_PORT                                               GPIOA
#define GPIO_UART3_Y_TX_PORT                                               GPIOB
#define GPIO_UART3_Y_RX_PIN                                       DL_GPIO_PIN_25
#define GPIO_UART3_Y_TX_PIN                                       DL_GPIO_PIN_12
#define GPIO_UART3_Y_IOMUX_RX                                    (IOMUX_PINCM55)
#define GPIO_UART3_Y_IOMUX_TX                                    (IOMUX_PINCM29)
#define GPIO_UART3_Y_IOMUX_RX_FUNC                     IOMUX_PINCM55_PF_UART3_RX
#define GPIO_UART3_Y_IOMUX_TX_FUNC                     IOMUX_PINCM29_PF_UART3_TX
#define UART3_Y_BAUD_RATE                                               (115200)
#define UART3_Y_IBRD_80_MHZ_115200_BAUD                                     (43)
#define UART3_Y_FBRD_80_MHZ_115200_BAUD                                     (26)




/* Defines for SPI_ICM */
#define SPI_ICM_INST                                                       SPI0
#define SPI_ICM_INST_IRQHandler                                 SPI0_IRQHandler
#define SPI_ICM_INST_INT_IRQN                                     SPI0_INT_IRQn
#define GPIO_SPI_ICM_PICO_PORT                                            GPIOB
#define GPIO_SPI_ICM_PICO_PIN                                    DL_GPIO_PIN_17
#define GPIO_SPI_ICM_IOMUX_PICO                                 (IOMUX_PINCM43)
#define GPIO_SPI_ICM_IOMUX_PICO_FUNC                 IOMUX_PINCM43_PF_SPI0_PICO
#define GPIO_SPI_ICM_POCI_PORT                                            GPIOB
#define GPIO_SPI_ICM_POCI_PIN                                    DL_GPIO_PIN_19
#define GPIO_SPI_ICM_IOMUX_POCI                                 (IOMUX_PINCM45)
#define GPIO_SPI_ICM_IOMUX_POCI_FUNC                 IOMUX_PINCM45_PF_SPI0_POCI
/* GPIO configuration for SPI_ICM */
#define GPIO_SPI_ICM_SCLK_PORT                                            GPIOA
#define GPIO_SPI_ICM_SCLK_PIN                                    DL_GPIO_PIN_12
#define GPIO_SPI_ICM_IOMUX_SCLK                                 (IOMUX_PINCM34)
#define GPIO_SPI_ICM_IOMUX_SCLK_FUNC                 IOMUX_PINCM34_PF_SPI0_SCLK



/* Defines for ADC */
#define ADC_INST                                                            ADC0
#define ADC_INST_IRQHandler                                      ADC0_IRQHandler
#define ADC_INST_INT_IRQN                                        (ADC0_INT_IRQn)
#define ADC_ADCMEM_ADC_Channel0                               DL_ADC12_MEM_IDX_0
#define ADC_ADCMEM_ADC_Channel0_REF              DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define ADC_ADCMEM_ADC_Channel0_REF_VOLTAGE_V                                     3.3
#define GPIO_ADC_C0_PORT                                                   GPIOA
#define GPIO_ADC_C0_PIN                                           DL_GPIO_PIN_27



/* Port definition for Pin Group ICM */
#define ICM_PORT                                                         (GPIOB)

/* Defines for CS: GPIOB.13 with pinCMx 30 on package pin 1 */
#define ICM_CS_PIN                                              (DL_GPIO_PIN_13)
#define ICM_CS_IOMUX                                             (IOMUX_PINCM30)
/* Port definition for Pin Group OLED_IIC */
#define OLED_IIC_PORT                                                    (GPIOB)

/* Defines for OLED_IIC_SCL: GPIOB.9 with pinCMx 26 on package pin 61 */
#define OLED_IIC_OLED_IIC_SCL_PIN                                (DL_GPIO_PIN_9)
#define OLED_IIC_OLED_IIC_SCL_IOMUX                              (IOMUX_PINCM26)
/* Defines for OLED_IIC_SDA: GPIOB.8 with pinCMx 25 on package pin 60 */
#define OLED_IIC_OLED_IIC_SDA_PIN                                (DL_GPIO_PIN_8)
#define OLED_IIC_OLED_IIC_SDA_IOMUX                              (IOMUX_PINCM25)
/* Port definition for Pin Group Motor_A */
#define Motor_A_PORT                                                     (GPIOA)

/* Defines for AIN1: GPIOA.31 with pinCMx 6 on package pin 39 */
#define Motor_A_AIN1_PIN                                        (DL_GPIO_PIN_31)
#define Motor_A_AIN1_IOMUX                                        (IOMUX_PINCM6)
/* Defines for AIN2: GPIOA.28 with pinCMx 3 on package pin 35 */
#define Motor_A_AIN2_PIN                                        (DL_GPIO_PIN_28)
#define Motor_A_AIN2_IOMUX                                        (IOMUX_PINCM3)
/* Defines for BIN1: GPIOA.1 with pinCMx 2 on package pin 34 */
#define Motor_B_BIN1_PORT                                                (GPIOA)
#define Motor_B_BIN1_PIN                                         (DL_GPIO_PIN_1)
#define Motor_B_BIN1_IOMUX                                        (IOMUX_PINCM2)
/* Defines for BIN2: GPIOB.21 with pinCMx 49 on package pin 20 */
#define Motor_B_BIN2_PORT                                                (GPIOB)
#define Motor_B_BIN2_PIN                                        (DL_GPIO_PIN_21)
#define Motor_B_BIN2_IOMUX                                       (IOMUX_PINCM49)
/* Port definition for Pin Group Gray_Addr */
#define Gray_Addr_PORT                                                   (GPIOB)

/* Defines for AD0: GPIOB.14 with pinCMx 31 on package pin 2 */
#define Gray_Addr_AD0_PIN                                       (DL_GPIO_PIN_14)
#define Gray_Addr_AD0_IOMUX                                      (IOMUX_PINCM31)
/* Defines for AD1: GPIOB.11 with pinCMx 28 on package pin 63 */
#define Gray_Addr_AD1_PIN                                       (DL_GPIO_PIN_11)
#define Gray_Addr_AD1_IOMUX                                      (IOMUX_PINCM28)
/* Defines for AD2: GPIOB.10 with pinCMx 27 on package pin 62 */
#define Gray_Addr_AD2_PIN                                       (DL_GPIO_PIN_10)
#define Gray_Addr_AD2_IOMUX                                      (IOMUX_PINCM27)
/* Defines for Key1: GPIOA.14 with pinCMx 36 on package pin 7 */
#define Key_Key1_PORT                                                    (GPIOA)
#define Key_Key1_PIN                                            (DL_GPIO_PIN_14)
#define Key_Key1_IOMUX                                           (IOMUX_PINCM36)
/* Defines for Key2: GPIOB.20 with pinCMx 48 on package pin 19 */
#define Key_Key2_PORT                                                    (GPIOB)
#define Key_Key2_PIN                                            (DL_GPIO_PIN_20)
#define Key_Key2_IOMUX                                           (IOMUX_PINCM48)
/* Defines for Key3: GPIOA.16 with pinCMx 38 on package pin 9 */
#define Key_Key3_PORT                                                    (GPIOA)
#define Key_Key3_PIN                                            (DL_GPIO_PIN_16)
#define Key_Key3_IOMUX                                           (IOMUX_PINCM38)
/* Defines for Key4: GPIOB.25 with pinCMx 56 on package pin 27 */
#define Key_Key4_PORT                                                    (GPIOB)
#define Key_Key4_PIN                                            (DL_GPIO_PIN_25)
#define Key_Key4_IOMUX                                           (IOMUX_PINCM56)
/* Defines for Buzzer: GPIOB.24 with pinCMx 52 on package pin 23 */
#define Peripheral_Buzzer_PORT                                           (GPIOB)
#define Peripheral_Buzzer_PIN                                   (DL_GPIO_PIN_24)
#define Peripheral_Buzzer_IOMUX                                  (IOMUX_PINCM52)
/* Defines for LED_1: GPIOA.26 with pinCMx 59 on package pin 30 */
#define Peripheral_LED_1_PORT                                            (GPIOA)
#define Peripheral_LED_1_PIN                                    (DL_GPIO_PIN_26)
#define Peripheral_LED_1_IOMUX                                   (IOMUX_PINCM59)
/* Defines for LED_2: GPIOB.22 with pinCMx 50 on package pin 21 */
#define Peripheral_LED_2_PORT                                            (GPIOB)
#define Peripheral_LED_2_PIN                                    (DL_GPIO_PIN_22)
#define Peripheral_LED_2_IOMUX                                   (IOMUX_PINCM50)
/* Defines for LED_3: GPIOA.29 with pinCMx 4 on package pin 36 */
#define Peripheral_LED_3_PORT                                            (GPIOA)
#define Peripheral_LED_3_PIN                                    (DL_GPIO_PIN_29)
#define Peripheral_LED_3_IOMUX                                    (IOMUX_PINCM4)
/* Port definition for Pin Group Signal */
#define Signal_PORT                                                      (GPIOB)

/* Defines for PIN_M: GPIOB.23 with pinCMx 51 on package pin 22 */
#define Signal_PIN_M_PIN                                        (DL_GPIO_PIN_23)
#define Signal_PIN_M_IOMUX                                       (IOMUX_PINCM51)
/* Defines for PIN_N: GPIOB.26 with pinCMx 57 on package pin 28 */
#define Signal_PIN_N_PIN                                        (DL_GPIO_PIN_26)
#define Signal_PIN_N_IOMUX                                       (IOMUX_PINCM57)

/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_Motor_PWM_init(void);
void SYSCFG_DL_SERVO_PWM_init(void);
void SYSCFG_DL_TIMG_SysTick_init(void);
void SYSCFG_DL_UART2_User_init(void);
void SYSCFG_DL_UART0_X_init(void);
void SYSCFG_DL_UART1_K230_init(void);
void SYSCFG_DL_UART3_Y_init(void);
void SYSCFG_DL_SPI_ICM_init(void);
void SYSCFG_DL_ADC_init(void);


bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */

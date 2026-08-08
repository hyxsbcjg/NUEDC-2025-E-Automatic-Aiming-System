/*
 * Copyright (c) 2023, Texas Instruments Incorporated
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
 *  ============ ti_msp_dl_config.c =============
 *  Configured MSPM0 DriverLib module definitions
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */

#include "ti_msp_dl_config.h"

DL_TimerA_backupConfig gMotor_PWMBackup;
DL_TimerA_backupConfig gSERVO_PWMBackup;
DL_UART_Main_backupConfig gUART3_YBackup;
DL_SPI_backupConfig gSPI_ICMBackup;

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */
SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    /* Module-Specific Initializations*/
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_Motor_PWM_init();
    SYSCFG_DL_SERVO_PWM_init();
    SYSCFG_DL_TIMG_SysTick_init();
    SYSCFG_DL_UART2_User_init();
    SYSCFG_DL_UART0_X_init();
    SYSCFG_DL_UART1_K230_init();
    SYSCFG_DL_UART3_Y_init();
    SYSCFG_DL_SPI_ICM_init();
    SYSCFG_DL_ADC_init();
    /* Ensure backup structures have no valid state */
	gMotor_PWMBackup.backupRdy 	= false;
	gSERVO_PWMBackup.backupRdy 	= false;

	gUART3_YBackup.backupRdy 	= false;
	gSPI_ICMBackup.backupRdy 	= false;

}
/*
 * User should take care to save and restore register configuration in application.
 * See Retention Configuration section for more details.
 */
SYSCONFIG_WEAK bool SYSCFG_DL_saveConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_saveConfiguration(Motor_PWM_INST, &gMotor_PWMBackup);
	retStatus &= DL_TimerA_saveConfiguration(SERVO_PWM_INST, &gSERVO_PWMBackup);
	retStatus &= DL_UART_Main_saveConfiguration(UART3_Y_INST, &gUART3_YBackup);
	retStatus &= DL_SPI_saveConfiguration(SPI_ICM_INST, &gSPI_ICMBackup);

    return retStatus;
}


SYSCONFIG_WEAK bool SYSCFG_DL_restoreConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_restoreConfiguration(Motor_PWM_INST, &gMotor_PWMBackup, false);
	retStatus &= DL_TimerA_restoreConfiguration(SERVO_PWM_INST, &gSERVO_PWMBackup, false);
	retStatus &= DL_UART_Main_restoreConfiguration(UART3_Y_INST, &gUART3_YBackup);
	retStatus &= DL_SPI_restoreConfiguration(SPI_ICM_INST, &gSPI_ICMBackup);

    return retStatus;
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_TimerA_reset(Motor_PWM_INST);
    DL_TimerA_reset(SERVO_PWM_INST);
    DL_TimerG_reset(TIMG_SysTick_INST);
    DL_UART_Main_reset(UART2_User_INST);
    DL_UART_Main_reset(UART0_X_INST);
    DL_UART_Main_reset(UART1_K230_INST);
    DL_UART_Main_reset(UART3_Y_INST);
    DL_SPI_reset(SPI_ICM_INST);
    DL_ADC12_reset(ADC_INST);

    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_TimerA_enablePower(Motor_PWM_INST);
    DL_TimerA_enablePower(SERVO_PWM_INST);
    DL_TimerG_enablePower(TIMG_SysTick_INST);
    DL_UART_Main_enablePower(UART2_User_INST);
    DL_UART_Main_enablePower(UART0_X_INST);
    DL_UART_Main_enablePower(UART1_K230_INST);
    DL_UART_Main_enablePower(UART3_Y_INST);
    DL_SPI_enablePower(SPI_ICM_INST);
    DL_ADC12_enablePower(ADC_INST);
    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{

    DL_GPIO_initPeripheralOutputFunction(GPIO_Motor_PWM_C0_IOMUX,GPIO_Motor_PWM_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_Motor_PWM_C0_PORT, GPIO_Motor_PWM_C0_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_Motor_PWM_C1_IOMUX,GPIO_Motor_PWM_C1_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_Motor_PWM_C1_PORT, GPIO_Motor_PWM_C1_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_SERVO_PWM_C0_IOMUX,GPIO_SERVO_PWM_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_SERVO_PWM_C0_PORT, GPIO_SERVO_PWM_C0_PIN);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART2_User_IOMUX_TX, GPIO_UART2_User_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART2_User_IOMUX_RX, GPIO_UART2_User_IOMUX_RX_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART0_X_IOMUX_TX, GPIO_UART0_X_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART0_X_IOMUX_RX, GPIO_UART0_X_IOMUX_RX_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART1_K230_IOMUX_TX, GPIO_UART1_K230_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART1_K230_IOMUX_RX, GPIO_UART1_K230_IOMUX_RX_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART3_Y_IOMUX_TX, GPIO_UART3_Y_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART3_Y_IOMUX_RX, GPIO_UART3_Y_IOMUX_RX_FUNC);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_ICM_IOMUX_SCLK, GPIO_SPI_ICM_IOMUX_SCLK_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_ICM_IOMUX_PICO, GPIO_SPI_ICM_IOMUX_PICO_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_SPI_ICM_IOMUX_POCI, GPIO_SPI_ICM_IOMUX_POCI_FUNC);

    DL_GPIO_initDigitalOutput(ICM_CS_IOMUX);

    DL_GPIO_initDigitalOutput(OLED_IIC_OLED_IIC_SCL_IOMUX);

    DL_GPIO_initDigitalOutput(OLED_IIC_OLED_IIC_SDA_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_A_AIN1_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_A_AIN2_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_B_BIN1_IOMUX);

    DL_GPIO_initDigitalOutput(Motor_B_BIN2_IOMUX);

    DL_GPIO_initDigitalOutput(Gray_Addr_AD0_IOMUX);

    DL_GPIO_initDigitalOutput(Gray_Addr_AD1_IOMUX);

    DL_GPIO_initDigitalOutput(Gray_Addr_AD2_IOMUX);

    DL_GPIO_initDigitalInputFeatures(Key_Key1_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Key_Key2_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Key_Key3_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Key_Key4_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalOutput(Peripheral_Buzzer_IOMUX);

    DL_GPIO_initDigitalOutput(Peripheral_LED_1_IOMUX);

    DL_GPIO_initDigitalOutput(Peripheral_LED_2_IOMUX);

    DL_GPIO_initDigitalOutput(Peripheral_LED_3_IOMUX);

    DL_GPIO_initDigitalInputFeatures(Signal_PIN_M_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(Signal_PIN_N_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_clearPins(GPIOA, Motor_A_AIN1_PIN |
		Motor_A_AIN2_PIN |
		Motor_B_BIN1_PIN |
		Peripheral_LED_1_PIN |
		Peripheral_LED_3_PIN);
    DL_GPIO_enableOutput(GPIOA, Motor_A_AIN1_PIN |
		Motor_A_AIN2_PIN |
		Motor_B_BIN1_PIN |
		Peripheral_LED_1_PIN |
		Peripheral_LED_3_PIN);
    DL_GPIO_clearPins(GPIOB, OLED_IIC_OLED_IIC_SCL_PIN |
		OLED_IIC_OLED_IIC_SDA_PIN |
		Motor_B_BIN2_PIN |
		Gray_Addr_AD0_PIN |
		Gray_Addr_AD1_PIN |
		Gray_Addr_AD2_PIN |
		Peripheral_Buzzer_PIN |
		Peripheral_LED_2_PIN);
    DL_GPIO_setPins(GPIOB, ICM_CS_PIN);
    DL_GPIO_enableOutput(GPIOB, ICM_CS_PIN |
		OLED_IIC_OLED_IIC_SCL_PIN |
		OLED_IIC_OLED_IIC_SDA_PIN |
		Motor_B_BIN2_PIN |
		Gray_Addr_AD0_PIN |
		Gray_Addr_AD1_PIN |
		Gray_Addr_AD2_PIN |
		Peripheral_Buzzer_PIN |
		Peripheral_LED_2_PIN);

}


static const DL_SYSCTL_SYSPLLConfig gSYSPLLConfig = {
    .inputFreq              = DL_SYSCTL_SYSPLL_INPUT_FREQ_16_32_MHZ,
	.rDivClk2x              = 3,
	.rDivClk1               = 0,
	.rDivClk0               = 0,
	.enableCLK2x            = DL_SYSCTL_SYSPLL_CLK2X_ENABLE,
	.enableCLK1             = DL_SYSCTL_SYSPLL_CLK1_DISABLE,
	.enableCLK0             = DL_SYSCTL_SYSPLL_CLK0_DISABLE,
	.sysPLLMCLK             = DL_SYSCTL_SYSPLL_MCLK_CLK2X,
	.sysPLLRef              = DL_SYSCTL_SYSPLL_REF_SYSOSC,
	.qDiv                   = 9,
	.pDiv                   = DL_SYSCTL_SYSPLL_PDIV_2
};
SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{

	//Low Power Mode is configured to be SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);
    DL_SYSCTL_setFlashWaitState(DL_SYSCTL_FLASH_WAIT_STATE_2);

    
	DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
	/* Set default configuration */
	DL_SYSCTL_disableHFXT();
	DL_SYSCTL_disableSYSPLL();
    DL_SYSCTL_configSYSPLL((DL_SYSCTL_SYSPLLConfig *) &gSYSPLLConfig);
    DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_2);
    DL_SYSCTL_setMCLKSource(SYSOSC, HSCLK, DL_SYSCTL_HSCLK_SOURCE_SYSPLL);

}


/*
 * Timer clock configuration to be sourced by  / 1 (80000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   80000000 Hz = 80000000 Hz / (1 * (0 + 1))
 */
static const DL_TimerA_ClockConfig gMotor_PWMClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 0U
};

static const DL_TimerA_PWMConfig gMotor_PWMConfig = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN_UP,
    .period = 8000,
    .isTimerWithFourCC = true,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_Motor_PWM_init(void) {

    DL_TimerA_setClockConfig(
        Motor_PWM_INST, (DL_TimerA_ClockConfig *) &gMotor_PWMClockConfig);

    DL_TimerA_initPWMMode(
        Motor_PWM_INST, (DL_TimerA_PWMConfig *) &gMotor_PWMConfig);

    // Set Counter control to the smallest CC index being used
    DL_TimerA_setCounterControl(Motor_PWM_INST,DL_TIMER_CZC_CCCTL0_ZCOND,DL_TIMER_CAC_CCCTL0_ACOND,DL_TIMER_CLC_CCCTL0_LCOND);

    DL_TimerA_setCaptureCompareOutCtl(Motor_PWM_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_0_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(Motor_PWM_INST, DL_TIMER_CC_UPDATE_METHOD_ZERO_EVT, DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
    DL_TimerA_setCaptureCompareValue(Motor_PWM_INST, 0, DL_TIMER_CC_0_INDEX);

    DL_TimerA_setCaptureCompareOutCtl(Motor_PWM_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_1_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(Motor_PWM_INST, DL_TIMER_CC_UPDATE_METHOD_ZERO_EVT, DL_TIMERA_CAPTURE_COMPARE_1_INDEX);
    DL_TimerA_setCaptureCompareValue(Motor_PWM_INST, 0, DL_TIMER_CC_1_INDEX);

    DL_TimerA_enableClock(Motor_PWM_INST);


    
    DL_TimerA_setCCPDirection(Motor_PWM_INST , DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT );
    DL_TimerA_enableShadowFeatures(Motor_PWM_INST);


}
/*
 * Timer clock configuration to be sourced by  / 8 (10000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   1000000 Hz = 10000000 Hz / (8 * (9 + 1))
 */
static const DL_TimerA_ClockConfig gSERVO_PWMClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_8,
    .prescale = 9U
};

static const DL_TimerA_PWMConfig gSERVO_PWMConfig = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN_UP,
    .period = 20000,
    .isTimerWithFourCC = true,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_SERVO_PWM_init(void) {

    DL_TimerA_setClockConfig(
        SERVO_PWM_INST, (DL_TimerA_ClockConfig *) &gSERVO_PWMClockConfig);

    DL_TimerA_initPWMMode(
        SERVO_PWM_INST, (DL_TimerA_PWMConfig *) &gSERVO_PWMConfig);

    // Set Counter control to the smallest CC index being used
    DL_TimerA_setCounterControl(SERVO_PWM_INST,DL_TIMER_CZC_CCCTL0_ZCOND,DL_TIMER_CAC_CCCTL0_ACOND,DL_TIMER_CLC_CCCTL0_LCOND);

    DL_TimerA_setCaptureCompareOutCtl(SERVO_PWM_INST, DL_TIMER_CC_OCTL_INIT_VAL_HIGH,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_0_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(SERVO_PWM_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
    DL_TimerA_setCaptureCompareValue(SERVO_PWM_INST, 1500, DL_TIMER_CC_0_INDEX);

    DL_TimerA_enableClock(SERVO_PWM_INST);


    
    DL_TimerA_setCCPDirection(SERVO_PWM_INST , DL_TIMER_CC0_OUTPUT );


}



/*
 * Timer clock configuration to be sourced by BUSCLK /  (5000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   1250000 Hz = 5000000 Hz / (8 * (3 + 1))
 */
static const DL_TimerG_ClockConfig gTIMG_SysTickClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_8,
    .prescale    = 3U,
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * TIMG_SysTick_INST_LOAD_VALUE = (1 ms * 1250000 Hz) - 1
 */
static const DL_TimerG_TimerConfig gTIMG_SysTickTimerConfig = {
    .period     = TIMG_SysTick_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_TIMG_SysTick_init(void) {

    DL_TimerG_setClockConfig(TIMG_SysTick_INST,
        (DL_TimerG_ClockConfig *) &gTIMG_SysTickClockConfig);

    DL_TimerG_initTimerMode(TIMG_SysTick_INST,
        (DL_TimerG_TimerConfig *) &gTIMG_SysTickTimerConfig);
    DL_TimerG_enableInterrupt(TIMG_SysTick_INST , DL_TIMERG_INTERRUPT_ZERO_EVENT);
    DL_TimerG_enableClock(TIMG_SysTick_INST);





}



static const DL_UART_Main_ClockConfig gUART2_UserClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART2_UserConfig = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART2_User_init(void)
{
    DL_UART_Main_setClockConfig(UART2_User_INST, (DL_UART_Main_ClockConfig *) &gUART2_UserClockConfig);

    DL_UART_Main_init(UART2_User_INST, (DL_UART_Main_Config *) &gUART2_UserConfig);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115190.78
     */
    DL_UART_Main_setOversampling(UART2_User_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART2_User_INST, UART2_User_IBRD_40_MHZ_115200_BAUD, UART2_User_FBRD_40_MHZ_115200_BAUD);


    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART2_User_INST,
                                 DL_UART_MAIN_INTERRUPT_RX);


    DL_UART_Main_enable(UART2_User_INST);
}

static const DL_UART_Main_ClockConfig gUART0_XClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART0_XConfig = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART0_X_init(void)
{
    DL_UART_Main_setClockConfig(UART0_X_INST, (DL_UART_Main_ClockConfig *) &gUART0_XClockConfig);

    DL_UART_Main_init(UART0_X_INST, (DL_UART_Main_Config *) &gUART0_XConfig);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115190.78
     */
    DL_UART_Main_setOversampling(UART0_X_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART0_X_INST, UART0_X_IBRD_40_MHZ_115200_BAUD, UART0_X_FBRD_40_MHZ_115200_BAUD);



    DL_UART_Main_enable(UART0_X_INST);
}

static const DL_UART_Main_ClockConfig gUART1_K230ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART1_K230Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART1_K230_init(void)
{
    DL_UART_Main_setClockConfig(UART1_K230_INST, (DL_UART_Main_ClockConfig *) &gUART1_K230ClockConfig);

    DL_UART_Main_init(UART1_K230_INST, (DL_UART_Main_Config *) &gUART1_K230Config);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 38400
     *  Actual baud rate: 38396.93
     */
    DL_UART_Main_setOversampling(UART1_K230_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART1_K230_INST, UART1_K230_IBRD_40_MHZ_38400_BAUD, UART1_K230_FBRD_40_MHZ_38400_BAUD);


    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART1_K230_INST,
                                 DL_UART_MAIN_INTERRUPT_RX);


    DL_UART_Main_enable(UART1_K230_INST);
}

static const DL_UART_Main_ClockConfig gUART3_YClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART3_YConfig = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART3_Y_init(void)
{
    DL_UART_Main_setClockConfig(UART3_Y_INST, (DL_UART_Main_ClockConfig *) &gUART3_YClockConfig);

    DL_UART_Main_init(UART3_Y_INST, (DL_UART_Main_Config *) &gUART3_YConfig);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115190.78
     */
    DL_UART_Main_setOversampling(UART3_Y_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART3_Y_INST, UART3_Y_IBRD_80_MHZ_115200_BAUD, UART3_Y_FBRD_80_MHZ_115200_BAUD);



    DL_UART_Main_enable(UART3_Y_INST);
}

static const DL_SPI_Config gSPI_ICM_config = {
    .mode        = DL_SPI_MODE_CONTROLLER,
    .frameFormat = DL_SPI_FRAME_FORMAT_MOTO3_POL0_PHA0,
    .parity      = DL_SPI_PARITY_NONE,
    .dataSize    = DL_SPI_DATA_SIZE_8,
    .bitOrder    = DL_SPI_BIT_ORDER_MSB_FIRST,
};

static const DL_SPI_ClockConfig gSPI_ICM_clockConfig = {
    .clockSel    = DL_SPI_CLOCK_BUSCLK,
    .divideRatio = DL_SPI_CLOCK_DIVIDE_RATIO_1
};

SYSCONFIG_WEAK void SYSCFG_DL_SPI_ICM_init(void) {
    DL_SPI_setClockConfig(SPI_ICM_INST, (DL_SPI_ClockConfig *) &gSPI_ICM_clockConfig);

    DL_SPI_init(SPI_ICM_INST, (DL_SPI_Config *) &gSPI_ICM_config);

    /* Configure Controller mode */
    /*
     * Set the bit rate clock divider to generate the serial output clock
     *     outputBitRate = (spiInputClock) / ((1 + SCR) * 2)
     *     5000000 = (80000000)/((1 + 7) * 2)
     */
    DL_SPI_setBitRateSerialClockDivider(SPI_ICM_INST, 7);
    /* Set RX and TX FIFO threshold levels */
    DL_SPI_setFIFOThreshold(SPI_ICM_INST, DL_SPI_RX_FIFO_LEVEL_1_2_FULL, DL_SPI_TX_FIFO_LEVEL_1_2_EMPTY);

    /* Enable module */
    DL_SPI_enable(SPI_ICM_INST);
}

/* ADC Initialization */
static const DL_ADC12_ClockConfig gADCClockConfig = {
    .clockSel       = DL_ADC12_CLOCK_SYSOSC,
    .divideRatio    = DL_ADC12_CLOCK_DIVIDE_8,
    .freqRange      = DL_ADC12_CLOCK_FREQ_RANGE_24_TO_32,
};
SYSCONFIG_WEAK void SYSCFG_DL_ADC_init(void)
{
    DL_ADC12_setClockConfig(ADC_INST, (DL_ADC12_ClockConfig *) &gADCClockConfig);
    DL_ADC12_configConversionMem(ADC_INST, ADC_ADCMEM_ADC_Channel0,
        DL_ADC12_INPUT_CHAN_0, DL_ADC12_REFERENCE_VOLTAGE_VDDA, DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0, DL_ADC12_AVERAGING_MODE_DISABLED,
        DL_ADC12_BURN_OUT_SOURCE_DISABLED, DL_ADC12_TRIGGER_MODE_AUTO_NEXT, DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
    DL_ADC12_setPowerDownMode(ADC_INST,DL_ADC12_POWER_DOWN_MODE_MANUAL);
    DL_ADC12_setSampleTime0(ADC_INST,500);
    /* Enable ADC12 interrupt */
    DL_ADC12_clearInterruptStatus(ADC_INST,(DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED
		 | DL_ADC12_INTERRUPT_OVERFLOW));
    DL_ADC12_enableInterrupt(ADC_INST,(DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED
		 | DL_ADC12_INTERRUPT_OVERFLOW));
    DL_ADC12_enableConversions(ADC_INST);
}


#include <stdio.h>
#include "inv_imu_driver.h"
#include "board.h"


#define ICM_USE_HARD_SPI
//#define ICM_USE_I2C

#if defined(ICM_USE_HARD_SPI)
#include "myspi.h"
#elif defined(ICM_USE_I2C)
#include "myiic.h"
#define ICM_I2C_ADDR 0x69
#endif

#define UI_I2C  0 /**< identifies I2C interface. */
#define UI_SPI4 1 /**< identifies 4-wire SPI interface. */

//#define	SPI_IMU_CS PAout(2)  //选中IMU	
#define ICM_CS_CLR  DL_GPIO_clearPins(ICM_PORT, ICM_CS_PIN) //CS = 0
#define ICM_CS_SET  DL_GPIO_setPins(ICM_PORT, ICM_CS_PIN) //CS = 1

#define INV_MSG(level,msg, ...) 	      my_printf("%d," msg "\r\n", __LINE__, ##__VA_ARGS__)

static inv_imu_device_t  imu_dev; /* Driver structure */

//static uint8_t discard_accel_samples; /* Indicates how many accel samples should be discarded */
//static uint8_t discard_gyro_samples; /* Indicates how many gyro samples should be discarded */



int si_print_error_if_any(int rc);
#define SI_CHECK_RC(rc)                                                                            \
	do {                                                                                           \
		if (si_print_error_if_any(rc)) {                                                           \
			INV_MSG(INV_MSG_LEVEL_ERROR, "At %s (line %d)", __FILE__, __LINE__);                   \
			delay_ms(100);                                                                   \
			return rc;                                                                             \
		}                                                                                          \
	} while (0)

/*
 * Error codes
 */
int si_print_error_if_any(int rc)
{
	if (rc != 0) {
		switch (rc) {
		case INV_IMU_ERROR:
			my_printf("Unspecified error (%d)", rc);
			break;
		case INV_IMU_ERROR_TRANSPORT:
			my_printf("Error occurred at transport level (%d)", rc);
			break;
		case INV_IMU_ERROR_TIMEOUT:
			my_printf("Action did not complete in the expected time window (%d)",rc);
			break;
		case INV_IMU_ERROR_BAD_ARG:
			my_printf("Invalid argument provided (%d)", rc);
			break;
		case INV_IMU_ERROR_EDMP_BUF_EMPTY:
			my_printf("EDMP buffer is empty (%d)", rc);
			break;
		default:
			my_printf("Unknown error (%d)", rc);
			break;
		}
	}

	return rc;
}
/*******************************************************************************
* 名    称： icm42688_read_regs
* 功    能： 连续读取多个寄存器的值
* 入口参数： reg: 起始寄存器地址 *buf数据指针,uint16_t len长度
* 出口参数： 无
* 作　　者： Baxiange
* 创建日期： 2024-07-25
* 修    改：
* 修改日期：
* 备    注： 使用SPI读取寄存器时要注意:最高位为读写位，详见datasheet page50.
*******************************************************************************/
static int icm45686_read_regs(uint8_t reg, uint8_t* buf, uint32_t len)
{
#if defined(ICM_USE_HARD_SPI)
    reg |= 0x80;
    ICM_CS_CLR;
    /* 写入要读的寄存器地址 */
    SPI2_ReadWriteByte(reg);
    /* 读取寄存器数据 */
    while(len)
	{
		*buf = SPI2_ReadWriteByte(0x00);
		len--;
		buf++;
	}
    ICM_CS_SET;
#elif defined(ICM_USE_I2C)
	IICreadBytes(ICM_I2C_ADDR, reg, len, buf);
#endif
	return 0;
}

static uint8_t io_write_reg(uint8_t reg, uint8_t value)
{
#if defined(ICM_USE_HARD_SPI)
    ICM_CS_CLR;
    /* 写入要读的寄存器地址 */
    /* 写入要读的寄存器地址 */
    SPI2_ReadWriteByte(reg);
    /* 读取寄存器数据 */
    SPI2_ReadWriteByte(value);
    ICM_CS_SET;
#elif defined(ICM_USE_I2C)
	IICwriteBytes(ICM_I2C_ADDR, reg, 1, &value);
#endif
    return 0;
}

static int icm45686_write_regs(uint8_t reg, const uint8_t* buf, uint32_t len)
{
	int rc;

	for (uint32_t i = 0; i < len; i++) 
	{
		rc = io_write_reg(reg + i, buf[i]);
		if (rc)
			return rc;
	}
	return 0;
}

/* Initializes IMU device and apply configuration. */
int setup_imu(int use_ln, int accel_en, int gyro_en)
{
	int                      rc     = 0;
	uint8_t                  whoami = 0;
	inv_imu_int_pin_config_t int_pin_config;
	inv_imu_int_state_t      int_config;

	/* Init transport layer */
	imu_dev.transport.read_reg   = icm45686_read_regs;
	imu_dev.transport.write_reg  = icm45686_write_regs;
	imu_dev.transport.serif_type = UI_SPI4;
	imu_dev.transport.sleep_us   = delay_us;

	/* Wait 3 ms to ensure device is properly supplied  */
	delay_us(3000);

	/* In SPI, configure slew-rate to prevent bus corruption on DK-SMARTMOTION-REVG */
	if (imu_dev.transport.serif_type == UI_SPI3 || imu_dev.transport.serif_type == UI_SPI4) {
		drive_config0_t drive_config0;
		drive_config0.pads_spi_slew = DRIVE_CONFIG0_PADS_SPI_SLEW_TYP_10NS;
		rc |= inv_imu_write_reg(&imu_dev, DRIVE_CONFIG0, 1, (uint8_t *)&drive_config0);
		SI_CHECK_RC(rc);
		delay_us(2); /* Takes effect 1.5 us after the register is programmed */
	}

	/* Check whoami */
	rc |= inv_imu_get_who_am_i(&imu_dev, &whoami);
	SI_CHECK_RC(rc);
	if (whoami != INV_IMU_WHOAMI) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "Erroneous WHOAMI value.");
		INV_MSG(INV_MSG_LEVEL_ERROR, "  - Read 0x%02x", whoami);
		INV_MSG(INV_MSG_LEVEL_ERROR, "  - Expected 0x%02x", INV_IMU_WHOAMI);
		return -1;
	}

	rc |= inv_imu_soft_reset(&imu_dev);
	SI_CHECK_RC(rc);

	/*
	 * Configure interrupts pins
	 * - Polarity High
	 * - Pulse mode
	 * - Push-Pull drive
	 */
	int_pin_config.int_polarity = INTX_CONFIG2_INTX_POLARITY_HIGH;
	int_pin_config.int_mode     = INTX_CONFIG2_INTX_MODE_PULSE;
	int_pin_config.int_drive    = INTX_CONFIG2_INTX_DRIVE_PP;
	rc |= inv_imu_set_pin_config_int(&imu_dev, INV_IMU_INT1, &int_pin_config);
	SI_CHECK_RC(rc);

	/* Interrupts configuration */
	memset(&int_config, INV_IMU_DISABLE, sizeof(int_config));
	int_config.INV_UI_DRDY = INV_IMU_ENABLE;
	rc |= inv_imu_set_config_int(&imu_dev, INV_IMU_INT1, &int_config);
	SI_CHECK_RC(rc);

	/* Set FSR */
	rc |= inv_imu_set_accel_fsr(&imu_dev, ACCEL_CONFIG0_ACCEL_UI_FS_SEL_4_G);
	rc |= inv_imu_set_gyro_fsr(&imu_dev, GYRO_CONFIG0_GYRO_UI_FS_SEL_1000_DPS);
	SI_CHECK_RC(rc);

	/* Set ODR */
	rc |= inv_imu_set_accel_frequency(&imu_dev, ACCEL_CONFIG0_ACCEL_ODR_200_HZ);
	rc |= inv_imu_set_gyro_frequency(&imu_dev, GYRO_CONFIG0_GYRO_ODR_200_HZ);
	SI_CHECK_RC(rc);

	/* Set BW = ODR/4 */
	rc |= inv_imu_set_accel_ln_bw(&imu_dev, IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_4);
	rc |= inv_imu_set_gyro_ln_bw(&imu_dev, IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_4);
	SI_CHECK_RC(rc);

	/* Sensor registers are not available in ULP, so select RCOSC clock to use LP mode. */
	rc |= inv_imu_select_accel_lp_clk(&imu_dev, SMC_CONTROL_0_ACCEL_LP_CLK_RCOSC);
	SI_CHECK_RC(rc);

	/* Set power modes */
	if (use_ln) {
		if (accel_en)
			rc |= inv_imu_set_accel_mode(&imu_dev, PWR_MGMT0_ACCEL_MODE_LN);
		if (gyro_en)
			rc |= inv_imu_set_gyro_mode(&imu_dev, PWR_MGMT0_GYRO_MODE_LN);
	} else {
		if (accel_en)
			rc |= inv_imu_set_accel_mode(&imu_dev, PWR_MGMT0_ACCEL_MODE_LP);
		if (gyro_en)
			rc |= inv_imu_set_gyro_mode(&imu_dev, PWR_MGMT0_GYRO_MODE_LP);
	}

	/* Discard N samples at 50Hz to ignore samples at sensor enabling time */
//	if (accel_en)
//		discard_accel_samples = (ACC_STARTUP_TIME_US / 20000) + 1;
//	if (gyro_en)
//		discard_gyro_samples = (GYR_STARTUP_TIME_US / 20000) + 1;

	SI_CHECK_RC(rc);

	return rc;
}


//int bsp_IcmGetRawData(float accel_mg[3], float gyro_dps[3])
//{
//	int rc = 0;
//	inv_imu_sensor_rawdata_t d;
//	
//	rc |= inv_imu_get_raw_data(&imu_dev, &d);
//	SI_CHECK_RC(rc);
//	
//	accel_mg[0] = (float)(d.accel_data[0] * 4 /* mg */) / 32.768;
//	accel_mg[1] = (float)(d.accel_data[1] * 4 /* mg */) / 32.768;
//	accel_mg[2] = (float)(d.accel_data[2] * 4 /* mg */) / 32.768;
//	gyro_dps[0] = (float)((d.gyro_data[0] - GyroOffset.Xoffset) * 2000 /* dps */) / 32768.0;
//	gyro_dps[1] = (float)((d.gyro_data[1] - GyroOffset.Yoffset) * 2000 /* dps */) / 32768.0;
//	gyro_dps[2] = (float)((d.gyro_data[2] - GyroOffset.Zoffset) * 2000 /* dps */) / 32768.0;
//	return 0;
//}

int Get_ICM_RawData(int16_t accel_data[3],int16_t gyro_data[3])
{
	int rc = 0;
	inv_imu_sensor_rawdata_t d;
	
	rc |= inv_imu_get_raw_data(&imu_dev, &d);
	SI_CHECK_RC(rc);
	
	accel_data[0] = d.accel_data[0]; 
    accel_data[1] = d.accel_data[1];
	accel_data[2] = d.accel_data[2];
	
	gyro_data[0] = d.gyro_data[0]; 
    gyro_data[1] = d.gyro_data[1];
	gyro_data[2] = d.gyro_data[2];
	
	
	return 0;
}

/**
 * @brief ICM-45686 初始化（中文注释版）
 *
 * @param use_ln   1 = 使用 LN（Low-Noise）模式；0 = 使用 LP（Low-Power）模式
 * @param accel_en 1 = 使能加速度计
 * @param gyro_en  1 = 使能陀螺仪
 * @return         0 = 初始化成功；其它值 = 参考 inv_imu 错误码
 */
int icm45686_init_cn(int use_ln, int accel_en, int gyro_en)
{
    int  rc = 0;               // 用于累加各 API 的返回值。一旦出错会被宏 SI_CHECK_RC 捕获
    uint8_t whoami = 0;        // 存放芯片 ID（WHO_AM_I 寄存器的值）

    /*================ 1. 配置底层读写接口 =================*/
    imu_dev.transport.read_reg   = icm45686_read_regs;   // SPI/I2C 读回调
    imu_dev.transport.write_reg  = icm45686_write_regs;  // SPI/I2C 写回调
    imu_dev.transport.serif_type = UI_SPI4;              // 本例使用 4 线 SPI
    imu_dev.transport.sleep_us   = delay_us;             // 驱动内部延时回调

    /*================ 2. 上电后等待 3 ms =================*/
    delay_us(3000);  // 让电源、电平稳定；手册推荐值

    /*================ 3. SPI 驱动能力（Slew-Rate）优化 =================
     * DK-SMARTMOTION-REVG 评估板官方示例推荐做法：
     * 把 SPI 输出沿速率设为 10 ns，避免线缆传输失真。
     */
    if (imu_dev.transport.serif_type == UI_SPI3 || imu_dev.transport.serif_type == UI_SPI4) {
        drive_config0_t drive_cfg;
        drive_cfg.pads_spi_slew = DRIVE_CONFIG0_PADS_SPI_SLEW_TYP_10NS;     // 10 ns 沿
        rc |= inv_imu_write_reg(&imu_dev, DRIVE_CONFIG0, 1, (uint8_t *)&drive_cfg);
        SI_CHECK_RC(rc);
        delay_us(2);  // 按手册：寄存器写入后 1.5 μs 生效，这里取整 2 μs
    }

    /*================ 4. 读取并校验 WHOAMI =================*/
    rc |= inv_imu_get_who_am_i(&imu_dev, &whoami);
    SI_CHECK_RC(rc);
    if (whoami != INV_IMU_WHOAMI) {              // 官方头文件里定义：ICM-45686 的 ID = 0x62
        my_printf("WHOAMI Warring 0x%02X, is 0x%02X\r\n", whoami, INV_IMU_WHOAMI);
        return -1;
    }

    /*================ 5. 软复位 =================*/
    rc |= inv_imu_soft_reset(&imu_dev);
    SI_CHECK_RC(rc);

    /*================ 6. 配置中断引脚极性/模式/驱动方式 =================
     * INT1 脚：高电平有效、脉冲模式、推挽输出
     */
    inv_imu_int_pin_config_t pin_cfg;
    pin_cfg.int_polarity = INTX_CONFIG2_INTX_POLARITY_HIGH; // 高电平中断
    pin_cfg.int_mode     = INTX_CONFIG2_INTX_MODE_PULSE;    // 脉冲而非电平
    pin_cfg.int_drive    = INTX_CONFIG2_INTX_DRIVE_PP;      // 推挽（PP）
    rc |= inv_imu_set_pin_config_int(&imu_dev, INV_IMU_INT1, &pin_cfg);
    SI_CHECK_RC(rc);

    /*================ 7. 只打开 DRDY（数据就绪）中断 =================*/
    inv_imu_int_state_t int_cfg;
    memset(&int_cfg, INV_IMU_DISABLE, sizeof(int_cfg)); // 先全部关闭
    int_cfg.INV_UI_DRDY = INV_IMU_ENABLE;               // 仅开 DRDY
    rc |= inv_imu_set_config_int(&imu_dev, INV_IMU_INT1, &int_cfg);
    SI_CHECK_RC(rc);

    /*================ 8. 设置量程 (Full-Scale Range, FSR) ================*/
    rc |= inv_imu_set_accel_fsr(&imu_dev, ACCEL_CONFIG0_ACCEL_UI_FS_SEL_4_G);       // 加速度 ±4 g
    rc |= inv_imu_set_gyro_fsr (&imu_dev, GYRO_CONFIG0_GYRO_UI_FS_SEL_2000_DPS);    // 陀螺仪 ±1000 °/s
    SI_CHECK_RC(rc);

    /*================ 9. 设置输出数据率 (Output Data Rate, ODR) ===========*/
    rc |= inv_imu_set_accel_frequency(&imu_dev, ACCEL_CONFIG0_ACCEL_ODR_800_HZ);    // 200 Hz
    rc |= inv_imu_set_gyro_frequency (&imu_dev, GYRO_CONFIG0_GYRO_ODR_800_HZ);      // 200 Hz
    SI_CHECK_RC(rc);

    /*================ 10. 数字低通滤波带宽 (LPF BW) ======================
     * 这里选用 “ODR/4”，即 50 Hz（因为 ODR=200 Hz）
     */
    rc |= inv_imu_set_accel_ln_bw(&imu_dev, IPREG_SYS2_REG_131_ACCEL_UI_LPFBW_DIV_4);
    rc |= inv_imu_set_gyro_ln_bw (&imu_dev, IPREG_SYS1_REG_172_GYRO_UI_LPFBW_DIV_4);
    SI_CHECK_RC(rc);

    /*================ 11. 低功耗 (LP) 模式时钟源选择 ====================
     * LP 模式下寄存器不可访问，因此切 RCOSC（内部 RC 振荡）给加速度计
     */
    rc |= inv_imu_select_accel_lp_clk(&imu_dev, SMC_CONTROL_0_ACCEL_LP_CLK_RCOSC);
    SI_CHECK_RC(rc);

    /*================ 12. 根据用户选择切换电源模式 =====================*/
    if (use_ln) {  // LN = Low-Noise，高性能
        if (accel_en)
            rc |= inv_imu_set_accel_mode(&imu_dev, PWR_MGMT0_ACCEL_MODE_LN);
        if (gyro_en)
            rc |= inv_imu_set_gyro_mode (&imu_dev, PWR_MGMT0_GYRO_MODE_LN);
    } else {       // LP = Low-Power，省电
        if (accel_en)
            rc |= inv_imu_set_accel_mode(&imu_dev, PWR_MGMT0_ACCEL_MODE_LP);
        if (gyro_en)
            rc |= inv_imu_set_gyro_mode (&imu_dev, PWR_MGMT0_GYRO_MODE_LP);
    }
    SI_CHECK_RC(rc);

    /*================ 13. 如果需要，可在这里丢弃上电后的前 N 个样本 ==============
     * 官方给的简单做法：ACC/GYR 各需要启动时间，对应样本数见手册
     * 本处注释掉，如有需要自行恢复
     */

    return rc;  // rc==0 表示整个流程无错误
}

	

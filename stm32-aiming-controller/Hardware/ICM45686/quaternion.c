
#include "quaternion.h"


// 全局变量定义
Quaternion quat = {1.0f, 0.0f, 0.0f, 0.0f};  // 初始化四元数
EulerAngle euler = {0.0f, 0.0f, 0.0f};       // 初始化欧拉角
gyro_param_t GyroOffset_data;               // 陀螺仪校准值

gyro_param_t Gyro_Actual;

int16_t accldata[3],gyrodata[3];


float FastSqrtI(float x)
{
    union {
        unsigned int i;
        float f;
    } l2f;
    l2f.f = x;
    l2f.i = 0x5F1F1412 - (l2f.i >> 1);
    return l2f.f * (1.69000231f - 0.714158168f * x * l2f.f * l2f.f);
}
//快速开方 当前测试环境比sqrt快15倍左右
float FastSqrt(float x)
{
    return x * FastSqrtI(x);
}


//快速反正弦 当前测试环境比asin快16倍左右
float FastAsin(float x)
{
    float y, g;
    static float num, den, result;
    long i;
    float sign = 1.0;

    y = x;
    if (y < (float)0.0) {
        y = -y;
        sign = -sign;
    }

    if (y > (float)0.5) {
        i = 1;
        if (y > (float)1.0) {
            result = 0.0;
            return result;
        }
        g = (1.0f - y) * 0.5f;
        y = -2.0f * FastSqrtI(g);
    }
    else {
        i = 0;
        if (y < (float)EPS_FLOAT) {
            result = y;
            if (sign < (float)0.0) {
                result = -result;
            }
            return result;
        }
        g = y * y;
    }
    num = ((ASINP_COEF3 * g + ASINP_COEF2) * g + ASINP_COEF1) * g;
    den = ((g + ASINQ_COEF2) * g + ASINQ_COEF1) * g + ASINQ_COEF0;
    result = num / den;
    result = result * y + y;
    if (i == 1) {
        result = result + (float)PI_2;
    }
    if (sign < (float)0.0) {
        result = -result;
    }
    return result;
}
//快速反正切 当前测试环境比atan2快9倍左右
float FastAtan2(float y, float x)
{
    float f, g;
    float num, den;
    static float result;
    int n;

    static const float a[4] = {0, (float)PI_6, (float)PI_2, (float)PI_3};

    if (x == (float)0.0) {
        if (y == (float)0.0) {
            result = 0.0;
            return result;
        }

        result = (float)PI_2;
        if (y > (float)0.0) {
            return result;
        }
        if (y < (float)0.0) {
            result = -result;
            return result;
        }
    }
    n = 0;
    num = y;
    den = x;

    if (num < (float)0.0) {
        num = -num;
    }
    if (den < (float)0.0) {
        den = -den;
    }
    if (num > den) {
        f = den;
        den = num;
        num = f;
        n = 2;
    }
    f = num / den;

    if (f > (float)TWO_MINUS_ROOT3) {
        num = f * (float)SQRT3_MINUS_1 - 1.0f + f;
        den = (float)SQRT3 + f;
        f = num / den;
        n = n + 1;
    }

    g = f;
    if (g < (float)0.0) {
        g = -g;
    }

    if (g < (float)EPS_FLOAT) {
        result = f;
    }
    else {
        g = f * f;
        num = (ATANP_COEF1 * g + ATANP_COEF0) * g;
        den = (g + ATANQ_COEF1) * g + ATANQ_COEF0;
        result = num / den;
        result = result * f + f;
    }
    if (n > 1) {
        result = -result;
    }
    result = result + a[n];

    if (x < (float)0.0) {
        result = PI - result;
    }
    if (y < (float)0.0) {
        result = -result;
    }
    return result;
}

float fAbs(float buf)
{
	if(buf >= 0) return buf;
	else return -buf;
}


void gyroOffsetInit(void)
{
#if GyroOffsetCaculate
		my_printf("start calu gyroOffset");
		GyroOffset_data.Xdata = 0;    //零漂定义
		GyroOffset_data.Ydata = 0;
		GyroOffset_data.Zdata = 0;
		for (uint16_t i = 0; i < 1000; i++)
		{
			Get_ICM_RawData(accldata,gyrodata);    // 获取陀螺仪角速度
			GyroOffset_data.Xdata += gyrodata[0];
			GyroOffset_data.Ydata += gyrodata[1];
			GyroOffset_data.Zdata += gyrodata[2];
			system_delay_ms (5);
		}

		GyroOffset_data.Xdata /= 1000;
		GyroOffset_data.Ydata /= 1000;
		GyroOffset_data.Zdata /= 1000;
		my_printf("%.6f %.6f %.6f\r\n",GyroOffset_data.Xdata,GyroOffset_data.Ydata,GyroOffset_data.Zdata);


#else
		GyroOffset_data.Xdata = 4.802000;
		GyroOffset_data.Ydata = -1.730000;
		GyroOffset_data.Zdata = 0.277000;
	
#endif

}





// 四元数初始化函数
void quaternion_init(void) {
    quat.q0 = 1.0f;
    quat.q1 = 0.0f;
    quat.q2 = 0.0f;
    quat.q3 = 0.0f;
}


// 四元数更新函数，基于陀螺仪角速度数据
void quaternion_update(void)
{
    // 获取陀螺仪数据并转换为弧度/秒
    // 获取原始角速度数据并去零偏
	
	Get_ICM_RawData(accldata,gyrodata);
	
	Gyro_Actual.Xdata = gyrodata[0] - GyroOffset_data.Xdata;
	Gyro_Actual.Ydata = gyrodata[1] - GyroOffset_data.Ydata;
	Gyro_Actual.Zdata = gyrodata[2] - GyroOffset_data.Zdata;
	float gx_raw = (Gyro_Actual.Xdata)*0.0010653f;
    float gy_raw = (Gyro_Actual.Ydata)*0.0010653f;
    float gz_raw = (Gyro_Actual.Zdata)*0.0010653f;


    // 对小角速度值置零（阈值滤波）
    if(fAbs(gx_raw) < 0.015f) gx_raw = 0;
    if(fAbs(gy_raw) < 0.015f) gy_raw = 0;
    if(fAbs(gz_raw) < 0.015f) gz_raw = 0;

    // 使用滤波后的角速度进行积分
    float gx = gx_raw;
    float gy = gy_raw;
    float gz = gz_raw;
    // 时间步长，1ms = 0.001s
    float dt = 0.01f;

    // 四元数微分方程
    float q0 = quat.q0;
    float q1 = quat.q1;
    float q2 = quat.q2;
    float q3 = quat.q3;
    //欧拉法求积分（一阶龙格库塔法）
    quat.q0 += (-q1 * gx - q2 * gy - q3 * gz) * 0.5f * dt;
    quat.q1 += ( q0 * gx + q2 * gz - q3 * gy) * 0.5f * dt;
    quat.q2 += ( q0 * gy - q1 * gz + q3 * gx) * 0.5f * dt;
    quat.q3 += ( q0 * gz + q1 * gy - q2 * gx) * 0.5f * dt;

    // 四元数归一化
    float norm = FastSqrt(quat.q0 * quat.q0 + quat.q1 * quat.q1 + quat.q2 * quat.q2 + quat.q3 * quat.q3);
    quat.q0 /= norm;
    quat.q1 /= norm;
    quat.q2 /= norm;
    quat.q3 /= norm;
}

// 将四元数转换为欧拉角
void quaternion_to_euler(void) {
    // 计算欧拉角
    //euler.roll  = atan2f(2.0f * (quat.q0 * quat.q1 + quat.q2 * quat.q3), 1.0f - 2.0f * (quat.q1 * quat.q1 + quat.q2 * quat.q2));
    //euler.pitch = asinf(2.0f * (quat.q0 * quat.q2 - quat.q3 * quat.q1));
    euler.yaw   = FastAtan2(2.0f * (quat.q0 * quat.q3 + quat.q1 * quat.q2), 1.0f - 2.0f * (quat.q2 * quat.q2 + quat.q3 * quat.q3));

    // 将弧度转换为角度
    //euler.roll  *= 180.0f / PI;
    //euler.pitch *= 180.0f / PI;
    euler.yaw   *= 180.0f / PI;
}


void IMU_Init(void)
{
	SPI2_Init();
	delay_ms(500);
	icm45686_init_cn(1,1,1);
	delay_ms(500);
	
	quaternion_init();
	gyroOffsetInit();
//	my_printf("IMU init successfully\r\n");
}	

void Get_Angle(void)
{
	quaternion_update();
	quaternion_to_euler();
}

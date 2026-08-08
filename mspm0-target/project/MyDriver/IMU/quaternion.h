#ifndef CODE_QUATERNION_H_
#define CODE_QUATERNION_H_

#include "inv_imu_driver.h"
#include "board.h"


#define GyroOffsetCaculate 0
#define system_delay_ms delay_ms
#define G           9.80665f                // m/s^2
#define RadtoDeg    57.324841f              //弧度到角度 (弧度 * 180/3.1415)
#define DegtoRad    0.0174533f              //角度到弧度 (角度 * 3.1415/180)
#define EPS_FLOAT (+3.452669830012e-4f)
#define ASINP_COEF1 (-2.7516555290596f)
#define ASINP_COEF2 (+2.9058762374859f)
#define ASINP_COEF3 (-5.9450144193246e-1f)
#define ASINQ_COEF0 (-1.6509933202424e+1f)
#define ASINQ_COEF1 (+2.4864728969164e+1f)
#define ASINQ_COEF2 (-1.0333867072113e+1f)
#define ATANP_COEF0 (-1.44008344874f)
#define ATANP_COEF1 (-7.20026848898e-1f)
#define ATANQ_COEF0 (+4.32025038919f)
#define ATANQ_COEF1 (+4.75222584599f)
#define PI   (3.141592653589793f) //零飘处理方法
#define PI_2 (1.5707963267948966192313216916398f)
#define PI_3 (1.0471975511965977461542144610932f)
#define PI_4 (0.78539816339744830961566084581988f)
#define PI_6 (0.52359877559829887307710723054658f)
#define TWO_MINUS_ROOT3 (0.26794919243112270647255365849413f)
#define SQRT3_MINUS_1 (0.73205080756887729352744634150587f)
#define SQRT3 (1.7320508075688772935274463415059f)
#define EPS_FLOAT (+3.452669830012e-4f)





typedef struct
{
    float Xdata;
    float Ydata;
    float Zdata;

}gyro_param_t;


// 定义四元数结构体
typedef struct {
    float q0;  // 四元数实部
    float q1;  // 四元数虚部 i
    float q2;  // 四元数虚部 j
    float q3;  // 四元数虚部 k
} Quaternion;

// 定义欧拉角结构体
typedef struct {
    float roll;   // 滚转角
    float pitch;  // 俯仰角
    float yaw;    // 偏航角
} EulerAngle;




// 声明全局变量
extern int16_t accldata[3],gyrodata[3];
extern Quaternion quat;
extern EulerAngle euler;
extern gyro_param_t Gyro_Actual;

// 函数声明
void gyroOffsetInit(void);
void quaternion_init(void);
void quaternion_update(void);
void quaternion_to_euler(void);
void Get_Angle(void);
void IMU_Init(void);



#endif /* CODE_QUATERNION_H_ */

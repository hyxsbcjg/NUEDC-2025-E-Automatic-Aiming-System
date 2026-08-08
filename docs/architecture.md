# 系统结构

## 1. MSPM0 目标车

入口为 `mspm0-target/project/empty.c`。系统初始化灰度传感器、IMU、电机、PID、OLED、K230 串口和 1 ms SysTick。当前主循环只调用 Q1：读取灰度与姿态，根据边线和转角控制左右电机，完成设定圈数后停车。

MSPM0 工程包含现有 `ti_msp_dl_config.c/.h`，它们是 SysConfig 的生成结果。修改硬件资源后应重新生成，而不是只编辑生成文件。

## 2. K230 视觉端

主用候选为 `k230-vision/cv lite find rect only send and jiaozhun.py`。它将 320×240 RGB565 画面转为灰度数组，通过 `cv_lite` 提取候选矩形，选择最小的有效矩形并计算中心。GPIO53 按键按下时，它在矩形区域检测紫色色块；松开后将该点保存为固定校准坐标。

每帧连续发送 12 字节：矩形中心帧和固定校准坐标帧。STM32 分别把两点作为控制目标与实际激光点，计算坐标误差并驱动二维机构。

## 3. STM32 瞄准控制器

入口为 `stm32-aiming-controller/User/main.c`。系统初始化 OLED、按键、激光/继电器、K230 串口、两路步进电机串口、PID 和定时器，并根据按键选择 Q1 到 Q5。

当前有效逻辑主要位于 Q3：

1. 归零 Y 轴并设置 X 轴速度。
2. 向 K230 请求矩形信息。
3. 收到目标后切换到坐标跟踪模式。
4. 使用视觉坐标误差驱动 X/Y 两轴。
5. 误差进入阈值后打开激光/继电器。

## 4. 版本与缺失部分

K230 目录中的 `det_video.py` 和 `find_rect_center.py` 是模型检测实验，依赖未随原目录提供的 `/sdcard/mp_deployment_source/deploy_config.json` 和对应 `.kmodel`。主用传统 CV 候选不依赖这些模型文件，但依赖固件自带的 `cv_lite` 扩展。仓库仍缺少实机固件版本、机构参数和完整板端联调证据。

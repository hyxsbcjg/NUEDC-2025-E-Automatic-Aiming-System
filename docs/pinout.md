# 主要接线

以下只列出已从当前源码确认的主要外部接口。实际接线前仍应对照开发板原理图、驱动器说明书和 SysConfig。

## STM32F103RC

- K230 USART1：`PA9` TX、`PA10` RX，115200、8N1。
- 调试 USART3：源码初始化的调试串口，具体复用请以 `Hardware/USART3.c` 为准。
- X/Y 步进电机通道：UART4 `PC10` TX、`PC11` RX；UART5 `PC12` TX、`PD2` RX，均为 115200。
- 辅助 USART2：`PA2` TX、`PA3` RX，38400。
- 按键：`PC7`、`PC8`、`PC9`、`PA8`。
- 激光/继电器控制：`PC15`。

## MSPM0G3507

MSPM0 的 GPIO、PWM、ADC、串口和时钟由 `mspm0-target/project/empty.syscfg` 与生成的 `ti_msp_dl_config.c/.h` 定义。请在 TI SysConfig 中读取具体引脚分配；不要只凭接口名称接线。

## 电气注意事项

- 确认 K230 与 MCU 共地，并检查双方 IO 电平兼容性。
- 步进驱动器、电机和激光应使用合适的独立电源；控制地按设计共地。
- 第一次联调先断开激光和电机动力电源，只验证逻辑电平与串口数据。

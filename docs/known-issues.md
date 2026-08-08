# 已知问题与验证边界

## 源码完成度

- MSPM0 目标车当前只启用 Q1；其余问法没有形成可运行入口。
- STM32 瞄准控制器的 Q1、Q4、Q5 为空，Q2 只有激光/继电器打开动作，Q3 是主要实现。
- 已补入原目录中的 8 个 K230 脚本。主用候选为传统 CV 版本；两个 AI 检测实验缺少外部 `deploy_config.json` 和 `.kmodel`，不能直接运行。
- 主用 K230 候选持续发送两组坐标但不处理 STM32 的 A1/A2 命令；旧版 `V1.0.py` 才实现命令切换。当前 STM32 仍会发送 A1/A2，因此协议层存在版本差异，虽不妨碍主用候选的连续坐标帧被解析。
- 原始 `V1.0.py` 第 341 行末尾有误输入的 `xx`；开源副本仅删除这两个字符以恢复语法，原资料未修改。
- 固定坐标默认值、紫色阈值、矩形阈值和 GPIO53 校准流程只来自源码，尚无板端复验记录。

## 构建边界

- MSPM0 原工程的 Keil 编译器版本字段为空；开源副本将其最小修正为 Arm Compiler 6.21，以匹配原资料中曾使用的工具链。
- Arm Compiler 6 不允许隐式函数声明；开源副本仅在 `MyDriver/UART/UART.h` 中为原有 `my_printf` 实现补了一条声明，没有改动函数逻辑。
- MSPM0 的 Before Build 命令依赖原 SDK 目录中的 `tools/keil/syscfg.bat`。本仓库保留现有 SysConfig 生成文件；是否能自动重新生成取决于本机 SDK/SysConfig 环境。
- STM32 工程基于 Arm Compiler 5 和 STM32F1 Device Family Pack。
- K230 脚本通过主机 CPython 语法编译只能验证语法结构；`media`、`machine`、`ulab`、`cv_lite` 和 `nncase_runtime` 是 CanMV 固件模块，必须在匹配的 K230 固件上验证。

2026-08-08 在整理副本中执行了新鲜 Rebuild：

- MSPM0：Arm Compiler 6.21，`0 Error(s), 0 Warning(s)`，生成 AXF 和 HEX；程序尺寸为 Code 29892、RO 3688、RW 48、ZI 7648。
- STM32：Arm Compiler 5，`0 Error(s), 0 Warning(s)`，生成 AXF；程序尺寸为 Code 12016、RO 2720、RW 416、ZI 2968。工程未启用 HEX 输出。
- MSPM0 的 Before Build 仍因仓库外 `tools/keil/syscfg.bat` 路径不存在而失败；本次编译使用仓库内已有的 `ti_msp_dl_config.c/.h`。因此不能把结果解释为 SysConfig 已成功重新生成。
- K230：8 个脚本均通过主机 Python `compile()` 语法检查；未执行 CanMV 模块导入或板端运行。

## 尚未证明

即使新鲜编译成功，也只证明当前源码在对应工具链下可以生成固件，不证明：

- 固件已烧录并在目标板启动；
- 传感器、K230、步进电机、激光和目标车接线正确；
- 坐标系方向、比例、机械零位和 PID 参数适配当前机构；
- 全部赛题问法、实时性能、命中精度和安全要求达标。

使用前应完成台架测试、限位和急停验证，再逐步进行低功率闭环联调。

# 2025 年全国大学生电子设计竞赛 E 题：简易自动瞄准系统

> MSPM0 目标车、STM32 瞄准控制器和 K230 视觉端的历史源码快照。

## 项目简介

仓库整合三个相互配合的工程：MSPM0G3507 目标车负责方形路线运行，STM32F103RC 控制器负责二维步进跟踪和激光/继电器，K230 负责矩形检测、固定点校准和坐标发送。

## 项目状态

- 发布类型：历史源码快照，不是完整复现包。
- MSPM0：当前只启用 `Q1_Proc()`；发布副本已完成新鲜 Rebuild。
- STM32：Q1、Q4、Q5 为空，Q2 仅打开激光/继电器，Q3 保留主要自动瞄准逻辑。
- K230：包含 8 个历史脚本；传统 CV 主用候选不依赖模型，两个 AI 实验脚本缺少外部模型和部署配置。
- 硬件：未完成实车、激光、步进机构或三端联调验证。

## 仓库结构

```text
.
├── mspm0-target/              # MSPM0G3507 目标车工程与 SDK 依赖
├── stm32-aiming-controller/   # STM32F103RC 瞄准控制工程
├── k230-vision/               # CanMV K230 视觉脚本
├── docs/                      # 架构、协议、引脚和已知问题
├── LICENSE
└── THIRD_PARTY_NOTICES.md
```

## 硬件与软件环境

- 目标车：MSPM0G3507、Keil、Arm Compiler 6.21、TI DFP 1.3.1
- 瞄准控制器：STM32F103RC、Keil、ARM Compiler 5、STM32F1 DFP 2.2.0
- 视觉端：K230 + 支持 `cv_lite`、`media`、`machine`、`ulab` 的 CanMV 固件
- 执行机构：二维步进电机、驱动器、继电器和激光器

## 快速开始

1. MSPM0：打开 `mspm0-target/project/keil/empty_LP_MSPM0G3507_nortos_keil.uvprojx`，核对 SysConfig、灰度顺序和电机方向后构建。
2. STM32：打开 `stm32-aiming-controller/Project.uvprojx`，先断开激光电源，核对限位、归零方向和步进范围。
3. K230：首选 `k230-vision/cv lite find rect only send and jiaozhun.py`，可在 IDE 中运行或复制为板端 `main.py`。
4. 校准键使用 GPIO53；坐标保存在 `/sdcard/fixed_coords.txt`，无 SD 卡时使用默认值 `(235, 129)`。
5. 先观察画面和串口帧，再连接步进机构；最后在防护条件满足后启用激光。

## 通信与接口

- K230 UART2：GPIO11 TX、GPIO12 RX，115200 baud，8N1。
- STM32 与 K230：TX/RX 交叉并共地。
- 坐标帧、题号命令和动作定义见 [通信协议](docs/protocol.md)。
- 完整接线见 [引脚说明](docs/pinout.md)，脚本选择见 [K230 说明](k230-vision/README.md)。

## 验证记录

- MSPM0 发布副本补全 Arm Compiler 6.21 字段和已有 `my_printf` 声明后完成 Rebuild。
- STM32 工程完成主机端构建核对；具体错误、警告和产物边界见 [已知问题](docs/known-issues.md)。
- K230 脚本完成静态筛选和语法核对，但未在当前 K230 硬件复验。

## 已知问题

- 多个题号流程为空或不完整，不能宣称覆盖全部赛题要求。
- AI 实验脚本缺少 `.kmodel` 和 `deploy_config.json`。
- 坐标方向、阈值、步进限位和命中判定需要按实物重新标定。
- 激光和运动机构具有安全风险；必须配置机械限位、急停和独立断电。
- 详见 [已知问题与复验边界](docs/known-issues.md)。

## 开源与许可

MSPM0、STM32 和 K230 原始目录均保持不变。项目原创代码和文档采用 [MIT License](LICENSE)；TI、Arm CMSIS、ST、传感器及其他第三方内容继续受各自许可约束，详见 [第三方说明](THIRD_PARTY_NOTICES.md)。

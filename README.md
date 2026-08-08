# 2025 年全国大学生电子设计竞赛 E 题：简易自动瞄准系统

这是从个人比赛资料中整理出的历史源码快照，包含三个相互配合的控制与视觉工程：

- `mspm0-target`：MSPM0G3507 目标车控制工程，负责灰度循迹和方形路线运行。
- `stm32-aiming-controller`：STM32F103RC 瞄准控制工程，负责按键交互、K230 坐标接收、二维步进电机跟踪和激光/继电器控制。
- `k230-vision`：CanMV K230 视觉脚本，负责矩形检测、固定点校准和坐标发送。

MSPM0 部分选用原资料中更新时间较新的 `7-30_1025/0Empty` 工程；STM32 部分来自 `Fight_Project/2025 E`；K230 部分来自 `K230/Project/2025 E`。三个原目录均保持不变。

## 重要说明

本仓库不是完整复现包，也不应被描述为完成全部赛题要求的最终作品：

- MSPM0 工程当前只启用了 `Q1_Proc()`，实现按设定圈数沿方形路线行驶。
- STM32 工程中 Q1、Q4、Q5 为空，Q2 仅打开激光/继电器；Q3 包含矩形搜索、视觉坐标跟踪和命中判定等主要逻辑。
- K230 目录包含 8 个历史脚本，没有 `.kmodel` 或完整模型部署目录。主用候选采用传统 CV，不依赖模型；两个 AI 检测实验脚本仍缺少其外部模型和 `deploy_config.json`。
- 未进行实车、激光、步进机构或双系统联调验证。源码和编译通过不等同于硬件可直接工作。

## 目录

```text
.
├─ mspm0-target/
│  ├─ project/                 # Keil 工程、业务代码和现有 SysConfig 生成文件
│  └─ source/                  # 构建所需 TI DriverLib、设备头文件和 CMSIS 头文件
├─ stm32-aiming-controller/    # STM32F103RC Keil 工程
├─ k230-vision/                # CanMV K230 视觉脚本及说明
└─ docs/
   ├─ architecture.md
   ├─ protocol.md
   ├─ pinout.md
   └─ known-issues.md
```

## 使用方法

### MSPM0 目标车

1. 安装 Keil MDK，并准备 Arm Compiler 6.21 和 TI MSPM0 Device Family Pack 1.3.1。
2. 打开 `mspm0-target/project/keil/empty_LP_MSPM0G3507_nortos_keil.uvprojx`。
3. 如需修改引脚或时钟，先在 TI SysConfig 中打开 `mspm0-target/project/empty.syscfg` 并重新生成配置。
4. 编译后下载到 MSPM0G3507。上电前检查电机方向、灰度传感器顺序以及急停条件。

仓库副本把原工程中空缺的编译器版本字段补为 Arm Compiler 6.21，并为已有 `my_printf` 实现补充函数声明；原始比赛目录没有被修改。

### STM32 瞄准控制器

1. 安装 Keil MDK、Arm Compiler 5 和 STM32F1 Device Family Pack 2.2.0。
2. 打开 `stm32-aiming-controller/Project.uvprojx`。
3. 根据 [接线说明](docs/pinout.md)连接 K230、步进电机驱动器和继电器。
4. 在 K230 上运行下一节所述视觉脚本，并确认两端符合 [串口协议](docs/protocol.md)。
5. 首次上电时断开激光电源，先验证串口坐标、限位、归零方向和电机运动范围，再启用激光。

### K230 视觉端

1. 使用支持 `cv_lite`、`media`、`machine` 和 `ulab` 的 CanMV K230 固件。
2. 首选脚本为 `k230-vision/cv lite find rect only send and jiaozhun.py`，可在 IDE 中直接运行，或复制为板端 `main.py` 设置开机运行。
3. K230 UART2 使用 GPIO11 TX、GPIO12 RX，115200、8N1；与 STM32 交叉连接 TX/RX，并共地。
4. GPIO53 配置为下拉输入。按住校准键时脚本在矩形内检测紫色色块；松开后把坐标保存到 `/sdcard/fixed_coords.txt`。未插 SD 卡时使用默认坐标 `(235, 129)`。
5. 先关闭激光，仅观察画面和串口帧。根据实景调整矩形、紫色阈值和机械坐标方向。

K230 各历史脚本的用途和限制见 [`k230-vision/README.md`](k230-vision/README.md)。

## 文档

- [系统结构](docs/architecture.md)
- [串口协议](docs/protocol.md)
- [主要接线](docs/pinout.md)
- [已知问题与验证边界](docs/known-issues.md)

## 安全提示

激光、电机和运动机构可能造成人身或设备伤害。调试时佩戴合适的激光防护用品，避免照射眼睛和反光表面，并为运动机构设置机械限位、急停和独立断电措施。

## 许可证

本仓库作者原创部分采用 [MIT License](LICENSE)。TI、Arm CMSIS、ST、传感器和其他第三方代码仍受各自许可证约束，详见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。

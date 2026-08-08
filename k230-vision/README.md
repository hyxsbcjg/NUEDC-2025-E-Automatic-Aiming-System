# K230 视觉脚本

本目录按原文件名收录 `D:/Study/K230/Project/2025 E` 中的 8 个历史脚本。没有随意合并算法版本，也没有补造缺失模型。

唯一源码修正是删除 `V1.0.py` 原第 341 行 `Display.show_image(...)` 后误输入的 `xx`，否则该历史脚本无法通过 Python 语法解析。原始资料目录未修改。

## 推荐入口

`cv lite find rect only send and jiaozhun.py` 是原目录中更新时间最新、功能最完整的候选：

- `cv_lite` 检测矩形并计算中心；
- GPIO53 按键触发矩形区域内的紫色色块校准；
- 校准坐标保存到 `/sdcard/fixed_coords.txt`；
- UART2 GPIO11/12、115200、8N1；
- 每帧发送矩形中心帧 `40 ... 50` 和固定坐标帧 `60 ... 70`。

在 CanMV IDE 中打开并运行即可。需要开机启动时，可在确认固件 API、相机、屏幕、SD 卡和引脚一致后，把它复制为板端 `main.py`。

## 历史脚本

- `cv lite find rect only send.py`：只发送矩形状态和中心。
- `cv lite find rect.py`：同时发送矩形与紫色/固定坐标。
- `V1.0.py`：传统图像接口版本，支持接收 `20 A1 A1 30` / `20 A2 A2 30` 并切换坐标模式。
- `find rect max.py`、`find_rect nb.py`：较早的矩形、透视和色块实验。
- `det_video.py`、`find_rect_center.py`：模型检测实验，需要原目录中不存在的 `/sdcard/mp_deployment_source/deploy_config.json` 和对应 `.kmodel`。

## 验证边界

这些脚本是 CanMV/MicroPython 环境代码。主机上的语法检查不能替代 K230 实机运行；相机画面、阈值、FPS、UART 波形、校准保存和与 STM32 的闭环控制均需在目标硬件上验证。

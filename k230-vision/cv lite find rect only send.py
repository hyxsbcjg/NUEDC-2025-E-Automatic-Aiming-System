# -*- coding: utf-8 -*-
'''
实验名称：矩形检测+固定点十字+STM32串口通信
实验平台：01Studio CanMV K230
功能说明：
1. 使用cv_lite进行矩形检测
2. 显示固定点十字
3. 检测到矩形发送坐标，未检测到发送0xAA
'''

import time, os, sys
import math
import cv_lite  # 导入cv_lite扩展模块
import ulab.numpy as np  # 导入numpy库
from media.sensor import *
from media.display import *
from media.media import *
from machine import UART, FPIOA

# ============================== 用户配置 ==============================
debug_mode = False         # 关闭调试模式，减少打印

# 串口配置
UART_TX = 11              # 串口发送引脚
UART_RX = 12              # 串口接收引脚
UART_BAUD = 115200        # 波特率

# 固定坐标（添加固定点十字坐标）
FIXED_X = 235             # 固定X坐标
FIXED_Y = 129             # 固定Y坐标

# 通信协议定义
RECT_HEAD = 0x40          # 矩形坐标帧头
RECT_TAIL = 0x50          # 矩形坐标帧尾

# 特殊标记
NO_RECT_MARKER = 0xaa     # 未检测到矩形的标记

# =====================================================================

# --------------------------- 硬件初始化 ---------------------------
# 串口初始化
fpioa = FPIOA()
fpioa.set_function(UART_TX, FPIOA.UART2_TXD)
fpioa.set_function(UART_RX, FPIOA.UART2_RXD)
uart = UART(UART.UART2, UART_BAUD, UART.EIGHTBITS, UART.PARITY_NONE, UART.STOPBITS_ONE)

# 屏幕分辨率设置
lcd_width = 800
lcd_height = 480

# 摄像头初始化
sensor = Sensor(width=1280, height=960)
sensor.reset()
sensor.set_framesize(width=320, height=240)
sensor.set_pixformat(Sensor.RGB565)

# 显示初始化
Display.init(Display.ST7701, width=lcd_width, height=lcd_height, to_ide=True)
MediaManager.init()
sensor.run()

# --------------------------- 配置参数 ---------------------------
# cv_lite矩形检测参数（与你的文档一致）
canny_thresh1 = 50        # Canny边缘检测低阈值
canny_thresh2 = 150       # Canny边缘检测高阈值
approx_epsilon = 0.04     # 多边形拟合精度（越小越精确）
area_min_ratio = 0.005    # 最小面积比例（相对于图像总面积）
max_angle_cos = 0.3       # 角度余弦阈值（越小越接近矩形）
gaussian_blur_size = 3    # 高斯模糊核尺寸（奇数）

# 矩形筛选参数（调整为与你的文档一致）
MIN_AREA = 1000           # 矩形最小面积（你的是1000，文档是100）
MAX_AREA = 100000         # 矩形最大面积
MIN_ASPECT_RATIO = 0.3
MAX_ASPECT_RATIO = 3.0

# --------------------------- 简化的串口通信类 ---------------------------
class SerialProtocol:
    def __init__(self, uart):
        self.uart = uart
        # 预分配发送缓冲区，提高效率
        self.send_buffer = bytearray(6)    # 矩形坐标数据

    def send_rect_status(self, rect_detected, rect_x=0, rect_y=0):
        """发送矩形状态
        如果检测到矩形：发送矩形中心坐标
        如果未检测到：发送0x40, 0xaa, 0xaa, 0xaa, 0xaa, 0x50
        """
        self.send_buffer[0] = RECT_HEAD

        if rect_detected:
            # 发送矩形中心坐标
            self.send_buffer[1] = (rect_x >> 8) & 0xFF
            self.send_buffer[2] = rect_x & 0xFF
            self.send_buffer[3] = (rect_y >> 8) & 0xFF
            self.send_buffer[4] = rect_y & 0xFF
        else:
            # 发送特殊标记表示未检测到矩形
            self.send_buffer[1] = NO_RECT_MARKER
            self.send_buffer[2] = NO_RECT_MARKER
            self.send_buffer[3] = NO_RECT_MARKER
            self.send_buffer[4] = NO_RECT_MARKER

        self.send_buffer[5] = RECT_TAIL

        # 发送6字节
        self.uart.write(self.send_buffer)

# --------------------------- 工具函数 ---------------------------
def calculate_distance(p1, p2):
    return math.sqrt((p2[0] - p1[0])**2 + (p2[1] - p1[1])**2)

def calculate_center(points):
    if not points:
        return (0, 0)
    sum_x = sum(p[0] for p in points)
    sum_y = sum(p[1] for p in points)
    return (int(sum_x / len(points)), int(sum_y / len(points)))

def is_valid_rect(corners):
    """验证矩形的有效性（与你的逻辑完全一致）"""
    edges = [calculate_distance(corners[i], corners[(i+1)%4]) for i in range(4)]

    # 对边比例校验
    ratio1 = edges[0] / max(edges[2], 0.1)
    ratio2 = edges[1] / max(edges[3], 0.1)
    valid_ratio = 0.5 < ratio1 < 1.5 and 0.5 < ratio2 < 1.5

    # 面积校验（使用叉积计算）
    area = 0
    for i in range(4):
        x1, y1 = corners[i]
        x2, y2 = corners[(i+1) % 4]
        area += (x1 * y2 - x2 * y1)
    area = abs(area) / 2
    valid_area = MIN_AREA < area < MAX_AREA

    # 宽高比校验
    width = max(p[0] for p in corners) - min(p[0] for p in corners)
    height = max(p[1] for p in corners) - min(p[1] for p in corners)
    aspect_ratio = width / max(height, 0.1)
    valid_aspect = MIN_ASPECT_RATIO < aspect_ratio < MAX_ASPECT_RATIO

    return valid_ratio and valid_area and valid_aspect

def sort_corners(corners):
    """将矩形角点按左上、右上、右下、左下顺序排序（与你的逻辑一致）"""
    center = calculate_center(corners)
    sorted_corners = sorted(corners, key=lambda p: math.atan2(p[1]-center[1], p[0]-center[0]))

    # 找到左上角点
    left_top = min(sorted_corners, key=lambda p: p[0]+p[1])
    index = sorted_corners.index(left_top)
    sorted_corners = sorted_corners[index:] + sorted_corners[:index]
    return sorted_corners

# --------------------------- 主循环 ---------------------------
clock = time.clock()
serial = SerialProtocol(uart)
image_shape = [sensor.height(), sensor.width()]  # [高, 宽] 用于cv_lite

# 坐标变量
rect_center_x = 0
rect_center_y = 0
rect_detected = False  # 是否检测到矩形

while True:
    clock.tick()
    img = sensor.snapshot()

    # ============ 使用cv_lite进行矩形检测 ============
    # 将RGB图像转为灰度图
    gray_img = img.to_grayscale()
    img_np = gray_img.to_numpy_ref()  # 转为numpy数组供cv_lite使用

    # 调用cv_lite矩形检测函数（与文档中的调用方式完全一致）
    rects = cv_lite.grayscale_find_rectangles_with_corners(
        image_shape,       # 图像尺寸 [高, 宽]
        img_np,            # 灰度图数据
        canny_thresh1,     # Canny低阈值
        canny_thresh2,     # Canny高阈值
        approx_epsilon,    # 多边形拟合精度
        area_min_ratio,    # 最小面积比例
        max_angle_cos,     # 角度余弦阈值
        gaussian_blur_size # 高斯模糊尺寸
    )

    # 筛选最小的有效矩形（与你的逻辑一致）
    min_area = float('inf')
    smallest_rect_corners = None

    for rect in rects:
        # rect格式: [x, y, w, h, c1.x, c1.y, c2.x, c2.y, c3.x, c3.y, c4.x, c4.y]
        if len(rect) >= 12:  # 确保数据完整
            # 提取四个角点（与文档完全一致）
            corners = [
                (rect[4], rect[5]),   # 角点1
                (rect[6], rect[7]),   # 角点2
                (rect[8], rect[9]),   # 角点3
                (rect[10], rect[11])  # 角点4
            ]

            # 验证矩形有效性
            if is_valid_rect(corners):
                # 计算面积（使用宽高计算，与你的代码一致）
                area = rect[2] * rect[3]  # 使用宽高计算面积
                # 更新最小矩形
                if area < min_area:
                    min_area = area
                    smallest_rect_corners = corners

    # 处理矩形
    if smallest_rect_corners:
        rect_detected = True
        sorted_corners = sort_corners(smallest_rect_corners)

        # 绘制矩形边框（红色）
        for i in range(4):
            p1 = sorted_corners[i]
            p2 = sorted_corners[(i + 1) % 4]
            img.draw_line(int(p1[0]), int(p1[1]), int(p2[0]), int(p2[1]),
                         color=(255, 0, 0), thickness=2)

        # 计算矩形中心
        rect_center_x, rect_center_y = calculate_center(sorted_corners)

        # 在矩形中心画十字（绿色）
        img.draw_cross(rect_center_x, rect_center_y, color=(0, 255, 0), size=8, thickness=2)
    else:
        rect_detected = False
        rect_center_x = 0
        rect_center_y = 0

    # ============ 画固定点十字（白色）============
    img.draw_cross(FIXED_X, FIXED_Y, size=12, color=(255, 255, 255), thickness=2)

    # 画图像中心十字（黄色，用于参考）
    img_center_x = img.width() // 2
    img_center_y = img.height() // 2
    img.draw_cross(img_center_x, img_center_y, size=15, color=(255, 255, 0), thickness=1)

    # ============ 发送矩形数据 ============
    serial.send_rect_status(rect_detected, rect_center_x, rect_center_y)

    # 显示图像
    Display.show_image(
        img,
        x=round((lcd_width - sensor.width())/2),
        y=round((lcd_height - sensor.height())/2)
    )

    # 显示FPS和状态
    if debug_mode:
        fps = clock.fps()
        if rect_detected:
            print(f"FPS: {fps:.1f}, Rect: ({rect_center_x},{rect_center_y})")
        else:
            print(f"FPS: {fps:.1f}, No Rect Detected (sending 0xAA)")

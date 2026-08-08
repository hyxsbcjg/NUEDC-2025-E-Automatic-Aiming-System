# -*- coding: utf-8 -*-
'''
实验名称：矩形检测+紫色色块检测+STM32串口通信
实验平台：01Studio CanMV K230
功能说明：
1. 使用cv_lite改进矩形检测
2. 检测紫色色块坐标
3. 根据STM32命令发送不同坐标数据
4. 紫色模式下未检测到紫色时发送固定坐标
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
enable_perfect_circle_generation = False  # 是否生成透视校正圆（影响性能）

# 串口配置
UART_TX = 11              # 串口发送引脚
UART_RX = 12              # 串口接收引脚
UART_BAUD = 115200        # 波特率

# 固定坐标
FIXED_X = 233             # 固定X坐标（修正为233）
FIXED_Y = 126             # 固定Y坐标

# 通信协议定义
CMD_HEAD = 0x20           # 命令帧头
CMD_TAIL = 0x30           # 命令帧尾
RECT_HEAD = 0x40          # 矩形坐标帧头
RECT_TAIL = 0x50          # 矩形坐标帧尾
PURPLE_HEAD = 0x60        # 紫色/固定坐标帧头
PURPLE_TAIL = 0x70        # 紫色/固定坐标帧尾

# 命令定义
CMD_FIXED_COORD = 0xa1    # 发送固定坐标命令
CMD_PURPLE_COORD = 0xa2   # 发送紫色色块坐标命令

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
# cv_lite矩形检测参数
canny_thresh1 = 50        # Canny边缘检测低阈值
canny_thresh2 = 150       # Canny边缘检测高阈值
approx_epsilon = 0.04     # 多边形拟合精度（越小越精确）
area_min_ratio = 0.005    # 最小面积比例（相对于图像总面积）
max_angle_cos = 0.3       # 角度余弦阈值（越小越接近矩形）
gaussian_blur_size = 3    # 高斯模糊核尺寸（奇数）

# 矩形筛选参数
MIN_AREA = 1000           # 矩形最小面积
MAX_AREA = 100000         # 矩形最大面积
MIN_ASPECT_RATIO = 0.3
MAX_ASPECT_RATIO = 3.0

# 圆形生成参数
BASE_RADIUS = 45
POINTS_PER_CIRCLE = 24

# 紫色LAB阈值 (L_min, L_max, a_min, a_max, b_min, b_max)
PURPLE_THRESHOLD = (88, 13, 26, 71, -95, 6)

# 基础矩形比例
RECT_WIDTH = 210
RECT_HEIGHT = 95
TARGET_ASPECT_RATIO = RECT_WIDTH / RECT_HEIGHT

# --------------------------- 串口通信类 ---------------------------
class SerialProtocol:
    def __init__(self, uart):
        self.uart = uart
        self.coord_mode = CMD_FIXED_COORD  # 默认发送固定坐标

        # 预分配发送缓冲区，提高效率
        self.send_buffer = bytearray(12)    # 可以存放两组坐标数据

    def send_all_data(self, rect_detected, rect_x, rect_y, purple_detected, purple_x, purple_y):
        """批量发送所有数据"""
        # 第一组：矩形状态
        self.send_buffer[0] = RECT_HEAD

        if rect_detected:
            self.send_buffer[1] = (rect_x >> 8) & 0xFF
            self.send_buffer[2] = rect_x & 0xFF
            self.send_buffer[3] = (rect_y >> 8) & 0xFF
            self.send_buffer[4] = rect_y & 0xFF
        else:
            self.send_buffer[1] = NO_RECT_MARKER
            self.send_buffer[2] = NO_RECT_MARKER
            self.send_buffer[3] = NO_RECT_MARKER
            self.send_buffer[4] = NO_RECT_MARKER

        self.send_buffer[5] = RECT_TAIL

        # 第二组：根据模式和检测结果选择坐标
        self.send_buffer[6] = PURPLE_HEAD

        # 紫色模式下，如果没检测到紫色，就发送固定坐标
        if self.coord_mode == CMD_PURPLE_COORD and purple_detected:
            # 紫色模式且检测到紫色
            self.send_buffer[7] = (purple_x >> 8) & 0xFF
            self.send_buffer[8] = purple_x & 0xFF
            self.send_buffer[9] = (purple_y >> 8) & 0xFF
            self.send_buffer[10] = purple_y & 0xFF
        else:
            # 固定坐标模式 或 紫色模式但未检测到紫色
            self.send_buffer[7] = (FIXED_X >> 8) & 0xFF
            self.send_buffer[8] = FIXED_X & 0xFF
            self.send_buffer[9] = (FIXED_Y >> 8) & 0xFF
            self.send_buffer[10] = FIXED_Y & 0xFF

        self.send_buffer[11] = PURPLE_TAIL

        # 一次性发送12字节
        self.uart.write(self.send_buffer)

    def process_command(self):
        """快速处理接收命令"""
        # 读取所有可用数据
        if self.uart.any() > 0:
            data = self.uart.read()
            if not data:
                return

            # 直接扫描数据，寻找有效命令帧
            i = 0
            data_len = len(data)
            while i <= data_len - 4:
                # 查找命令帧头
                if data[i] == CMD_HEAD and data[i+3] == CMD_TAIL:
                    d1 = data[i+1]
                    d2 = data[i+2]

                    # 快速处理命令
                    if d1 == d2:  # 有效命令格式
                        if d1 == CMD_FIXED_COORD:
                            self.coord_mode = CMD_FIXED_COORD
                        elif d1 == CMD_PURPLE_COORD:
                            self.coord_mode = CMD_PURPLE_COORD

                    i += 4  # 跳过已处理的帧
                else:
                    i += 1  # 继续搜索

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
    """验证矩形的有效性"""
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
    """将矩形角点按左上、右上、右下、左下顺序排序"""
    center = calculate_center(corners)
    sorted_corners = sorted(corners, key=lambda p: math.atan2(p[1]-center[1], p[0]-center[0]))

    # 找到左上角点
    left_top = min(sorted_corners, key=lambda p: p[0]+p[1])
    index = sorted_corners.index(left_top)
    sorted_corners = sorted_corners[index:] + sorted_corners[:index]
    return sorted_corners

# --------------------------- 紫色检测函数 ---------------------------
def detect_purple_blobs(img, roi=None,
                        pixels_threshold=80, area_threshold=80,
                        aspect_min=0.4, aspect_max=2.5,
                        solidity_min=0.35, margin=5, merge=True):
    """
    检测紫色色块
    返回：按有效面积降序的有效色块列表
    """
    if roi is not None:
        rx, ry, rw, rh = roi
        rx = max(0, rx)
        ry = max(0, ry)
        rw = max(0, min(img.width() - rx, rw))
        rh = max(0, min(img.height() - ry, rh))
        if rw <= 0 or rh <= 0:
            roi = None

    try:
        blobs = img.find_blobs([PURPLE_THRESHOLD],
                               roi=roi,
                               pixels_threshold=pixels_threshold,
                               area_threshold=area_threshold,
                               merge=merge,
                               margin=margin)
    except Exception as e:
        blobs = img.find_blobs([PURPLE_THRESHOLD],
                               pixels_threshold=pixels_threshold,
                               area_threshold=area_threshold,
                               merge=merge,
                               margin=margin)

    valid = []
    for b in blobs or []:
        try:
            x, y, w, h = b.rect()
            cx, cy = b.cx(), b.cy()
            pix = b.pixels() if hasattr(b, "pixels") else w*h
        except:
            x, y, w, h = b[0], b[1], b[2], b[3]
            try:
                cx, cy = b.cx(), b.cy()
            except:
                cx, cy = x + w//2, y + h//2
            pix = w*h

        box_area = max(1, w*h)
        solidity = pix / float(box_area)
        aspect = w / float(max(1, h))

        if (aspect_min <= aspect <= aspect_max) and (solidity >= solidity_min):
            valid.append((x, y, w, h, cx, cy, pix, solidity))

    valid.sort(key=lambda t: t[6], reverse=True)
    return valid[:1]  # 只返回最大的一个

# --------------------------- 主循环 ---------------------------
clock = time.clock()
serial = SerialProtocol(uart)
image_shape = [sensor.height(), sensor.width()]  # [高, 宽] 用于cv_lite

# 坐标变量
rect_center_x = 0
rect_center_y = 0
purple_center_x = 0
purple_center_y = 0
rect_detected = False  # 是否检测到矩形
purple_detected = False  # 是否检测到紫色

while True:
    clock.tick()
    img = sensor.snapshot()

    # 处理串口命令
    serial.process_command()

    # ============ 使用cv_lite进行矩形检测 ============
    # 将RGB图像转为灰度图
    gray_img = img.to_grayscale()
    img_np = gray_img.to_numpy_ref()  # 转为numpy数组供cv_lite使用

    # 调用cv_lite矩形检测函数
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

    # 筛选最小的有效矩形
    min_area = float('inf')
    smallest_rect_corners = None

    for rect in rects:
        # rect格式: [x, y, w, h, c1.x, c1.y, c2.x, c2.y, c3.x, c3.y, c4.x, c4.y]
        if len(rect) >= 12:  # 确保数据完整
            # 提取四个角点
            corners = [
                (rect[4], rect[5]),   # 角点1
                (rect[6], rect[7]),   # 角点2
                (rect[8], rect[9]),   # 角点3
                (rect[10], rect[11])  # 角点4
            ]

            # 验证矩形有效性
            if is_valid_rect(corners):
                # 计算面积
                area = rect[2] * rect[3]  # 使用宽高计算面积
                # 更新最小矩形
                if area < min_area:
                    min_area = area
                    smallest_rect_corners = corners

    # 处理矩形
    rect_roi = None
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

        # 画矩形中心的白色细圆圈
        img.draw_circle(rect_center_x, rect_center_y, 8, color=(255, 255, 255), thickness=1)

        # 设置紫色检测ROI
        xs = [p[0] for p in sorted_corners]
        ys = [p[1] for p in sorted_corners]
        x0, y0 = min(xs), min(ys)
        x1, y1 = max(xs), max(ys)
        pad = 8
        rect_roi = (x0 - pad, y0 - pad, (x1 - x0) + 2*pad, (y1 - y0) + 2*pad)
    else:
        rect_detected = False
        rect_center_x = 0
        rect_center_y = 0

    # 紫色色块检测
    blobs = detect_purple_blobs(
        img,
        roi=rect_roi,
        pixels_threshold=80,
        area_threshold=60,
        solidity_min=0.20,
        margin=6, merge=True
    )

    # 更新紫色中心坐标
    if blobs:
        purple_detected = True  # 标记检测到紫色
        for (x, y, w, h, cx, cy, pix, solidity) in blobs:
            purple_center_x = cx
            purple_center_y = cy
            img.draw_rectangle(x, y, w, h, color=(255, 0, 255), thickness=2)
            img.draw_cross(cx, cy, color=(255, 0, 255), thickness=2)
            print(cx,cy)
    else:
        purple_detected = False  # 标记未检测到紫色
        purple_center_x = 0
        purple_center_y = 0

    # 画固定坐标十字（白色）
    img.draw_cross(FIXED_X, FIXED_Y, size=12, color=(255, 255, 255), thickness=1)

    # 画图像中心十字（白色）
    img_center_x = img.width() // 2
    img_center_y = img.height() // 2
    img.draw_cross(img_center_x, img_center_y, size=15, color=(255, 255, 255), thickness=1)

    # ============ 高频批量发送数据 ============
    # 使用优化的批量发送方法，传入紫色检测标志
    serial.send_all_data(rect_detected, rect_center_x, rect_center_y,
                         purple_detected, purple_center_x, purple_center_y)

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
            if serial.coord_mode == CMD_PURPLE_COORD:
                if purple_detected:
                    print(f"FPS: {fps:.1f}, Rect: ({rect_center_x},{rect_center_y}), Purple: ({purple_center_x},{purple_center_y})")
                else:
                    print(f"FPS: {fps:.1f}, Rect: ({rect_center_x},{rect_center_y}), No Purple (sending Fixed)")
            else:
                print(f"FPS: {fps:.1f}, Rect: ({rect_center_x},{rect_center_y}), Fixed Mode")
        else:
            print(f"FPS: {fps:.1f}, No Rect Detected (sending 0xAA)")

# -*- coding: utf-8 -*-
'''
实验名称：矩形检测+紫色色块检测+STM32串口通信
实验平台：01Studio CanMV K230
功能说明：
1. 检测矩形中心坐标
2. 检测紫色色块坐标
3. 根据STM32命令发送不同坐标数据
'''

import time, os, sys
import math
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
FIXED_Y = 114             # 固定Y坐标

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
# 矩形检测参数
thresholds = [(0, 52)]    # 二值化阈值（矩形用）
MIN_AREA = 1000           # 矩形最小面积
MAX_AREA = 100000         # 矩形最大面积
MIN_ASPECT_RATIO = 0.3
MAX_ASPECT_RATIO = 3.0

# 圆形生成参数
BASE_RADIUS = 45
POINTS_PER_CIRCLE = 24

# 紫色LAB阈值 (L_min, L_max, a_min, a_max, b_min, b_max)
PURPLE_THRESHOLD = (43, 69, 22, 67, -94, 10)

# 基础矩形比例
RECT_WIDTH = 210
RECT_HEIGHT = 95
TARGET_ASPECT_RATIO = RECT_WIDTH / RECT_HEIGHT

# --------------------------- 串口通信类 ---------------------------
class SerialProtocol:
    def __init__(self, uart):
        self.uart = uart
        self.coord_mode = CMD_FIXED_COORD  # 默认发送固定坐标
        self.cmd_buffer = []

    def send_rect_coordinates(self, x, y):
        """发送矩形坐标数据（0x40...0x50）"""
        # 将坐标拆分为高低字节
        cx_h = (x >> 8) & 0xFF
        cx_l = x & 0xFF
        cy_h = (y >> 8) & 0xFF
        cy_l = y & 0xFF

        # 组装数据帧: 0x40, cx_h, cx_l, cy_h, cy_l, 0x50
        data = bytes([RECT_HEAD, cx_h, cx_l, cy_h, cy_l, RECT_TAIL])
        self.uart.write(data)

    def send_purple_or_fixed_coordinates(self, x, y):
        """发送紫色或固定坐标数据（0x60...0x70）"""
        # 将坐标拆分为高低字节
        cx_h = (x >> 8) & 0xFF
        cx_l = x & 0xFF
        cy_h = (y >> 8) & 0xFF
        cy_l = y & 0xFF

        # 组装数据帧: 0x60, cx_h, cx_l, cy_h, cy_l, 0x70
        data = bytes([PURPLE_HEAD, cx_h, cx_l, cy_h, cy_l, PURPLE_TAIL])
        self.uart.write(data)

    def process_command(self):
        """处理接收到的命令"""
        # 读取可用数据
        if self.uart.any() > 0:
            data = self.uart.read()
            if data:
                self.cmd_buffer.extend(data)

        # 查找完整的命令帧
        while len(self.cmd_buffer) >= 4:
            # 查找帧头
            if self.cmd_buffer[0] == CMD_HEAD:
                # 检查是否有完整帧
                if len(self.cmd_buffer) >= 4 and self.cmd_buffer[3] == CMD_TAIL:
                    # 提取命令
                    d1 = self.cmd_buffer[1]
                    d2 = self.cmd_buffer[2]

                    # 处理命令
                    if d1 == CMD_FIXED_COORD and d2 == CMD_FIXED_COORD:
                        self.coord_mode = CMD_FIXED_COORD
                    elif d1 == CMD_PURPLE_COORD and d2 == CMD_PURPLE_COORD:
                        self.coord_mode = CMD_PURPLE_COORD

                    # 移除已处理的帧
                    self.cmd_buffer = self.cmd_buffer[4:]
                else:
                    # 不完整的帧，等待更多数据
                    break
            else:
                # 丢弃无效字节
                self.cmd_buffer.pop(0)

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
    edges = [calculate_distance(corners[i], corners[(i+1)%4]) for i in range(4)]
    ratio1 = edges[0] / max(edges[2], 0.1)
    ratio2 = edges[1] / max(edges[3], 0.1)
    valid_ratio = 0.5 < ratio1 < 1.5 and 0.5 < ratio2 < 1.5

    area = 0.5 * abs(sum(corners[i][0]*corners[(i+1)%4][1] - corners[(i+1)%4][0]*corners[i][1] for i in range(4)))
    valid_area = MIN_AREA < area < MAX_AREA

    width = max(p[0] for p in corners) - min(p[0] for p in corners)
    height = max(p[1] for p in corners) - min(p[1] for p in corners)
    aspect_ratio = width / max(height, 0.1)
    valid_aspect = MIN_ASPECT_RATIO < aspect_ratio < MAX_ASPECT_RATIO

    return valid_ratio and valid_area and valid_aspect

def sort_corners(corners):
    center = calculate_center(corners)
    sorted_c = sorted(corners, key=lambda p: math.atan2(p[1]-center[1], p[0]-center[0]))
    left_top = min(sorted_c, key=lambda p: p[0]+p[1])
    index = sorted_c.index(left_top)
    return sorted_c[index:] + sorted_c[:index]

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

# 坐标变量
rect_center_x = 0
rect_center_y = 0
purple_center_x = 0
purple_center_y = 0

while True:
    clock.tick()
    img = sensor.snapshot()

    # 处理串口命令
    serial.process_command()

    # 图像处理（矩形检测）
    gray_img = img.to_grayscale()
    binary_img = gray_img.binary([thresholds[0]])
    binary_img.erode(1)
    binary_img.dilate(3)

    # 找最小的矩形
    min_area = float('inf')
    smallest_rect_corners = None
    for r in binary_img.find_rects(threshold=12000):
        corners = r.corners()
        if is_valid_rect(corners):
            area = 0.5 * abs(sum(corners[i][0]*corners[(i+1)%4][1] - corners[(i+1)%4][0]*corners[i][1] for i in range(4)))
            if area < min_area:
                min_area = area
                smallest_rect_corners = corners

    # 处理矩形
    rect_roi = None
    if smallest_rect_corners:
        sorted_corners = sort_corners(smallest_rect_corners)

        # 绘制矩形边框（红色）
        for i in range(4):
            p1 = sorted_corners[i]
            p2 = sorted_corners[(i + 1) % 4]
            img.draw_line(p1[0], p1[1], p2[0], p2[1], color=(255, 0, 0), thickness=2)

        # 计算矩形中心
        rect_center_x, rect_center_y = calculate_center(sorted_corners)
        img.draw_cross(rect_center_x, rect_center_y, size=10, color=(0, 255, 0), thickness=2)

        # 设置紫色检测ROI
        xs = [p[0] for p in sorted_corners]
        ys = [p[1] for p in sorted_corners]
        x0, y0 = min(xs), min(ys)
        x1, y1 = max(xs), max(ys)
        pad = 8
        rect_roi = (x0 - pad, y0 - pad, (x1 - x0) + 2*pad, (y1 - y0) + 2*pad)

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
        for (x, y, w, h, cx, cy, pix, solidity) in blobs:
            purple_center_x = cx
            purple_center_y = cy
            img.draw_rectangle(x, y, w, h, color=(255, 0, 255), thickness=2)
            img.draw_cross(cx, cy, color=(255, 0, 255), thickness=2)

    # 画固定坐标十字（白色）
    img.draw_cross(FIXED_X, FIXED_Y, size=12, color=(255, 255, 255), thickness=2)

    # 画图像中心十字（白色）
    img_center_x = img.width() // 2
    img_center_y = img.height() // 2
    img.draw_cross(img_center_x, img_center_y, size=15, color=(255, 255, 255), thickness=1)

    # 每帧都发送坐标数据（无延时）
    # 第一组：发送矩形中心坐标（0x40...0x50）
    serial.send_rect_coordinates(rect_center_x, rect_center_y)

    # 第二组：根据模式发送紫色或固定坐标（0x60...0x70）
    if serial.coord_mode == CMD_FIXED_COORD:
        # 发送固定坐标
        serial.send_purple_or_fixed_coordinates(FIXED_X, FIXED_Y)
    else:  # CMD_PURPLE_COORD
        # 发送紫色色块坐标
        serial.send_purple_or_fixed_coordinates(purple_center_x, purple_center_y)

    # 显示图像
    Display.show_image(
        img,
        x=round((lcd_width - sensor.width())/2),
        y=round((lcd_height - sensor.height())/2)
    )

# -*- coding: utf-8 -*-
'''
实验名称：矩形检测+固定点十字+STM32串口通信+按键色块检测
实验平台：01Studio CanMV K230
功能说明：
1. 使用cv_lite进行矩形检测
2. 显示固定点十字
3. 检测到矩形发送坐标，未检测到发送0xAA
4. 按键按下时在矩形内检测紫色色块，松开时更新固定坐标
5. 使用SD卡存储固定坐标，掉电不丢失
'''

import time, os, sys
import math
import cv_lite  # 导入cv_lite扩展模块
import ulab.numpy as np  # 导入numpy库
from media.sensor import *
from media.display import *
from media.media import *
from machine import UART, FPIOA, Pin

# ============================== 用户配置 ==============================
debug_mode = False         # 关闭调试模式，减少打印

# 串口配置
UART_TX = 11              # 串口发送引脚
UART_RX = 12              # 串口接收引脚
UART_BAUD = 115200        # 波特率

# 配置文件路径（使用SD卡）
CONFIG_FILE = "/sdcard/fixed_coords.txt"

# ============================== SD卡文件操作 ==============================
def wait_for_sdcard(timeout=5):
    """等待SD卡挂载完成"""
    print("等待SD卡挂载...")
    start_time = time.time()

    while time.time() - start_time < timeout:
        try:
            # 尝试列出SD卡目录
            os.listdir("/sdcard")
            print("SD卡已挂载")
            return True
        except:
            time.sleep(0.1)  # 等待100ms后重试

    print("SD卡挂载超时")
    return False

def create_default_config(default_x=235, default_y=129):
    """创建默认配置文件"""
    try:
        with open(CONFIG_FILE, 'w') as f:
            f.write(f"{default_x}\n{default_y}\n")
            f.flush()  # 强制写入
        print(f"创建默认配置文件: ({default_x}, {default_y})")
        return True
    except Exception as e:
        print(f"创建配置文件失败: {e}")
        return False

def load_fixed_coordinates():
    """从SD卡加载固定坐标，如果文件不存在则使用默认值"""
    default_x, default_y = 235, 129

    # 等待SD卡就绪
    if not wait_for_sdcard(timeout=3):
        print("SD卡未就绪，使用默认坐标")
        return default_x, default_y

    # 多次尝试读取
    for attempt in range(3):
        try:
            # 检查文件是否存在
            try:
                os.stat(CONFIG_FILE)
                file_exists = True
            except:
                file_exists = False

            if not file_exists:
                # 文件不存在，创建默认文件
                print("配置文件不存在，创建默认文件")
                if create_default_config(default_x, default_y):
                    return default_x, default_y
                else:
                    raise Exception("无法创建配置文件")

            # 读取文件
            with open(CONFIG_FILE, 'r') as f:
                lines = f.readlines()
                if len(lines) >= 2:
                    x = int(lines[0].strip())
                    y = int(lines[1].strip())
                    # 验证坐标范围
                    if 0 <= x < 320 and 0 <= y < 240:
                        print(f"从SD卡加载坐标: ({x}, {y})")
                        return x, y
                    else:
                        print(f"坐标超出范围: ({x}, {y})，使用默认值")
                        return default_x, default_y
                else:
                    print("文件格式错误，使用默认坐标")
                    # 重新创建文件
                    create_default_config(default_x, default_y)
                    return default_x, default_y

        except Exception as e:
            print(f"第{attempt + 1}次读取失败: {e}")
            if attempt < 2:  # 还有重试机会
                time.sleep(0.2)
            else:
                print(f"读取失败，使用默认坐标: ({default_x}, {default_y})")
                # 尝试创建默认文件
                create_default_config(default_x, default_y)
                return default_x, default_y

def save_fixed_coordinates(x, y):
    """保存固定坐标到SD卡"""
    # 验证坐标范围
    if not (0 <= x < 320 and 0 <= y < 240):
        print(f"坐标超出范围: ({x}, {y})，不保存")
        return False

    # 多次尝试保存
    for attempt in range(3):
        try:
            # 写入文件
            with open(CONFIG_FILE, 'w') as f:
                f.write(f"{x}\n{y}\n")
                f.flush()  # 强制刷新到磁盘

            # 验证写入（延迟一下再读取）
            time.sleep(0.05)
            with open(CONFIG_FILE, 'r') as f:
                lines = f.readlines()
                if len(lines) >= 2:
                    verify_x = int(lines[0].strip())
                    verify_y = int(lines[1].strip())
                    if verify_x == x and verify_y == y:
                        print(f"坐标已保存到SD卡: ({x}, {y})")
                        return True

            print(f"第{attempt + 1}次保存验证失败")

        except Exception as e:
            print(f"第{attempt + 1}次保存失败: {e}")

        if attempt < 2:  # 还有重试机会
            time.sleep(0.1)

    print("坐标保存失败")
    return False

# ============================== 系统初始化 ==============================
# 程序启动延迟，确保系统完全初始化
print("\n" + "="*50)
print("K230 矩形检测系统启动")
print("="*50)
print("等待系统初始化...")
time.sleep(1)  # 给SD卡等外设时间完成初始化

# 加载坐标
FIXED_X, FIXED_Y = load_fixed_coordinates()
print(f"当前固定坐标: ({FIXED_X}, {FIXED_Y})")
print("="*50 + "\n")

# 通信协议定义
RECT_HEAD = 0x40          # 矩形坐标帧头
RECT_TAIL = 0x50          # 矩形坐标帧尾
FIXED_HEAD = 0x60         # 固定坐标帧头
FIXED_TAIL = 0x70         # 固定坐标帧尾

# 特殊标记
NO_RECT_MARKER = 0xaa     # 未检测到矩形的标记

# 紫色LAB阈值 (L_min, L_max, a_min, a_max, b_min, b_max)
PURPLE_THRESHOLD = (43, 69, 22, 67, -94, 10)

# =====================================================================

# --------------------------- 硬件初始化 ---------------------------
# 串口初始化
fpioa = FPIOA()
fpioa.set_function(UART_TX, FPIOA.UART2_TXD)
fpioa.set_function(UART_RX, FPIOA.UART2_RXD)
uart = UART(UART.UART2, UART_BAUD, UART.EIGHTBITS, UART.PARITY_NONE, UART.STOPBITS_ONE)

# 按键初始化（使用Pin53作为按键输入）
fpioa.set_function(53, FPIOA.GPIO53)
button = Pin(53, Pin.IN, Pin.PULL_DOWN)  # 下拉电阻，按下为高电平

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

# --------------------------- 简化的串口通信类 ---------------------------
class SerialProtocol:
    def __init__(self, uart):
        self.uart = uart
        # 预分配发送缓冲区，提高效率
        self.send_buffer = bytearray(12)    # 可以存放两组坐标数据

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
        self.uart.write(self.send_buffer[:6])

    def send_fixed_coordinates(self, x, y):
        """发送固定坐标数据（0x60...0x70）"""
        self.send_buffer[0] = FIXED_HEAD
        self.send_buffer[1] = (x >> 8) & 0xFF
        self.send_buffer[2] = x & 0xFF
        self.send_buffer[3] = (y >> 8) & 0xFF
        self.send_buffer[4] = y & 0xFF
        self.send_buffer[5] = FIXED_TAIL

        # 发送6字节
        self.uart.write(self.send_buffer[:6])

    def send_all_data(self, rect_detected, rect_x, rect_y, fixed_x, fixed_y):
        """批量发送所有数据（12字节）"""
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

        # 第二组：固定坐标
        self.send_buffer[6] = FIXED_HEAD
        self.send_buffer[7] = (fixed_x >> 8) & 0xFF
        self.send_buffer[8] = fixed_x & 0xFF
        self.send_buffer[9] = (fixed_y >> 8) & 0xFF
        self.send_buffer[10] = fixed_y & 0xFF
        self.send_buffer[11] = FIXED_TAIL

        # 一次性发送12字节
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
def detect_purple_blobs(img, roi=None):
    """检测紫色色块"""
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
                               pixels_threshold=80,
                               area_threshold=60,
                               merge=True,
                               margin=6)

        # 找最大的色块
        if blobs:
            largest_blob = max(blobs, key=lambda b: b.pixels())
            return largest_blob
        else:
            return None
    except:
        return None

# --------------------------- 主循环 ---------------------------
clock = time.clock()
serial = SerialProtocol(uart)
image_shape = [sensor.height(), sensor.width()]  # [高, 宽] 用于cv_lite

# 坐标变量
rect_center_x = 0
rect_center_y = 0
rect_detected = False  # 是否检测到矩形

# 按键和色块检测相关变量
last_button_state = 0
is_detecting_purple = False
purple_center_x = 0
purple_center_y = 0
rect_roi = None  # 保存矩形ROI

# 主循环
print("开始主循环...")
while True:
    clock.tick()
    img = sensor.snapshot()

    # ============ 按键检测 ============
    current_button_state = button.value()

    # 检测按键按下（上升沿）
    if current_button_state == 1 and last_button_state == 0:
        is_detecting_purple = True
        purple_center_x = 0
        purple_center_y = 0
        if debug_mode:
            print("按键按下，开始检测紫色")

    # 检测按键释放（下降沿）
    elif current_button_state == 0 and last_button_state == 1:
        is_detecting_purple = False
        # 如果检测到了紫色色块，更新固定坐标
        if purple_center_x != 0 and purple_center_y != 0:
            FIXED_X = purple_center_x
            FIXED_Y = purple_center_y
            # 保存到SD卡
            if save_fixed_coordinates(FIXED_X, FIXED_Y):
                print(f"固定坐标已更新: ({FIXED_X}, {FIXED_Y})")
            else:
                print("警告：坐标更新但保存失败")

    last_button_state = current_button_state

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

        # 设置矩形ROI（用于紫色检测）
        xs = [p[0] for p in sorted_corners]
        ys = [p[1] for p in sorted_corners]
        x0, y0 = min(xs), min(ys)
        x1, y1 = max(xs), max(ys)
        pad = 5  # 稍微扩大一点ROI
        rect_roi = (x0 - pad, y0 - pad, (x1 - x0) + 2*pad, (y1 - y0) + 2*pad)
    else:
        rect_detected = False
        rect_center_x = 0
        rect_center_y = 0
        rect_roi = None

    # ============ 紫色色块检测（仅在按键按下且有矩形时） ============
    if is_detecting_purple and rect_roi is not None:
        purple_blob = detect_purple_blobs(img, rect_roi)
        if purple_blob:
            # 更新紫色中心坐标
            purple_center_x = purple_blob.cx()
            purple_center_y = purple_blob.cy()

            # 绘制紫色色块边框和中心
            img.draw_rectangle(purple_blob.rect(), color=(255, 0, 255), thickness=2)
            img.draw_cross(purple_center_x, purple_center_y, color=(255, 0, 255), size=5, thickness=2)

            # 在左上角显示坐标
            coord_text = f"Purple: ({purple_center_x}, {purple_center_y})"
            img.draw_string_advanced(10, 10, 20, coord_text, color=(255, 0, 255))

    # ============ 画固定点十字（白色）============
    img.draw_cross(FIXED_X, FIXED_Y, size=12, color=(255, 255, 255), thickness=2)

    # 画图像中心十字（黄色，用于参考）
    img_center_x = img.width() // 2
    img_center_y = img.height() // 2
    img.draw_cross(img_center_x, img_center_y, size=15, color=(255, 255, 0), thickness=1)

    # ============ 发送数据 ============
    # 使用批量发送方法，一次发送12字节
    serial.send_all_data(rect_detected, rect_center_x, rect_center_y, FIXED_X, FIXED_Y)

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
            print(f"FPS: {fps:.1f}, Rect: ({rect_center_x},{rect_center_y}), Fixed: ({FIXED_X},{FIXED_Y})")
        else:
            print(f"FPS: {fps:.1f}, No Rect, Fixed: ({FIXED_X},{FIXED_Y})")

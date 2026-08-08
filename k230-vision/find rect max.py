# -*- coding: utf-8 -*-
'''
实验名称：最小矩形检测与全角度圆形校正 (带功能开关版)
实验平台：01Studio CanMV K230
教程：wiki.01studio.cc
说明：基于矩形主方向角精确校正透视变形，确保任意角度下圆形都显示为正圆
'''

import time, os, sys
import math
from media.sensor import *
from media.display import *
from media.media import *
from machine import UART, FPIOA

# ============================== 用户配置 ==============================
# True: 启用并绘制透视校正后的正圆，这会消耗更多CPU资源，导致帧率降低。
# False: 只侦测矩形并标记其中心点，帧率更高。
enable_perfect_circle_generation = False
# =====================================================================

# --------------------------- 硬件初始化 ---------------------------
# 串口初始化
fpioa = FPIOA()
fpioa.set_function(3, FPIOA.UART1_TXD)
fpioa.set_function(4, FPIOA.UART1_RXD)
uart = UART(UART.UART1, 115200)

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
thresholds = [(0, 52)]  # 二值化阈值（矩形用）
MIN_AREA = 1000         # 矩形最小面积
MAX_AREA = 100000       # 矩形最大面积
MIN_ASPECT_RATIO = 0.3
MAX_ASPECT_RATIO = 3.0

BASE_RADIUS = 45
POINTS_PER_CIRCLE = 24

# ✅ 修正：LAB 阈值需保证 min < max；下面给出一组更常见的“紫色”阈值初始值
# (L_min, L_max, a_min, a_max, b_min, b_max)
PURPLE_THRESHOLD = (69, 43, 22, 67, -94, 10)

# 基础矩形比例（可根据实际需求调整）
RECT_WIDTH = 210
RECT_HEIGHT = 95
TARGET_ASPECT_RATIO = RECT_WIDTH / RECT_HEIGHT

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

def send_circle_points(points):
    if not points:
        return
    count = len(points)
    msg = f"$$C,{count},"
    for x, y in points:
        msg += f"{x},{y},"
    msg = msg.rstrip(',') + "##"
    uart.write(msg)

def get_perspective_matrix(src_pts, dst_pts):
    A, B = [], []
    for i in range(4):
        x, y = src_pts[i]; u, v = dst_pts[i]
        A.extend([[x, y, 1, 0, 0, 0, -u*x, -u*y], [0, 0, 0, x, y, 1, -v*x, -v*y]])
        B.extend([u, v])
    n = 8
    for i in range(n):
        max_row = i
        for j in range(i + 1, n):
            if abs(A[j][i]) > abs(A[max_row][i]): max_row = j
        A[i], A[max_row] = A[max_row], A[i]; B[i], B[max_row] = B[max_row], B[i]
        pivot = A[i][i]
        if abs(pivot) < 1e-8: return None
        for j in range(i, n): A[i][j] /= pivot
        B[i] /= pivot
        for j in range(n):
            if i != j:
                factor = A[j][i]
                for k in range(i, n): A[j][k] -= factor * A[i][k]
                B[j] -= factor * B[i]
    return [[B[0], B[1], B[2]], [B[3], B[4], B[5]], [B[6], B[7], 1.0]]

def transform_points(points, matrix):
    transformed = []
    for x, y in points:
        x_h = x * matrix[0][0] + y * matrix[0][1] + matrix[0][2]
        y_h = x * matrix[1][0] + y * matrix[1][1] + matrix[1][2]
        w_h = x * matrix[2][0] + y * matrix[2][1] + matrix[2][2]
        if abs(w_h) > 1e-8: transformed.append((x_h / w_h, y_h / w_h))
    return transformed

def sort_corners(corners):
    center = calculate_center(corners)
    sorted_c = sorted(corners, key=lambda p: math.atan2(p[1]-center[1], p[0]-center[0]))
    left_top = min(sorted_c, key=lambda p: p[0]+p[1])
    index = sorted_c.index(left_top)
    return sorted_c[index:] + sorted_c[:index]

# -------------- 紫色色块检测（优化版） --------------
def detect_purple_blobs(img, roi=None,
                        pixels_threshold=80, area_threshold=80,
                        aspect_min=0.4, aspect_max=2.5,
                        solidity_min=0.35, margin=5, merge=True):
    """
    img: 原图
    roi: (x, y, w, h)；若为 None 则全图
    返回：按有效面积降序的有效色块列表（只保留最大一个以减少打印）
    """
    if roi is not None:
        # 裁剪到图像边界
        rx, ry, rw, rh = roi
        rx = max(0, rx); ry = max(0, ry)
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
        # 某些固件若 roi 超界会抛异常
        blobs = img.find_blobs([PURPLE_THRESHOLD],
                               pixels_threshold=pixels_threshold,
                               area_threshold=area_threshold,
                               merge=merge,
                               margin=margin)

    valid = []
    for b in blobs or []:
        # 兼容 API：既支持属性也支持方法
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

    # 只保留面积最大的一个，减少噪声与打印
    valid.sort(key=lambda t: t[6], reverse=True)
    return valid[:1]

# --------------------------- 主循环 ---------------------------
clock = time.clock()

while True:
    clock.tick()
    img = sensor.snapshot()

    # 图像处理（矩形用）
    gray_img = img.to_grayscale()
    binary_img = gray_img.binary([thresholds[0]])
    binary_img.erode(1)
    binary_img.dilate(3)

#     先找矩形，以便给紫色检测提供 ROI
    min_area = float('inf')
    smallest_rect_corners = None
    for r in binary_img.find_rects(threshold=12000):
        corners = r.corners()
        if is_valid_rect(corners):
            area = 0.5 * abs(sum(corners[i][0]*corners[(i+1)%4][1] - corners[(i+1)%4][0]*corners[i][1] for i in range(4)))
            if area < min_area:
                min_area = area
                smallest_rect_corners = corners

#     只处理最小的矩形
    rect_roi = None
    if smallest_rect_corners:
        sorted_corners = sort_corners(smallest_rect_corners)

        # 绘制矩形边框（红）
        for i in range(4):
            p1 = sorted_corners[i]
            p2 = sorted_corners[(i + 1) % 4]
            img.draw_line(p1[0], p1[1], p2[0], p2[1], color=(255, 0, 0), thickness=2)

        # 矩形中心（绿十字）
        center_x, center_y = calculate_center(sorted_corners)
        img.draw_cross(center_x, center_y, size=10, color=(0, 255, 0), thickness=2)

        # 作为紫色检测的 ROI：取矩形包围盒并外扩
        xs = [p[0] for p in sorted_corners]
        ys = [p[1] for p in sorted_corners]
        x0, y0 = min(xs), min(ys)
        x1, y1 = max(xs), max(ys)
        pad = 8
        rect_roi = (x0 - pad, y0 - pad, (x1 - x0) + 2*pad, (y1 - y0) + 2*pad)

        # 正圆开关
        if enable_perfect_circle_generation:
            width = calculate_distance(sorted_corners[0], sorted_corners[1])
            height = calculate_distance(sorted_corners[1], sorted_corners[2])
            actual_aspect = width / max(height, 0.1)
            is_horizontal = actual_aspect >= 1.0

            virtual_rect = [(0,0), (RECT_WIDTH,0), (RECT_WIDTH,RECT_HEIGHT), (0,RECT_HEIGHT)] if is_horizontal else [(0,0), (RECT_HEIGHT,0), (RECT_HEIGHT,RECT_WIDTH), (0,RECT_WIDTH)]
            matrix = get_perspective_matrix(virtual_rect, sorted_corners)

            if matrix:
                if is_horizontal:
                    radius_x = BASE_RADIUS
                    radius_y = BASE_RADIUS / actual_aspect
                else:
                    radius_x = BASE_RADIUS * actual_aspect
                    radius_y = BASE_RADIUS
                vx, vy = calculate_center(virtual_rect)

                pts = []
                for i in range(POINTS_PER_CIRCLE):
                    ang = 2 * math.pi * i / POINTS_PER_CIRCLE
                    x = vx + radius_x * math.cos(ang)
                    y = vy + radius_y * math.sin(ang)
                    pts.append((x, y))

                mapped = transform_points(pts, matrix)
                int_points = [(int(round(x)), int(round(y))) for x, y in mapped]
                for (px, py) in int_points:
                    img.draw_circle(px, py, 2, color=(255, 0, 255), thickness=2)

                mapped_center = transform_points([(vx, vy)], matrix)
                if mapped_center:
                    mcx, mcy = map(int, map(round, mapped_center[0]))
                    img.draw_circle(mcx, mcy, 3, color=(0, 0, 255), thickness=1)

                # send_circle_points(int_points)

#    ====== 紫色色块检测（优化版，带ROI）======
    blobs = detect_purple_blobs(
        img,
        roi=rect_roi,               # 有矩形就用 ROI，没有就全图
        pixels_threshold=80,       # 可按实景抬高/降低
        area_threshold=60,
        solidity_min=0.20,
        margin=6, merge=True
    )

    for (x, y, w, h, cx, cy, pix, solidity) in blobs:
        img.draw_rectangle(x, y, w, h, color=(255, 0, 255), thickness=2)
        img.draw_cross(cx, cy, color=(255, 0, 255), thickness=2)
        # 打印关键信息（只打印保留的最大一个）
        print(f"[PURPLE] center=({cx},{cy}) box=({x},{y},{w},{h}) pixels={pix} solidity={solidity:.2f}")

#     --- 图像中心白色十字（用于显示自检） ---
    img.draw_cross(232, 114, size=12, color=(255, 255, 255), thickness=2)
    img_center_x = img.width() // 2
    img_center_y = img.height() // 2
    img.draw_cross(img_center_x, img_center_y, size=15, color=(255, 255, 255), thickness=1)

    # 显示图像
    Display.show_image(
        img,
        x=round((lcd_width - sensor.width())/2),
        y=round((lcd_height - sensor.height())/2)
    )

    # 在终端打印FPS
#    print(f"FPS: {clock.fps():.2f}")

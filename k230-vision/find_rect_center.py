import os
import ujson
import aicube
from media.sensor import *
from media.display import *
from media.media import *
from time import *
import nncase_runtime as nn
import ulab.numpy as np
import time, utime, image, random, gc

# ============================== 用户配置 ==============================
debug_mode   = True       # True 显示调试信息
display_mode = "lcd"      # "lcd" 或 "hdmi"
# =====================================================================

# --- 分辨率设定 ---
if display_mode == "lcd":
    DISPLAY_WIDTH  = ALIGN_UP(800, 16)
    DISPLAY_HEIGHT = 480
else:
    DISPLAY_WIDTH  = ALIGN_UP(1920, 16)
    DISPLAY_HEIGHT = 1080

OUT_RGB888P_WIDTH  = ALIGN_UP(1080, 16)
OUT_RGB888P_HEIGH  = 720

# --- 颜色 ---
COLOR_TARGET = (255, 255, 0, 255)  # 黄色十字
COLOR_CENTER = (255, 255, 255,255) # 白色准星

root_path   = "/sdcard/mp_deployment_source/"
config_path = root_path + "deploy_config.json"

# --------------------------- 工具类 ---------------------------
class ScopedTiming:
    def __init__(self, info="", enable_profile=True):
        self.info = info
        self.enable_profile = enable_profile
    def __enter__(self):
        if self.enable_profile:
            self.start_time = time.time_ns()
        return self
    def __exit__(self, exc_type, exc_value, traceback):
        if self.enable_profile:
            t = (time.time_ns()-self.start_time)/1_000_000
            print(f"{self.info} took {t:.2f} ms")

def read_deploy_config(path):
    try:
        with open(path, 'r') as f:
            return ujson.load(f)
    except Exception as e:
        print("read_deploy_config err:", e)
        return None

# --------------------------- 主检测函数 ---------------------------
def detection():
    print("Pure vision detection task started")
    dep = read_deploy_config(config_path)
    if not dep:
        print("load deploy_config.json failed.")
        return

    kmodel_name           = dep["kmodel_path"]
    labels                = dep["categories"]
    confidence_threshold  = dep["confidence_threshold"]
    nms_threshold         = dep["nms_threshold"]
    img_size              = dep["img_size"]
    num_classes           = dep["num_classes"]
    nms_option            = dep["nms_option"]
    model_type            = dep["model_type"]
    if model_type == "AnchorBaseDet":
        anchors = dep["anchors"][0] + dep["anchors"][1] + dep["anchors"][2]

    kmodel_frame_size = img_size
    frame_size        = [OUT_RGB888P_WIDTH, OUT_RGB888P_HEIGH]
    strides           = [8, 16, 32]

    # --- letterbox padding ---
    ori_w, ori_h = OUT_RGB888P_WIDTH, OUT_RGB888P_HEIGH
    width, height = kmodel_frame_size
    ratio = min(float(width)/ori_w, float(height)/ori_h)
    new_w, new_h = int(ratio*ori_w), int(ratio*ori_h)
    dw, dh = (width-new_w)/2, (height-new_h)/2
    top,bottom = int(round(dh-0.1)), int(round(dh+0.1))
    left,right = int(round(dw-0.1)), int(round(dw+0.1))

    # --- 初始化 KPU + AI2D ---
    kpu   = nn.kpu()
    ai2d  = nn.ai2d()
    kpu.load_kmodel(root_path + kmodel_name)
    ai2d.set_dtype(nn.ai2d_format.NCHW_FMT, nn.ai2d_format.NCHW_FMT, np.uint8, np.uint8)
    ai2d.set_pad_param(True,[0,0,0,0, top,bottom,left,right],0,[114,114,114])
    ai2d.set_resize_param(True, nn.interp_method.tf_bilinear, nn.interp_mode.half_pixel)
    ai2d_builder = ai2d.build([1,3,OUT_RGB888P_HEIGH,OUT_RGB888P_WIDTH], [1,3,height,width])

    # --- 摄像头 ---
    sensor = Sensor(id=2)
    sensor.reset()
    sensor.set_hmirror(False); sensor.set_vflip(False)
    sensor.set_framesize(width=DISPLAY_WIDTH, height=DISPLAY_HEIGHT)
    sensor.set_pixformat(PIXEL_FORMAT_YUV_SEMIPLANAR_420)
    sensor.set_framesize(width=OUT_RGB888P_WIDTH, height=OUT_RGB888P_HEIGH, chn=CAM_CHN_ID_2)
    sensor.set_pixformat(PIXEL_FORMAT_RGB_888_PLANAR, chn=CAM_CHN_ID_2)

    # --- 显示 ---
    sensor_bind_info = sensor.bind_info(x=0, y=0, chn=CAM_CHN_ID_0)
    Display.bind_layer(**sensor_bind_info, layer=Display.LAYER_VIDEO1)
    if display_mode == "lcd":
        Display.init(Display.ST7701, to_ide=True)
    else:
        Display.init(Display.LT9611, to_ide=True)

    osd_img = image.Image(DISPLAY_WIDTH, DISPLAY_HEIGHT, image.ARGB8888)

    try:
        MediaManager.init()
        sensor.run()

        data = np.ones((1,3,width,height), dtype=np.uint8)
        ai2d_out = nn.from_numpy(data)

        while True:
            with ScopedTiming("total", debug_mode):
                frame = sensor.snapshot(chn=CAM_CHN_ID_2)
                if frame.format() != image.RGBP888:
                    continue

                ai2d_in = nn.from_numpy(frame.to_numpy_ref())
                ai2d_builder.run(ai2d_in, ai2d_out)

                kpu.set_input_tensor(0, ai2d_out)
                kpu.run()

                results=[]
                for i in range(kpu.outputs_size()):
                    t = kpu.get_output_tensor(i).to_numpy()
                    t = t.reshape((-1,))
                    results.append(t); gc.collect()

                # --- 后处理 ---
                if model_type == "AnchorBaseDet":
                    det = aicube.anchorbasedet_post_process(results[0],results[1],results[2],
                                kmodel_frame_size,frame_size,strides,
                                num_classes,confidence_threshold,nms_threshold,
                                anchors,nms_option)
                elif model_type == "GFLDet":
                    det = aicube.gfldet_post_process(results[0],results[1],results[2],
                                kmodel_frame_size,frame_size,strides,
                                num_classes,confidence_threshold,nms_threshold,nms_option)
                else:
                    det = aicube.anchorfreedet_post_process(results[0],results[1],results[2],
                                kmodel_frame_size,frame_size,strides,
                                num_classes,confidence_threshold,nms_threshold,nms_option)

                # --- 清屏并画中心准星 ---
                osd_img.clear()
                osd_img.draw_cross(DISPLAY_WIDTH//2, DISPLAY_HEIGHT//2,
                                   30, color=COLOR_CENTER, thickness=2)

                # --- 只保留最高置信度的 yuan ---
                best_box=None; best_score=0.0
                for box in det or []:
                    cid, score = box[0], box[1]
                    if labels[cid]=="yuan" and score>best_score:
                        best_box, best_score = box, score

                if best_box is not None:
                    x1,y1,x2,y2 = best_box[2:6]
                    disp_x1 = int(x1*DISPLAY_WIDTH  / OUT_RGB888P_WIDTH)
                    disp_y1 = int(y1*DISPLAY_HEIGHT / OUT_RGB888P_HEIGH)
                    disp_w  = int((x2-x1)*DISPLAY_WIDTH  / OUT_RGB888P_WIDTH)
                    disp_h  = int((y2-y1)*DISPLAY_HEIGHT / OUT_RGB888P_HEIGH)
                    cx = disp_x1 + disp_w//2
                    cy = disp_y1 + disp_h//2

                    if debug_mode:
                        osd_img.draw_cross(cx, cy, 8, color=COLOR_TARGET, thickness=1)
                        print(f"yuan: score={best_score:.2f} center=({cx},{cy})")

                Display.show_image(osd_img, 0, 0, Display.LAYER_OSD3)
                gc.collect()
    except Exception as e:
        print("Err:", e)
    finally:
        os.exitpoint(os.EXITPOINT_ENABLE_SLEEP)
        try: sensor.stop()
        except: pass
        Display.deinit(); MediaManager.deinit()
        gc.collect(); time.sleep(1); nn.shrink_memory_pool()
    print("Detection task ended.")

if __name__ == "__main__":
    detection()

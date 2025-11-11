import cv2
import mediapipe as mp

# 初始化objectron模块
mp_objectron = mp.solutions.objectron
objectron = mp_objectron.Objectron(
    static_image_mode=False,
    max_num_objects=5,
    min_detection_confidence=0.5,
    min_tracking_confidence=0.99,
    model_name='Shoe'
)

# 初始化绘图工具
mp_drawing = mp.solutions.drawing_utils

# 打开摄像头（RTSP流）
cap = cv2.VideoCapture("rtsp://admin:xr12345678@@192.168.3.38:554/Streaming/Channels/101")

# ========== 核心配置：缩放比例（0.1~1.0 缩小，>1.0 放大） ==========
SCALE_RATIO = 0.8  # 缩为原尺寸的 80%（推荐 0.6~1.2 之间）

while cap.isOpened():
    success, image = cap.read()
    if not success:
        print("无法读取摄像头数据。")
        continue

    # 1. 获取原图像尺寸（高 x 宽）
    h, w = image.shape[:2]
    # 2. 计算缩放后的新尺寸（保持宽高比）
    new_w = int(w * SCALE_RATIO)
    new_h = int(h * SCALE_RATIO)
    # 3. 缩放图像
    image_scaled = cv2.resize(
        image,
        (new_w, new_h),
        interpolation=cv2.INTER_LINEAR  # 缩小用 INTER_LINEAR，放大用 INTER_CUBIC 更清晰
    )

    # 4. 转换为RGB格式并检测
    image_rgb = cv2.cvtColor(image_scaled, cv2.COLOR_BGR2RGB)
    results = objectron.process(image_rgb)

    # 5. 转换回BGR格式并绘制
    image_display = cv2.cvtColor(image_rgb, cv2.COLOR_RGB2BGR)
    if results.detected_objects:
        for detected_object in results.detected_objects:
            mp_drawing.draw_landmarks(
                image_display,
                detected_object.landmarks_2d,
                mp_objectron.BOX_CONNECTIONS,
                mp_drawing.DrawingSpec(color=(0, 255, 0), thickness=2, circle_radius=2),
                mp_drawing.DrawingSpec(color=(0, 0, 255), thickness=2)
            )

    # 6. 显示窗口（自动适应缩放后的尺寸）
    cv2.imshow('Objectron Detection (Ratio Scaled)', image_display)

    # 按 'q' 键退出
    if cv2.waitKey(5) & 0xFF == ord('q'):
        break

# 释放资源
cap.release()
cv2.destroyAllWindows()

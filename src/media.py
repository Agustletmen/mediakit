import cv2
import mediapipe as mp

# 初始化 MediaPipe Face Detection
mp_face_detection = mp.solutions.face_detection
mp_drawing = mp.solutions.drawing_utils
face_detection = mp_face_detection.FaceDetection(
    model_selection=1,  # 0 适用于近距离人脸，1 适用于最远 5 米的人脸
    min_detection_confidence=0.5  # 最小检测置信度
)

# 打开摄像头
cap = cv2.VideoCapture("rtsp://192.168.3.115/video1")

while cap.isOpened():
    success, image = cap.read()
    if not success:
        print("无法获取摄像头画面")
        continue

    # 将 BGR 图像转换为 RGB
    image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

    # 进行人脸检测
    results = face_detection.process(image_rgb)

    # 如果检测到人脸，绘制检测框
    if results.detections:
        for detection in results.detections:
            # 绘制人脸检测框和关键点
            mp_drawing.draw_detection(image, detection)

    # 显示结果
    cv2.imshow('MediaPipe Face Detection', image)

    # 按 'q' 退出
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# 释放资源
cap.release()
cv2.destroyAllWindows()
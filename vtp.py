import cv2
import os

# 配置参数
video_path = "test.mp4"   # 视频路径
output_folder = "frames"  # 输出文件夹
os.makedirs(output_folder, exist_ok=True)

n = 12  # 每秒保存 n 帧

# 读取视频
cap = cv2.VideoCapture(video_path)
fps = cap.get(cv2.CAP_PROP_FPS)  # 获取视频帧率

# 确保 fps 不为 0，防止计算错误
if fps == 0 or fps is None:
    print("错误: 无法获取视频的帧率（FPS），请检查视频文件。")
    cap.release()
    exit(1)

frame_interval = int(fps / n)  # 计算每隔多少帧保存一次

print(f"视频帧率: {fps:.2f} fps，每 {frame_interval} 帧保存 1 帧")

frame_count = 0
saved_count = 0

while True:
    ret, frame = cap.read()
    if not ret:
        break

    if frame_count % frame_interval == 0:
        frame_filename = os.path.join(output_folder, f"frame_{saved_count:04d}.bmp")  # 保存为 BMP
        cv2.imwrite(frame_filename, frame)
        print(f"保存 {frame_filename}")
        saved_count += 1

    frame_count += 1

# 释放资源
cap.release()
cv2.destroyAllWindows()

print(f"共保存 {saved_count} 张 BMP 图片")

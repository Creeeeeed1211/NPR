import cv2
import os
from glob import glob


image_folder = "build/output"  
output_video_path = "output_video.mp4"  
#frame for video
fps = 4  

image_files = sorted(glob(os.path.join(image_folder, "*.bmp")))

# 确保有图片可用
if not image_files:
    print("error , no bmp image in file")
    exit(1)

# 读取第一张图片
first_frame = cv2.imread(image_files[0])
height, width, _ = first_frame.shape

# 创建视频写入器 (MP4 编码，H.264 格式)
fourcc = cv2.VideoWriter_fourcc(*"mp4v")  # MP4 编码
video_writer = cv2.VideoWriter(output_video_path, fourcc, fps, (width, height))

# loop
for img_file in image_files:
    frame = cv2.imread(img_file)
    if frame is None:
        print(f"error, skip {img_file}")
        continue
    video_writer.write(frame)  # 写入视频帧
    print(f"load {img_file}")

# 释放资源
video_writer.release()
print(f"video saved {output_video_path}")


import cv2
import os
from glob import glob

# 配置参数
image_folder = "output"  # BMP 图片所在的文件夹
output_video_path = "output_video.mp4"  # 输出视频路径
fps = 16  # 视频帧率 (可以根据需要调整)

# 获取所有 BMP 文件，并按文件名排序
image_files = sorted(glob(os.path.join(image_folder, "*.bmp")))

# 确保有图片可用
if not image_files:
    print("错误: 没有找到 BMP 图片！请检查 output 目录是否有图片。")
    exit(1)

# 读取第一张图片，获取尺寸信息
first_frame = cv2.imread(image_files[0])
height, width, _ = first_frame.shape

# 创建视频写入器 (MP4 编码，H.264 格式)
fourcc = cv2.VideoWriter_fourcc(*"mp4v")  # MP4 编码
video_writer = cv2.VideoWriter(output_video_path, fourcc, fps, (width, height))

# 逐帧写入视频
for img_file in image_files:
    frame = cv2.imread(img_file)
    if frame is None:
        print(f"警告: 无法读取 {img_file}，跳过...")
        continue
    video_writer.write(frame)  # 写入视频帧
    print(f"已添加 {img_file}")

# 释放资源
video_writer.release()
print(f"视频已保存为 {output_video_path}")



import cv2
import os

video_path = "test.mp4"   
output_folder = "build/frames"  
os.makedirs(output_folder, exist_ok=True)
#frame
n = 1  

cap = cv2.VideoCapture(video_path)
fps = cap.get(cv2.CAP_PROP_FPS)  


if fps == 0 or fps is None:
    print("error cannot read video please check video  file")
    cap.release()
    exit(1)

frame_interval = int(fps / n)  # 计算每隔多少帧保存一次

print(f"frame: {fps:.2f} fps，{frame_interval} per frame")

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


cap.release()
cv2.destroyAllWindows()

print(f" {saved_count} imaged saved in total")

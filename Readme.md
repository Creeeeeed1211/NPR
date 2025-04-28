How to run on linux ：
1. open a terminal in NPR folder and type command

mkdir build

cd build

mkdir frames

cmake ..

make

2. put the your image into the frame folder , it can be one or more images.
3. in command line
./SDL2.Test

Video Input:
This project also receive video input, put the video under the NPR folder and run vtp.py file 

python vtp.py

this will turn your video into a frame stream. then run in buid folder with command line

./SDL2.Test

next run ptv.py in NPR folder

python ptv.py


there will be a rendered video called "output_video.mp4" in the NPR folder.

Notice :
1.In Bragg building lab, some of machines cannot play .mp4 file by deafualt, just change another one is the most concise solution.

2.It takes time to render a video with a high frame rate, user can change the rate in vtp.py file. There is a rendered video sample in the folder.




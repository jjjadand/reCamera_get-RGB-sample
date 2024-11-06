import subprocess
import atexit
import time
import os
import numpy as np
import json



rtsp_url = 'rtsp://admin:admin@192.168.42.1:554/test'

with open('config.json', 'r') as f:
    config = json.load(f)

# get param, width and height
width = config['resolution']['width']
height = config['resolution']['height']

frame_size = width * height * 3  



process = subprocess.Popen(['sudo', './device'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

time.sleep(3)


command = [
    'ffmpeg',
    #'-rtsp_transport', 'tcp', #not good
    #'-probesize', '11171',
    '-fflags', 'nobuffer',
    '-i', rtsp_url,
    '-f', 'rawvideo',
    '-pix_fmt', 'rgb24',
    '-fps_mode', 'passthrough',
    '-rtsp_flags', 'listen',
    'pipe:1'
]


with subprocess.Popen(command, stdout=subprocess.PIPE, bufsize=10**8) as proc:
    frame_number = 0
    while True:
        # 读取一帧数据
        raw_frame = proc.stdout.read(frame_size)
        if not raw_frame:
            break

        #  RGB data 2 numpy array， (height, width, 3)
        #if frame_number % 2 == 0:
        print("read from buffer")
        frame_array = np.frombuffer(raw_frame, dtype=np.uint8).reshape((height, width, 3))
        
        # save as .npy 
        if frame_number % 5 == 0:
            filename = f'frame_{frame_number:04d}.npy'
            np.save(filename, frame_array)
        
            print(f"Saved frame {frame_number} to {filename}")
        frame_number += 1

# kill "device"
def cleanup():
    process.terminate()
    process.wait()

atexit.register(cleanup)



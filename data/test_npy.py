import numpy as np
from PIL import Image
import os
import glob


npy_dir = "./npy_frames"  
output_dir = "./images"  
os.makedirs(output_dir, exist_ok=True)  

npy_files = sorted(glob.glob(os.path.join(npy_dir, "frame_*.npy")))


for npy_file in npy_files:

    frame_array = np.load(npy_file)


    image = Image.fromarray(frame_array, "RGB")


    frame_number = os.path.splitext(os.path.basename(npy_file))[0]
    output_path = os.path.join(output_dir, f"{frame_number}.png")


    image.save(output_path)
    print(f"Saved image {output_path}")




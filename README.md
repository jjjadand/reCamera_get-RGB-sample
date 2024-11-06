# Project Setup and Execution Guide

Follow these steps to configure the environment and run the script.

## Setup FFmpeg

Before running the script, configure FFmpeg as follows:



```bash
cd ffmpeg-bin
sudo chmod 755 *
sudo cp * /usr/bin

Go back to the root directory of the project

```bash
cd ffmpeg-so
sudo chmod 755 *
sudo cp * /usr/lib

## Getting start

Make sure your environment has the necessary dependencies installed. Ensure that 'numpy' is installed:

```bash
cd build
sudo python sample.py

'sample.py' will obtain the RGB information of the camera in real time and store it in the numpy array.

## Other

- data/: Contains '.npy; files of the frames processed by the script.
- 'test_npy.py': A script to convert .npy files from the npy_frames directory into .png images.

This should help guide someone through setting up and running your project.







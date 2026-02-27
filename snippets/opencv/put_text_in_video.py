#!/usr/bin/env python3

"""

A common way to embed text into video is have a text in a file separated per line, each line representing 1 video frame.
For videos, this is usually 24/30/50/60/etc frames per second.

"""

import os
import numpy as np
import cv2

data = np.loadtxt("data.txt")

# font
font = cv2.FONT_HERSHEY_SIMPLEX

# org
org_true = (15, 30)
org_pred = (15, 65)

# fontScale
fontScale = 1

# Blue color in BGR
color_true = (5, 255, 5)
color_pred = (255, 255, 255)

# Line thickness of 2 px
thickness = 3

cap = cv2.VideoCapture("video.mp4")

if (cap.isOpened() == False):
    print("Error opening video file")
    exit(1)

fps = cap.get(cv2.CAP_PROP_FPS)
frames = []
i = 0
while(cap.isOpened()):

    # read each video frame
    ret, frame = cap.read()

    if ret == True:
        value = data[i]
        
        bgnd_color = (0,0,0)

        # draw speeds on frame
        frame = cv2.putText(frame, f"Value:     {value:.3f}", org_true, font,
                    fontScale, bgnd_color, 15, cv2.LINE_AA)
        frame = cv2.putText(frame, f"Value:     {value:.3f}", org_true, font,
                            fontScale, color_true, thickness, cv2.LINE_AA)

        # save to list
        frames.append(frame)

        # increment counter
        i += 1

    # Break if nothing is returned
    else:
        break

cap.release()
del cap

# write the video back out with the text in it
w, h = frames[-1].shape[:2][::-1]
out = cv2.VideoWriter("text_on_video.mp4",
                      cv2.VideoWriter_fourcc(*'MP4V'),
                      fps,
                      (w, h))

for frame in frames:
    out.write(frame)
out.release()

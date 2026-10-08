#!/usr/bin/env python

import cv2
import time

def info(cap):
    width = cap.get(cv2.CAP_PROP_FRAME_WIDTH)
    height = cap.get(cv2.CAP_PROP_FRAME_HEIGHT)
    fps = cap.get(cv2.CAP_PROP_FPS)
    fourcc = cap.get(cv2.CAP_PROP_FOURCC)
    fmt = cap.get(cv2.CAP_PROP_FORMAT)
    codec = cap.get(cv2.CAP_PROP_CODEC_PIXEL_FORMAT)
    print("Frame Size: ({}x{}) FPS: {}".format(width, height, fps))
    print("FOURCC: {} Format: {} Codec: {}".format(fourcc, fmt, codec))
    print()

def main():
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        raise IOError("Cannot open camera")
   
    info(cap)

    title = "Camera"
    fps = 0
    start = time.monotonic()
    while True:
        fps += 1
        end = time.monotonic()
        if end-start >= 1:
            title = "Camera (FPS: {})".format(fps/(end-start))
            fps = 0
            start = end

        ret, frame = cap.read()
        frame = cv2.resize(frame, (640, 480), interpolation=cv2.INTER_LANCZOS4)
        cv2.imshow("Camera", frame)
        cv2.setWindowTitle("Camera", title)

        key = cv2.waitKey(1)
        if key == 27:
            break

    cap.release()
    cv2.destroyAllWindows()

main()

